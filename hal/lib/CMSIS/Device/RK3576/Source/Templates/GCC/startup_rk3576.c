/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 Rockchip Electronics Co., Ltd.
 */

#include "hal_base.h"

#ifdef HAL_AP_CORE

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Vectors(void) __attribute__ ((section("RESET")));
void Reset_Handler(void);
void Default_Handler(void);
void IRQ_Handler(void);
void IRQ_HardIrqHandler(void);
void IRQ_HardIrqPreemptHandler(void);

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
void Undef_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void PAbt_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void DAbt_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
void FIQ_Handler(void) __attribute__ ((weak, alias("Default_Handler"))) HAL_VISIBLE;

#ifndef HAL_GIC_MODULE_ENABLED
void IRQ_Handler(void) __attribute__ ((weak, alias("Default_Handler")));
#else
#ifdef HAL_GIC_PREEMPT_FEATURE_ENABLED
HAL_VISIBLE void IRQ_HardIrqPreemptHandler(void)
{
    uint32_t irqn;

    irqn = HAL_GIC_GetActiveIRQ();

    if ((irqn >= 1020 && irqn <= 1023)) {
        return;
    }

    HAL_GIC_TouchICC();

#ifdef HAL_GPIO_IRQ_GROUP_MODULE_ENABLED
    HAL_GPIO_IRQ_GROUP_DispatchGIRQs(irqn);
#endif

    __enable_irq();

    HAL_IRQ_HANDLER_IRQHandler(irqn);

    __disable_irq();

    HAL_GIC_EndOfInterrupt(irqn);
}
#else
HAL_VISIBLE void IRQ_HardIrqHandler(void)
{
    uint32_t irqn;

    irqn = HAL_GIC_GetActiveIRQ();

    if (irqn >= HAL_GIC_IRQ_LINE_COUNT) {
        return;
    }

    if (irqn <= 15) {
        HAL_GIC_EndOfInterrupt(irqn);
    }

    HAL_IRQ_HANDLER_IRQHandler(irqn);

    if (irqn > 15) {
        HAL_GIC_EndOfInterrupt(irqn);
    }
}
#endif

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
#pragma GCC push_options
#pragma GCC optimize ("-fomit-frame-pointer")
#elif defined(__ARMCC_VERSION)
#pragma push
#pragma-fomit-frame-pointer
#endif

