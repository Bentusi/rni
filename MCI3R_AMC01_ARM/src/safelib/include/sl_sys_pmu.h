/** @file sys_sys_pmu.h
*   @brief System Pmu Header File
*   @date 25.July.2013
*   @version 03.06.00
*   
*   This file contains:
*   - Pmu Interface Functions
*   .
*   which are relevant for the performance monitor unit driver.
*/

/* (c) Texas Instruments 2009-2013, All rights reserved. */

#ifndef _SL_PMU_H_
#define _SL_PMU_H_

#include "sl_sys_common.h"

/* USER CODE BEGIN (0) */
/* USER CODE END */

/** @def SL_PMUCOUNTER0
*   @brief pmu event counter 0
*
*   Alias for pmu event counter 0
*/
#define SL_PMUCOUNTER0 0x00000001U

/** @def SL_PMUCOUNTER1
*   @brief pmu event counter 1
*
*   Alias for pmu event counter 1
*/
#define SL_PMUCOUNTER1 0x00000002U

/** @def SL_PMUCOUNTER2
*   @brief pmu event counter 2
*
*   Alias for pmu event counter 2
*/
#define SL_PMUCOUNTER2 0x00000004U

/** @def SL_PMUCYCLE_COUNTER
*   @brief pmu cycle counter
*
*   Alias for pmu event counter
*/
#define SL_PMUCYCLE_COUNTER 0x80000000U

/** @enum sl_pmuEvent
*   @brief pmu event
*
*   Alias for pmu event counter increment source
*/
enum sl_pmuEvent
{
    SL_PMU_INST_CACHE_MISS                     = 0x01U,
    SL_PMU_DATA_CACHE_MISS                     = 0x03U,
    SL_PMU_DATA_CACHE_ACCESS                   = 0x04U,
    SL_PMU_DATA_READ_ARCH_EXECUTED             = 0x06U,
    SL_PMU_DATA_WRITE_ARCH_EXECUTED            = 0x07U,
    SL_PMU_INST_ARCH_EXECUTED                  = 0x08U,
    SL_PMU_EXCEPTION_TAKEN                     = 0x09U,
    SL_PMU_EXCEPTION_RETURN_ARCH_EXECUTED      = 0x0AU,
    SL_PMU_CHANGE_TO_CONTEXT_ID_EXECUTED       = 0x0BU,
    SL_PMU_SW_CHANGE_OF_PC_ARCH_EXECUTED       = 0x0CU,
    SL_PMU_BRANCH_IMM_INST_ARCH_EXECUTED       = 0x0DU,
    SL_PMU_PROC_RETURN_ARCH_EXECUTED           = 0x0EU,
    SL_PMU_UNALIGNED_ACCESS_ARCH_EXECUTED      = 0x0FU,
    SL_PMU_BRANCH_MISSPREDICTED                = 0x10U,
    SL_PMU_CYCLE_COUNT                         = 0x11U,
    SL_PMU_PREDICTABLE_BRANCHES                = 0x12U,
    SL_PMU_INST_BUFFER_STALL                   = 0x40U,
    SL_PMU_DATA_DEPENDENCY_INST_STALL          = 0x41U,
    SL_PMU_DATA_CACHE_WRITE_BACK               = 0x42U,
    SL_PMU_EXT_MEMORY_REQUEST                  = 0x43U,
    SL_PMU_LSU_BUSY_STALL                      = 0x44U,
    SL_PMU_FORCED_DRAIN_OFSTORE_BUFFER         = 0x45U,
    SL_PMU_FIQ_DISABLED_CYCLE_COUNT            = 0x46U,
    SL_PMU_IRQ_DISABLED_CYCLE_COUNT            = 0x47U,
    SL_PMU_ETMEXTOUT_0                         = 0x48U,
    SL_PMU_ETMEXTOUT_1                         = 0x49U,
    SL_PMU_INST_CACHE_TAG_ECC_ERROR            = 0x4AU,
    SL_PMU_INST_CACHE_DATA_ECC_ERROR           = 0x4BU,
    SL_PMU_DATA_CACHE_TAG_ECC_ERROR            = 0x4CU,
    SL_PMU_DATA_CACHE_DATA_ECC_ERROR           = 0x4DU,
    SL_PMU_TCM_FATAL_ECC_ERROR_PREFETCH        = 0x4EU,
    SL_PMU_TCM_FATAL_ECC_ERROR_LOAD_STORE      = 0x4FU,
    SL_PMU_STORE_BUFFER_MERGE                  = 0x50U,
    SL_PMU_LSU_STALL_STORE_BUFFER_FULL         = 0x51U,
    SL_PMU_LSU_STALL_STORE_QUEUE_FULL          = 0x52U,
    SL_PMU_INTEGER_DIV_EXECUTED                = 0x53U,
    SL_PMU_STALL_INTEGER_DIV                   = 0x54U,
    SL_PMU_PLD_INST_LINE_FILL                  = 0x55U,
    SL_PMU_PLD_INST_NO_LINE_FILL               = 0x56U,
    SL_PMU_NON_CACHEABLE_ACCESS_AXI_MASTER     = 0x57U,
    SL_PMU_INST_CACHE_ACCESS                   = 0x58U,
    SL_PMU_DOUBLE_DATA_CACHE_ISSUE             = 0x59U,
    SL_PMU_DUAL_ISSUE_CASE_A                   = 0x5AU,
    SL_PMU_DUAL_ISSUE_CASE_B1_B2_F2_F2D        = 0x5BU,
    SL_PMU_DUAL_ISSUE_OTHER                    = 0x5CU,
    SL_PMU_DP_FLOAT_INST_EXCECUTED             = 0x5DU,
    SL_PMU_DUAL_ISSUED_PAIR_INST_ARCH_EXECUTED = 0x5EU,
    SL_PMU_DATA_CACHE_DATA_FATAL_ECC_ERROR     = 0x60U,
    SL_PMU_DATA_CACHE_TAG_FATAL_ECC_ERROR      = 0x61U,
    SL_PMU_PROCESSOR_LIVE_LOCK                 = 0x62U,
    SL_PMU_ATCM_MULTI_BIT_ECC_ERROR            = 0x64U,
    SL_PMU_B0TCM_MULTI_BIT_ECC_ERROR           = 0x65U,
    SL_PMU_B1TCM_MULTI_BIT_ECC_ERROR           = 0x66U,
    SL_PMU_ATCM_SINGLE_BIT_ECC_ERROR           = 0x67U,
    SL_PMU_B0TCM_SINGLE_BIT_ECC_ERROR          = 0x68U,
    SL_PMU_B1TCM_SINGLE_BIT_ECC_ERROR          = 0x69U,
    SL_PMU_TCM_COR_ECC_ERROR_LOAD_STORE        = 0x6AU,
    SL_PMU_TCM_COR_ECC_ERROR_PREFETCH          = 0x6BU,
    SL_PMU_TCM_FATAL_ECC_ERROR_AXI_SLAVE       = 0x6CU,
    SL_PMU_TCM_COR_ECC_ERROR_AXI_SLAVE         = 0x6DU
};

