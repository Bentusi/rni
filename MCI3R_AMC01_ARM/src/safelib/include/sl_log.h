/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                                                                          |**
**|         Copyright (c) 2012 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/
/**
 * @file        sl_log.h
 * @version     2.3.1
 * @brief       API's for logging(errors,results) and profiling of functions
 *
 * @details This file provides the prototypes of the API's which are used for\n
 * function result and error logging and the API's which can be  used for the profiling\n
 * of the selftest and fault injection API's.
 */
#ifndef SL_LOG_H_
#define SL_LOG_H_
#include <sl_api.h>

/** \defgroup grp_err_codes_log Error codes used in log functions
 *
 *  Following macros are used to indicate errors in function entry point
 *
 */
/**@{*/
#define 	ERR_TYPE_ENTRY_CON 	0x00000001u		/**< Indicates error due to invalid entry condition */
#define 	ERR_TYPE_PARAM		0x00000002u 	/**< Indicates error due to invalid parameter */
/**@}*/

typedef struct __SL_Log_Result_Struct {
	uint32 funcId;
	uint32 TestType;
	uint32 result;
	uint32 errum;
}_SL_Log_Result_Struct;

typedef struct __SL_Log_Error_Struct {
	uint32 funcId;
	uint32 errType;
	uint32 errNum;
} _SL_Log_Error_Struct;

/** \defgroup grp_log_fid Function IDs for error/result log buffers
 *
 *  Following macros are to be used by the error & result log functions to indicate the function ID
 *
 */
/**@{*/
typedef enum _sl_log_func_id{
	FUNC_ID_ST_SRAM=0xA, 		/**< SRAM SelfTest API */
	FUNC_ID_ST_FLASH, 			/**< Flash selftest API */
	FUNC_ID_ST_ADC, 			/**< ADC SelfTest API */
	FUNC_ID_ST_PBIST, 			/**< PBIST SelfTest API */
	FUNC_ID_ST_PBIST_STATUS, 	/**< PBIST Status function */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
	FUNC_ID_ST_CCMR4F, 			/**< CCM SelfTest API */
	FUNC_ID_ST_CCMR4F_STATUS, 	/**< CCM Selft test status query API */
	FUNC_ID_ST_L2L3INTERCONNECT, /**< L2L3 interconnect SelfTest API */
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
	FUNC_ID_ST_CCMR5F,			/**< CCM SelfTest API */
	FUNC_ID_ST_CCMR5F_STATUS, 	/**< CCM Selft test status query API */
	FUNC_ID_ST_MEMINTRCNT,		/**< Memory Interconnect SelfTest API */
	FUNC_ID_ST_MAINPERIPHINTRCNT,		/**< Main Peripheral Interconnect SelfTest API */
	FUNC_ID_ST_PERIPHSEGINTRCNT,		/**< Peripheral Segment1 Interconnect SelfTest API */

#endif
	FUNC_ID_ST_PSCON, 			/**< PSCON selftest API */
	FUNC_ID_ST_STC, 			/**< STC selftest API */
	FUNC_ID_ST_EFUSE, 			/**< EFuse SelfTest API */
	FUNC_ID_ST_DMA,
	FUNC_ID_ST_VIM,
	FUNC_ID_ST_HET,
	FUNC_ID_ST_HTU,
	FUNC_ID_ST_SPI,
	FUNC_ID_ST_MIBSPI,
	FUNC_ID_ST_CAN,
	FUNC_ID_ST_GIO,
	FUNC_ID_ST_SCI,
	FUNC_ID_ST_LIN

} SL_Log_FuncId;
/**@}*/
#if FUNC_RESULT_LOG_ENABLED
/**
 * @brief   Funcion logs the result of the execution
 *
 * This function is used by the SafeTI&trade; Hercules&trade; Diagnostic Library APIs to log the result of the function call. Applications can
 * later use this buffer (&codes) to debug the result of the function call
 *
 * @param [in]  fnId - Function ID
 * @param [in]  testType - Selected test type
 * @param [in]  stResult - Result of the self test
 * @param [in]  errNum - Function specific error code.
 *
 * @return      None
 *
 *
 * @reqtag
 */
