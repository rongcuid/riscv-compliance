// See LICENSE for license details.

#ifndef _ENV_PHYSICAL_SINGLE_CORE_H
#define _ENV_PHYSICAL_SINGLE_CORE_H

#include "../encoding.h"

//-----------------------------------------------------------------------
// Begin Macro
//-----------------------------------------------------------------------

#if 0
#define RVTEST_RV64U                                                    \
  .macro init;                                                          \
  .endm

#define RVTEST_RV64UF                                                   \
  .macro init;                                                          \
  RVTEST_FP_ENABLE;                                                     \
  .endm

#define RVTEST_RV32U                                                    \
  .macro init;                                                          \
  .endm

#define RVTEST_RV32UF                                                   \
  .macro init;                                                          \
  RVTEST_FP_ENABLE;                                                     \
  .endm

#define RVTEST_RV64M                                                    \
  .macro init;                                                          \
  RVTEST_ENABLE_MACHINE;                                                \
  .endm

#define RVTEST_RV64S                                                    \
  .macro init;                                                          \
  RVTEST_ENABLE_SUPERVISOR;                                             \
  .endm
#endif

#define RVTEST_RV32M                                                    \
  .macro init;                                                          \
  RVTEST_ENABLE_MACHINE;                                                \
  .endm

#if 0
#define RVTEST_RV32S                                                    \
  .macro init;                                                          \
  RVTEST_ENABLE_SUPERVISOR;                                             \
  .endm
#endif

#if __riscv_xlen == 64
# define CHECK_XLEN li a0, 1; slli a0, a0, 31; bgez a0, 1f; RVTEST_PASS; 1:
#else
# define CHECK_XLEN li a0, 1; slli a0, a0, 31; bltz a0, 1f; RVTEST_PASS; 1:
#endif

#if 0
#define INIT_PMP                                                        \
  la t0, 1f;                                                            \
  csrw mtvec, t0;                                                       \
  li t0, -1;        /* Set up a PMP to permit all accesses */           \
  csrw pmpaddr0, t0;                                                    \
  li t0, PMP_NAPOT | PMP_R | PMP_W | PMP_X;                             \
  csrw pmpcfg0, t0;                                                     \
  .align 2;                                                             \
1:

#define INIT_SPTBR                                                      \
  la t0, 1f;                                                            \
  csrw mtvec, t0;                                                       \
  csrwi sptbr, 0;                                                       \
  .align 2;                                                             \
1:

#
#define DELEGATE_NO_TRAPS                                               \
  la t0, 1f;                                                            \
  csrw mtvec, t0;                                                       \
  csrwi medeleg, 0;                                                     \
  csrwi mideleg, 0;                                                     \
  csrwi mie, 0;                                                         \
  .align 2;                                                             \
1:

#define RVTEST_ENABLE_SUPERVISOR                                        \
  li a0, MSTATUS_MPP & (MSTATUS_MPP >> 1);                              \
  csrs mstatus, a0;                                                     \
  li a0, SIP_SSIP | SIP_STIP;                                           \
  csrs mideleg, a0;                                                     \

#endif

#define RVTEST_ENABLE_MACHINE                                           \
  li a0, MSTATUS_MPP;                                                   \
  csrs mstatus, a0;                                                     \

#if 0
#define RVTEST_FP_ENABLE                                                \
  li a0, MSTATUS_FS & (MSTATUS_FS >> 1);                                \
  csrs mstatus, a0;                                                     \
  csrwi fcsr, 0

#endif

#define RISCV_MULTICORE_DISABLE                                         \
  csrr a0, mhartid;                                                     \
  1: bnez a0, 1b

//#define EXTRA_TVEC_USER
#define EXTRA_TVEC_MACHINE
#define EXTRA_INIT \
  extra_init: \
        la t2, _bstart; \
        la t1, _bend; \
        2: \
        sw zero, 0(t2); \
        addi t2, t2, 4; \
        blt t2, t1, 2b; \
        li t0, 0x80000000; \
        sw zero, 0(t0); /* Trigger memory scanning */\

#if 0
  extra_init: \
        la t1, _etext;                      \
        la t2, _data; \
        la t3, _edata; \
        1: \
        lw t4, 0(t1); \
        sw t4, 0(t2); \
        addi t1, t1, 4; \
        addi t2, t2, 4; \
        blt t2, t3, 1b; \
        la t2, _bstart; \
        la t1, _bend; \
        2: \
        sw zero, 0(t2); \
        addi t2, t2, 4; \
        blt t2, t1, 2b; \
        li t0, 0x80000000; \
        li t1, 0; \
        sw t1, 0(t0); \

