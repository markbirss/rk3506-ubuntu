/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  ******************************************************************************
  * @file    gicv3.c
  * @author  Derrick
  * @version V0.1
  * @date    2019/5/15
  * @brief   GICV3 Driver
  *
  ******************************************************************************
  */

#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_GICV3

#include "gic.h"
#include "gicv3.h"
#include "cp15.h"

#define PLAT_RK_CLST_TO_CPUID_SHIFT 6
#define MPIDR_AFFINITY_BITS     8
#define MPIDR_AFFLVL_MASK       0xff
#define MPIDR_CLUSTER_MASK      (MPIDR_AFFLVL_MASK << MPIDR_AFFINITY_BITS)
typedef unsigned int (*mpidr_hash_fn)(rt_uint32_t mpidr);

struct arm_gic
{
    rt_uint32_t offset;     /* the first interrupt index in the vector table */
    rt_uint32_t dist_hw_base;   /* the base address of the gic distributor */
    rt_uint32_t cpu_hw_base;   /* the base address of the gic cpu interface */
    rt_uint32_t gicr_hw_base;   /* the base address of the gic re-distributor */
    rt_uint32_t gicr_base_addrs[ARM_GIC_MAX_RD_NR];
};

/* 'ARM_GIC_MAX_NR' is the number of cores */
static struct arm_gic _gic_table[ARM_GIC_MAX_NR];
static unsigned int _gic_max_irq;
static int _gicv3_sre_en;

int plat_core_pos_by_mpidr(rt_uint32_t mpidr)
{
    return (mpidr & MPIDR_AFFLVL_MASK) +
           ((mpidr & MPIDR_CLUSTER_MASK) >> PLAT_RK_CLST_TO_CPUID_SHIFT);
}

unsigned int plat_my_core_pos(void)
{
    return plat_core_pos_by_mpidr(rt_cpu_get_smp_id());
}

/* Our default, arbitrary priority value. Linux only uses one anyway. */
#define DEFAULT_PMR_VALUE   0xf0

static void gicd_wait_for_rwp(rt_uint32_t dist_base)
{
    uint32_t count = 1000000;

    while (GIC_DIST_CTRL(dist_base) & GICD_CTLR_RWP)
    {
        count--;
        if (!count)
        {
            rt_kprintf("GICD RWP timeout\n");
            return;
        }
        __asm__ volatile("nop");
    };
}

static void gicr_wait_for_rwp(rt_uint32_t rbase)
{
    uint32_t count = 1000000;

    while (GIC_RD_CTRL(rbase) & GICR_CTLR_RWP)
    {
        count--;
        if (!count)
        {
            rt_kprintf("GICR(0x%x) RWP timeout\n", rbase);
            return;
        }
        __asm__ volatile("nop");
    };
}

static void gic_enable_redist(rt_uint32_t rbase, int enable)
{
    rt_uint32_t count = 1000000;    /* 1s! */
    rt_uint32_t val;

    val = GIC_RD_WAKER(rbase);
    if (enable)
        /* Wake up this CPU redistributor */
        val &= ~GICR_WAKER_ProcessorSleep;
    else
        val |= GICR_WAKER_ProcessorSleep;

    GIC_RD_WAKER(rbase) = val;

    if (!enable)        /* Check that GICR_WAKER is writeable */
    {
        val = GIC_RD_WAKER(rbase);
        if (!(val & GICR_WAKER_ProcessorSleep))
            return; /* No PM support in this redistributor */
    }

    while (--count)
    {
        val = GIC_RD_WAKER(rbase);
        if (enable ^ (val & GICR_WAKER_ChildrenAsleep))
            break;
        __asm__ volatile("nop");
    }
    if (!count)
        rt_kprintf("redistributor failed to %s...\n",
                   enable ? "wakeup" : "sleep");
}

static inline int gic_enable_sre(void)
{
    uint32_t val;

    val = read_icc_sre();
    if (val & ICC_SRE_EL1_SRE)
        return 1;

    val |= ICC_SRE_EL1_SRE;
    write_icc_sre(val);
    val = read_icc_sre();

    return !!(val & ICC_SRE_EL1_SRE);
}

static int gic_validate_dist_version(rt_uint32_t dist_base)
{
    uint32_t reg = GIC_DIST_ICPIDR2(dist_base) & GIC_PIDR2_ARCH_MASK;

    if (reg != GIC_PIDR2_ARCH_GICv3 && reg != GIC_PIDR2_ARCH_GICv4)
        return -ENODEV;

    return 0;
}