void SL_Log_Result(SL_Log_FuncId fnId, SL_SelfTestType log_testType, SL_SelfTest_Result log_stResult, uint32 errNum);
#endif


/**
 * @brief   This function logs the errors that were captured by the SafeTI&trade; Hercules&trade; Diagnostic library API in the log buffer.
 *
 * This function is used by the SafeTI&trade; Hercules&trade; Diagnostic Library APIs to log the errors generated during the function call. Applications can
 * later use this buffer (&codes) to debug or analyze the errors generated in the process of executing the function
 *
 * @param [in]  fnId - Function ID
 * @param [in]  errorType - Error type
 * @param [in]  errNum - Function specific error code.
 *
 * @return      None
 *
 *
 * @reqtag
 */
void SL_Log_Error(SL_Log_FuncId fnId, uint32 errType, uint32 errNum);
/**@}*/

/** \defgroup profiling_structures structure for storing the profiling information for selftest and pbist.
 *
 *  Following strucutures are used in the profiling of pbist and selftest/fault injection API.
 *
 */
/**@{*/

/** \struct _SL_Profile_Struct
  *
  * \brief structure for storing the profiling information of each testtype.
  *
  */
typedef struct __SL_Profile_Struct {
	uint32 last_entrytick;				/**< Timestamp taken at entry of the SafeTI diagnostic library API.*/
	uint32 last_exittick;				/**< Timestamp taken at exit of the SafeTI diagnostic library AP.*/
	uint32 error_creation_tick;			/**< Timestamp taken just before the fault injection diagnostic.*/
	uint32 execution_count;				/**< Number of times the test was run.*/
	uint32 cumilative_execution_tick;	/**< Total of each execution of the API*/
    uint32 esm_entrytick;				/**< Timestamp taken at entry point of ESM handler for this fault.*/
    uint32 esm_exittick;				/**< Timestamp taken at exit point of the ESM handler for this fault.*/
    uint32 aborthandler_entrytick;		/**< Timestamp taken at entry point of the data abort handler for this fault.*/
    uint32 aborthandler_exittick;		/**< Timestamp taken at exit point of the data abort handler for this fault.*/
} _SL_Profile_Struct;

/** \struct _SL_Profile_Pbist_Struct
  *
  * \brief structure for storing the profiling information of PBIST tests on various ramgroups fir different algorithms.
  *
  */
typedef struct __SL_Profile_Pbist_Struct {
	uint32 last_entrytick; 				/**< Timestamp for the entry to the pbist API.*/
	uint32 last_exittick;  				/**< Timestamp for the exit from the pbist status.*/
	uint32 execution_count;				/**< Time taken to run the pbist and get the status of the pbist staus.*/
	uint32 cumilative_execution_tick;   /**< Cumulative execution count.*/
} _SL_Profile_Pbist_Struct;
/**@}*/


#define PBIST_MEM_MAX   (uint32)29u
#define PBIST_ALGO_MAX  (uint32)30u

#if FUNCTION_PROFILING_ENABLED
/*these variables will be defined in the sl_log.c and if necessary they can be used externally*/
extern _SL_Profile_Struct SL_Profile_Struct[TESTTYPE_MAX - TESTTYPE_MIN];
extern _SL_Profile_Pbist_Struct SL_Pbist_Profile_Struct[PBIST_MEM_MAX][PBIST_ALGO_MAX];
extern uint32 SL_Active_Profile_Testtype;

/**
 * @brief   This function initializes the pmu module for counting execution ticks.
 *
 * This function is used by the SafeTI&trade; Hercules&trade; Diagnostic Library APIs to initialized the profiling module so as to profile the SafeTI&trade; Hercules&trade; Diagnostic library
 * API's.The API initializes the pmu module to count execution ticks or cycle counts , and also updates global variables used for cycle
 * time compensation
 *
 * @param [in]  None
 * @param [out] None
 *
 * @return      None
 *
 *
 * @reqtag
 */
