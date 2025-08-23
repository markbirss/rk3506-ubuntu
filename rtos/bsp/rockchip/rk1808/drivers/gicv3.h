/**
  * Copyright (c) 2019 Fuzhou Rockchip Electronics Co., Ltd
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Change Logs:
  * Date           Author       Notes
  * 2019-05-15     Derrick      first implementation.
  */

#ifndef ARM_GICV3_H
#define ARM_GICV3_H

/*
 * Distributor registers. We assume we're running non-secure, with ARE
 * being set. Secure-only and non-ARE registers are not described.
 */
#define GIC_DIST_CTRL(hw_base)              __REG32((hw_base) + 0x000)
#define GIC_DIST_TYPE(hw_base)              __REG32((hw_base) + 0x004)
#define GIC_DIST_IIDR(hw_base)              __REG32((hw_base) + 0x008)
#define GIC_DIST_STATUSR(hw_base)       __REG32((hw_base) + 0x010)
#define GIC_DIST_SETSPI_NSR(hw_base)        __REG32((hw_base) + 0x040)
#define GIC_DIST_CLRSPI_NSR(hw_base)        __REG32((hw_base) + 0x048)
#define GIC_DIST_SETSPI_SR(hw_base)         __REG32((hw_base) + 0x050)
#define GIC_DIST_CLRSPI_SR(hw_base)         __REG32((hw_base) + 0x058)
#define GIC_DIST_IGROUP(hw_base, n)     __REG32((hw_base) + 0x080 + ((n)/32) * 4)
#define GIC_DIST_ENABLE_SET(hw_base, n)     __REG32((hw_base) + 0x100 + ((n)/32) * 4)
#define GIC_DIST_ENABLE_CLEAR(hw_base, n)   __REG32((hw_base) + 0x180 + ((n)/32) * 4)
#define GIC_DIST_PENDING_SET(hw_base, n)    __REG32((hw_base) + 0x200 + ((n)/32) * 4)
#define GIC_DIST_PENDING_CLEAR(hw_base, n)  __REG32((hw_base) + 0x280 + ((n)/32) * 4)
#define GIC_DIST_ACTIVE_SET(hw_base, n)     __REG32((hw_base) + 0x300 + ((n)/32) * 4)
#define GIC_DIST_ACTIVE_CLEAR(hw_base, n)   __REG32((hw_base) + 0x380 + ((n)/32) * 4)
#define GIC_DIST_PRI(hw_base, n)        __REG32((hw_base) + 0x400 +  ((n)/4) * 4)
#define GIC_DIST_TARGET(hw_base, n)     __REG32((hw_base) + 0x800 +  ((n)/4) * 4)
#define GIC_DIST_CONFIG(hw_base, n)     __REG32((hw_base) + 0xc00 + ((n)/16) * 4)
#define GIC_DIST_IGRPMODR(hw_base, n)       __REG32((hw_base) + 0xd00 + ((n)/32) * 4)
#define GIC_DIST_NSACR(hw_base, n)          __REG32((hw_base) + 0xe00 + ((n)/16) * 4)
#define GIC_DIST_SOFTINT(hw_base)           __REG32((hw_base) + 0xf00)
#define GIC_DIST_CPENDSGI(hw_base, n)       __REG32((hw_base) + 0xf10 + ((n)/4) * 4)
#define GIC_DIST_SPENDSGI(hw_base, n)       __REG32((hw_base) + 0xf20 + ((n)/4) * 4)
#define GIC_DIST_IROUTER(hw_base, n)        __REG32((hw_base) + 0x6000 + (n) * 8)
#define GIC_DIST_ICPIDR2(hw_base)           __REG32((hw_base) + 0xffe8)

/* for secure access */
#define GICD_CTLR_RWP           (1U << 31)
#define GICD_CTLR_E1NWF         (1U << 7)
#define GICD_CTLR_DS            (1U << 6)
#define GICD_CTLR_ARE_NS        (1U << 5)
#define GICD_CTLR_ARE_S             (1U << 4)
#define GICD_CTLR_ENABLE_G1S        (1U << 2)
#define GICD_CTLR_ENABLE_G1N        (1U << 1)
#define GICD_CTLR_ENABLE_G0     (1U << 0)

#define GICD_TYPER_LPIS         (1U << 17)
#define GICD_TYPER_MBIS         (1U << 16)