static void gicv3_rdistif_base_addrs_probe(rt_uint32_t index,
        mpidr_hash_fn mpidr_to_core_pos)
{
    rt_uint32_t mpidr;
    rt_uint32_t proc_num;
    rt_uint64_t typer_val;
    rt_uint32_t rdistif_base = _gic_table[index].gicr_hw_base;

    /*
     * Iterate over the Redistributor frames. Store the base address of each
     * frame in the platform provided array. Use the "Processor Number"
     * field to index into the array if the platform has not provided a hash
     * function to convert an MPIDR (obtained from the "Affinity Value"
     * field into a linear index.
     */
    do
    {
        typer_val = readq((void *)&GIC_RD_TYPE(rdistif_base));
        if (mpidr_to_core_pos)
        {
            mpidr = mpidr_from_gicr_typer(typer_val);
            proc_num = mpidr_to_core_pos(mpidr);
        }
        else
        {
            proc_num = (typer_val >> TYPER_PROC_NUM_SHIFT) &
                       TYPER_PROC_NUM_MASK;
        }

        RT_ASSERT(proc_num < ARM_GIC_MAX_RD_NR);

        _gic_table[index].gicr_base_addrs[proc_num] = rdistif_base;
        rdistif_base += (1 << GICR_PCPUBASE_SHIFT);
    }
    while (!(typer_val & TYPER_LAST_BIT));
}

static void gic_dist_config(rt_uint32_t dist_base, int irq_nr)
{
    unsigned int i;

    for (i = 32; i < _gic_max_irq; i += 32)
    {
        GIC_DIST_IGROUP(dist_base, i) = 0;
        GIC_DIST_IGRPMODR(dist_base, i) = ~0;
    }

    /*
     * Set all global interrupts to be level triggered, active low.
     */
    for (i = 32; i < irq_nr; i += 16)
        GIC_DIST_CONFIG(dist_base, i) = GICD_INT_ACTLOW_LVLTRIG;

    /*
     * Set priority on all global interrupts.
     */
    for (i = 32; i < irq_nr; i += 4)
        GIC_DIST_PRI(dist_base, i) = GICD_INT_DEF_PRI_X4;

    /*
     * Deactivate and disable all SPIs. Leave the PPI and SGIs
     * alone as they are in the redistributor registers on GICv3.
     */
    for (i = 32; i < irq_nr; i += 32)
    {
        GIC_DIST_ACTIVE_CLEAR(dist_base, i) = GICD_INT_EN_CLR_X32;
        GIC_DIST_ENABLE_CLEAR(dist_base, i) = GICD_INT_EN_CLR_X32;
    }

    gicd_wait_for_rwp(dist_base);
}

static void gic_rd_config(rt_uint32_t rbase)
{
    int i;

    GIC_RD_IGROUP0(rbase) = 0;
    GIC_RD_IGRPMODR0(rbase) = ~0;

    /*
     * Deal with the banked PPI and SGI interrupts - disable all
     * PPI interrupts, ensure all SGI interrupts are enabled.
     * Make sure everything is deactivated.
     */
    GIC_RD_ACTIVE_CLEAR0(rbase) = GICD_INT_EN_CLR_X32;
    GIC_RD_ENABLE_CLEAR0(rbase) = ~0;

    gicr_wait_for_rwp(rbase);

    /*
     * Set priority on PPI and SGI interrupts
     */
    for (i = 0; i < 32; i += 4)
        GIC_RD_PRI0(rbase, i) = GICD_INT_DEF_PRI_X4;

    GIC_RD_CONFIG1(rbase) = 0;
}

static void gic_cpu_sys_reg_init(void)
{
    RT_ASSERT(gic_enable_sre());

    /* Set priority mask register */
    write_icc_pmr(DEFAULT_PMR_VALUE);

    /* EOI deactivates interrupt too (mode 0) */
    write_icc_ctlr(ICC_CTLR_EL1_EOImode_drop_dir);

    /* enable group1 for curent secure state */
    write_icc_igrpen1(IGRPEN1_ENABLE_G1_BIT);
}

static void gic_cpu_init(rt_uint32_t cpu_base)
{
    /* Set priority mask register */
    GIC_CPU_PRIMASK(cpu_base) = DEFAULT_PMR_VALUE;

    /* EOI deactivates interrupt too (mode 0) */
    GIC_CPU_CTRL(cpu_base) |= (1 << 1);
}

int arm_gic_dist_init(rt_uint32_t index, rt_uint32_t dist_base, rt_uint32_t rd_base, int irq_start)
{
    RT_ASSERT(gic_validate_dist_version(dist_base) == 0);

    _gic_table[index].dist_hw_base = dist_base;
    _gic_table[index].gicr_hw_base = rd_base;
    _gic_table[index].offset = irq_start;

    _gic_max_irq = GICD_TYPER_IRQS(GIC_DIST_TYPE(_gic_table[index].dist_hw_base));

    gicv3_rdistif_base_addrs_probe(index, NULL);

    /* Disable the distributor */
    GIC_DIST_CTRL(dist_base) &=
        ~(GICD_CTLR_ENABLE_G1S | GICD_CTLR_ENABLE_G1N | GICD_CTLR_ENABLE_G0);
    gicd_wait_for_rwp(dist_base);

    GIC_DIST_CTRL(dist_base) |= (GICD_CTLR_ARE_NS | GICD_CTLR_ARE_S);

    gicd_wait_for_rwp(dist_base);

    gic_dist_config(dist_base, _gic_max_irq);

    GIC_DIST_CTRL(dist_base) |=
        (GICD_CTLR_ENABLE_G0 | GICD_CTLR_ENABLE_G1S);
    gicd_wait_for_rwp(dist_base);

    _gicv3_sre_en = gic_enable_sre();

    return 0;
}

