/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 * Copyright (c) 2019-Present Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020/03/26     Huaqi        Nuclei RISC-V Core porting code.
 */

#include <rthw.h>
#include <rtthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "cpuport.h"

#define SYSTICK_TICK_CONST                      (SOC_TIMER_FREQ / RT_TICK_PER_SECOND)

/* Interrupt level for kernel systimer interrupt and software timer interrupt */
#define RT_KERNEL_INTERRUPT_LEVEL               0

/* Initial CSR MSTATUS value when thread created */
#define RT_INITIAL_MSTATUS                      (MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS_INITIAL)

/**
 * @brief from thread used interrupt context switch
 *
 */
volatile rt_ubase_t  rt_interrupt_from_thread = 0;
/**
 * @brief to thread used interrupt context switch
 *
 */
volatile rt_ubase_t  rt_interrupt_to_thread   = 0;
/**
 * @brief flag to indicate context switch in interrupt or not
 *
 */
volatile rt_ubase_t rt_thread_switch_interrupt_flag = 0;

/**
 * @brief thread stack frame of saved context
 *
 */
struct rt_hw_stack_frame
{
    rt_ubase_t epc;        /*!< epc - epc    - program counter                     */
    rt_ubase_t ra;         /*!< x1  - ra     - return address for jumps            */
    rt_ubase_t t0;         /*!< x5  - t0     - temporary register 0                */
    rt_ubase_t t1;         /*!< x6  - t1     - temporary register 1                */
    rt_ubase_t t2;         /*!< x7  - t2     - temporary register 2                */
    rt_ubase_t s0_fp;      /*!< x8  - s0/fp  - saved register 0 or frame pointer   */
    rt_ubase_t s1;         /*!< x9  - s1     - saved register 1                    */
    rt_ubase_t a0;         /*!< x10 - a0     - return value or function argument 0 */
    rt_ubase_t a1;         /*!< x11 - a1     - return value or function argument 1 */
    rt_ubase_t a2;         /*!< x12 - a2     - function argument 2                 */
    rt_ubase_t a3;         /*!< x13 - a3     - function argument 3                 */
    rt_ubase_t a4;         /*!< x14 - a4     - function argument 4                 */
    rt_ubase_t a5;         /*!< x15 - a5     - function argument 5                 */
#ifndef __riscv_32e
    rt_ubase_t a6;         /*!< x16 - a6     - function argument 6                 */
    rt_ubase_t a7;         /*!< x17 - s7     - function argument 7                 */
    rt_ubase_t s2;         /*!< x18 - s2     - saved register 2                    */
    rt_ubase_t s3;         /*!< x19 - s3     - saved register 3                    */
    rt_ubase_t s4;         /*!< x20 - s4     - saved register 4                    */
    rt_ubase_t s5;         /*!< x21 - s5     - saved register 5                    */
    rt_ubase_t s6;         /*!< x22 - s6     - saved register 6                    */
    rt_ubase_t s7;         /*!< x23 - s7     - saved register 7                    */
    rt_ubase_t s8;         /*!< x24 - s8     - saved register 8                    */
    rt_ubase_t s9;         /*!< x25 - s9     - saved register 9                    */
    rt_ubase_t s10;        /*!< x26 - s10    - saved register 10                   */
    rt_ubase_t s11;        /*!< x27 - s11    - saved register 11                   */
    rt_ubase_t t3;         /*!< x28 - t3     - temporary register 3                */
    rt_ubase_t t4;         /*!< x29 - t4     - temporary register 4                */
    rt_ubase_t t5;         /*!< x30 - t5     - temporary register 5                */
    rt_ubase_t t6;         /*!< x31 - t6     - temporary register 6                */
#endif
    rt_ubase_t mstatus;    /*!<              - machine status register             */
};

/**
 * This function will initialize thread stack
 *
 * @param tentry the entry of thread
 * @param parameter the parameter of entry
 * @param stack_addr the beginning stack address
 * @param texit the function will be called when thread exit
 *
 * @return stack address
 */
rt_uint8_t *rt_hw_stack_init(void       *tentry,
                             void       *parameter,
                             rt_uint8_t *stack_addr,
                             void       *texit)
{
    struct rt_hw_stack_frame *frame;
    rt_uint8_t         *stk;
    int                i;

    stk  = stack_addr + sizeof(rt_ubase_t);
    stk  = (rt_uint8_t *)RT_ALIGN_DOWN((rt_ubase_t)stk, REGBYTES);
    stk -= sizeof(struct rt_hw_stack_frame);

    frame = (struct rt_hw_stack_frame *)stk;

    for (i = 0; i < sizeof(struct rt_hw_stack_frame) / sizeof(rt_ubase_t); i++)
    {
        ((rt_ubase_t *)frame)[i] = 0xdeadbeef;
    }

    frame->ra      = (rt_ubase_t)texit;
    frame->a0      = (rt_ubase_t)parameter;
    frame->epc     = (rt_ubase_t)tentry;

    frame->mstatus = RT_INITIAL_MSTATUS;

    return stk;
}