#define GICD_TYPER_ID_BITS(typer)   ((((typer) >> 19) & 0x1f) + 1)
#define GICD_TYPER_IRQS(typer)      ((((typer) & 0x1f) + 1) * 32)
#define GICD_TYPER_LPIS         (1U << 17)

#define GICD_IROUTER_SPI_MODE_ONE   (0U << 31)
#define GICD_IROUTER_SPI_MODE_ANY   (1U << 31)
#define IROUTER_IRM_SHIFT   31
#define IROUTER_IRM_MASK    0x1
#define gicd_irouter_val_from_mpidr(mpidr, irm)     \
    (((mpidr) & ~(0xff << 24)) |        \
     (irm & IROUTER_IRM_MASK) << IROUTER_IRM_SHIFT)

#define GIC_PIDR2_ARCH_MASK     0xf0
#define GIC_PIDR2_ARCH_GICv3        0x30
#define GIC_PIDR2_ARCH_GICv4        0x40

#define GIC_V3_DIST_SIZE        0x10000

#define GICD_ENABLE         0x1
#define GICD_DISABLE            0x0
#define GICD_INT_ACTLOW_LVLTRIG     0x0
#define GICD_INT_EN_CLR_X32     0xffffffff
#define GICD_INT_EN_SET_SGI     0x0000ffff
#define GICD_INT_EN_CLR_PPI     0xffff0000
#define GICD_INT_DEF_PRI        0xa0
#define GICD_INT_DEF_PRI_X4     ((GICD_INT_DEF_PRI << 24) |\
                    (GICD_INT_DEF_PRI << 16) |\
                    (GICD_INT_DEF_PRI << 8) |\
                    GICD_INT_DEF_PRI)

/*
 * Re-Distributor registers, offsets from RD_base
 */
#define GIC_RD_CTRL(hw_base)        __REG32((hw_base) + 0x000)
#define GIC_RD_IIDR(hw_base)        __REG32((hw_base) + 0x004)
#define GIC_RD_TYPE(hw_base)        __REG32((hw_base) + 0x008)
#define GIC_RD_STATUSR(hw_base)         __REG32((hw_base) + 0x010)
#define GIC_RD_WAKER(hw_base)           __REG32((hw_base) + 0x014)
#define GIC_RD_SETLPIR(hw_base)         __REG32((hw_base) + 0x040)
#define GIC_RD_CLRLPIR(hw_base)         __REG32((hw_base) + 0x048)
#define GIC_RD_PROPBASER(hw_base)       __REG32((hw_base) + 0x070)
#define GIC_RD_PENDBASER(hw_base)       __REG32((hw_base) + 0x078)
#define GIC_RD_INVLPIR(hw_base)         __REG32((hw_base) + 0x0a0)
#define GIC_RD_INVALLR(hw_base)         __REG32((hw_base) + 0x0b0)
#define GIC_RD_SYNCR(hw_base)           __REG32((hw_base) + 0x0c0)
#define GIC_RD_ICPIDR2(hw_base)         __REG32((hw_base) + 0xffe8)

#define GICR_PCPUBASE_SHIFT 0x11
#define GICR_SGIBASE_OFFSET (1 << 0x10) /* 64 KB */

/*
 * Re-Distributor registers, offsets from SGI_base
 */
#define GIC_RD_IGROUP0(hw_base)     __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0x080)
#define GIC_RD_ENABLE_SET0(hw_base)     __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0x100)
#define GIC_RD_ENABLE_CLEAR0(hw_base)   __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0x180)
#define GIC_RD_PENDING_SET0(hw_base)    __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0x200)
#define GIC_RD_PENDING_CLEAR0(hw_base)  __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0x280)
#define GIC_RD_ACTIVE_SET0(hw_base)     __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0x300)
#define GIC_RD_ACTIVE_CLEAR0(hw_base)   __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0x380)
#define GIC_RD_PRI0(hw_base, n)     __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0x400 +  ((n)/4) * 4)
#define GIC_RD_CONFIG0(hw_base)     __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0xc00)
#define GIC_RD_CONFIG1(hw_base)     __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0xc04)
#define GIC_RD_IGRPMODR0(hw_base)       __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0xd00)
#define GIC_RD_NSACR(hw_base)       __REG32((hw_base) + GICR_SGIBASE_OFFSET + 0xe00)

#define GICR_CTLR_RWP       (1U << 31)
#define GICR_CTLR_ENABLE_LPIS       (1U << 0)

