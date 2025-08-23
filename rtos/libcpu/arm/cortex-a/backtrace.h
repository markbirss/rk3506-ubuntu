/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-29     Jesven       the first version
 */

#ifndef __BACKTRACE_H
#define __BACKTRACE_H

#ifndef __ASSEMBLY__
#include <rtthread.h>
#include <armv7.h>

/* Unwind reason code according the the ARM EABI documents */
enum unwind_reason_code
{
    URC_OK = 0,         /* operation completed successfully */
    URC_CONTINUE_UNWIND = 8,
    URC_FAILURE = 9         /* unspecified failure of some kind */
};

struct unwind_idx
{
    unsigned long addr_offset;
    unsigned long insn;
};

struct unwind_table
{
    const struct unwind_idx *start;
    const struct unwind_idx *origin;
    const struct unwind_idx *stop;
    unsigned long begin_addr;
    unsigned long end_addr;
};

struct stackframe
{
    /*
     * FP member should hold R7 when CONFIG_THUMB2_KERNEL is enabled
     * and R11 otherwise.
     */
    unsigned long fp;
    unsigned long sp;
    unsigned long lr;
    unsigned long pc;
};

struct pt_regs
{
    unsigned long uregs[18];
};

#define ARM_cpsr    uregs[16]
#define ARM_pc      uregs[15]
#define ARM_lr      uregs[14]
#define ARM_sp      uregs[13]
#define ARM_ip      uregs[12]
#define ARM_fp      uregs[11]
#define ARM_r10     uregs[10]
#define ARM_r9      uregs[9]
#define ARM_r8      uregs[8]
#define ARM_r7      uregs[7]
#define ARM_r6      uregs[6]
#define ARM_r5      uregs[5]
#define ARM_r4      uregs[4]
#define ARM_r3      uregs[3]
#define ARM_r2      uregs[2]
#define ARM_r1      uregs[1]
#define ARM_r0      uregs[0]
#define ARM_ORIG_r0 uregs[17]

#define instruction_pointer(regs)   (regs)->ARM_pc

#ifdef CONFIG_THUMB2_KERNEL
#define frame_pointer(regs) (regs)->ARM_r7
#else
#define frame_pointer(regs) (regs)->ARM_fp
#endif

#define UND_Stack_Size          0x00000400
#define SVC_Stack_Size          0x00000400
#define ABT_Stack_Size          0x00000400
#define RT_FIQ_STACK_PGSZ       0x00000000
#define RT_IRQ_STACK_PGSZ       0x00000800
#define USR_Stack_Size          0x00000400

#define SUB_UND_Stack_Size      0x00000400
#define SUB_SVC_Stack_Size      0x00000400
#define SUB_ABT_Stack_Size      0x00000400
#define SUB_RT_FIQ_STACK_PGSZ   0x00000000
#define SUB_RT_IRQ_STACK_PGSZ   0x00000400
#define SUB_USR_Stack_Size      0x00000400

#define ISR_Stack_Size  (UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + \
                 RT_FIQ_STACK_PGSZ + RT_IRQ_STACK_PGSZ)

#define SUB_ISR_Stack_Size  (SUB_UND_Stack_Size + SUB_SVC_Stack_Size + SUB_ABT_Stack_Size + \
                 SUB_RT_FIQ_STACK_PGSZ + SUB_RT_IRQ_STACK_PGSZ)

extern const struct unwind_idx __exidx_start[];
extern const struct unwind_idx __exidx_end[];
extern unsigned char stack_start[ISR_Stack_Size];
extern unsigned long sub_stack_top;

int unwind_frame(struct stackframe *frame, const struct unwind_idx **origin_idx, const struct unwind_idx exidx_start[], const struct unwind_idx exidx_end[]);
void unwind_backtrace(struct pt_regs *regs, const struct unwind_idx exidx_start[], const struct unwind_idx exidx_end[]);

void rt_unwind(struct rt_hw_exp_stack *regs, unsigned int pc_adj);

#endif  /* !__ASSEMBLY__ */

#endif  /* __BACKTRACE_H */