/**
 * @brief Do rt-thread context switch in interrupt context
 *
 * @param from thread sp of from thread
 * @param to thread sp of to thread
 */
void rt_hw_context_switch_interrupt(rt_ubase_t from, rt_ubase_t to)
{
    if (rt_thread_switch_interrupt_flag == 0)
        rt_interrupt_from_thread = from;

    rt_interrupt_to_thread = to;
    rt_thread_switch_interrupt_flag = 1;
    RT_YIELD();
}

/**
 * @brief Do rt-thread context switch in task context
 *
 * @param from thread sp of from thread
 * @param to thread sp of to thread
 */
void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to)
{
    rt_hw_context_switch_interrupt(from, to);
}

/**
 * @brief shutdown CPU
 *
 */
RT_WEAK void rt_hw_cpu_shutdown()
{
    rt_base_t level;
    rt_kprintf("shutdown...\n");

    level = rt_hw_interrupt_disable();
    while (level)
    {
        RT_ASSERT(0);
    }
}

/**
 * @brief Do extra task switch code
 *
 * @details
 *
 * - Clear software timer interrupt request flag
 * - clear rt_thread_switch_interrupt_flag to 0
 */
void rt_hw_taskswitch(void)
{
    /* Clear Software IRQ, A MUST */
    SysTimer_ClearSWIRQ();
    rt_thread_switch_interrupt_flag = 0;
}

/**
 * @brief Setup systimer and software timer interrupt
 *
 * @details
 *
 * - Set Systimer interrupt as NON-VECTOR interrupt with lowest interrupt level
 * - Set software timer interrupt as VECTOR interrupt with lowest interrupt level
 * - Enable these two interrupts
 */
void rt_hw_ticksetup(void)
{
    uint64_t ticks = SYSTICK_TICK_CONST;

    /* Make SWI and SysTick the lowest priority interrupts. */
    /* Stop and clear the SysTimer. SysTimer as Non-Vector Interrupt */
    SysTick_Config(ticks);
    ECLIC_DisableIRQ(SysTimer_IRQn);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, RT_KERNEL_INTERRUPT_LEVEL);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_EnableIRQ(SysTimer_IRQn);

    /* Set SWI interrupt level to lowest level/priority, SysTimerSW as Vector Interrupt */
    ECLIC_SetShvIRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimerSW_IRQn, RT_KERNEL_INTERRUPT_LEVEL);
    ECLIC_EnableIRQ(SysTimerSW_IRQn);
}

/**
 * systimer interrupt handler eclic_mtip_handler
 * is hard coded in startup_<Device>.S
 * We define SysTick_Handler as eclic_mtip_handler
 * for easy understanding
 */
#define SysTick_Handler     eclic_mtip_handler

/**
 * @brief This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* Reload systimer */
    SysTick_Reload(SYSTICK_TICK_CONST);

    /* enter interrupt */
    rt_interrupt_enter();

    /* tick increase */
    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * @brief Disable cpu interrupt
 *
 * @details
 *
 * - Disable cpu interrupt by clear MIE bit in MSTATUS
 * - Return the previous value in MSTATUS before clear MIE bit
 *
 * @return the previous value in MSTATUS before clear MIE bit
 */
rt_base_t rt_hw_interrupt_disable(void)
{
    return __RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
}

/**
 * @brief Restore previous saved interrupt status
 *
 * @param level previous saved MSTATUS value
 */
void rt_hw_interrupt_enable(rt_base_t level)
{
    __RV_CSR_WRITE(CSR_MSTATUS, level);
}

static struct rt_irq_desc irq_desc[NUM_INTERRUPTS];
#if defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1)
/**
 * \brief  Initialize a specific IRQ and register the handler
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        NMI interrupt handler address
 * \param [in]  shv         \ref ECLIC_NON_VECTOR_INTERRUPT means non-vector mode, and \ref ECLIC_VECTOR_INTERRUPT is vector mode
 * \param [in]  trig_mode   see \ref ECLIC_TRIGGER_Type
 * \param [in]  lvl         interupt level
 * \param [in]  priority    interrupt priority
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific eclic interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section(FLASHXIP mode), handler could not be installed
 */
static int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void* handler)
{
    if ((IRQn > NUM_INTERRUPTS) || (shv > ECLIC_VECTOR_INTERRUPT) \
        || (trig_mode > ECLIC_NEGTIVE_EDGE_TRIGGER)) {
        return -1;
    }

    /* set interrupt vector mode */
    ECLIC_SetShvIRQ(IRQn, shv);
    /* set interrupt trigger mode and polarity */
    ECLIC_SetTrigIRQ(IRQn, trig_mode);
    /* set interrupt level */
    ECLIC_SetLevelIRQ(IRQn, lvl);
    /* set interrupt priority */
    ECLIC_SetPriorityIRQ(IRQn, priority);
    if ((handler != NULL) && (shv != ECLIC_NON_VECTOR_INTERRUPT)) {
        /* set interrupt handler entry to vector table */
        ECLIC_SetVector(IRQn, (rv_csr_t)handler);
    }
    /* enable interrupt */
    ECLIC_EnableIRQ(IRQn);
    return 0;
}
#endif