#define TYPER_AFF_VAL_SHIFT 32
#define TYPER_PROC_NUM_SHIFT    8
#define TYPER_LAST_SHIFT    4

#define TYPER_AFF_VAL_MASK  0xffffffff
#define TYPER_PROC_NUM_MASK 0xffff

#define TYPER_LAST_BIT      (1 << TYPER_LAST_SHIFT)
#define mpidr_from_gicr_typer(typer_val)    (((typer_val) >> 32) & 0xffffff)
#define GICR_TYPER_CPU_NUMBER(r)    (((r) >> 8) & 0xffff)

#define GICR_WAKER_ProcessorSleep   (1U << 1)
#define GICR_WAKER_ChildrenAsleep   (1U << 2)

#define GICR_PROPBASER_NonShareable (0U << 10)
#define GICR_PROPBASER_InnerShareable   (1U << 10)
#define GICR_PROPBASER_OuterShareable   (2U << 10)
#define GICR_PROPBASER_SHAREABILITY_MASK (3UL << 10)
#define GICR_PROPBASER_nCnB (0U << 7)
#define GICR_PROPBASER_nC   (1U << 7)
#define GICR_PROPBASER_RaWt (2U << 7)
#define GICR_PROPBASER_RaWb (3U << 7)
#define GICR_PROPBASER_WaWt (4U << 7)
#define GICR_PROPBASER_WaWb (5U << 7)
#define GICR_PROPBASER_RaWaWt       (6U << 7)
#define GICR_PROPBASER_RaWaWb       (7U << 7)
#define GICR_PROPBASER_CACHEABILITY_MASK (7U << 7)
#define GICR_PROPBASER_IDBITS_MASK  (0x1f)

#define GICR_PENDBASER_NonShareable (0U << 10)
#define GICR_PENDBASER_InnerShareable   (1U << 10)
#define GICR_PENDBASER_OuterShareable   (2U << 10)
#define GICR_PENDBASER_SHAREABILITY_MASK (3UL << 10)
#define GICR_PENDBASER_nCnB (0U << 7)
#define GICR_PENDBASER_nC   (1U << 7)
#define GICR_PENDBASER_RaWt (2U << 7)
#define GICR_PENDBASER_RaWb (3U << 7)
#define GICR_PENDBASER_WaWt (4U << 7)
#define GICR_PENDBASER_WaWb (5U << 7)
#define GICR_PENDBASER_RaWaWt       (6U << 7)
#define GICR_PENDBASER_RaWaWb       (7U << 7)
#define GICR_PENDBASER_CACHEABILITY_MASK (7U << 7)

#define LPI_PROP_GROUP1         (1 << 1)
#define LPI_PROP_ENABLED        (1 << 0)

/*
 * ITS registers, offsets from ITS_base
 */
#define GITS_CTLR           0x0000
#define GITS_IIDR           0x0004
#define GITS_TYPER          0x0008
#define GITS_CBASER         0x0080
#define GITS_CWRITER            0x0088
#define GITS_CREADR         0x0090
#define GITS_BASER          0x0100
#define GITS_PIDR2          GICR_PIDR2

#define GITS_TRANSLATER         0x10040

#define GITS_CTLR_ENABLE        (1U << 0)
#define GITS_CTLR_QUIESCENT     (1U << 31)

#define GITS_TYPER_DEVBITS_SHIFT    13
#define GITS_TYPER_DEVBITS(r)       ((((r) >> GITS_TYPER_DEVBITS_SHIFT) & 0x1f) + 1)
#define GITS_TYPER_PTA          (1UL << 19)

#define GITS_CBASER_VALID       (1UL << 63)
#define GITS_CBASER_nCnB        (0UL << 59)
#define GITS_CBASER_nC          (1UL << 59)
#define GITS_CBASER_RaWt        (2UL << 59)
#define GITS_CBASER_RaWb        (3UL << 59)
#define GITS_CBASER_WaWt        (4UL << 59)
#define GITS_CBASER_WaWb        (5UL << 59)
#define GITS_CBASER_RaWaWt      (6UL << 59)
#define GITS_CBASER_RaWaWb      (7UL << 59)
#define GITS_CBASER_CACHEABILITY_MASK   (7UL << 59)
#define GITS_CBASER_NonShareable    (0UL << 10)
#define GITS_CBASER_InnerShareable  (1UL << 10)
#define GITS_CBASER_OuterShareable  (2UL << 10)
#define GITS_CBASER_SHAREABILITY_MASK   (3UL << 10)