int arm_gic_cpu_init(rt_uint32_t index, rt_uint32_t cpu_base)
{
    rt_uint32_t rbase = _gic_table[index].gicr_base_addrs[plat_my_core_pos()];

    _gic_table[index].cpu_hw_base = cpu_base;

    gic_enable_redist(rbase, 1);
    gic_rd_config(rbase);

    if (_gicv3_sre_en)
        gic_cpu_sys_reg_init();
    else
        gic_cpu_init(cpu_base);

    return 0;
}

int arm_gic_get_active_irq(rt_uint32_t index)
{
    int irq;

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    if (_gicv3_sre_en)
        irq = (read_icc_iar1() & GIC_ICCIAR_INTID_MASK) >> GIC_ICCIAR_INTID_SHIFT;
    else
        irq = (GIC_CPU_INTACK(_gic_table[index].cpu_hw_base) & GIC_ICCIAR_INTID_MASK) >> GIC_ICCIAR_INTID_SHIFT;

    irq += _gic_table[index].offset;

    return irq;
}

void arm_gic_ack(rt_uint32_t index, int irq)
{
    RT_ASSERT(index < ARM_GIC_MAX_NR);

    irq = irq - _gic_table[index].offset;
    RT_ASSERT(irq >= 0);

    if (_gicv3_sre_en)
        write_icc_eoir1(irq);
    else
        GIC_CPU_EOI(_gic_table[index].cpu_hw_base) = irq;
}

void arm_gic_clear_pending(rt_uint32_t index, int irq)
{
    rt_uint32_t mask = 1 << (irq % 32);

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    irq = irq - _gic_table[index].offset;
    RT_ASSERT(irq >= 0);

    if (irq >= 0 && irq < 32)
    {
        GIC_RD_PENDING_CLEAR0(_gic_table[index].gicr_base_addrs[plat_my_core_pos()]) = mask;
    }
    else if (irq >= 32 && irq < _gic_max_irq)
    {
        GIC_DIST_PENDING_CLEAR(_gic_table[index].dist_hw_base, irq) = mask;
    }
}

void arm_gic_clear_active(rt_uint32_t index, int irq)
{
    rt_uint32_t mask = 1 << (irq % 32);

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    irq = irq - _gic_table[index].offset;
    RT_ASSERT(irq >= 0);

    if (irq >= 0 && irq < 32)
        GIC_RD_ACTIVE_CLEAR0(_gic_table[index].gicr_base_addrs[plat_my_core_pos()]) = mask;
    else if (irq >= 32 && irq < _gic_max_irq)
        GIC_DIST_ACTIVE_CLEAR(_gic_table[index].dist_hw_base, irq) = mask;
}

void arm_gic_umask(rt_uint32_t index, int irq)
{
    rt_uint32_t mask = 1 << (irq % 32);

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    irq = irq - _gic_table[index].offset;
    RT_ASSERT(irq >= 0);

    if (irq >= 0 && irq < 32)
        GIC_RD_ENABLE_SET0(_gic_table[index].gicr_base_addrs[plat_my_core_pos()])  = mask;
    else if (irq >= 32 && irq < _gic_max_irq)
        GIC_DIST_ENABLE_SET(_gic_table[index].dist_hw_base, irq) = mask;
}

void arm_gic_mask(rt_uint32_t index, int irq)
{
    rt_uint32_t mask = 1 << (irq % 32);

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    irq = irq - _gic_table[index].offset;
    RT_ASSERT(irq >= 0);

    if (irq >= 0 && irq < 32)
    {
        GIC_RD_ENABLE_CLEAR0(_gic_table[index].gicr_base_addrs[plat_my_core_pos()]) = mask;
        gicr_wait_for_rwp(_gic_table[index].gicr_base_addrs[plat_my_core_pos()]);
    }
    else if (irq >= 32 && irq < _gic_max_irq)
    {
        GIC_DIST_ENABLE_CLEAR(_gic_table[index].dist_hw_base, irq) = mask;
        gicd_wait_for_rwp(_gic_table[index].dist_hw_base);
    }
}

/* Set up the cpu mask for the specific interrupt */
void arm_gic_set_cpu(rt_uint32_t index, int irq, unsigned int cpumask)
{
    uint64_t gic_affinity_val;

    RT_ASSERT(index < ARM_GIC_MAX_NR);

    irq = irq - _gic_table[index].offset;
    RT_ASSERT(irq >= 0);

    gic_affinity_val =
        gicd_irouter_val_from_mpidr(rt_cpu_get_smp_id(), 0);

    writeq(gic_affinity_val, (void *)&GIC_DIST_IROUTER(_gic_table[index].dist_hw_base, irq));
}

void arm_gic_dump_type(rt_uint32_t index)
{}

void arm_gic_dump(rt_uint32_t index)
{}

#endif