#if (defined(__TEE_PRESENT) && (__TEE_PRESENT == 1)) && (defined(__ECLIC_PRESENT) && (__ECLIC_PRESENT == 1))
/**
 * \brief  Initialize a specific IRQ and register the handler for supervisor mode
 * \details
 * This function set vector mode, trigger mode and polarity, interrupt level and priority,
 * assign handler for specific IRQn.
 * \param [in]  IRQn        NMI interrupt handler address
 * \param [in]  shv         \ref ECLIC_NON_VECTOR_INTERRUPT means non-vector mode, and \ref ECLIC_VECTOR_INTERRUPT is vector mode
 * \param [in]  trig_mode   see \ref ECLIC_TRIGGER_Type
 * \param [in]  lvl         interupt level
 * \param [in]  priority    interrupt priority
 * \param [in]  handler     interrupt handler, if NULL, handler will not be installed
 * \return       -1 means invalid input parameter. 0 means successful.
 * \remarks
 * - This function use to configure specific eclic S-mode interrupt and register its interrupt handler and enable its interrupt.
 * - If the vector table is placed in read-only section (FLASHXIP mode), handler could not be installed.
 */
static int32_t ECLIC_Register_IRQ_S(IRQn_Type IRQn, uint8_t shv, ECLIC_TRIGGER_Type trig_mode, uint8_t lvl, uint8_t priority, void* handler)
{
    if ((IRQn > NUM_INTERRUPTS) || (shv > ECLIC_VECTOR_INTERRUPT) \
        || (trig_mode > ECLIC_NEGTIVE_EDGE_TRIGGER)) {
        return -1;
    }

    /* set interrupt vector mode */
    ECLIC_SetShvIRQ_S(IRQn, shv);
    /* set interrupt trigger mode and polarity */
    ECLIC_SetTrigIRQ_S(IRQn, trig_mode);
    /* set interrupt level */
    ECLIC_SetLevelIRQ_S(IRQn, lvl);
    /* set interrupt priority */
    ECLIC_SetPriorityIRQ_S(IRQn, priority);
    if (handler != NULL) {
        /* set interrupt handler entry to vector table */
        ECLIC_SetVector_S(IRQn, (rv_csr_t)handler);
    }
    /* enable interrupt */
    ECLIC_EnableIRQ_S(IRQn);
    return 0;
}
#endif

/**
 * This function will mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_mask(int vector)
{
    ECLIC_DisableIRQ(vector);
}

/**
 * This function will un-mask a interrupt.
 * @param vector the interrupt number
 */
void rt_hw_interrupt_umask(int vector)
{
    ECLIC_EnableIRQ(vector);
}

/**
 * This function will install a interrupt service routine to a interrupt.
 * @param vector the interrupt number
 * @param handler the interrupt service routine to be installed
 * @param param the parameter for interrupt service routine
 * @param name the interrupt name
 *
 * @return the old handler
 */
rt_isr_handler_t rt_hw_interrupt_install(int vector, rt_isr_handler_t handler,
        void *param, const char *name)
{
    rt_isr_handler_t old_handler = RT_NULL;

    old_handler = irq_desc[vector].handler;
    if (handler != RT_NULL) {
        ECLIC_Register_IRQ(vector, ECLIC_NON_VECTOR_INTERRUPT,
                           ECLIC_LEVEL_TRIGGER, INTLEVEL, INTPRIORITY, (void*)handler);
        irq_desc[vector].handler = (rt_isr_handler_t)handler;
        irq_desc[vector].param = param;
#ifdef RT_USING_INTERRUPT_INFO
        irq_desc[vector].counter = 0;
#endif
    }

    return old_handler;
}

/**
 * This function will handle the eclic interrupt.
 * @param vector the interrupt number
 */
static void eclic_irq_handle(unsigned int i_mcause)
{
    rt_isr_handler_t isr_func;
    rt_uint32_t irq;
    void *param;

    irq = i_mcause & 0xfff;
    isr_func = irq_desc[irq].handler;
    param = irq_desc[irq].param;
    isr_func(irq, param);
#ifdef RT_USING_INTERRUPT_INFO
    irq_desc[irq].counter ++;
#endif
}

/**
 * This function will handle eclic interrupt trap.
 * @param i_mcause the mcause
 * @param i_mepc the m-mode return pc
 * @param i_sp the sp
 */
void handle_trap(unsigned int i_mcause, unsigned int i_mepc, unsigned int i_sp)
{
    /*
     * The Bit 31: 0: Exception or NMI; 1: Eclic Interrupts
     * The handle_trap is called by irq_entry which handle
     * non-vector Eclic interrupts.
     * The vector_base handle vector Eclic Interrupts.
     * The exc_entry handle Exception or NMI.
     */
    if ((i_mcause & 0x80000000) == 0x80000000) {
        if ((i_mcause & 0xfff) <= 18 ) {
            SysTick_Handler();
        } else {
            eclic_irq_handle(i_mcause);
        }
    }
}