#define GITS_BASER_NR_REGS      8

#define GITS_BASER_VALID        (1UL << 63)
#define GITS_BASER_nCnB         (0UL << 59)
#define GITS_BASER_nC           (1UL << 59)
#define GITS_BASER_RaWt         (2UL << 59)
#define GITS_BASER_RaWb         (3UL << 59)
#define GITS_BASER_WaWt         (4UL << 59)
#define GITS_BASER_WaWb         (5UL << 59)
#define GITS_BASER_RaWaWt       (6UL << 59)
#define GITS_BASER_RaWaWb       (7UL << 59)
#define GITS_BASER_CACHEABILITY_MASK    (7UL << 59)
#define GITS_BASER_TYPE_SHIFT       (56)
#define GITS_BASER_TYPE(r)      (((r) >> GITS_BASER_TYPE_SHIFT) & 7)
#define GITS_BASER_ENTRY_SIZE_SHIFT (48)
#define GITS_BASER_ENTRY_SIZE(r)    ((((r) >> GITS_BASER_ENTRY_SIZE_SHIFT) & 0x1f) + 1)
#define GITS_BASER_NonShareable     (0UL << 10)
#define GITS_BASER_InnerShareable   (1UL << 10)
#define GITS_BASER_OuterShareable   (2UL << 10)
#define GITS_BASER_SHAREABILITY_SHIFT   (10)
#define GITS_BASER_SHAREABILITY_MASK    (3UL << GITS_BASER_SHAREABILITY_SHIFT)
#define GITS_BASER_PAGE_SIZE_SHIFT  (8)
#define GITS_BASER_PAGE_SIZE_4K     (0UL << GITS_BASER_PAGE_SIZE_SHIFT)
#define GITS_BASER_PAGE_SIZE_16K    (1UL << GITS_BASER_PAGE_SIZE_SHIFT)
#define GITS_BASER_PAGE_SIZE_64K    (2UL << GITS_BASER_PAGE_SIZE_SHIFT)
#define GITS_BASER_PAGE_SIZE_MASK   (3UL << GITS_BASER_PAGE_SIZE_SHIFT)
#define GITS_BASER_PAGES_MAX        256

#define GITS_BASER_TYPE_NONE        0
#define GITS_BASER_TYPE_DEVICE      1
#define GITS_BASER_TYPE_VCPU        2
#define GITS_BASER_TYPE_CPU     3
#define GITS_BASER_TYPE_COLLECTION  4
#define GITS_BASER_TYPE_RESERVED5   5
#define GITS_BASER_TYPE_RESERVED6   6
#define GITS_BASER_TYPE_RESERVED7   7

/*
 * ITS commands
 */
#define GITS_CMD_MAPD           0x08
#define GITS_CMD_MAPC           0x09
#define GITS_CMD_MAPVI          0x0a
#define GITS_CMD_MOVI           0x01
#define GITS_CMD_DISCARD        0x0f
#define GITS_CMD_INV            0x0c
#define GITS_CMD_MOVALL         0x0e
#define GITS_CMD_INVALL         0x0d
#define GITS_CMD_INT            0x03
#define GITS_CMD_CLEAR          0x04
#define GITS_CMD_SYNC           0x05

/*
 * CPU interface registers
 */
#define GIC_CPU_CTRL(hw_base)       __REG32((hw_base) + 0x00)
#define GIC_CPU_PRIMASK(hw_base)    __REG32((hw_base) + 0x04)
#define GIC_CPU_BINPOINT(hw_base)   __REG32((hw_base) + 0x08)
#define GIC_CPU_INTACK(hw_base)     __REG32((hw_base) + 0x0c)
#define GIC_CPU_EOI(hw_base)        __REG32((hw_base) + 0x10)
#define GIC_CPU_RUNNINGPRI(hw_base) __REG32((hw_base) + 0x14)
#define GIC_CPU_HIGHPRI(hw_base)    __REG32((hw_base) + 0x18)
#define GIC_CPU_ALIAS_BINPOINT(hw_base)     __REG32((hw_base) + 0x1c)
#define GIC_CPU_ACTIVEPRIOI(hw_base)        __REG32((hw_base) + 0xd0)
#define GIC_CPU_IDENT(hw_base)          __REG32((hw_base) + 0xfc)
#define GIC_CPU_DEACTIVATE(hw_base) __REG32((hw_base) + 0x1000)