#endif

#define EXTRA_INIT_TIMER

#define INTERRUPT_HANDLER j other_exception /* No interrupts should occur */

#define RVTEST_CODE_BEGIN                                               \
        .section .text.init;                                            \
        .align  6;                                                      \
        .weak stvec_handler;                                            \
        .weak mtvec_handler;                                            \
        .globl _start;                                                  \
_start:                                                                 \
        /* reset vector */                                              \
        j reset_vector;                                                 \
        .align 2;                                                       \
trap_vector:                                                            \
        /* test whether the test came from pass/fail */                 \
        csrr t5, mcause;                                                \
        li t6, CAUSE_USER_ECALL;                                        \
        beq t5, t6, unhandled_exception;                                       \
        li t6, CAUSE_SUPERVISOR_ECALL;                                  \
        beq t5, t6, unhandled_exception;                                       \
        li t6, CAUSE_MACHINE_ECALL;                                     \
        beq t5, t6, unhandled_exception;                                       \
        /* if an mtvec_handler is defined, jump to it */                \
        la t5, mtvec_handler;                                           \
        beqz t5, 1f;                                                    \
        jr t5;                                                          \
        /* was it an interrupt or an exception? */                      \
  1:    csrr t5, mcause;                                                \
        bgez t5, handle_exception;                                      \
        INTERRUPT_HANDLER;                                              \
handle_exception:                                                       \
        /* we don't know how to handle whatever the exception was */    \
  other_exception:                                                      \
        /* some unhandlable exception occurred */                       \
  1:    ori TESTNUM, TESTNUM, 1337;                                     \
  unhandled_exception:                                                         \
        j unhandled_exception;                                                 \
reset_vector:                                                           \
        RISCV_MULTICORE_DISABLE;                                        \
        li TESTNUM, 0;                                                  \
        la t0, trap_vector;                                             \
        csrw mtvec, t0;                                                 \
        CHECK_XLEN;                                                     \
1:      csrwi mstatus, 0;                                               \
        init;                                                           \
        EXTRA_INIT;                                                     \
        EXTRA_INIT_TIMER;                                               \
        la t0, 1f;                                                      \
        csrw mepc, t0;                                                  \
        csrr a0, mhartid;                                               \
        mret;                                                           \
1:                                                                      \
begin_testcode:


//-----------------------------------------------------------------------
// End Macro
//-----------------------------------------------------------------------

#define RVTEST_CODE_END                                                 \
end_testcode:                                                           \
        unimp

//-----------------------------------------------------------------------
// Pass/Fail Macro
//-----------------------------------------------------------------------
//#define RVTEST_SYNC fence
#define RVTEST_SYNC nop

// IO Port 0x80000000: Command to testbench
// Commands:
// 0 - scan for result base address
// 1 - test_pass
// 2 - test_fail
// 3 - test_halt
#define RVTEST_PASS                                                     \
        RVTEST_SYNC;                                                    \
        li TESTNUM, 1;                                                  \
        SWSIG (0, TESTNUM);                                                   \
        li t0, 0x80000000; \
        li t1, 1; \
        sw t1, 0(t0); \
        ecall

#define TESTNUM gp
#define RVTEST_FAIL                                                     \
        RVTEST_SYNC;                                                    \
1:      beqz TESTNUM, 1b;                                               \
        sll TESTNUM, TESTNUM, 1;                                        \
        or TESTNUM, TESTNUM, 1;                                         \
        SWSIG (0, TESTNUM);                                                   \
        li t0, 0x80000000; \
        li t1, 2; \
        sw t1, 0(t0); \
        ecall

//-----------------------------------------------------------------------
// Data Section Macro
//-----------------------------------------------------------------------

#define EXTRA_DATA
#define RVTEST_DATA_BEGIN

#if 0
#define RVTEST_DATA_BEGIN                                               \
        EXTRA_DATA                                                      \
        .pushsection .tohost,"aw",@progbits;                            \
        .align 8; .global tohost; tohost: .dword 0;                     \
        .align 8; .global fromhost; fromhost: .dword 0;                 \
        .popsection;                                                    \
        .align 4; .global begin_signature; begin_signature:

#endif

#define RVTEST_DATA_END .align 4; .global end_signature; end_signature:

#endif