#ifdef __aarch64__
HAL_VISIBLE void IRQ_Handler(void)
{
#ifdef HAL_GIC_PREEMPT_FEATURE_ENABLED
    __ASM volatile (
        "stp    x0, x1, [sp, #-16]!           \n" //save x0~x30
        "stp    x2, x3, [sp, #-16]!           \n"
        "stp    x4, x5, [sp, #-16]!           \n"
        "stp    x6, x7, [sp, #-16]!           \n"
        "stp    x8, x9, [sp, #-16]!           \n"
        "stp    x10, x11, [sp, #-16]!         \n"
        "stp    x12, x13, [sp, #-16]!         \n"
        "stp    x14, x15, [sp, #-16]!         \n"
        "stp    x16, x17, [sp, #-16]!         \n"
        "stp    x18, x19, [sp, #-16]!         \n"
        "stp    x20, x21, [sp, #-16]!         \n"
        "stp    x22, x23, [sp, #-16]!         \n"
        "stp    x24, x25, [sp, #-16]!         \n"
        "stp    x26, x27, [sp, #-16]!         \n"
        "stp    x28, x29, [sp, #-16]!         \n"
        "stp    x30, xzr, [sp, #-16]!         \n"
        "mrs    x0, elr_el1                   \n"
        "mrs    x1, spsr_el1                  \n"
        "stp    x0, x1, [sp, #-16]!           \n" //save elr_el1,spsr_el1
#if (__FPU_USED == 1)
        "mrs    x28, fpcr                     \n" //save fpu
        "mrs    x29, fpsr                     \n"
        "stp    x28, x29, [sp, #-16]!         \n"
        "str    q0, [sp, #-16]!               \n" //save q0~q15
        "str    q1, [sp, #-16]!               \n"
        "str    q2, [sp, #-16]!               \n"
        "str    q3, [sp, #-16]!               \n"
        "str    q4, [sp, #-16]!               \n"
        "str    q5, [sp, #-16]!               \n"
        "str    q6, [sp, #-16]!               \n"
        "str    q7, [sp, #-16]!               \n"
        "str    q8, [sp, #-16]!               \n"
        "str    q9, [sp, #-16]!               \n"
        "str    q10, [sp, #-16]!              \n"
        "str    q11, [sp, #-16]!              \n"
        "str    q12, [sp, #-16]!              \n"
        "str    q13, [sp, #-16]!              \n"
        "str    q14, [sp, #-16]!              \n"
        "str    q15, [sp, #-16]!              \n"
#endif
        "bl     IRQ_HardIrqPreemptHandler     \n"
#if (__FPU_USED == 1)
        "ldr    q15, [sp], #16                \n" //restore q0~q15
        "ldr    q14, [sp], #16                \n"
        "ldr    q13, [sp], #16                \n"
        "ldr    q12, [sp], #16                \n"
        "ldr    q11, [sp], #16                \n"
        "ldr    q10, [sp], #16                \n"
        "ldr    q9, [sp], #16                 \n"
        "ldr    q8, [sp], #16                 \n"
        "ldr    q7, [sp], #16                 \n"
        "ldr    q6, [sp], #16                 \n"
        "ldr    q5, [sp], #16                 \n"
        "ldr    q4, [sp], #16                 \n"
        "ldr    q3, [sp], #16                 \n"
        "ldr    q2, [sp], #16                 \n"
        "ldr    q1, [sp], #16                 \n"
        "ldr    q0, [sp], #16                 \n"
        "ldp    x28, x29, [sp], #16           \n" //restore fpu
        "msr    fpcr, x28                     \n"
        "msr    fpsr, x29                     \n"
#endif
        "ldp    x0, x1, [sp], #16             \n" //restore elr_el1,spsr_el1
        "msr    elr_el1, x0                   \n"
        "msr    spsr_el1, x1                  \n"
        "ldp    x30, xzr, [sp], #16           \n"
        "ldp    x28, x29, [sp], #16           \n"
        "ldp    x26, x27, [sp], #16           \n"
        "ldp    x24, x25, [sp], #16           \n"
        "ldp    x22, x23, [sp], #16           \n"
        "ldp    x20, x21, [sp], #16           \n"
        "ldp    x18, x19, [sp], #16           \n"
        "ldp    x16, x17, [sp], #16           \n"
        "ldp    x14, x15, [sp], #16           \n"
        "ldp    x12, x13, [sp], #16           \n"
        "ldp    x10, x11, [sp], #16           \n"
        "ldp    x8, x9, [sp], #16             \n"
        "ldp    x6, x7, [sp], #16             \n"
        "ldp    x4, x5, [sp], #16             \n"
        "ldp    x2, x3, [sp], #16             \n"
        "ldp    x0, x1, [sp], #16             \n"
        "eret                                 \n"
        );
#else
    __ASM volatile (
        "stp     x0, x1, [sp, #-16]!                      \n" // save x0~x30
        "stp     x2, x3, [sp, #-16]!                      \n"
        "stp     x4, x5, [sp, #-16]!                      \n"
        "stp     x6, x7, [sp, #-16]!                      \n"
        "stp     x8, x9, [sp, #-16]!                      \n"
        "stp     x10, x11, [sp, #-16]!                    \n"
        "stp     x12, x13, [sp, #-16]!                    \n"
        "stp     x14, x15, [sp, #-16]!                    \n"
        "stp     x16, x17, [sp, #-16]!                    \n"
        "stp     x18, x19, [sp, #-16]!                    \n"
        "stp     x20, x21, [sp, #-16]!                    \n"
        "stp     x22, x23, [sp, #-16]!                    \n"
        "stp     x24, x25, [sp, #-16]!                    \n"
        "stp     x26, x27, [sp, #-16]!                    \n"
        "stp     x28, x29, [sp, #-16]!                    \n"
        "stp     x30, xzr, [sp, #-16]!                    \n"
        "bl      IRQ_HardIrqHandler                       \n"
        "ldp     x30, xzr, [sp], #16                      \n" // restore x0~x30
        "ldp     x28, x29, [sp], #16                      \n"
        "ldp     x26, x27, [sp], #16                      \n"
        "ldp     x24, x25, [sp], #16                      \n"
        "ldp     x22, x23, [sp], #16                      \n"
        "ldp     x20, x21, [sp], #16                      \n"
        "ldp     x18, x19, [sp], #16                      \n"
        "ldp     x16, x17, [sp], #16                      \n"
        "ldp     x14, x15, [sp], #16                      \n"
        "ldp     x12, x13, [sp], #16                      \n"
        "ldp     x10, x11, [sp], #16                      \n"
        "ldp     x8, x9, [sp], #16                        \n"
        "ldp     x6, x7, [sp], #16                        \n"
        "ldp     x4, x5, [sp], #16                        \n"
        "ldp     x2, x3, [sp], #16                        \n"
        "ldp     x0, x1, [sp], #16                        \n"
        "eret                                             \n"
        );
#endif /* HAL_GIC_PREEMPT_FEATURE_ENABLED */
}
#else
HAL_VISIBLE void IRQ_Handler(void)
{
#ifdef HAL_GIC_PREEMPT_FEATURE_ENABLED
    __ASM volatile (
        "sub    lr, lr, #4                                \n"
        "stm    sp, {r0, lr}                              \n" // save r0, lr, spsr
        "mrs    lr, SPSR                                  \n"
        "str    lr, [sp, #8]                              \n"
        "mrs    r0, CPSR                                  \n"
        "eor    r0, r0, #1                                \n"
        "msr    SPSR_fsxc, r0                             \n" // set spsr to svc mode
        "mov    r0, sp                                    \n"
        "ldr    lr, =hw_irq                               \n"
        "movs   pc, lr                                    \n" // switch to svc mode
        "hw_irq:                                          \n"
        "sub    sp, sp, #72                               \n"
        "stmia  sp, {r1 - r12}                            \n" // save the cpu context
        "ldmia  r0, {r3 - r5}                             \n"
        "add    r7, sp, #48                               \n"
        "add    r2, sp, #72                               \n"
        "str    r3, [sp, #-4]!                            \n"
        "mov    r3, lr                                    \n"
        "stmia  r7, {r2 - r5}                             \n"
        "mrc    p15, 0, r6, c12, c12, 0                   \n" // get the irq id
        "cmp    r6, #1020                                 \n"
        "bhs    IRQ_HandlerEnd                            \n"
        "vmrs   r8, fpexc                                 \n"
        "tst    r8, #(1<<30)                              \n"
        "beq    1f                                        \n"
        "vstmdb sp!, {d0-d15}                             \n" // save fpu context
        "vstmdb sp!, {d16-d31}                            \n"
        "vmrs   r9, fpscr                                 \n"
        "stmfd  sp!, {r9}                                 \n"
        "1:                                               \n"
        "stmfd  sp!, {r8}                                 \n"
        "mrc    p15, 0, r2, c12, c11, 3                   \n" // get the running priority
        "mrc    p15, 0, r7, c4, c6, 0                     \n" // get the interrupt priority mask
        "mcr    p15, 0, r2, c4, c6, 0                     \n" // set the interrupt priority mask
        "mcr    p15, 0, r6, c12, c12, 1                   \n" // end of interrupt
        "cmp    r6, #16                                   \n"
        "bhs    2f                                        \n"
        "mcr    p15, 0, r6, c12, c11, 1                   \n" // deactivate sgi interrupt
        "2:                                               \n"
        "mov    r0, r6                                    \n"
        "bl     IRQ_HardIrqPreemptHandler                 \n"
        "ldmfd  sp!, {r8}                                 \n"
        "vmsr   fpexc, r8                                 \n"
        "tst    r8, #(1<<30)                              \n"
        "beq    1f                                        \n"
        "ldmfd  sp!, {r9}                                 \n"
        "vmsr   fpscr, r9                                 \n"
        "vldmia sp!, {d16-d31}                            \n"
        "vldmia sp!, {d0-d15}                             \n"
        "1:                                               \n"
        "cpsid  i                                         \n"
        "cmp    r6, #16                                   \n"
        "blo    2f                                        \n" // for sgi bypass deactivate interrupt
        "mcr    p15, 0, r6, c12, c11, 1                   \n" // deactivate interrupt
        "2:                                               \n"
        "mcr    p15, 0, r7, c4, c6, 0                     \n" // set the interrupt priority mask
        "IRQ_HandlerEnd:                                  \n"
        "msr    spsr_cxsf, r5                             \n" // restore the spsr
        "ldmia  sp, {r0 - pc}^                            \n" // restore the CPU context then exit irq handler
        );
#else
    __ASM volatile (
        "stmfd  sp!, {r0-r12,lr}                          \n" // save the CPU context
        "bl     IRQ_HardIrqHandler                        \n"
        "ldmfd  sp!, {r0-r12,lr}                          \n" // restore the CPU context
        "subs   pc,  lr, #4                               \n"
        );
#endif /* HAL_GIC_PREEMPT_FEATURE_ENABLED */
}
#endif /* __aarch64__ */

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
#pragma GCC pop_options
#elif defined(__ARMCC_VERSION)
#pragma pop
#endif

