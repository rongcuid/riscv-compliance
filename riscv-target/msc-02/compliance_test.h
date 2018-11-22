// RISC-V Compliance Test Header File
// Copyright (c) 2017, Codasip Ltd. All Rights Reserved.
// See LICENSE for license details.
//
// Description: Common header file for RV32I tests

#ifndef _COMPLIANCE_TEST_H
#define _COMPLIANCE_TEST_H

#include "riscv_test.h"

//-----------------------------------------------------------------------
// RV Compliance Macros
//-----------------------------------------------------------------------

// IO Port 0x80000000: Command on writing non-zero
// Commands:
// 1 - test_pass
// 2 - test_fail
// 3 - test_halt
// On halt, send 
#define RV_COMPLIANCE_HALT                  \
        li t0, 0x80000000; \
        li t1, 3; \
        sw t1, 0(t0); \
        ecall \

#define RV_COMPLIANCE_RV32M                 \
        RVTEST_RV32M                        \

#define RV_COMPLIANCE_CODE_BEGIN            \
        RVTEST_CODE_BEGIN                   \

#define RV_COMPLIANCE_CODE_END              \
        RVTEST_CODE_END                     \

#define RV_COMPLIANCE_DATA_BEGIN            \
        RVTEST_DATA_BEGIN                   \
        .word 0xdeadc0de;                    \

#define RV_COMPLIANCE_DATA_END              \
        .word 0xdeaddead;                    \
        RVTEST_DATA_END                     \

#endif