#define GIC_ICCIAR_INTID_SHIFT     (0)       /* Bits 0-9: Interrupt ID */
#define GIC_ICCIAR_INTID_MASK      (0xffffff << GIC_ICCIAR_INTID_SHIFT)

#define GICC_ENABLE     0x1
#define GICC_INT_PRI_THRESHOLD      0xf0

#define GIC_CPU_CTRL_EOImodeNS      (1 << 9)

#define GICC_IAR_INT_ID_MASK        0x3ff
#define GICC_INT_SPURIOUS   1023
#define GICC_DIS_BYPASS_MASK        0x1e0

#define ICC_CTLR_EL1_EOImode_drop_dir   (0U << 1)
#define ICC_CTLR_EL1_EOImode_drop   (1U << 1)
#define ICC_SRE_EL1_SRE         (1U << 0)

#define ICC_IAR1_EL1_SPURIOUS       0x3ff

#define ICC_SRE_EL2_SRE         (1 << 0)
#define ICC_SRE_EL2_ENABLE      (1 << 3)

#define ICC_SGI1R_TARGET_LIST_SHIFT 0
#define ICC_SGI1R_TARGET_LIST_MASK  (0xffff << ICC_SGI1R_TARGET_LIST_SHIFT)
#define ICC_SGI1R_AFFINITY_1_SHIFT  16
#define ICC_SGI1R_AFFINITY_1_MASK   (0xff << ICC_SGI1R_AFFINITY_1_SHIFT)
#define ICC_SGI1R_SGI_ID_SHIFT      24
#define ICC_SGI1R_SGI_ID_MASK       (0xfULL << ICC_SGI1R_SGI_ID_SHIFT)
#define ICC_SGI1R_AFFINITY_2_SHIFT  32
#define ICC_SGI1R_AFFINITY_2_MASK   (0xffULL << ICC_SGI1R_AFFINITY_1_SHIFT)
#define ICC_SGI1R_IRQ_ROUTING_MODE_BIT  40
#define ICC_SGI1R_AFFINITY_3_SHIFT  48
#define ICC_SGI1R_AFFINITY_3_MASK   (0xffULL << ICC_SGI1R_AFFINITY_1_SHIFT)

#define CTLR_ENABLE_G1NS_SHIFT      1
#define CTLR_ENABLE_G1NS_BIT        (1 << CTLR_ENABLE_G1NS_SHIFT)

#define GIC_PRI_MASK            0xff
#define IGRPEN1_ENABLE_G1_BIT       0x1

#define GIC_HIGHEST_SEC_PRIORITY    0x0

static inline uint64_t readq(void *addr)
{
    uint64_t val = 0;

    __asm__ __volatile__("ldrd r0, r1, [%0]\n"
                         "strd r0, r1, [%1]\n"
                         :
                         : "r"(addr), "r"(&val)
                         : "r0", "r1");

    return val;
}

static inline void writeq(uint64_t val, void *addr)
{
    __asm__ __volatile__("ldrd r0, r1, [%1]\n\t"
                         "strd r0, r1, [%0]\n\t"
                         :
                         : "r"(addr), "r"(&val)
                         : "r0", "r1");
}

static inline void readq_div(void *addr, uint32_t *val0, uint32_t *val1)
{
    __asm__ __volatile__("ldrd r0, r1, [%0]\n"
                         "str r0, [%1]\n"
                         "str r1, [%2]\n"
                         :
                         : "r"(addr), "r"(val0), "r"(val1)
                         : "r0", "r1");
}

static inline void writeq_div(void *addr, uint32_t val0, uint32_t val1)
{
    __asm__ __volatile__("ldr r0, [%1]\n"
                         "ldr r1, [%2]\n"
                         "strd r0, r1, [%0]\n"
                         :
                         : "r"(addr), "r"(&val0), "r"(&val1)
                         : "r0", "r1");
}

/*
 * We need a value to serve as a irq-type for LPIs. Choose one that will
 * hopefully pique the interest of the reviewer.
 */
#define GIC_IRQ_TYPE_LPI        0xa110c8ed