#endif /* HAL_GIC_MODULE_ENABLED */

#ifdef __aarch64__
#define vector_table_align .align 11    /* Vector tables must be placed at a 2KB-aligned address */
#define vector_entry_align .align 7     /* Each entry is 128B */

void Sync_Handler(void) __attribute__ ((weak, alias("Default_Handler"))) HAL_VISIBLE;
void SError_Handler(void) __attribute__ ((weak, alias("Default_Handler"))) HAL_VISIBLE;

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector Table
 *----------------------------------------------------------------------------*/
HAL_VISIBLE void Vectors(void)
{
    __ASM volatile (
        ".align 11                                        \n"
        // Current EL with SP0
        ".align 7                                         \n"
        "B      Reset_Handler                             \n" // Synchronous
        ".align 7                                         \n"
        "B      IRQ_Handler                               \n" // IRQ/vIRQ
        ".align 7                                         \n"
        "B      FIQ_Handler                               \n" // FIQ/vFIQ
        ".align 7                                         \n"
        "B      SError_Handler                            \n" // SError

        // Current EL with SPx
        ".align 7                                         \n"
        "B      Sync_Handler                              \n" // Synchronous
        ".align 7                                         \n"
        "B      IRQ_Handler                               \n" // IRQ/vIRQ
        ".align 7                                         \n"
        "B      FIQ_Handler                               \n" // FIQ/vFIQ
        ".align 7                                         \n"
        "B      SError_Handler                            \n" // SError

        // Lower EL using AArch64
        ".align 7                                         \n"
        "B      .                                         \n" // Synchronous
        ".align 7                                         \n"
        "B      .                                         \n" // IRQ/vIRQ
        ".align 7                                         \n"
        "B      .                                         \n" // FIQ/vFIQ
        ".align 7                                         \n"
        "B      .                                         \n" // SError

        // Lower EL using AArch32
        ".align 7                                         \n"
        "B      .                                         \n" // Synchronous
        ".align 7                                         \n"
        "B      .                                         \n" // IRQ/vIRQ
        ".align 7                                         \n"
        "B      .                                         \n" // FIQ/vFIQ
        ".align 7                                         \n"
        "B      .                                         \n" // SError
        );
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
HAL_VISIBLE void Reset_Handler(void)
{
    __ASM volatile (

        // Mask interrupts
        "MSR     DAIFSet, #3                             \n"

        // Check exception level, only support EL1
        "MRS     x0, CurrentEL                          \n"
        "AND     x0, x0, #(3 << 2)                      \n"
        "CMP     x0, #(1 << 2)                          \n"
        "BNE     hang                                   \n"

        //Reset SCTLR Settings
        "MRS     x0, SCTLR_EL1                           \n" // Read SCTLR_EL1 System Control register
        "BIC     x0, x0, #(1 << 12)                      \n" // Clear I bit 12 to disable I Cache
        "BIC     x0, x0, #(1 << 2)                       \n" // Clear C bit 2 to disable D Cache
        "BIC     x0, x0, #1                              \n" // Clear M bit 0 to disable MMU
        "BIC     x0, x0, #(1 << 11)                      \n" // Clear Z bit 11 to disable branch prediction
        "BIC     x0, x0, #(1 << 13)                      \n" // Clear V bit 13 to disable hives
        "MSR     SCTLR_EL1, x0                           \n" // Write value back to SCTLR_EL1 System Control register
        "ISB                                             \n" // Ensure changes take effect

        // Set Vector Base Address Register (VBAR) to point to this application's vector table
        "LDR     x0, =Vectors                            \n"
        "MSR     VBAR_EL1, x0                            \n"

        // Setup Stack for EL1
        "LDR     x1, =__StackTop                         \n"
        "MOV     SP, x1                                  \n"

        // Call DataInit
        "BL     DataInit                                 \n"

        // Call SystemInit
        "BL     SystemInit                               \n"

        // Unmask interrupts
        "MSR    DAIFClr, #3                              \n"

        // Call _start
        "BL     _start                                   \n"

        // Hang
        "hang:                                           \n"
        "WFI                                             \n"
        "B      hang                                     \n"
        );
}
#else
/*----------------------------------------------------------------------------
  Exception / Interrupt Vector Table
 *----------------------------------------------------------------------------*/
HAL_VISIBLE void Vectors(void)
{
    __ASM volatile (
        "LDR    PC, =Reset_Handler                        \n"
        "LDR    PC, =Undef_Handler                        \n"
        "LDR    PC, =SVC_Handler                          \n"
        "LDR    PC, =PAbt_Handler                         \n"
        "LDR    PC, =DAbt_Handler                         \n"
        "NOP                                              \n"
        "LDR    PC, =IRQ_Handler                          \n"
        "LDR    PC, =FIQ_Handler                          \n"
        );
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
HAL_VISIBLE void Reset_Handler(void)
{
    __ASM volatile (

        // Mask interrupts
        "CPSID   if                                      \n"

        // Reset SCTLR Settings
        "MRC     p15, 0, R0, c1, c0, 0                   \n" // Read CP15 System Control register
        "BIC     R0, R0, #(0x1 << 12)                    \n" // Clear I bit 12 to disable I Cache
        "BIC     R0, R0, #(0x1 <<  2)                    \n" // Clear C bit  2 to disable D Cache
        "BIC     R0, R0, #0x1                            \n" // Clear M bit  0 to disable MMU
        "BIC     R0, R0, #(0x1 << 11)                    \n" // Clear Z bit 11 to disable branch prediction
        "BIC     R0, R0, #(0x1 << 13)                    \n" // Clear V bit 13 to disable hivecs
        "MCR     p15, 0, R0, c1, c0, 0                   \n" // Write value back to CP15 System Control register
        "ISB                                             \n"

        // Configure ACTLR
        "MRC     p15, 0, r0, c1, c0, 1                   \n" // Read CP15 Auxiliary Control Register
        "ORR     r0, r0, #(1 <<  1)                      \n" // Enable L2 prefetch hint (UNK/WI since r4p1)
        "MCR     p15, 0, r0, c1, c0, 1                   \n" // Write CP15 Auxiliary Control Register

        // Set Vector Base Address Register (VBAR) to point to this application's vector table
        "LDR    R0, =Vectors                             \n"
        "MCR    p15, 0, R0, c12, c0, 0                   \n"

        // Setup Stack for each exceptional mode
        "CPS    #0x11                                    \n" // FIQ
        "LDR    SP, =Image$$FIQ_STACK$$ZI$$Limit         \n"
        "CPS    #0x12                                    \n" // IRQ
        "LDR    SP, =Image$$IRQ_STACK$$ZI$$Limit         \n"
        "CPS    #0x13                                    \n" // SVC
        "LDR    SP, =Image$$SVC_STACK$$ZI$$Limit         \n"
        "CPS    #0x17                                    \n" // ABT
        "LDR    SP, =Image$$ABT_STACK$$ZI$$Limit         \n"
        "CPS    #0x1B                                    \n" // UND
        "LDR    SP, =Image$$UND_STACK$$ZI$$Limit         \n"
        "CPS    #0x1F                                    \n" // SYS
        "LDR    SP, =Image$$SYS_STACK$$ZI$$Limit         \n"
        "CPS    #0x13                                    \n" // SVC

        // Call DataInit
        "BL     DataInit                                 \n"

        // Call SystemInit
        "BL     SystemInit                               \n"

        // Unmask interrupts
        "CPSIE  if                                       \n"

        // Call _start
        "BL     _start                                   \n"
        );
}
#endif

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
HAL_VISIBLE void Default_Handler(void)
{
    while (1) {
        ;
    }
}

#endif /* HAL_AP_CORE */