void SL_Init_Profiling(void);

/**
 * @brief   This function starts profiling of an API for particular testtype.
 *
 * This function is used by the SafeTI&trade; Hercules&trade; Diagnostic Library APIs to start profiling of the SafeTI&trade; Hercules&trade; Diagnostic library code for particular testtype.
 * This API will be called within the selftest or faultinjection API's so as to start profiling the particular testtype
 *
 * @param [in]  profile_testyype the testtype for which the profiling information has to be updated
 * @param [out] None
 *
 * @return      None
 *
 *
 * @reqtag
 */
void SL_Start_Profiling(SL_SelfTestType profile_testyype);

/**
 * @brief   This function stops profiling of an API for particular testtype.
 *
 * This function is used by the SafeTI&trade; Hercules&trade; Diagnostic Library APIs to stop profiling of the SafeTI&trade; Hercules&trade; Diagnostic library code for particular testtype.
 * This API will be called within the selftest or faultinjection API's so as to stop profiling the particular testtype and then update
 * the global array with profiling information necessary.
 *
 * @param [in]  profile_testyype the testtype for which the profiling information has to be updated
 * @param [out] None
 *
 * @return      None
 *
 *
 * @reqtag
 */
void SL_Stop_Profiling(SL_SelfTestType profile_testyype);

/**
 * @brief   This API is used for recording the timestamp at which the error has been created.
 *
 * This function is used by the SafeTI&trade; Hercules&trade; Diagnostic Library APIs to record the error creation timestamp  SafeTI&trade; Hercules&trade; Diagnostic library code for particular testtype.
 * This API will be called within the selftest or faultinjection API's so as to record the error creation timestamp particular testtype and then update
 * the global array with profiling information necessary.
 *
 * @param [in]  profile_testyype the testtype for which the profiling information has to be updated
 * @param [out] None
 *
 * @return      None
 *
 *
 * @reqtag
 */
void SL_Record_Errorcreationtick(SL_SelfTestType profile_testyype);


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

/**
 * @brief   This function starts profiling of an PBIST API for valid ramgrop and algoinfo.
 *
 * This function is used by the application to start profiling of the SafeTI&trade; Hercules&trade; Diagnostic library code for PBIST selftest for valid values of
 * ramGroup and algoinfo.This API will be called by the application before calling the PBIS selftest API.
 *
 * @param [in]  ramGroup the ramGroup for which the profiling information has to be updated
 * @param [in]  algoInfo the algoInfo for which the profiling information has to be updated
 * @param [out] None
 *
 * @return      boolean
 *
 *
 * @reqtag
 */
boolean SL_Start_Profiling_PBIST(register uint32 ramGroup, register uint32 algoInfo);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
boolean SL_Start_Profiling_PBIST(register uint64 ramGroup, register uint32 algoInfo);
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

/**
 * @brief   This function stops profiling of an PBIST API for valid ramgrop and algoinfo.
 *
 * This function is used by the application to stop profiling of the SafeTI&trade; Hercules&trade; Diagnostic library code for PBIST selftest for valid values of
 * ramGroup and algoinfo.This API shall be called by the application after calling the API for getting the status of the PBIST selftest.
 *
 * @param [in]  ramGroup the ramGroup for which the profiling information has to be updated
 * @param [in]  algoInfo the algoInfo for which the profiling information has to be updated
 * @param [out] None
 *
 * @return      boolean
 *
 *
 * @reqtag
 */
boolean SL_Stop_Profiling_PBIST(register uint32 ramGroup, register uint32 algoInfo);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
boolean SL_Stop_Profiling_PBIST(register uint64 ramGroup, register uint32 algoInfo);
#endif

#endif
#endif /* SL_LOG_H_ */