#define _DEFINE_COPROCR_WRITE_FUNC(_name, coproc, opc1, CRn, CRm, opc2) \
static inline void write_## _name(uint32_t v)           \
{                                   \
    __asm__ __volatile__ ("mcr "#coproc","#opc1",%0,"#CRn","#CRm","#opc2 : : "r" (v));\
}

#define _DEFINE_COPROCR_READ_FUNC(_name, coproc, opc1, CRn, CRm, opc2)  \
static inline uint32_t read_ ## _name(void)             \
{                                   \
    uint32_t v;                         \
    __asm__ __volatile__ ("mrc "#coproc","#opc1",%0,"#CRn","#CRm","#opc2 : "=r" (v));\
    return v;                           \
}

#define _DEFINE_COPROCR_WRITE_FUNC_64(_name, coproc, opc1, CRm)     \
static inline void write64_## _name(uint64_t v)             \
{                                   \
    __asm__ __volatile__ ("mcrr "#coproc","#opc1", %Q0, %R0,"#CRm : : "r" (v));\
}

#define _DEFINE_COPROCR_READ_FUNC_64(_name, coproc, opc1, CRm)      \
static inline uint64_t read64_## _name(void)                \
{   uint64_t v;                         \
    __asm__ __volatile__ ("mrrc "#coproc","#opc1", %Q0, %R0,"#CRm : "=r" (v));\
    return v;                           \
}

/* Define read function for coproc register */
#define DEFINE_COPROCR_READ_FUNC(_name, ...)                \
    _DEFINE_COPROCR_READ_FUNC(_name, __VA_ARGS__)

#define DEFINE_COPROCR_WRITE_FUNC(_name, ...)               \
    _DEFINE_COPROCR_WRITE_FUNC(_name, __VA_ARGS__)

/* Define read & write function for coproc register */
#define DEFINE_COPROCR_RW_FUNCS(_name, ...)                 \
    _DEFINE_COPROCR_READ_FUNC(_name, __VA_ARGS__)           \
    _DEFINE_COPROCR_WRITE_FUNC(_name, __VA_ARGS__)

/* Define 64 bit read function for coproc register */
#define DEFINE_COPROCR_READ_FUNC_64(_name, ...)             \
    _DEFINE_COPROCR_READ_FUNC_64(_name, __VA_ARGS__)

/* Define 64 bit read function for coproc register */
#define DEFINE_COPROCR_WRITE_FUNC_64(_name, ...)            \
    _DEFINE_COPROCR_WRITE_FUNC_64(_name, __VA_ARGS__)

/* Define 64 bit read & write function for coproc register */
#define DEFINE_COPROCR_RW_FUNCS_64(_name, ...)              \
    _DEFINE_COPROCR_READ_FUNC_64(_name, __VA_ARGS__)        \
    _DEFINE_COPROCR_WRITE_FUNC_64(_name, __VA_ARGS__)

#define __ACCESS_CP15(Op1, CRn, CRm, Op2)   p15, Op1, CRn, CRm, Op2
#define __ACCESS_CP15_64(Op1, CRm)      p15, Op1, CRm

#define ICC_EOIR1           __ACCESS_CP15(0, c12, c12, 1)
#define ICC_DIR             __ACCESS_CP15(0, c12, c11, 1)
#define ICC_IAR1            __ACCESS_CP15(0, c12, c12, 0)
#define ICC_BPR1            __ACCESS_CP15(0, c12, c12, 3)
#define ICC_SGI1R           __ACCESS_CP15_64(0, c12)
#define ICC_PMR             __ACCESS_CP15(0, c4, c6, 0)
#define ICC_CTLR            __ACCESS_CP15(0, c12, c12, 4)
#define ICC_SRE             __ACCESS_CP15(0, c12, c12, 5)
#define ICC_IGRPEN1         __ACCESS_CP15(0, c12, c12, 7)


DEFINE_COPROCR_READ_FUNC(icc_iar1, ICC_IAR1)

DEFINE_COPROCR_WRITE_FUNC(icc_eoir1, ICC_EOIR1)

DEFINE_COPROCR_WRITE_FUNC(icc_dir, ICC_DIR)

DEFINE_COPROCR_RW_FUNCS(icc_ctlr, ICC_CTLR)

DEFINE_COPROCR_RW_FUNCS(icc_sre, ICC_SRE)

DEFINE_COPROCR_RW_FUNCS(icc_bpr1, ICC_BPR1)

DEFINE_COPROCR_RW_FUNCS(icc_pmr, ICC_PMR);

DEFINE_COPROCR_RW_FUNCS(icc_igrpen1, ICC_IGRPEN1)

DEFINE_COPROCR_WRITE_FUNC_64(icc_sgi1r, ICC_SGI1R)

#endif