/** @fn void _sl_pmuInit_(void)
*   @brief Initialize Performance Monitor Unit
*/
void _sl_pmuInit_(void);

/** @fn void _sl_pmuEnableCountersGlobal_(void)
*   @brief Enable and reset cycle counter and all 3 event counters
*/
void _sl_pmuEnableCountersGlobal_(void);

/** @fn void _sl_pmuDisableCountersGlobal_(void)
*   @brief Disable cycle counter and all 3 event counters
*/
void _sl_pmuDisableCountersGlobal_(void);

/** @fn void _sl_pmuResetCycleCounter_(void)
*   @brief Reset cycle counter
*/
void _sl_pmuResetCycleCounter_(void);

/** @fn void _sl_pmuResetEventCounters_(void)
*   @brief Reset event counters 0-2
*/
void _sl_pmuResetEventCounters_(void);

/** @fn void _sl_pmuResetCounters_(void)
*   @brief Reset cycle counter and event counters 0-2
*/
void _sl_pmuResetCounters_(void);

/** @fn void _sl_pmuStartCounters_(uint32 counters)
*   @brief Starts selected counters
*   @param[in] counters - Counter mask
*/
void _sl_pmuStartCounters_(uint32 counters);

/** @fn void _sl_pmuStopCounters_(uint32 counters)
*   @brief Stops selected counters
*   @param[in] counters - Counter mask
*/
void _sl_pmuStopCounters_(uint32 counters);

/** @fn void _sl_pmuSetCountEvent_(uint32 counter, uint32 event)
*   @brief Set event counter count event
*   @param[in] counter - Counter select 0..2
*   @param[in] event   - Count event
*/
void _sl_pmuSetCountEvent_(uint32 counter, uint32 event);

/** @fn uint32 _sl_pmuGetCycleCount_(void)
*   @brief Returns current cycle counter value
*
*   @return cycle count.
*/
uint32 _sl_pmuGetCycleCount_(void);

/** @fn uint32 _sl_pmuGetEventCount_(uint32 counter)
*   @brief Returns current event counter value
*   @param[in] counter - Counter select 0..2
*
*   @return event counter count.
*/
uint32 _sl_pmuGetEventCount_(uint32 counter);

/** @fn uint32 _sl_pmuGetOverflow_(void)
*   @brief Returns current overflow register and clear flags
*
*   @return overflow flags.
*/
uint32 _sl_pmuGetOverflow_(void);


#endif /* _SL_PMU_H_ */
