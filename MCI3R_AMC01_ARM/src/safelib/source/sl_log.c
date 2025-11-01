/*
 * sl_log.c
 *
 *  Created on: Nov 23, 2012
 *      Author: a0132176
 */

/* Comments regarding various justificatios for deviation from MISRA-C coding guidelines
 * are provided at the bottom of the file */

#include <sl_config.h>
#include <sl_log.h>
#if FUNCTION_PROFILING_ENABLED
#include <sl_sys_pmu.h>
#include <math.h>
#include <string.h>
#endif

#define SL_LOG_INIT_TOKEN 0x1234ABCDu

#if FUNCTION_PROFILING_ENABLED
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_1*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( SL_Profile_Struct , "PROFILE_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="PROFILE_DATA"
#endif
/** \brief global array of structure _SL_Profile_Struct that stores the profiling infomration for each testtype  */
_SL_Profile_Struct SL_Profile_Struct[TESTTYPE_MAX - TESTTYPE_MIN];
/** \brief global array of structure _SL_Profile_Pbist_Struct that stores the profiling infomration for each PBIST possible case  */
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_1*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( SL_Pbist_Profile_Struct , "PROFILE_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma section = "PROFILE_DATA"
#endif
_SL_Profile_Pbist_Struct SL_Pbist_Profile_Struct[PBIST_MEM_MAX][PBIST_ALGO_MAX];
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_1*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( SL_Cycles_PMU_start , "PROFILE_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="PROFILE_DATA"
#endif
/** \brief variables used for storing the start,end compensations cycles of the pmu */
uint32 SL_Cycles_PMU_start;
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_1*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( SL_Cycles_PMU_end , "PROFILE_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="PROFILE_DATA"
#endif
uint32 SL_Cycles_PMU_end;
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_1*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( SL_Cycles_PMU_comp , "PROFILE_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="PROFILE_DATA"
#endif
uint32 SL_Cycles_PMU_comp;
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_1*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( SL_Cycles_PMU_measure , "PROFILE_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="PROFILE_DATA"
#endif
uint32 SL_Cycles_PMU_measure;
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_1*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( SL_Active_Profile_Testtype , "PROFILE_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="PROFILE_DATA"
#endif
/** \brief to store the current test which has been run,this will be used in the exception handlers.c file */
uint32 SL_Active_Profile_Testtype;
#endif


#if defined(FUNCTION_PARAM_CHECK_ENABLED) || defined(FUNC_ENTRY_COND_CHECK_ENABLED)
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_2*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( _SL_Log_Initialized , "LOG_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="LOG_DATA"
#endif
/*SAFETYMCUSW 25 D MR:8.7 <APPROVED> "Statically allocated memory needs to be available to entire application." */
static uint32 _SL_Log_Initialized = SL_LOG_INIT_TOKEN;
#endif

#if(FUNC_RESULT_LOG_ENABLED == 1)
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_2*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( _SL_Log_Result_Buffer , "LOG_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="LOG_DATA";
#endif
_SL_Log_Result_Struct _SL_Log_Result_Buffer[LOG_RESULT_BUFFER_COUNT];
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_2*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( _SL_Log_Result_Index , "LOG_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="LOG_DATA";
#endif
uint32 _SL_Log_Result_Index = 0u;
#endif
#if FUNC_RESULT_LOG_ENABLED
void SL_Log_Result(SL_Log_FuncId fnId, SL_SelfTestType log_testType, SL_SelfTest_Result log_stResult, uint32 errNum)
{
	/* Log only if the buffer has been allocated */
#if (LOG_RESULT_BUFFER_COUNT >0)
	if (SL_LOG_INIT_TOKEN == _SL_Log_Initialized) {
		/* Wrap to first location if the buffer end is reached, else move to the next location */
		if (LOG_RESULT_BUFFER_COUNT == _SL_Log_Result_Index) {
			_SL_Log_Result_Index = 0U;
		}
		_SL_Log_Result_Buffer[_SL_Log_Result_Index].funcId		= fnId;
		_SL_Log_Result_Buffer[_SL_Log_Result_Index].testType	= log_testType;
		_SL_Log_Result_Buffer[_SL_Log_Result_Index].result 		= log_stResult;
		_SL_Log_Result_Buffer[_SL_Log_Result_Index].errum 		= errNum;

		_SL_Log_Result_Index++;
	}
#endif
	return;
}
#endif


/* enable the below functions only if profiling is ebnabled*/
#if FUNCTION_PROFILING_ENABLED

void SL_Init_Profiling(void)
{
	 memset(SL_Profile_Struct,0,sizeof(SL_Profile_Struct));
	 memset(SL_Pbist_Profile_Struct,0,sizeof(SL_Pbist_Profile_Struct));
	_sl_pmuDisableCountersGlobal_();
	_sl_pmuResetCounters_();
	_sl_pmuResetCycleCounter_();
	_sl_pmuSetCountEvent_(SL_PMUCOUNTER0,SL_PMU_CYCLE_COUNT);
	_sl_pmuEnableCountersGlobal_();
	_sl_pmuResetCounters_();
	_sl_pmuStartCounters_(SL_PMUCYCLE_COUNTER);
	SL_Cycles_PMU_start = _sl_pmuGetCycleCount_();
	_sl_pmuStopCounters_(SL_PMUCYCLE_COUNTER);
	SL_Cycles_PMU_end = _sl_pmuGetCycleCount_();
	/*Calculating compensation cycles for compensating time spent by pmu*/
	SL_Cycles_PMU_comp = SL_Cycles_PMU_end - SL_Cycles_PMU_start;
}

void SL_Start_Profiling(SL_SelfTestType profile_testyype)
{

	SL_Profile_Struct[profile_testyype - TESTTYPE_MIN].execution_count++;
	SL_Active_Profile_Testtype = profile_testyype;
	_sl_pmuResetCounters_();
	_sl_pmuStartCounters_(SL_PMUCYCLE_COUNTER);
	SL_Cycles_PMU_start = _sl_pmuGetCycleCount_();
	SL_Profile_Struct[profile_testyype - TESTTYPE_MIN].last_entrytick = SL_Cycles_PMU_start;
}

static boolean IsPowerOfTwo(uint32 x)
{
	if(0 == (x & (x - 1)))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
boolean SL_Start_Profiling_PBIST(register uint32 ramGroup, register uint32 algoInfo)
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
boolean SL_Start_Profiling_PBIST(uint64 ramGroup, register uint32 algoInfo)
#endif
{
	boolean retVal;
	uint32 ramgrpidx,algogrpidx;
	ramgrpidx = log2(ramGroup);
	algogrpidx = log2(algoInfo);
	if((TRUE != IsPowerOfTwo(ramGroup))||(TRUE !=  IsPowerOfTwo(algoInfo)))
	{
		retVal =  FALSE;
	}
	else
	{
		SL_Pbist_Profile_Struct[ramgrpidx][algogrpidx].execution_count++;
		_sl_pmuResetCounters_();
		_sl_pmuStartCounters_(SL_PMUCYCLE_COUNTER);
		SL_Cycles_PMU_start = _sl_pmuGetCycleCount_();
		SL_Pbist_Profile_Struct[ramgrpidx][algogrpidx].last_entrytick= SL_Cycles_PMU_start;
		retVal = TRUE;
	}
	return retVal;
}

void SL_Stop_Profiling(SL_SelfTestType profile_testyype)
{
	_sl_pmuStopCounters_(SL_PMUCYCLE_COUNTER);
	SL_Cycles_PMU_end = _sl_pmuGetCycleCount_();
	/*Measure the exectuion time*/
	SL_Cycles_PMU_measure = SL_Cycles_PMU_end - SL_Cycles_PMU_start - SL_Cycles_PMU_comp;
	/*accumulate the execution ticks for testtype*/
	SL_Profile_Struct[profile_testyype - TESTTYPE_MIN].cumilative_execution_tick += SL_Cycles_PMU_measure;
	SL_Profile_Struct[profile_testyype - TESTTYPE_MIN].last_exittick = SL_Cycles_PMU_end;
}

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
boolean SL_Stop_Profiling_PBIST(register uint32 ramGroup, register uint32 algoInfo)
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
boolean SL_Stop_Profiling_PBIST(register uint64 ramGroup, register uint32 algoInfo)
#endif
{
	boolean retVal;
	uint32 ramgrpidx,algogrpidx;
	_sl_pmuStopCounters_(SL_PMUCYCLE_COUNTER);
	SL_Cycles_PMU_end = _sl_pmuGetCycleCount_();
	ramgrpidx = log2(ramGroup);
	algogrpidx = log2(algoInfo);
	if((TRUE != IsPowerOfTwo(ramGroup))&&(TRUE !=  IsPowerOfTwo(algoInfo)))
	{
		retVal =  FALSE;
	}
	else
	{
		SL_Cycles_PMU_measure = SL_Cycles_PMU_end - SL_Cycles_PMU_start - SL_Cycles_PMU_comp;
		SL_Pbist_Profile_Struct[ramgrpidx][algogrpidx].cumilative_execution_tick += SL_Cycles_PMU_measure;
		SL_Pbist_Profile_Struct[ramgrpidx][algogrpidx].last_exittick= SL_Cycles_PMU_end;
		retVal = TRUE;
	}
	return retVal;
}

void SL_Record_Errorcreationtick(SL_SelfTestType profile_testyype)
{
	SL_Profile_Struct[profile_testyype - TESTTYPE_MIN].error_creation_tick = _sl_pmuGetCycleCount_();
}
#endif

#if defined(FUNCTION_PARAM_CHECK_ENABLED) || defined(FUNC_ENTRY_COND_CHECK_ENABLED)
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_2*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( _SL_Log_Error_Buffer , "LOG_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="LOG_DATA"
#endif
/*SAFETYMCUSW 25 D MR:8.7 <APPROVED> "Statically allocated memory needs to be available to entire application." */
_SL_Log_Error_Struct _SL_Log_Error_Buffer[LOG_ERROR_BUFFER_COUNT];
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_2*/
#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION ( _SL_Log_Error_Index , "LOG_DATA" );
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma location="LOG_DATA"
#endif
/*SAFETYMCUSW 25 D MR:8.7 <APPROVED> "Statically allocated memory needs to be available to entire application." */
uint32 _SL_Log_Error_Index = 0u;
#endif

void SL_Log_Error(SL_Log_FuncId fnId, uint32 errType, uint32 errNum)
{
#if(LOG_ERROR_BUFFER_COUNT>0)
	/* Log only if the buffer indexes have been initialized and buffers allocated */
	if (SL_LOG_INIT_TOKEN == _SL_Log_Initialized) {
		/* Wrap around if at end of the buffer, else goto next location in the buffer */
		if (LOG_ERROR_BUFFER_COUNT == _SL_Log_Error_Index) {
			_SL_Log_Error_Index = 0U;
		}
		_SL_Log_Error_Buffer[_SL_Log_Error_Index].funcId 	= fnId;
		_SL_Log_Error_Buffer[_SL_Log_Error_Index].errType 	= errType;
		_SL_Log_Error_Buffer[_SL_Log_Error_Index].errNum  	= errNum;

		_SL_Log_Error_Index++;
	}
#endif
	return;
}


/*Comment_1:
 * "Reason -  used for creation of section for the profiling data used for optimization"*/

/*Comment_2:
 * "Reason -  used for creation of section for the log data used for debugging"*/

