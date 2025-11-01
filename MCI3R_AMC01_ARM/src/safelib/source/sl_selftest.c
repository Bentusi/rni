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
 * @file        sl_selftest.c
 * @version     2.3.1
 * @brief       File contains the Self Test APIs
 *
 * @details The file implements the  api's for carrying out selftests and fault injection tests on/n
 * all the peripherals (includes Safety island peripherals also).The utility functions or/n
 * support functions which are used by this file are defined in either sl_misc.c or/n
 * sl_priv.c
 */

/* Comments regarding various justificatios for deviation from MISRA-C coding guidelines
 * are provided at the bottom of the file */

#include <sl_api.h>
#include <sl_priv.h>
#define FLASH_DIAG_MODE_ENABLED 0x5U


/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_1*/
#ifdef __TI_COMPILER_VERSION__
/* 16 byte alignment is required to ensure that the starting address is always an even bank */
#pragma DATA_ALIGN(sramEccTestBuff, 16);
#endif
#ifdef __IAR_SYSTEMS_ICC__
#pragma data_alignment=16
#endif
/*SAFETYMCUSW 25 D MR:8.7 <APPROVED> "Statically allocated memory needs to be available to entire application." */
volatile uint64 sramEccTestBuff[4] = {0x0u};

/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_2*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/

boolean SL_SelfTest_SRAM(SL_SelfTestType testType, boolean bMode, SL_SelfTest_Result* sram_stResult)
{
    volatile boolean _sl_fault_injection = FALSE;
    boolean retVal = FALSE;
    volatile uint32 ramRead, regBkupIntEnaSet, regBckupErrInfulence;

	volatile uint64* eccB1;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
	volatile uint32 regBkupRamTh1, regBkupRamTh2;
	volatile uint64* eccB2;
	register uint32 tempVal;
    uint8 iTestCycle=0u;
    uint8 testIterations=0u;
#endif
	volatile uint32 irqStatus;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    register uint64 ramread64;
    volatile uint64* tp; /*test pointer for livelock creation*/
    uint32 ram1uerraddr,ram2uerraddr;
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    /* If fault inject set global variable to flag to the ESM handler that it is a fault injection */
    if((SRAM_ECC_2BIT_FAULT_INJECT == testType)||
        (SRAM_ECC_1BIT_FAULT_INJECTION == testType)||
        (SRAM_ECC_ERROR_PROFILING_FAULT_INJECT == testType)){
        _sl_fault_injection=TRUE;
    }
#endif
#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */
    /*LDRA_INSPECTWINDOW 50 */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if ((_SELFTEST_SRAMECC_MIN > testType) || (_SELFTEST_SRAMECC_MAX < testType) ||
            !CHECK_RANGE_RAM_PTR(sram_stResult)) {
        SL_Log_Error(FUNC_ID_ST_SRAM, ERR_TYPE_PARAM, 0U);
        return(retVal);
    }
#endif
#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /* Error if not in privilege mode */
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_SRAM, ERR_TYPE_ENTRY_CON, 2U);
        return(retVal);
    }
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    /* For MISRA C fix for LDRA - 408S*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    uint32 tmpRAMCTRL1=sl_tcram1REG->RAMCTRL;
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    uint32 tmpRAMCTRL2=sl_tcram2REG->RAMCTRL;
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        uint32 tmpRAMCTRL=l2ramwREG->RAMCTRL;
        volatile uint32* buffPtr;
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    /* Error if ECC is not enabled */
    if ((SRAM_PAR_ADDR_CTRL_SELF_TEST == testType)
            && (((SRAM_ADDR_PARITY_DISABLED) == BF_GET(tmpRAMCTRL1, TCRAM_RAMCTRL_ADDRPARDIS_START, TCRAM_RAMCTRL_ADDRPARDIS_LEN))
                || ((SRAM_ADDR_PARITY_DISABLED) == BF_GET(tmpRAMCTRL2, TCRAM_RAMCTRL_ADDRPARDIS_START, TCRAM_RAMCTRL_ADDRPARDIS_LEN)))) {
        SL_Log_Error(FUNC_ID_ST_SRAM, ERR_TYPE_ENTRY_CON, 1U);
        return(retVal);
    }
    if(((SRAM_ECC_ERROR_FORCING_1BIT == testType)||
            (SRAM_ECC_ERROR_FORCING_2BIT == testType)||
            (SRAM_RADECODE_DIAGNOSTICS == testType)) &&
             (((SRAM_ECC_DISABLED) == BF_GET(tmpRAMCTRL1, TCRAM_RAMCTRL_ECC_DET_EN_START, TCRAM_RAMCTRL_ECC_DET_EN_LEN))
                 ||((SRAM_ECC_DISABLED) == BF_GET(tmpRAMCTRL2, TCRAM_RAMCTRL_ECC_DET_EN_START, TCRAM_RAMCTRL_ECC_DET_EN_LEN)))) {
        SL_Log_Error(FUNC_ID_ST_SRAM, ERR_TYPE_ENTRY_CON, 1U);
        return(retVal);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if(SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_SRAM, ERR_TYPE_ENTRY_CON, 3U);
        return(retVal);
    }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    /* Error if ECC is not enabled */
    if(((SRAM_ECC_ERROR_FORCING_1BIT == testType)||
            (SRAM_ECC_ERROR_FORCING_2BIT == testType)||
			(SRAM_RADECODE_DIAGNOSTICS == testType)) &&
             ((SRAM_ECC_DISABLED) == BF_GET(tmpRAMCTRL, L2RAM_RAMCTRL_ECC_DETECT_EN_START, L2RAM_RAMCTRL_ECC_DETECT_EN_LENGTH)))
    {
        SL_Log_Error(FUNC_ID_ST_SRAM, ERR_TYPE_ENTRY_CON, 1U);
        return(retVal);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((TRUE) == SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_SRAM, ERR_TYPE_ENTRY_CON, 3U);
        return(retVal);
    }
#endif

    retVal = TRUE;      /* Function executed successfully */

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    regBkupRamTh1 = sl_tcram1REG->RAMTHRESHOLD;
    regBkupRamTh2 = sl_tcram2REG->RAMTHRESHOLD;
#endif

    /* False warning */
    switch(testType) {

	case SRAM_ECC_ERROR_FORCING_2BIT:
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case SRAM_ECC_2BIT_FAULT_INJECT:
        /* Read-unlock the error addresses for subsequent captures */
        ramRead= sl_tcram1REG->RAMUERRADDR;
        ramRead= sl_tcram2REG->RAMUERRADDR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /* Clear any previous diagnostic errors status bits [22,21,20,19,12,11,10, 4] - required before trigger */
        BIT_SET(l2ramwREG->RAMERRSTATUS, L2RAM_RAMERRSTATUS_CLRALL);
#endif

        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_6*/
        sramEccTestBuff[2] = 0UL;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_6*/
        sramEccTestBuff[3] = 0UL;
#endif
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_7*/
        /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> Comment_8*/
        /*SAFETYMCUSW 344 S MR:11.5 <APPROVED> Comment_9*/
        eccB1 = &sramEccTestBuff[2];
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        eccB1 = eccB1 + (0x00400000u/sizeof(uint64));
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_7*/
        /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> Comment_8*/
        /*SAFETYMCUSW 344 S MR:11.5 <APPROVED> Comment_9*/
        eccB2 = &sramEccTestBuff[3];
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        eccB2 = eccB2 + (0x00400000u/sizeof(uint64));

        /* Enable Writes to ECC RAM */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_tcram1REG->RAMCTRL, TCRAM_RAMCTRL_ECCWREN);

        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_tcram2REG->RAMCTRL, TCRAM_RAMCTRL_ECCWREN);

        /* cause a 2-bit ECC error */
        /* Flip 2 bits, this flip in ECC works as 2 bit error when the data part used for ECC is zero*/
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_7*/
        *eccB1 ^= TCRAM_SYN_2BIT_DATA_ECC;
        _SL_Barrier_Data_Access();
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_7*/
        *eccB2 ^= TCRAM_SYN_2BIT_DATA_ECC;
        _SL_Barrier_Data_Access();
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        buffPtr = (volatile uint32*)&sramEccTestBuff[2];
#if defined(_TMS570LC43x_)
        l2ramwREG->DIAGDATAVECTOR_H = *buffPtr;
        buffPtr++;
        l2ramwREG->DIAGDATAVECTOR_L = *buffPtr;
#else
        /* Write test vectors */
		l2ramwREG->DIAGDATAVECTOR_L = *buffPtr;
		buffPtr++;
		l2ramwREG->DIAGDATAVECTOR_H = *buffPtr;

#endif
        l2ramwREG->DIAG_ECC = (*eccB1) ^ L2RAM_SYN_2BIT_DATA_ECC;

#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        ramRead = sl_tcram1REG->RAMCTRL;
        /*flag is set to indicate the current test which is ongoing and
        These flags are used in the sl_esm.c so as to mask the esm callback*/
        if(SRAM_ECC_2BIT_FAULT_INJECT == testType)
        {
        	(void)SL_FLAG_SET(SRAM_ECC_2BIT_FAULT_INJECT);
        }
        else
#endif
        {
        	(void)SL_FLAG_SET(SRAM_ECC_ERROR_FORCING_2BIT);
        }

#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        _SL_Barrier_Data_Access();

        /* Set the self test flag for a self test to indicate the esm handler that this is done as a part of selftest */
        /* read from location with 2-bit ECC error this will cause a data abort to be generated */
        /*SAFETYMCUSW 446 S MR:10.1 <APPROVED> Comment_11*/
        ramread64 = sramEccTestBuff[2];
        _SL_Barrier_Data_Access();
        /* Restore ctrl registers */
        sl_tcram1REG->RAMCTRL &= ~TCRAM_RAMCTRL_ECCWREN;

        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        ramRead = sl_tcram2REG->RAMCTRL;
        /* Set the self test flag for a self test to indicate the esm handler that this is done as a part of selftest */
        /*SAFETYMCUSW 446 S MR:10.1 <APPROVED> Comment_11*/
        ramread64 = sramEccTestBuff[3];
        _SL_Barrier_Data_Access();
        /* Restore ctrl registers */
        sl_tcram2REG->RAMCTRL &= ~TCRAM_RAMCTRL_ECCWREN;

        if(SRAM_ECC_ERROR_FORCING_2BIT == testType){
            ram1uerraddr=sl_tcram1REG->RAMUERRADDR;
            ram2uerraddr=sl_tcram2REG->RAMUERRADDR;
            /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
            status functions to get the pass/failure information*/
            /* Check the error status on both banks */
    		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
            /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> Comment_8*/
            /*SAFETYMCUSW 344 S MR:11.5 <APPROVED> Comment_9*/
    		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_1"*/
            /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> Comment_8*/
            /*SAFETYMCUSW 344 S MR:11.5 <APPROVED> Comment_9*/
            /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
            /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
            if ((((uint32)&sramEccTestBuff[2] & TCRAM_RAMUERRADDR_UNC_ERRADD) == ram1uerraddr)
            		&& (((uint32)&sramEccTestBuff[3] & TCRAM_RAMUERRADDR_UNC_ERRADD) == (ram2uerraddr)) &&
                    ((uint32)(1u << ESM_G3ERR_B1TCM_ECC_UNCORR) == (esmREG->SR1[2] & (uint32)(1u << ESM_G3ERR_B1TCM_ECC_UNCORR)))&&
                    ((uint32)(1u << ESM_G3ERR_B0TCM_ECC_UNCORR) == (esmREG->SR1[2] & (uint32)(1u << ESM_G3ERR_B0TCM_ECC_UNCORR)))) {
                *sram_stResult = ST_PASS;
            } else {
                *sram_stResult = ST_FAIL;
            }
            /* Clear nError */
            _SL_HoldNClear_nError();

            /* Clear the ESM Status */
            esmREG->SR1[2] = ((uint32)(1u << ESM_G3ERR_B0TCM_ECC_UNCORR) | (uint32)(1u << ESM_G3ERR_B1TCM_ECC_UNCORR));
            /* Clear double bit error anyways */
            sl_tcram1REG->RAMERRSTATUS |= TCRAM_RAMERRSTATUS_DER;
            sl_tcram2REG->RAMERRSTATUS |= TCRAM_RAMERRSTATUS_DER;
            /* Compute uncorrupted ECC */
            sramEccTestBuff[2] = 0UL;
            sramEccTestBuff[3] = 0UL;
        }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)

        /* Equality check test */
        l2ramwREG->RAMTEST = (uint32) 0x0000008Au; /* 1->Trigger, 8->Eq., test mode, A->Test enable */
        BIT_SET(l2ramwREG->RAMTEST, L2RAM_RAMTEST_TST_TRIGGER);

        /* Wait for the test to complete */
        /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
        while (L2RAM_RAMTEST_TST_TRIGGER == (l2ramwREG->RAMTEST & L2RAM_RAMTEST_TST_TRIGGER));

        if(SRAM_ECC_ERROR_FORCING_2BIT == testType)
        {
            /* Check if the diagnostic error status bits have been set */
            if((1 << ESM_G2ERR_L2RAMW_UNCORR_B) == ((esmREG->SSR2 & (1u << ESM_G2ERR_L2RAMW_UNCORR_B))) &&
                    ((l2ramwREG->RAMERRSTATUS & (1 << L2RAM_RAMERRSTATUS_DRDE)) == (1 << L2RAM_RAMERRSTATUS_DRDE)) &&
                    ((l2ramwREG->RAMERRSTATUS & (1 << L2RAM_RAMERRSTATUS_DWDE)) == (1 << L2RAM_RAMERRSTATUS_DWDE)))
            {
                *sram_stResult = ST_PASS;
            }
            else
            {
                *sram_stResult = ST_FAIL;
            }
            /* Clear any previous diagnostic errors status bits [22,21,20,19,12,11,10, 4] - required before trigger */
            BIT_SET(l2ramwREG->RAMERRSTATUS, L2RAM_RAMERRSTATUS_CLRALL);
            _SL_HoldNClear_nError(); /* Clear nError */
            BIT_SET(esmREG->SSR2,(uint32)(1u << ESM_G2ERR_L2RAMW_UNCORR_B)); /* Clear shadow status register */
        }

#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*clear the flags which indicate tests ongoing*/
		if(SRAM_ECC_2BIT_FAULT_INJECT == testType)
		{
		   /*clear the flags which indicate tests ongoing*/
			SL_FLAG_CLEAR(SRAM_ECC_2BIT_FAULT_INJECT);
		}
		else
#endif
		{
		   /*clear the flags which indicate tests ongoing*/
			SL_FLAG_CLEAR(SRAM_ECC_ERROR_FORCING_2BIT);
		}
        break;

    case SRAM_ECC_ERROR_FORCING_1BIT:
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case SRAM_ECC_1BIT_FAULT_INJECTION:
    case SRAM_ECC_ERROR_PROFILING:
    case SRAM_ECC_ERROR_PROFILING_FAULT_INJECT:
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /* Clear any previous diagnostic errors status bits [22,21,20,19,12,11,10, 4] - required before trigger */
        BIT_SET(l2ramwREG->RAMERRSTATUS, L2RAM_RAMERRSTATUS_CLRALL);
#endif

        if(SRAM_ECC_ERROR_FORCING_1BIT == testType)
        {
        	(void)SL_FLAG_SET(SRAM_ECC_ERROR_FORCING_1BIT);
        }

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
        status functions to get the pass/failure information*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBkupIntEnaSet = esmREG->IESR1;
        regBckupErrInfulence = esmREG->EEPAPR1;
         if((SRAM_ECC_ERROR_FORCING_1BIT == testType)||(SRAM_ECC_ERROR_PROFILING == testType)){
             esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_B0TCM_CORRERR)|
                                 GET_ESM_BIT_NUM(ESM_G1ERR_B1TCM_CORRERR);
             esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_B0TCM_CORRERR)|
                                 GET_ESM_BIT_NUM(ESM_G1ERR_B1TCM_CORRERR);

         }
#endif

        sramEccTestBuff[0] = 0UL;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        sramEccTestBuff[1] = 0UL;
        testIterations=1u;
#endif
        /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> Comment_8*/
        /*SAFETYMCUSW 344 S MR:11.5 <APPROVED> Comment_9*/
		eccB1 = &sramEccTestBuff[0];
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        eccB1 = eccB1 + (0x00400000u/sizeof(uint64));

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> Comment_8*/
        /*SAFETYMCUSW 344 S MR:11.5 <APPROVED> Comment_9*/
		eccB2 = &sramEccTestBuff[1];
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        eccB2 = eccB2 + (0x00400000u/sizeof(uint64));

        /* Enable Writes to ECC RAM */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_tcram1REG->RAMCTRL, TCRAM_RAMCTRL_ECCWREN);

        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_tcram2REG->RAMCTRL, TCRAM_RAMCTRL_ECCWREN);

        regBkupRamTh1 = sl_tcram1REG->RAMTHRESHOLD;
        regBkupRamTh2 = sl_tcram2REG->RAMTHRESHOLD;
        /* Clear RAMOCCUR before setting value */
        sl_tcram1REG->RAMOCCUR=0u;
        sl_tcram2REG->RAMOCCUR=0u;
        if((SRAM_ECC_ERROR_PROFILING_FAULT_INJECT == testType)||(SRAM_ECC_ERROR_PROFILING == testType)){
            sl_tcram1REG->RAMTHRESHOLD = TCRAM_RAMTHRESHOLD_FOR_TEST;
            sl_tcram2REG->RAMTHRESHOLD = TCRAM_RAMTHRESHOLD_FOR_TEST;
            testIterations=(uint8)TCRAM_RAMTHRESHOLD_FOR_TEST;
        }else{
            sl_tcram1REG->RAMTHRESHOLD = 1u;
            sl_tcram2REG->RAMTHRESHOLD = 1u;
            testIterations=1u;
        }

        *sram_stResult = ST_PASS;
        for(iTestCycle=1u;iTestCycle<=testIterations;iTestCycle++){
            _SL_Barrier_Data_Access();
            /* cause a 1-bit ECC error */
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_7*/
            *eccB1 ^= TCRAM_SYN_1BIT_DATA_ECC;
            /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_7*/
            *eccB2 ^= TCRAM_SYN_1BIT_DATA_ECC;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            buffPtr = (volatile uint32*)&sramEccTestBuff[0];

        /* Write test vectors */
#if defined(_TMS570LC43x_)
        l2ramwREG->DIAGDATAVECTOR_H = *buffPtr;
        buffPtr++;
        l2ramwREG->DIAGDATAVECTOR_L = *buffPtr;
#else
        /* Write test vectors */
		l2ramwREG->DIAGDATAVECTOR_L = *buffPtr;
		buffPtr++;
		l2ramwREG->DIAGDATAVECTOR_H = *buffPtr;

#endif
        l2ramwREG->DIAG_ECC = (*eccB1) ^ L2RAM_SYN_1BIT_DATA_ECC;

#endif

#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
            /* read from location with to be generate error */

            _SL_Barrier_Data_Access();
            /*SAFETYMCUSW 446 S MR:10.1 <APPROVED> Comment_11*/
            ramread64 = sramEccTestBuff[0];
            /*SAFETYMCUSW 446 S MR:10.1 <APPROVED> Comment_11*/
            _SL_Barrier_Data_Access();
            ramread64 = sramEccTestBuff[1];
            _SL_Barrier_Data_Access();
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
            if((sl_tcram1REG->RAMOCCUR == (iTestCycle % testIterations))
               && (sl_tcram2REG->RAMOCCUR == (iTestCycle % testIterations))){
                /*Testcase is still in ST_PASS as long as RAMOCCUR gets incremented*/
                *sram_stResult = ST_PASS;
            }else{
                /*Testcase fails if RAMOCCUR does not get incremented*/
                *sram_stResult = ST_FAIL;
                break;
            }
            sramEccTestBuff[0] = 0UL;
            sramEccTestBuff[1] = 0UL;
        }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)

        /* Equality check in compare */
        l2ramwREG->RAMTEST = (uint32) 0x0000008Au; /* 1->Trigger, 8->Eq., test mode, A->Test enable */
        l2ramwREG->RAMTEST = (uint32) 0x00000100u;

        /* Wait for the test to complete */
        /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
        while (L2RAM_RAMTEST_TST_TRIGGER == (l2ramwREG->RAMTEST & L2RAM_RAMTEST_TST_TRIGGER));
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /* If in fault injection mode, this will not clear the status and ESM status */
        if(!((SRAM_ECC_1BIT_FAULT_INJECTION== testType) || (SRAM_ECC_ERROR_PROFILING_FAULT_INJECT == testType))){
            if(ST_FAIL != *sram_stResult){
                /* Check if the single bit errors have been generated for both banks */
            	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
            	/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                if ((TCRAM_RAMERRSTATUS_ADDR_SERR == (uint32)(sl_tcram1REG->RAMERRSTATUS & TCRAM_RAMERRSTATUS_ADDR_SERR))
                		/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                        && ((TCRAM_RAMERRSTATUS_ADDR_SERR == (uint32)(sl_tcram2REG->RAMERRSTATUS & TCRAM_RAMERRSTATUS_ADDR_SERR)))
                        && (((uint32)(1u << ESM_G1ERR_B0TCM_CORRERR) | (uint32)(1u << ESM_G1ERR_B1TCM_CORRERR))
                                == (esmREG->SR1[0] & ((uint32)(1u << ESM_G1ERR_B0TCM_CORRERR) | (uint32)(1u << ESM_G1ERR_B1TCM_CORRERR))))){
                    /*LDRA_INSPECTWINDOW 50 */
            		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                    /*SAFETYMCUSW 134 S MR:12.2 <APPROVED> Comment_8*/
                    /*SAFETYMCUSW 344 S MR:11.5 <APPROVED> Comment_9*/
                	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                	/*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
                    if( ((SRAM_ECC_ERROR_FORCING_1BIT==testType) &&
                    		(sl_tcram2REG->RAMSERRADDR == ((uint32)&sramEccTestBuff[1] & ((uint32)0x0003FFFFU)))) || (SRAM_ECC_ERROR_PROFILING==testType)) {
                        *sram_stResult = ST_PASS;
                    }else{
                        *sram_stResult = ST_FAIL;
                    }
                    sl_tcram1REG->RAMOCCUR = 0x00000000u;
                    sl_tcram2REG->RAMOCCUR = 0x00000000u;
                } else {
                    *sram_stResult = ST_FAIL;
                }
            }
            /* Clear the ESM Status */
            esmREG->SR1[0] = ((uint32)(1u << ESM_G1ERR_B0TCM_CORRERR) | (uint32)(1u << ESM_G1ERR_B1TCM_CORRERR));
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            BIT_SET(sl_tcram1REG->RAMERRSTATUS, TCRAM_RAMERRSTATUS_ADDR_SERR);/* Clear for subsequent operation */
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            BIT_SET(sl_tcram2REG->RAMERRSTATUS, TCRAM_RAMERRSTATUS_ADDR_SERR);
            /* Compute uncorrupted ECC */
            sramEccTestBuff[0] = 0UL;
            sramEccTestBuff[1] = 0UL;
        }
        sl_tcram1REG->RAMTHRESHOLD = regBkupRamTh1;
        sl_tcram2REG->RAMTHRESHOLD = regBkupRamTh2;
        /* Restore ctrl registers */
        sl_tcram1REG->RAMCTRL &= ~TCRAM_RAMCTRL_ECCWREN;
        sl_tcram2REG->RAMCTRL &= ~TCRAM_RAMCTRL_ECCWREN;

        /*Restore grp1 esm interrupt enable*/
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /*if(SRAM_ECC_ERROR_FORCING_1BIT == testType){*/
        /* Check if the diagnostic error status bits have been set */
        if(((esmREG->SSR2 & (1u << ESM_G2ERR_L2RAMW_UNCORR_B)) == (1 << ESM_G2ERR_L2RAMW_UNCORR_B)) &&
            ((l2ramwREG->RAMERRSTATUS & (1 << L2RAM_RAMERRSTATUS_DRSE)) == (1 << L2RAM_RAMERRSTATUS_DRSE)) &&
            ((l2ramwREG->RAMERRSTATUS & (1 << L2RAM_RAMERRSTATUS_DWSE)) == (1 << L2RAM_RAMERRSTATUS_DWSE)))
        {
            *sram_stResult = ST_PASS;
        }
        else
        {
            *sram_stResult = ST_FAIL;
        }
        /* Clear diagnostic errors status bits [22,21,20,19,12,11,10, 4] */
        BIT_SET(l2ramwREG->RAMERRSTATUS, L2RAM_RAMERRSTATUS_CLRALL);
        _SL_HoldNClear_nError(); /* Clear nError */
        BIT_SET(esmREG->SSR2,(uint32)(1u << ESM_G2ERR_L2RAMW_UNCORR_B)); /* Clear shadow status register */
#endif
    	if(SRAM_ECC_ERROR_FORCING_1BIT == testType)
        {
    		SL_FLAG_CLEAR(SRAM_ECC_ERROR_FORCING_1BIT);
        }

        break;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case SRAM_PAR_ADDR_CTRL_SELF_TEST:

    	/* set testtype flag and check in esm handler (not callback) */
    	(void)SL_FLAG_SET(SRAM_PAR_ADDR_CTRL_SELF_TEST);

        /* Once the tests are started, any access to RAM will generate parity errors. So block interrupts */
        irqStatus = _SL_Disable_IRQ();
        /* Should the interrupt routine be backed up and restored before this test is run or
         * should we use the Shadow register only. Because the ISR reads IOFFHR and IOFFLR
         * which in turn clears the status register, which will still be there in Shadow register
         *
         * Pointing ISR to a dummy ISR which does not read IOFFHR/IOFFLR can resolve this
         *
         */

        /* Added for bug fix*/
        tempVal = sl_tcram1REG->RAMPERADDR; /* RAMPERRADDR register must be read for subsequent updates */
        tempVal = sl_tcram2REG->RAMPERADDR;
        tempVal = tempVal; /* avoid compiler warning */
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        /* Override parity (actually flip).. NO RAM ACCESS from this point (except intentional errors) */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        sl_tcram1REG->RAMCTRL = (sl_tcram1REG->RAMCTRL & (~TCRAM_RAMCTRL_ADDRPAR_OVR_MASK)) | TCRAM_RAMCTRL_ADDR_PARITY_OVER;
        /* Access any RAM location */
        _SL_Barrier_Data_Access();
        ramread64 = sramEccTestBuff[0]; /* Generate read parity error on B0 */
        _SL_Barrier_Data_Access();
        /* Restore parity, so that we can use the stack */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        sl_tcram1REG->RAMCTRL = (sl_tcram1REG->RAMCTRL & (~TCRAM_RAMCTRL_ADDRPAR_OVR_MASK));


        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/        sl_tcram2REG->RAMCTRL = (sl_tcram2REG->RAMCTRL & (~TCRAM_RAMCTRL_ADDRPAR_OVR_MASK)) | TCRAM_RAMCTRL_ADDR_PARITY_OVER;
        ramread64 = sramEccTestBuff[1]; /* Generate read parity error on B1 */
        _SL_Barrier_Data_Access();
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        sl_tcram2REG->RAMCTRL = (sl_tcram2REG->RAMCTRL & (~TCRAM_RAMCTRL_ADDRPAR_OVR_MASK));


        /* Have to check the Shadow register as the normal ESM status register is cleared during the interrupt handling */
        /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if ((TCRAM_RAMERRSTAT_RADDRPAR_FAIL == (uint32)(sl_tcram1REG->RAMERRSTATUS &
        		TCRAM_RAMERRSTAT_RADDRPAR_FAIL)) &&  /* Read parity error on B1 */
                (TCRAM_RAMERRSTAT_RADDRPAR_FAIL ==
                		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                		(uint32)(sl_tcram2REG->RAMERRSTATUS &
                		TCRAM_RAMERRSTAT_RADDRPAR_FAIL)) &&  /* Read parity error on B2 */
                (GET_ESM_BIT_NUM(ESM_G2ERR_B0TCM_ADDPAR) ==
                		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                		(esmREG->SSR2 & GET_ESM_BIT_NUM(ESM_G2ERR_B0TCM_ADDPAR))) &&   /* B1 Parity error */
                (GET_ESM_BIT_NUM(ESM_G2ERR_B1TCM_ADDPAR) ==
                		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                		(esmREG->SSR2 & GET_ESM_BIT_NUM(ESM_G2ERR_B1TCM_ADDPAR)))) {    /* B2 parity error */
            *sram_stResult = ST_PASS;
        } else {
            *sram_stResult = ST_FAIL;
        }
        /* Anyways Clear the interrupt bits */
        esmREG->SSR2 = (uint32)(1U << ESM_G2ERR_B0TCM_ADDPAR);
        esmREG->SSR2 = (uint32)(1U << ESM_G2ERR_B1TCM_ADDPAR);
        esmREG->SR1[1] = (uint32)(1U << ESM_G2ERR_B0TCM_ADDPAR);
        esmREG->SR1[1] = (uint32)(1U << ESM_G2ERR_B1TCM_ADDPAR);
        sl_tcram1REG->RAMERRSTATUS = TCRAM_RAMERRSTAT_RADDRPAR_FAIL;
        sl_tcram2REG->RAMERRSTATUS = TCRAM_RAMERRSTAT_RADDRPAR_FAIL;
        tempVal = sl_tcram1REG->RAMPERADDR; /* RAMPERRADDR register must be read for subsequent updates */
        tempVal = sl_tcram2REG->RAMPERADDR;
        tempVal = tempVal; /* avoid compiler warning */

        if (ST_PASS == *sram_stResult) {
            /* Override parity (actually flip).. NO RAM ACCESS from this point (except intentional errors) */
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            sl_tcram1REG->RAMCTRL = (sl_tcram1REG->RAMCTRL & 0xF0FFFFFFU) | TCRAM_RAMCTRL_ADDR_PARITY_OVER;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            sl_tcram2REG->RAMCTRL = (sl_tcram2REG->RAMCTRL & 0xF0FFFFFFU) | TCRAM_RAMCTRL_ADDR_PARITY_OVER;
            sramEccTestBuff[0] = 0xAAAAAAAABBBBBBBBUL; /* Generate write parity error on B1 & B2 */
            sramEccTestBuff[1] = 0xBBBBBBBBAAAAAAAAUL;
            /* Restore parity, so that we can use the stack */
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            sl_tcram1REG->RAMCTRL = (sl_tcram1REG->RAMCTRL & 0xF0FFFFFFU);
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            sl_tcram2REG->RAMCTRL = (sl_tcram2REG->RAMCTRL & 0xF0FFFFFFU);
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
            /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
            /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
            if ((TCRAM_RAMERRSTAT_WADDRPAR_FAIL == (uint32)(sl_tcram1REG->RAMERRSTATUS & TCRAM_RAMERRSTAT_WADDRPAR_FAIL)) &&  /* Write parity error on B1 */
                    (TCRAM_RAMERRSTAT_WADDRPAR_FAIL == (uint32)(sl_tcram2REG->RAMERRSTATUS & TCRAM_RAMERRSTAT_WADDRPAR_FAIL)) &&  /* Write parity error on B2 */
                    (0 != (esmREG->SSR2 & ((uint32)1U << ESM_G2ERR_B0TCM_ADDPAR))) &&    /* B1 Parity error */
                    (0 != (esmREG->SSR2 & ((uint32)1U << ESM_G2ERR_B1TCM_ADDPAR)))) {    /* B2 parity error */
                *sram_stResult = ST_PASS;
            } else {
                *sram_stResult = ST_FAIL;
            }

            /* Restore parity */
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            sl_tcram1REG->RAMCTRL = (sl_tcram1REG->RAMCTRL & 0xF0FFFFFFU);
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            sl_tcram2REG->RAMCTRL = (sl_tcram2REG->RAMCTRL & 0xF0FFFFFFU);

            /* Anyways Clear the interrupt bits */
            esmREG->SSR2 = (uint32)(1U << ESM_G2ERR_B0TCM_ADDPAR);
            esmREG->SSR2 = (uint32)(1U << ESM_G2ERR_B1TCM_ADDPAR);
            esmREG->SR1[1] = (uint32)(1U << ESM_G2ERR_B0TCM_ADDPAR);
            esmREG->SR1[1] = (uint32)(1U << ESM_G2ERR_B1TCM_ADDPAR);
            sl_tcram1REG->RAMERRSTATUS = TCRAM_RAMERRSTAT_WADDRPAR_FAIL;
            sl_tcram2REG->RAMERRSTATUS = TCRAM_RAMERRSTAT_WADDRPAR_FAIL;
            tempVal = sl_tcram1REG->RAMPERADDR; /* RAMPERRADDR register must be read for subsequent updates */
            tempVal = sl_tcram2REG->RAMPERADDR;
            tempVal = tempVal; /* avoid compiler warning */
        }
        _SL_HoldNClear_nError(); /* Clear nError */
        /* Restore interrupts */
        _SL_Restore_IRQ(irqStatus);
        ramread64 = ramread64; /* avoid compiler warning */

        /* Clear testtype flag */
        SL_FLAG_CLEAR(SRAM_PAR_ADDR_CTRL_SELF_TEST);

        break;
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
    case SRAM_RADECODE_DIAGNOSTICS:
        retVal = TRUE;

#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif

        /*flag is set to indicate the current test which is ongoing and
        These flags are used in the sl_esm.c so as to mask the esm callback*/
        (void)SL_FLAG_SET(SRAM_RADECODE_DIAGNOSTICS);
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        _SL_SelfTest_SRAM_RAD(sl_tcram1REG, sram_stResult);
        _SL_HoldNClear_nError(); /* Clear nError */
        if (ST_PASS == *sram_stResult) {
            esmREG->SR1[1] = (uint32)(1U << ESM_G2ERR_B0TCM_UNCORR);
            esmREG->SSR2 = (uint32)(1U << ESM_G2ERR_B0TCM_UNCORR);
            /*flag is set to indicate the current test which is ongoing and
            These flags are used in the sl_esm.c so as to mask the esm callback*/
            (void)SL_FLAG_SET(SRAM_RADECODE_DIAGNOSTICS);
            _SL_SelfTest_SRAM_RAD(sl_tcram2REG, sram_stResult);
            _SL_HoldNClear_nError(); /* Clear nError */
            if (ST_PASS == *sram_stResult) {
                esmREG->SR1[1] = (uint32)(1U << ESM_G2ERR_B1TCM_UNCORR);
                esmREG->SSR2 = (uint32)(1U << ESM_G2ERR_B1TCM_UNCORR);
            }
        }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        _SL_SelfTest_SRAM_RAD(l2ramwREG, sram_stResult);
        _SL_HoldNClear_nError(); /* Clear nError */

#endif
        /*clear the flags which indicate tests ongoing*/
        SL_FLAG_CLEAR(SRAM_RADECODE_DIAGNOSTICS);
        break;
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case SRAM_LIVELOCK_DIAGNOSTICS:
    	(void)SL_FLAG_SET(testType);

    	tp = (volatile uint64 *)0x08000004u; /*start address of unaligned access*/
		eccB1 = (volatile uint64 *)0x08000000u; /*bank1 base word*/
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        eccB1 = eccB1 + (0x00400000u/sizeof(uint64)); /*pointer to ecc for bank1 base word*/
		eccB2 = (volatile uint64 *)0x08000008u; /*bank2 base word*/
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        eccB2 = eccB2 + (0x00400000u/sizeof(uint64)); /*pointer to ecc for bank2 base word*/

        /* Enable Writes to ECC RAM */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_tcram1REG->RAMCTRL, TCRAM_RAMCTRL_ECCWREN);
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_tcram2REG->RAMCTRL, TCRAM_RAMCTRL_ECCWREN);

        regBkupRamTh1 = sl_tcram1REG->RAMTHRESHOLD;
        regBkupRamTh2 = sl_tcram2REG->RAMTHRESHOLD;

        /* set threshold to 1 */
        sl_tcram1REG->RAMTHRESHOLD = 0x1u;
        sl_tcram2REG->RAMTHRESHOLD = 0x1u;

            _SL_Barrier_Data_Access();

            /* cause a 1-bit ECC error */
            *eccB1 ^= TCRAM_SYN_1BIT_DATA_ECC;

            *eccB2 ^= TCRAM_SYN_1BIT_DATA_ECC;

            /* read from location with to be generate error */
            _SL_Barrier_Data_Access();

        	ramread64 = (*tp); /*unaligned read*/

            _SL_Barrier_Data_Access();

    	SL_FLAG_CLEAR(testType);

    	break;
#endif

    default:
        retVal = FALSE;
        break;
    }
    /* If the test was fault injection and not self test, then do not mask the call back & clear the *
    *  FAULT_INJECTION flag for subsequent runs*/
             _sl_fault_injection=FALSE;

#if(FUNC_RESULT_LOG_ENABLED == 1)
    /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_14*/
    SL_Log_Result(FUNC_ID_ST_SRAM,testType, *sram_stResult , 0U);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define flashBadECC1    0x20000000u
#define flashBadECC2    0x20000008u
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define flashBadECC1    0x00000008u
#define flashBadECC2    0x000000016u
/* Flash diagnostics must be run from SRAM  */
#ifdef __TI_COMPILER_VERSION__
#pragma CODE_SECTION (SL_SelfTest_Flash, ".sl_stflash_SRAM")
#elif __IAR_SYSTEMS_ICC__
#pragma location = ".sl_stflash_SRAM"
#else
__attribute__((section(".sl_stflash_SRAM")))
#endif
#endif
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_Flash (SL_SelfTestType testType, boolean bMode, SL_SelfTest_Result* flash_stResult)
{
    boolean retVal = FALSE;
    uint32 fdiagCtrl = F021F_FDIAGCTRL_DIAG_EN_KEY, regBkupIntEnaSet;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    uint32 regBkupFparOvr, regBckupErrInfulence, regBckupFdiagctrl;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    uint32 content, index, regBkupCamAvail, regBckupFdiagctrl, regBckupErrctrl, regBckupErrInfulence, vimCleared = 0;
#endif
    volatile boolean _sl_fault_injection = FALSE;
    volatile uint32 flashread = 0u;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    /* If fault inject set global variable to flag to the ESM handler that it is a fault injection */
    if((FLASH_ADDRESS_ECC_FAULT_INJECT == testType)||
        (FLASH_ADDRESS_PARITY_FAULT_INJECT == testType)||
        (FLASH_ECC_TEST_MODE_2BIT_FAULT_INJECT == testType)){
        _sl_fault_injection=TRUE;
    }
#endif
#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in rage */
    /*LDRA_INSPECTWINDOW 50 */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if ((_SELFTEST_FLASHECC_MIN > testType) || (_SELFTEST_FLASHECC_MAX < testType) || !CHECK_RANGE_RAM_PTR(flash_stResult)) {
        SL_Log_Error(FUNC_ID_ST_FLASH, ERR_TYPE_PARAM, 0U);
        return(retVal);
    }
#endif
#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /* Error if not in privilege mode */
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_FLASH, ERR_TYPE_ENTRY_CON, 2U);
        return(retVal);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((boolean)(TRUE) == SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_FLASH, ERR_TYPE_ENTRY_CON, 3U);
        return(retVal);
    }
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    switch(testType) {
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case FLASH_ECC_ADDR_TAG_REG_MODE:
        regBckupFdiagctrl = flashWREG->FDIAGCTRL;
        /* Read to unfreeze the error address registers */
        flashread = flashWREG->FUNCERRADD;
        fdiagCtrl |= F021F_FDIAGCTRL_DMODE_ADDR_TAG;
        /* clear flash status flags */
        flashWREG->FEDACSTATUS = (F021F_FEDACSTATUS_ADD_TAG_ERR |  F021F_EE_STATUS_D_UNC_ERR_FLG);
        flashWREG->FDIAGCTRL = fdiagCtrl;
        /* address for test in FRAWDATAL*/
        flashWREG->FRAWDATAL = flashWREG->FPRIMADDTAG;

        flashWREG->FREDUADDTAG^=(uint32)~0;
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        flashWREG->FDIAGCTRL |=F021F_FDIAGCTRL_DIAG_TRIG;
        _SL_HoldNClear_nError();
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
        if ((F021F_FEDACSTATUS_ADD_TAG_ERR == (uint32)(flashWREG->FEDACSTATUS & F021F_FEDACSTATUS_ADD_TAG_ERR)) &&
            (GET_ESM_BIT_NUM(ESM_G3ERR_FMC_UNCORR) == (esmREG->SR1[2] & GET_ESM_BIT_NUM(ESM_G3ERR_FMC_UNCORR))))
        {
            /*Clear ESM status */
            esmREG->SR1[2]=GET_ESM_BIT_NUM(ESM_G3ERR_FMC_UNCORR);
            *flash_stResult = ST_PASS;
            /* clear flash status flags */
            flashWREG->FEDACSTATUS = (F021F_FEDACSTATUS_ADD_TAG_ERR |  F021F_EE_STATUS_D_UNC_ERR_FLG);
            /* Read to unfreeze the error address registers */
            flashread = flashWREG->FUNCERRADD;

            flashWREG->FDIAGCTRL = regBckupFdiagctrl;
            flashWREG->FDIAGCTRL = fdiagCtrl;
            /*Load primary address tag(with a different value address in FRAW_DATAL != FPRIMADDTAG )*/
            flashWREG->FPRIMADDTAG^=(uint32)~0;
            /*Load duplicate address tag*/
            flashWREG->FRAWDATAL =flashWREG->FREDUADDTAG;
            flashWREG->FDIAGCTRL |=F021F_FDIAGCTRL_DIAG_TRIG;
            _SL_HoldNClear_nError();
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
            if ((F021F_FEDACSTATUS_ADD_TAG_ERR == (uint32)(flashWREG->FEDACSTATUS & F021F_FEDACSTATUS_ADD_TAG_ERR))&&
                (GET_ESM_BIT_NUM(ESM_G3ERR_FMC_UNCORR) == (esmREG->SR1[2] & GET_ESM_BIT_NUM(ESM_G3ERR_FMC_UNCORR))))
            {
                *flash_stResult = ST_PASS;
            }
            else
            {
                *flash_stResult = ST_FAIL;
            }
        }
        else
        {
            *flash_stResult = ST_FAIL;
            /*if any of the test fails break the loop as the test case has failed*/
        }
        /*Clear ESM status */
        esmREG->SR1[2]=GET_ESM_BIT_NUM(ESM_G3ERR_FMC_UNCORR);
        /* clear flash status flags */
        flashWREG->FEDACSTATUS = (F021F_FEDACSTATUS_ADD_TAG_ERR |  F021F_EE_STATUS_D_UNC_ERR_FLG);
        /* Read to unfreeze the error address registers */
        flashread = flashWREG->FUNCERRADD;
        flashWREG->FDIAGCTRL = regBckupFdiagctrl;
        retVal = TRUE;
        break;
#endif
    case FLASH_ECC_TEST_MODE_1BIT:

    	/*Set test flag*/
    	(void)SL_FLAG_SET(FLASH_ECC_TEST_MODE_1BIT);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /* Backup parity over ride register */
        regBkupFparOvr = flashWREG->FPAROVR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /* Backup the EPC error control register*/
        regBckupErrctrl = epcREG1->EPCCNTRL;
#endif
        /* Backup the FLASH diag control register */
        regBckupFdiagctrl = flashWREG->FDIAGCTRL;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /* Read to unfreeze the error address registers */
        flashread = flashWREG->FCORERRADD;
        flashWREG->FEDACSTATUS= F021F_FEDACSTATUS_ERR_ZERO_FLG;
#endif
        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        regBkupIntEnaSet = esmREG->IESR1;

        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBckupErrInfulence = esmREG->EEPAPR1;

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /* Disable interrupt on CAM/FIFO full */
        if((vimREG->REQMASKCLR3 & VIM_CHANNEL_EPC_FIFO_CAM_FULL) == VIM_CHANNEL_EPC_FIFO_CAM_FULL) {
            BIT_SET(vimREG->REQMASKCLR3, VIM_CHANNEL_EPC_FIFO_CAM_FULL);
            vimCleared = 1;
        }
#endif
        if(!(_sl_fault_injection)){
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
            esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_FMC_CORR);
            esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_FMC_CORR);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_EPC_SERR);
            esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_EPC_SERR);
#endif
        }
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /* Clear any pending CAM full/overflow events */
        BIT_SET(epcREG1->EPCERRSTAT, (EPC_ERR_CAM_FULL | EPC_ERR_CAM_OVRFLW));

        /* Clear any pending FIFO full/overflow events */
        BIT_SET(epcREG1->FIFOFULLSTAT, EPC_FIFO_FULL);
        BIT_SET(epcREG1->OVRFLWSTAT, EPC_FIFO_OVRFLW);

        /* Enable EPC CAM/FIFO full events */
        BIT_SET(epcREG1->EPCCNTRL, EPC_CTRL_FULL_EN);
        /* Enable EPC CAM/FIFO overflow events (also enables correctable error event) */
        BF_SET(epcREG1->EPCCNTRL, EPC_CTRL_SERR_EN, EPC_CTRL_SERR_START, EPC_CTRL_SERR_LENGTH);
#endif

        /* Setup the diag mode for 1 bit error */
        fdiagCtrl |= F021F_FDCTRL_DMODE_TEST_MODE;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        flashWREG->FPAROVR = F021F_FPAROVR_PAR_OVR_ENABLE|F021F_FPAROVR_BUS_PAR_ENABLE;
        flashWREG->FPAROVR |= F021F_FPAROVR_SYN_1BIT_DATA_ECC;
#endif
        flashWREG->FDIAGCTRL = fdiagCtrl;
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /* Prepare the values of data and ecc to be XORed */
        flashWREG->FEMU_DMSW = 0U;
        flashWREG->FEMU_DLSW = 0U;
        flashWREG->FEMU_ECC = F021F_FEMU_ECC_SYN_1BIT_DATA_ECC;
        /* Select the right port for reading from */
        BF_SET(flashWREG->FDIAGCTRL, F021F_FDIAGCTRL_DIAG_BUF_SEL_A, F021F_FDIAGCTRL_DIAG_EN_START, F021F_FDIAGCTRL_DIAG_EN_LENGTH);


        regBkupCamAvail = epcREG1->CAMAVAILSTAT;
#endif

#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        regBkupCamAvail = epcREG1->CAMAVAILSTAT;
#endif
        /* Enable the trigger bit of the diagnostic */
        flashWREG->FDIAGCTRL |= F021F_FDIAGCTRL_DIAG_TRIG;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 58 S MR:14.3 <APPROVED> Comment_16*/
        flashread = *(volatile uint32 *)flashBadECC2;
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
        /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
        if ((F021F_FEDACSTATUS_ERR_ZERO_FLG == (uint32)(flashWREG->FEDACSTATUS & F021F_FEDACSTATUS_ERR_ZERO_FLG))
                && (flashWREG->FCORERRADD == (uint32)0x8u)
                && ((esmREG->SR1[0] & BIT(ESM_G1ERR_FMC_CORR)) == BIT(ESM_G1ERR_FMC_CORR))) {
            /* Clear flash & ESM status registers */
            flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_ERR_ZERO_FLG;
            esmREG->SR1[0] = BIT(ESM_G1ERR_FMC_CORR);
            flashread = flashWREG->FCORERRADD;
            /* Repeat the test for a different address to ensure that the FCORERRADD register is updated correctly */
            /*SAFETYMCUSW 58 S MR:14.3 <APPROVED> Comment_16*/
            flashWREG->FDIAGCTRL |= F021F_FDIAGCTRL_DIAG_TRIG;
            flashread = *(volatile uint32 *)flashBadECC1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
            /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
            if ((F021F_FEDACSTATUS_ERR_ZERO_FLG == (uint32)(flashWREG->FEDACSTATUS & F021F_FEDACSTATUS_ERR_ZERO_FLG))
                    && (flashWREG->FCORERRADD == (uint32)0x0u)
                    && ((esmREG->SR1[0] & BIT(ESM_G1ERR_FMC_CORR)) == BIT(ESM_G1ERR_FMC_CORR))) {
                *flash_stResult = ST_PASS;
            } else {
                *flash_stResult = ST_FAIL;
            }
        } else {
            *flash_stResult = ST_FAIL;
        }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)

        /*SAFETYMCUSW 58 S MR:14.3 <APPROVED> Comment_16*/
        flashread = *(volatile uint32 *)flashBadECC1;

        if (((((epcREG1->EPCERRSTAT & EPC_ERR_CAM_FULL) == EPC_ERR_CAM_FULL) ||
                ((epcREG1->EPCERRSTAT & EPC_ERR_CAM_OVRFLW) == EPC_ERR_CAM_OVRFLW) ||
                (epcREG1->CAMAVAILSTAT != regBkupCamAvail))) &&
                ((((esmREG->SR1[0]) & BIT(ESM_G1ERR_EPC_SERR))) == (BIT(ESM_G1ERR_EPC_SERR)))) {
                    /* Clear ESM status registers */
                    esmREG->SR1[0] = BIT(ESM_G1ERR_EPC_SERR);

            if(epcREG1->CAMAVAILSTAT != regBkupCamAvail) {
                /* Clear the CAM entries */
                /* Set CAM indices to available */
                for(content = 0U; content<32U; content++) {
                    if((volatile uint32 *)(epcREG1->CAM_CONTENT[content] & EPC_CAM_CONTENT_ADDR) == (volatile uint32 *)(flashBadECC1 & EPC_CAM_CONTENT_ADDR)) {
                        break;
                    }
                }
                /* Calculate the index location */
                index = content/4;
                index = content - index*4;
                content = content/4;
                BF_SET(epcREG1->CAM_INDEX[content], EPC_CAM_INDEX_CLEAR, EPC_CAM_INDEX_START(index), EPC_CAM_INDEX_LENGTH);
            }

            *flash_stResult = ST_PASS;

        } else {
            *flash_stResult = ST_FAIL;
        }
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /* Anyways clear flash & ESM status registers */
        flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_ERR_ZERO_FLG;
        esmREG->SR1[0] = BIT(ESM_G1ERR_FMC_CORR);
        flashread = flashWREG->FCORERRADD;
#endif
        /* Clear the diag mode settings */
        flashWREG->FDIAGCTRL &=~(F021F_FDCTRL_DMODE_TEST_MODE|F021F_FDIAGCTRL_DIAG_EN_MASK); /* Clear Diagnostics enable key */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        flashWREG->FPAROVR = regBkupFparOvr;
#endif
        flashWREG->FDIAGCTRL = regBckupFdiagctrl;

        /*Restore grp1 esm interrupt enable*/
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)

        /* Restore EPC registers*/
        epcREG1->EPCCNTRL = regBckupErrctrl;
		epcREG1->CAMAVAILSTAT = regBkupCamAvail;

        if(vimCleared == 1) {
            BIT_SET(vimREG->REQMASKSET3, VIM_CHANNEL_EPC_FIFO_CAM_FULL);
        }

#endif
    	/*Clear test flag*/
    	SL_FLAG_CLEAR(FLASH_ECC_TEST_MODE_1BIT);

        retVal = TRUE; /* Tests were run */
        break;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case FLASH_ADDRESS_PARITY_SELF_TEST:
    case FLASH_ADDRESS_PARITY_FAULT_INJECT:
        /* Backup registers */
        regBkupFparOvr = flashWREG->FPAROVR;
        regBckupFdiagctrl = flashWREG->FDIAGCTRL;
        /* Read to unfreeze the error address registers */
        flashread = flashWREG->FUNCERRADD;
        flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_ADD_PAR_ERR;
        fdiagCtrl |= F021F_FDCTRL_DMODE_TEST_MODE;
        flashWREG->FPAROVR = F021F_FPAROVR_PAR_OVR_ENABLE|F021F_FPAROVR_BUS_PAR_ENABLE;
        flashWREG->FDIAGCTRL = fdiagCtrl;
        /*flag is set to indicate the current test which is ongoing and
        These flags are used in the sl_esm.c so as to mask the esm callback*/
        if(testType == FLASH_ADDRESS_PARITY_SELF_TEST)
        {
        	(void)SL_FLAG_SET(FLASH_ADDRESS_PARITY_SELF_TEST);
        }
        else
        {
        	(void)SL_FLAG_SET(FLASH_ADDRESS_PARITY_FAULT_INJECT);
        }
        flashWREG->FPAROVR |= F021F_FPAROVR_ADD_INV_PAR;
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        _SL_Barrier_Data_Access();
        flashread = *(volatile uint32 *)flashBadECC1;

        if(FLASH_ADDRESS_PARITY_FAULT_INJECT != testType)
        {
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
            if ((F021F_FEDACSTATUS_ADD_PAR_ERR == (uint32)(flashWREG->FEDACSTATUS & F021F_FEDACSTATUS_ADD_PAR_ERR))) {
                flashWREG->FPAROVR = regBkupFparOvr;
              _SL_HoldNClear_nError(); /*Clear nError */
                /* Clear flash & ESM status registers */
                flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_ADD_PAR_ERR;
                esmREG->SR1[1] = GET_ESM_BIT_NUM(ESM_G2ERR_FMC_UNCORR);
                flashread = flashWREG->FUNCERRADD;
                *flash_stResult = ST_PASS;
            }else {
                *flash_stResult = ST_FAIL;
            }
            /* Anyways clear flash & ESM status registers */
            flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_ADD_PAR_ERR;
            esmREG->SR1[1] =  GET_ESM_BIT_NUM(ESM_G2ERR_FMC_UNCORR);
            esmREG->SSR2 = GET_ESM_BIT_NUM(ESM_G2ERR_FMC_UNCORR);
            flashread = flashWREG->FUNCERRADD;

            /* Clear the diag mode settings */
            flashWREG->FPAROVR = regBkupFparOvr;
            flashWREG->FDIAGCTRL = regBckupFdiagctrl;
        }
        retVal = TRUE; /* Tests were run */
        /*clear the flags which indicate tests ongoing*/
        if(testType == FLASH_ADDRESS_PARITY_SELF_TEST)
        {
        	SL_FLAG_CLEAR(FLASH_ADDRESS_PARITY_SELF_TEST);
        }
        else
        {
        	SL_FLAG_CLEAR(FLASH_ADDRESS_PARITY_FAULT_INJECT);
        }
        break;
#endif
    case FLASH_ECC_TEST_MODE_2BIT:
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case FLASH_ECC_TEST_MODE_2BIT_FAULT_INJECT:
    case FLASH_ADDRESS_ECC_SELF_TEST:
    case FLASH_ADDRESS_ECC_FAULT_INJECT:

        /* Backup parity over ride register */
        regBkupFparOvr = flashWREG->FPAROVR;
#endif
        regBckupFdiagctrl = flashWREG->FDIAGCTRL;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /* Read to unfreeze the error address registers */
        flashread = flashWREG->FUNCERRADD;
        flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_B1_UNC_ERR;
        flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_ERR_ZERO_FLG;
#endif

        /* Setup the diag mode for 2 bit error */
        fdiagCtrl |= F021F_FDCTRL_DMODE_TEST_MODE;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        flashWREG->FPAROVR = F021F_FPAROVR_PAR_OVR_ENABLE|F021F_FPAROVR_BUS_PAR_ENABLE;
        if((FLASH_ADDRESS_ECC_SELF_TEST == testType)||(FLASH_ADDRESS_ECC_FAULT_INJECT == testType ))
        {
            flashWREG->FPAROVR |= F021F_FPAROVR_SYN_ADDRESS_ECC;
        }
        else /* FLASH_ECC_TEST_MODE_2BIT || FLASH_ECC_TEST_MODE_2BIT_FAULT_INJECT */
        {
            flashWREG->FPAROVR |= F021F_FPAROVR_SYN_2BIT_DATA_ECC;
        }
#endif
        flashWREG->FDIAGCTRL = fdiagCtrl;
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        flashWREG->FEMU_DMSW = 0U;
        flashWREG->FEMU_DLSW = 0U;
        flashWREG->FEMU_ECC = F021F_FEMU_ECC_SYN_2BIT_DATA_ECC;
        BF_SET(flashWREG->FDIAGCTRL, F021F_FDIAGCTRL_DIAG_BUF_SEL_A, F021F_FDIAGCTRL_DIAG_EN_START, F021F_FDIAGCTRL_DIAG_EN_LENGTH);
#endif


        /*flag is set to indicate the current test which is ongoing and
        These flags are used in the sl_esm.c so as to mask the esm callback*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        if ((FLASH_ECC_TEST_MODE_2BIT_FAULT_INJECT != testType)&&(FLASH_ADDRESS_ECC_FAULT_INJECT != testType)) 
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        if (FLASH_ECC_TEST_MODE_2BIT == testType) 
#endif
        {
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        	(void)SL_FLAG_SET(FLASH_ECC_TEST_MODE_2BIT);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            /* Test must run from RAM - cannot execute the function SL_FLAG_SET
             * from Flash, so copying the functionality inline.  */
            extern boolean sl_priv_flag_set[];
            sl_priv_flag_set[FLASH_ECC_TEST_MODE_2BIT-TESTTYPE_MIN] = TRUE;
#endif

#if FUNCTION_PROFILING_ENABLED
		    SL_Record_Errorcreationtick(testType);
#endif

        	flashWREG->FDIAGCTRL |= F021F_FDIAGCTRL_DIAG_TRIG;
		
            /*SAFETYMCUSW 58 S MR:14.3 <APPROVED> Comment_16*/
            flashread = *(volatile uint32 *)flashBadECC2;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
            /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
            if ((F021F_FEDACSTATUS_B1_UNC_ERR == (uint32)(flashWREG->FEDACSTATUS & F021F_FEDACSTATUS_B1_UNC_ERR))
                    && (flashWREG->FUNCERRADD == (uint32)0x8u)
                    && (BIT(ESM_G3ERR_FMC_UNCORR) == (esmREG->SR1[2] & BIT(ESM_G3ERR_FMC_UNCORR)))) {
                _SL_HoldNClear_nError(); /* Clear nError */
                /* Clear flash & ESM status registers */
                flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_B1_UNC_ERR;
                esmREG->SR1[2] = BIT(ESM_G3ERR_FMC_UNCORR);

                /* Repeat the test for a different address to ensure that the FCORERRADD register is updated correctly */
                /*SAFETYMCUSW 58 S MR:14.3 <APPROVED> Comment_16*/
                flashWREG->FDIAGCTRL = fdiagCtrl;
                flashWREG->FDIAGCTRL |= F021F_FDIAGCTRL_DIAG_TRIG;
                flashread = *(volatile uint32 *)flashBadECC1;
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                if ((F021F_FEDACSTATUS_B1_UNC_ERR == (uint32)(flashWREG->FEDACSTATUS & F021F_FEDACSTATUS_B1_UNC_ERR))
                        && (flashWREG->FUNCERRADD == (uint32)0x0u) && (BIT(ESM_G3ERR_FMC_UNCORR) == (esmREG->SR1[2] & BIT(ESM_G3ERR_FMC_UNCORR)))) {
                    _SL_HoldNClear_nError();
                    *flash_stResult = ST_PASS;
                } else {
                    *flash_stResult = ST_FAIL;
                }
            } else {
                *flash_stResult = ST_FAIL;
            }
			/* Anyways clear flash & ESM status registers */
			#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
			flashWREG->FEDACSTATUS = F021F_FEDACSTATUS_B1_UNC_ERR;
			flashread = flashWREG->FUNCERRADD;
			#endif
            esmREG->SR1[2] =  BIT(ESM_G3ERR_FMC_UNCORR);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            if (BIT(ESM_G2ERR_CR5_BUSERR) == (esmREG->SSR2 & BIT(ESM_G2ERR_CR5_BUSERR))) {
                /* Clear nError */
                _SL_HoldNClear_nError();
                esmREG->SSR2 = BIT(ESM_G2ERR_CR5_BUSERR);
                *flash_stResult = ST_PASS;
            } else {
                *flash_stResult = ST_FAIL;
            }
#endif
            /* Clear the diag mode settings */
			#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
			flashWREG->FPAROVR = regBkupFparOvr;
			#endif
            flashWREG->FDIAGCTRL = regBckupFdiagctrl;
            /* Clear the flags which indicate tests ongoing*/
            SL_FLAG_CLEAR(FLASH_ECC_TEST_MODE_2BIT);
        }
        /* Tests were run */
        retVal = TRUE;
        break;

    default:
        retVal = FALSE;
        break;
    }
    /* If the test was fault injection and not self test, then do not mask the call back & clear the *
     *  FAULT_INJECTION flag for subsequent runs*/
    _sl_fault_injection=FALSE;

#if(FUNC_RESULT_LOG_ENABLED == 1)
    /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_14*/
    SL_Log_Result(FUNC_ID_ST_FLASH, testType, *flash_stResult , 0U);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define feeBadECC1        (*(volatile uint32 *)0xF0200000u)
#define feeBadECC2        (*(volatile uint32 *)0xF0200008u)
#define ADDR_DATA_MSW                        (0xF0200004u)
#define ADDR_DATA_LSW                        (0xF0200000u)
#define FEE_TEST_DATA                        0u
#define FEE_ERROR_POS                        2u
#define FEE_SYNDROME_BIT2_SINGLE_BIT_ERROR    0xD3u
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_FEE (SL_SelfTestType testType, boolean bMode, SL_SelfTest_Result* fee_stResult)
{
    boolean retVal = FALSE;
    uint32 fdiagCtrl = F021F_FDIAGCTRL_DIAG_EN_KEY;
    uint32 regBckupFdiagctrl, regBkupIntEnaSet, regBckupErrInfulence;
    uint8 ecc;
    uint8 syndrome=0U;
    volatile boolean _sl_fault_injection = FALSE;
    volatile uint32 flashread = 0u;

#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif
    /* If fault inject set global variable to flag to the ESM handler that it is a fault injection */
    if((FEE_ECC_TEST_MODE_1BIT_FAULT_INJECT == testType)||
        (FEE_ECC_TEST_MODE_2BIT_FAULT_INJECT == testType)){
        _sl_fault_injection=TRUE;
    }
#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in rage */
    /*LDRA_INSPECTWINDOW 50 */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if ((_SELFTEST_FEEECC_MIN > testType) || (_SELFTEST_FEEECC_MAX < testType) || !CHECK_RANGE_RAM_PTR(fee_stResult)) {
        SL_Log_Error(FUNC_ID_ST_FLASH, ERR_TYPE_PARAM, 0U);
        return(retVal);
    }
#endif
#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /* Error if not in privilege mode */
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_FLASH, ERR_TYPE_ENTRY_CON, 2U);
        return(retVal);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((boolean)(TRUE) == SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_FLASH, ERR_TYPE_ENTRY_CON, 3U);
        return(retVal);
    }
#endif
    switch(testType) {

    case FEE_ECC_DATA_CORR_MODE:
    case FEE_ECC_TEST_MODE_1BIT:
    case FEE_ECC_TEST_MODE_1BIT_FAULT_INJECT:
    case FEE_ECC_TEST_MODE_2BIT:
    case FEE_ECC_TEST_MODE_2BIT_FAULT_INJECT:

    	if(FEE_ECC_TEST_MODE_1BIT == testType)
    	{
    		(void)SL_FLAG_SET(FEE_ECC_TEST_MODE_1BIT);
    	}

        regBckupFdiagctrl = flashWREG->FDIAGCTRL;
        fdiagCtrl |= F021F_FDCTRL_DMODE_DATA_CORR;
        /*  Clear all the status errors*/
        flashWREG->EESTATUS |= ~0;
        /* Clear this to unfree error reporting */
        flashread=flashWREG->EEUNCERRADD;
        flashread=flashWREG->EECORRERRADD;

        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
        status functions to get the pass/failure information*/        
        regBkupIntEnaSet = esmREG->IESR4;
        regBckupErrInfulence = esmREG->IEPSR4;
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if(!(_sl_fault_injection)){
            esmREG->IECR4 = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_CORRERR)|GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
            esmREG->IEPCR4 = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_CORRERR)|GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
        }

        /* Load FEMU_XX regs in order to generate ecc */
        flashWREG->FEMUADDR = (uint32)ADDR_DATA_MSW;
        flashWREG->FEMUDMSW = FEE_TEST_DATA;
        flashWREG->FEMUDLSW = FEE_TEST_DATA;

        /*ecc for the correct data*/
        ecc = flashWREG->FEMUECC;
        /* Load corrupt data in FEMU_XXX regs single bit error */
        flashWREG->FEMUDMSW = FEE_TEST_DATA;
        /* Flip a bit and generate single bit error*/
        if((FEE_ECC_TEST_MODE_2BIT_FAULT_INJECT == testType) || (FEE_ECC_TEST_MODE_2BIT == testType)){
            flashWREG->FEMUDLSW = FEE_TEST_DATA ^ (uint32)(0x3U<<(FEE_ERROR_POS));
        }else{
            flashWREG->FEMUDLSW = FEE_TEST_DATA ^ BIT(FEE_ERROR_POS);
        }

        /* Set SECDED mode as fee as only fee is supported for this mode
         * and enable the Diagnosis*/
        flashWREG->FDIAGCTRL = fdiagCtrl|F021F_FDIAGCTRL_DIAGECCSEL_FEE;
        flashWREG->FEMUECC = ecc;
        /* Trigger the test */
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        flashWREG->FDIAGCTRL |=F021F_FDIAGCTRL_DIAG_TRIG;

        if((FEE_ECC_DATA_CORR_MODE == testType) || (FEE_ECC_TEST_MODE_1BIT == testType)){
            /* Check the error flags, if single bit error and only one of ERR_ONE_FLG or ERR_ZERO_FLG is set */
        	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
            if((F021F_EE_STATUS_D_CORR_ERR_FLG == (uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_D_CORR_ERR_FLG)) &&
                    (GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_CORRERR) == (esmREG->SR4[0] & GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_CORRERR))) &&
                    (((F021F_EE_STATUS_ERR_ZERO_FLG ==
                    		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                    		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                    		/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                    		(uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_ERR_ZERO_FLG))    &&
                        (!(F021F_EE_STATUS_ERR_ONE_FLG ==
                        		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                        		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                        		/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                        		(uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_ERR_ONE_FLG))))
                        ||
                        ((!(F021F_EE_STATUS_ERR_ZERO_FLG ==
                        		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                        		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                        		/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                        		(uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_ERR_ZERO_FLG)))    &&
                                (F021F_EE_STATUS_ERR_ONE_FLG ==
                                		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                                		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                                		/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                                		(uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_ERR_ONE_FLG)))) &&
                                        (FEE_ERROR_POS ==
                                        		/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
                                        		/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                                        		/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                                        		(uint32)(flashWREG->EECORRERRPOS & F021F_EECORRERRPOS_MASK))){
                    *fee_stResult = ST_PASS;
                }
            else{
                *fee_stResult = ST_FAIL;
            }
            /*Clear the status set during the test*/
            flashWREG->EESTATUS |= F021F_EE_STATUS_D_CORR_ERR_FLG|
                                    F021F_EE_STATUS_ERR_ZERO_FLG|
                                    F021F_EE_STATUS_ERR_ONE_FLG;
            esmREG->SR4[0] = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_CORRERR);
        }
        else if(FEE_ECC_TEST_MODE_2BIT == testType){
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
        	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
        	/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
            if((F021F_EE_STATUS_D_UNC_ERR_FLG == (uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_D_UNC_ERR_FLG)) &&
                    ((F021F_EE_STATUS_UNC_ERR_FLG ==
                    		/*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
                    		(uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_UNC_ERR_FLG)))
                    && (flashWREG->EEUNCERRADD == ((uint32)ADDR_DATA_MSW & F021F_EEUNCERRADD_MASK))
                    && (GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR) == (esmREG->SR4[0] & GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR)))){
                *fee_stResult = ST_PASS;
                /*Clear the status set during the test*/
                flashWREG->EESTATUS |= F021F_EE_STATUS_D_UNC_ERR_FLG|
                                        F021F_EE_STATUS_UNC_ERR_FLG;
                esmREG->SR4[0] = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
            }
            else{
                *fee_stResult = ST_FAIL;
            }
        }
        else
        {
            *fee_stResult = ST_FAIL;
        }
        retVal = TRUE;
        /* Clear the diag mode settings */
        flashWREG->FDIAGCTRL &=~(F021F_FDCTRL_DMODE_DATA_CORR|F021F_FDIAGCTRL_DIAG_EN_MASK); /* Clear Diagnostics enable key */
        flashWREG->FDIAGCTRL = regBckupFdiagctrl;
        /* Read and unfreeze error reporting */
        flashread=flashWREG->EEUNCERRADD;
        flashread=flashWREG->EECORRERRADD;

        /*Restore grp1 esm interrupt enable*/
        esmREG->IESR4 = regBkupIntEnaSet;
        esmREG->IEPSR4 = regBckupErrInfulence;

    	if(FEE_ECC_TEST_MODE_1BIT == testType)
    	{
    		SL_FLAG_CLEAR(FEE_ECC_TEST_MODE_1BIT);
    	}

        break;

    case FEE_ECC_SYN_REPORT_MODE:
        regBckupFdiagctrl = flashWREG->FDIAGCTRL;
        /*  Clear all the status errors*/
        flashWREG->EESTATUS |= ~0;
        /* Clear this to unfree error reporting */
        flashread=flashWREG->EEUNCERRADD;
        /*load FEMU_XX regs in order to generate ecc and use it for next operations*/
        flashWREG->FEMUADDR = (uint32)ADDR_DATA_MSW;
        fdiagCtrl |= F021F_FDIAGCTRL_DMODE_SYN_RPT;
        flashWREG->FEMUDMSW = FEE_TEST_DATA;
        flashWREG->FEMUDLSW = FEE_TEST_DATA;
        /* Store ECC*/
        ecc = flashWREG->FEMUECC;

        flashWREG->FEMUDMSW = FEE_TEST_DATA;
        flashWREG->FEMUDLSW = FEE_TEST_DATA ^ BIT(FEE_ERROR_POS);
        /* Calculate the Syndrome value */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        syndrome = ecc ^ flashWREG->FEMUECC;

        /* Set the data back to the original data */
        flashWREG->FEMUDMSW = FEE_TEST_DATA;
        flashWREG->FEMUDLSW = FEE_TEST_DATA;
        /* Set the FDIAGCTRL register*/
        /* Set SECDED mode as fee as only fee is supported for this mode */
        flashWREG->FDIAGCTRL = fdiagCtrl|F021F_FDIAGCTRL_DIAGECCSEL_FEE;
        /* Introduce the single bit error corresponding to the calculated syndrome */
        flashWREG->FEMUDMSW = FEE_TEST_DATA;
        flashWREG->FEMUDLSW = FEE_TEST_DATA ^ BIT(FEE_ERROR_POS);
        /* Trigger the diagnosis */
        flashWREG->FDIAGCTRL |=(F021F_FDIAGCTRL_DIAG_TRIG);
        /* Now the FEMUECC would contain the actual syndrome, as the mode is FLASH_ECC_SYN_REPORT_MODE
         * Check this value to the calculated syndrome, if it is match the diagnosis passes */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if((syndrome) == flashWREG->FEMUECC){
            *fee_stResult = ST_PASS;
        }
        else
        {
            *fee_stResult = ST_FAIL;
        }
        /* Clear the diag mode settings */
        flashWREG->FDIAGCTRL &=~(F021F_FDCTRL_DMODE_DATA_CORR|F021F_FDIAGCTRL_DIAG_EN_MASK); /* Clear Diagnostics enable key */
        flashWREG->FDIAGCTRL = regBckupFdiagctrl;
        /*Clear the status set during the test*/
        flashWREG->EESTATUS |= ~0;
        retVal = TRUE;
        /* Read and unfreeze error reporting */
        flashread=flashWREG->EEUNCERRADD;
        break;

    case FEE_ECC_MALFUNCTION_MODE1:
        regBckupFdiagctrl = flashWREG->FDIAGCTRL;
        /* Clear this to unfree error reporting */
        flashread=flashWREG->EEUNCERRADD;

        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
        status functions to get the pass/failure information*/        
        regBkupIntEnaSet = esmREG->IESR4;
        regBckupErrInfulence = esmREG->IEPSR4;
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if(!(_sl_fault_injection)){
            esmREG->IECR4 = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
            esmREG->IEPCR4 = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
        }

        /* Set diagmode and SECDED mode as fee as only fee is supported for this mode */
        fdiagCtrl |= F021F_FDIAGCTRL_DMODE_MALFUNC1|F021F_FDIAGCTRL_DIAGECCSEL_FEE;
        /*  Clear all the status errors*/
        flashWREG->EESTATUS |= (uint32)~0;

        /* Enable EDACMODE and put a non zero value is FRAWECC*/
        flashWREG->FDIAGCTRL = fdiagCtrl;
        flashWREG->FRAWECC = 1U;
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        /* Trigger the test */
        flashWREG->FDIAGCTRL |=F021F_FDIAGCTRL_DIAG_TRIG;
        /* Check error */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
        if((F021F_EE_STATUS_CME == (uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_CME))&&
            (GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR) == (esmREG->SR4[0] & GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR)))){
            *fee_stResult = ST_PASS;
        }
        else
        {
            *fee_stResult = ST_FAIL;
        }
        /* Clear ESM bits */
        esmREG->SR4[0] = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
        /* Clear the diag mode settings */
        flashWREG->FDIAGCTRL &=~(F021F_FDCTRL_DMODE_DATA_CORR|F021F_FDIAGCTRL_DIAG_EN_MASK); /* Clear Diagnostics enable key */
        flashWREG->FDIAGCTRL = regBckupFdiagctrl;
        /*Clear the status set during the test*/
        flashWREG->EESTATUS |= ~0;
        retVal = TRUE;
        /* Read and unfreeze error reporting */
        flashread=flashWREG->EEUNCERRADD;

        /*Restore grp1 esm interrupt enable*/
        esmREG->IESR4 = regBkupIntEnaSet;
        esmREG->IEPSR4 = regBckupErrInfulence;
        break;

    case FEE_ECC_MALFUNCTION_MODE2:
        regBckupFdiagctrl = flashWREG->FDIAGCTRL;
        flashWREG->FDIAGCTRL=0U;
        /* Read and unfreeze error reporting */
        flashread=flashWREG->EEUNCERRADD;

        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
        status functions to get the pass/failure information*/        
        regBkupIntEnaSet = esmREG->IESR4;
        regBckupErrInfulence = esmREG->IEPSR4;
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if(!(_sl_fault_injection)){
            esmREG->IECR4= GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
            esmREG->IEPCR4 = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
        }

        /* Set diagmode and SECDED mode as fee as only fee is supported for this mode */
        flashWREG->FDIAGCTRL|= F021F_FDIAGCTRL_DIAGECCSEL_FEE;
        flashWREG->FDIAGCTRL|= F021F_FDIAGCTRL_DMODE_MALFUNC2;

        /*  Clear all the status errors*/
        flashWREG->EESTATUS |= ~0;

        /* Enable EDACMODE*/
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(flashWREG->FDIAGCTRL, F021F_FDIAGCTRL_DIAG_EN_KEY);
        /* Invert data for the ecc calculation and put zero in FRAWECC */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        flashWREG->FRAWDATAH = ~flashWREG->FRAWDATAH;
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        flashWREG->FRAWDATAL = ~flashWREG->FRAWDATAL;
        flashWREG->FRAWECC = 0U;
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        /* Trigger the test */
        flashWREG->FDIAGCTRL |=F021F_FDIAGCTRL_DIAG_TRIG;

        /* Check Error */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_25*/
        if((F021F_EE_STATUS_CME == (uint32)(flashWREG->EESTATUS & F021F_EE_STATUS_CME))&&
                (GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR) == (esmREG->SR4[0] & GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR)))){
            *fee_stResult = ST_PASS;
        }
        else
        {
            *fee_stResult = ST_FAIL;
        }
        /* Clear ESM bits */
        esmREG->SR4[0] = GET_ESM_BIT_NUM(ESM_G1ERR_EEPROM_UNCORRERR);
        /* Clear the diag mode settings */
        flashWREG->FDIAGCTRL &=~(F021F_FDCTRL_DMODE_DATA_CORR|F021F_FDIAGCTRL_DIAG_EN_MASK); /* Clear Diagnostics enable key */
        flashWREG->FDIAGCTRL = regBckupFdiagctrl;
        /*Clear the status set during the test*/
        flashWREG->EESTATUS |= ~0;
        retVal = TRUE;
        /* Read and unfreeze error reporting */
        flashread=flashWREG->EEUNCERRADD;
        /*Restore grp1 esm interrupt enable*/
        esmREG->IESR4 = regBkupIntEnaSet;
        esmREG->IEPSR4 = regBckupErrInfulence;
        break;
    default:
        retVal = FALSE;
        break;
    }
    /* If the test was fault injection and not self test, then do not mask the call back & clear the *
     *  FAULT_INJECTION flag for subsequent runs*/
    _sl_fault_injection=FALSE;

#if(FUNC_RESULT_LOG_ENABLED == 1)
    /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> Comment_14*/
    SL_Log_Result(FUNC_ID_ST_FLASH, testType, *fee_stResult , 0U);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}
#endif

/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
/*SAFETYMCUSW 62 D MR: 16.7 <APPROVED> Comment_17*/
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
boolean SL_SelfTest_PBIST (register SL_SelfTestType testType, register uint64 ramGroup, register uint32 algoInfo)
#else
boolean SL_SelfTest_PBIST (register SL_SelfTestType testType, register uint32 ramGroup, register uint32 algoInfo)
#endif
{
    volatile uint32 tempVal; /*volatile to prevent from being optimized out*/
    register boolean retVal = FALSE;

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (0 != pbistREG->PACT)
    {
        SL_Log_Error(FUNC_ID_ST_PBIST, ERR_TYPE_ENTRY_CON, 1u);
        retVal = FALSE;
        return retVal;
    }

    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_PBIST, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return retVal;
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((boolean)(TRUE) == SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_PBIST, ERR_TYPE_ENTRY_CON, 2u);
        return(retVal);
    }
#endif

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /*SAFETYMCUSW 437 S MR: 17.3 <APPROVED> Comment_19*/
    if ((_SELFTEST_PBIST_MIN > testType) || (_SELFTEST_PBIST_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_PBIST, ERR_TYPE_PARAM, 0u);
        retVal = FALSE;
        return retVal;
    }
    if (ALL_PBIST_RAM_GROUPS != (ALL_PBIST_RAM_GROUPS | ramGroup)) {
        SL_Log_Error(FUNC_ID_ST_PBIST, ERR_TYPE_PARAM, 1u);
        retVal = FALSE;
        return retVal;
    }
    if (ALL_PBISALGO != (ALL_PBISALGO | algoInfo)) {
        SL_Log_Error(FUNC_ID_ST_PBIST, ERR_TYPE_PARAM, 2u);
        retVal = FALSE;
        return retVal;
    }
#endif
    switch(testType) {
        case PBIST_EXECUTE:
        case PBIST_EXECUTE_OVERRIDE:
            /* PBIST ROM clock frequency = HCLK frequency /2 */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            BF_SET(systemREG1->MSTGCR, ROM_CLK_SRC_HCKL_DIV_2, BF_ROM_DIV_START, BF_ROM_DIV_LENGTH);
            /* Enable PBIST controller */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            systemREG1->MSINENA = SYSREG1_MSINENA_MSIENA;
            /* Enable PBIST Contrlller */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            BF_SET(systemREG1->MSTGCR, ENABLE_MEM_STEST_CTRL, BF_MSTGENA_START, BF_MSTGENA_LENGTH);
            /* Wait for 32 VBUS Clocks */
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            for (tempVal=0u; tempVal<(VBUS_CLK_CYCLES + (VBUS_CLK_CYCLES * 1u)); tempVal++)
            {

            }
            /* Enable PBIST clocks and ROM clock */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            pbistREG->PACT = (PBIST_PACT_PACT1 | PBIST_PACT_PACT0);
            /* Select the algorithms */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            pbistREG->ALGO = algoInfo;
            if (PBIST_EXECUTE_OVERRIDE == testType) {
                /* Override RINFOx with ROM contents */
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                pbistREG->OVER = ENABLE_OVERRIDE;
            }
            else {
                /* Select RAM groups */
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                pbistREG->RINFOL = (uint32)(ramGroup&0xFFFFFFFFu);
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                pbistREG->RINFOU = 0x00000000u; /* No RAM group between 33-64 */
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
                pbistREG->RINFOU = (uint32)((ramGroup>>32)&0xFFFFFFFFu); /* No RAM group between 33-64 */
#endif
                /* Do not override RINFOx with ROM contents */
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                 pbistREG->OVER = DISABLE_OVERRIDE;
            }
            /* Algorithm code is loaded from ROM */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            pbistREG->ROM = ALG_RAM_FROM_ROM;
            /* Start PBIST */
            /* Note: If executing on TCM RAM, Stack Contents are corrupted, so be careful with return data */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            pbistREG->DLR = (PBIST_DLR_DLR4 | PBIST_DLR_DLR2);
            retVal = TRUE;
         break;
        default:
        	/* nothing here - comment to avoid misra-c warning */
            break;
    }
#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_PBIST, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
    return(retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_Status_PBIST(SL_PBIST_FailInfo* param1)
{
	boolean retVal = FALSE;
	boolean tmp;
#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /*LDRA_INSPECTWINDOW 50 */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if (FALSE == CHECK_RANGE_RAM_PTR(param1)) {
        SL_Log_Error(FUNC_ID_ST_PBIST_STATUS, ERR_TYPE_PARAM, 0u);
        return retVal;
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_PBIST_STATUS, ERR_TYPE_ENTRY_CON, 0u);
        return retVal;
    }
#endif
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if ((systemREG1->MSTCGSTAT & 0x1u) == 0x0u)
        {
           return retVal;
        }
        else
        {
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
        	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            if ((pbistREG->FSRF0 == (uint32)0u) && (pbistREG->FSRF1 == (uint32)0u)) {
                /* Return PASS*/
                param1->stResult             = ST_PASS;
                param1->port0FailCount         = 0u;
                param1->port1FailCount         = 0u;
                param1->port0FailAddress    = 0u;
                param1->port1FailAddress    = 0u;
                param1->port0FailData       = 0u;
                param1->port1FailData       = 0u;
            } else {
                /* Store the failure data in OUT params */
                param1->stResult            = ST_FAIL;
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                /*SAFETYMCUSW 439 S MR: 11.3 <APPROVED> "The base address of the peripheral registers are actually macros" */
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                param1->port0FailCount      = (uint8) (pbistREG->FSRC0 & 0xFFu);
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                param1->port1FailCount      = (uint8) (pbistREG->FSRC1 & 0xFFu);
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                param1->port0FailAddress    = (uint16) (pbistREG->FSRA0 & 0xFFFFu);
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                param1->port1FailAddress    = (uint16) (pbistREG->FSRA1 & 0xFFFFu);
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                param1->port0FailData       = pbistREG->FSRDL0;
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                param1->port1FailData       = pbistREG->FSRDL1;
            }
        /* Stop the PBIST - storing retval in tmp to avoid misra-c violation */
        tmp = SL_SelfTest_PBIST_StopExec();

        tmp = tmp; /* to avoid compiler warning */

#if(FUNC_RESULT_LOG_ENABLED == 1)
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
        SL_Log_Result(FUNC_ID_ST_PBIST_STATUS, (SL_SelfTestType)0, param1->stResult, 0u);
#endif
        }
        retVal = TRUE;
    return retVal;
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_WaitCompletion_PBIST(void)
{
	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	
	register boolean retVal = FALSE;
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if ((systemREG1->MSTCGSTAT & 0x1u) != 0x0u) {
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
        systemREG1->MSTCGSTAT = 0x1u;
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
		retVal = TRUE;
    } else {
    	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	
	retVal = FALSE;
    }
    return retVal;
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
boolean SL_SelfTest_PBIST_StopExec(void)
{
	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	register boolean retVal = TRUE;
    /* Stop the PBIST */
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    pbistREG->PACT = 0x0u;
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    BF_SET(systemREG1->MSTGCR, DISABLE_MEM_STEST_CTRL, BF_MINITGENA_START, BF_MINITGENA_LENGTH);
    return(retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
/****************************************************************************************************
* Identifier:   SCOD-AMC01-315 (Trace to: SLD-AMC01-315)
* Function:     SL_SelfTest_STC
* Description:  STC诊断
* Input:        testType 测试类型
*               bmode    测试模式
*               config   配置参数
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
boolean SL_SelfTest_STC(register SL_SelfTestType testType, register boolean bMode, register SL_STC_Config* config)
{
    register uint32 tempVal = 0U;
    boolean retval = FALSE;

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in rage */
    /*LDRA_INSPECTWINDOW 50 */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 437 S MR: 17.3 <APPROVED> Comment_19*/
    if ((_SELFTEST_STC_MIN > testType) || (_SELFTEST_STC_MAX < testType)||
            !CHECK_RANGE_RAM_PTR(config)) {
        SL_Log_Error(FUNC_ID_ST_STC, ERR_TYPE_PARAM, 0U);
        retval = FALSE;
        return(retval);
    }
    /* Error if interval count is greater than supported */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    if(config->intervalCount>STC_MAX_INTERVALS)
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    if(config->intervalCount>STC1_MAX_INTERVALS)
#endif
    {
        SL_Log_Error(FUNC_ID_ST_STC, ERR_TYPE_PARAM, 0U);
        retval = FALSE;
        return(retval);
    }
    if(config->stcClockDiv>STC_MAX_CLOCK_DIV){
        SL_Log_Error(FUNC_ID_ST_STC, ERR_TYPE_PARAM, 0U);
        retval = FALSE;
        return(retval);
    }
#endif
#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /* Error if not in privilege mode */
        if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
            SL_Log_Error(FUNC_ID_ST_STC, ERR_TYPE_ENTRY_CON, 2U);
            retval = FALSE;
            return(retval);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((boolean)(TRUE) == SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_STC, ERR_TYPE_ENTRY_CON, 3U);
        return(retval);
    }
#endif

    /* Configure the clock divider */
    systemREG2->STCCLKDIV = ((uint32)(config->stcClockDiv & (uint32)0x07u) << 24u);

    /* Configure the interval count & restart/continue */
    tempVal = 0U;
    tempVal |= ((uint32)config->intervalCount << STC_STCGCR0_INTCOUNT_START);
    if (TRUE == config->restartInterval0) {
        tempVal |= STC_STCGCR0_RS_CNT;
    }
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    sl_stcREG->STCGCR0 = tempVal;

    /* If in compare selfcheck (fault injection) mode then setup STCSCSCR */
    if (STC_COMPARE_SELFCHECK == testType) {
        sl_stcREG->STCSCSCR = (uint32)(STC_STCSCSCR_FAULT_INS|STC_STCSCSCR_SELF_CHECK_KEY);
    }

    /* Setup the timeout value */
    sl_stcREG->STCTPR = config->timeoutCounter;

    /* Enable STC run */
    sl_stcREG->STCGCR1 = STC_STCGCR1_STC_ENA;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)

    stcREG1->STCGCR0 = tempVal;

    /* If in compare selfcheck (fault injection) mode then setup STCSCSCR */
    if (STC1_COMPARE_SELFCHECK == testType) {
        stcREG1->STCSCSCR = (uint32)(STC_STCSCSCR_FAULT_INS|STC_STCSCSCR_SELF_CHECK_KEY);
    }

    /* Setup the timeout value */
    stcREG1->STCTPR = config->timeoutCounter;

    /* Segment 0 core select - to be run in parallel */
    BF_SET(stcREG1->STCGCR1, STC_GCR1_SEG0_CORE_PARLL, STC_GCR1_SEG0_CORE_SEL_START, STC_GCR1_SEG0_CORE_SEL_LENGTH);

    /* Enable STC run */
    stcREG1->STCGCR1 = STC_STCGCR1_STC_ENA;
#endif
    for (tempVal = 0u; tempVal < 32u; tempVal++) {
    }
#if(FUNC_RESULT_LOG_ENABLED == 1)
    /* The system goes into reset after the test, and the result of the test has to be found out from there
     * Now logging the value of result as 0xEF which denotes 'do not know'state
     */
    SL_Log_Result(FUNC_ID_ST_STC,testType, (SL_SelfTest_Result)0xEFU , 0u);
#endif
    /* Kick off the STC execution */
    _SL_Kickoff_STC_execution();

    /* Execution never comes here !!! */
    retval = TRUE;
    return (retval);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_Status_STC (SL_STC_FailInfo* failInfostc)
{
	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	boolean retVal = FALSE;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (STC_STCGSTAT_TEST_DONE == (STC_STCGSTAT_TEST_DONE & sl_stcREG->STCGSTAT)) 
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (STC_STCGSTAT_TEST_DONE == (STC_STCGSTAT_TEST_DONE & stcREG1->STCGSTAT)) 
#endif
    {
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        retVal = TRUE;/* Test was completed */

        /* Get test results */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (STC_STCGSTAT_TEST_FAIL == (STC_STCGSTAT_TEST_FAIL & sl_stcREG->STCGSTAT)) {
            /* Test completed & failed */
            failInfostc->stResult = ST_FAIL;
            /* Clear the error */
            sl_stcREG->STCGSTAT = STC_STCGSTAT_TEST_FAIL;
        }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        retVal = TRUE;/* Test was completed */
            /* Get test results */
        if (STC_STCGSTAT_TEST_FAIL == (STC_STCGSTAT_TEST_FAIL & stcREG1->STCGSTAT)) {
            /* Test completed & failed */
            failInfostc->stResult = ST_FAIL;
            /* Clear the error */
            stcREG1->STCGSTAT = STC_STCGSTAT_TEST_FAIL;
        }
#endif
        else {
            /* Test completed & passed */
            failInfostc->stResult = ST_PASS;
        }
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (STC_STCFSTAT_CPU1_FAIL == (STC_STCFSTAT_CPU1_FAIL & sl_stcREG->STCFSTAT)) {
            sl_stcREG->STCFSTAT = STC_STCFSTAT_CPU1_FAIL;
            failInfostc->CPU1Failure = ST_PASS;
        }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (STC_STCFSTAT_CPU1_FAIL == (STC_STCFSTAT_CPU1_FAIL & stcREG1->STCFSTAT)) {
            stcREG1->STCFSTAT = STC_STCFSTAT_CPU1_FAIL;
            failInfostc->CPU1Failure = ST_PASS;
        }
#endif
        else {
            failInfostc->CPU1Failure = ST_FAIL;
        }
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (STC_STCFSTAT_CPU2_FAIL == (STC_STCFSTAT_CPU2_FAIL & sl_stcREG->STCFSTAT)) {
            sl_stcREG->STCFSTAT = STC_STCFSTAT_CPU2_FAIL;
            failInfostc->CPU2Failure = ST_PASS;
        }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (STC_STCFSTAT_CPU2_FAIL == (STC_STCFSTAT_CPU2_FAIL & stcREG1->STCFSTAT)) {
            stcREG1->STCFSTAT = STC_STCFSTAT_CPU2_FAIL;
            failInfostc->CPU2Failure = ST_PASS;
        }
#endif
        else {
            failInfostc->CPU2Failure = ST_FAIL;
        }
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (STC_STCFSTAT_TO_ERR == (STC_STCFSTAT_TO_ERR & sl_stcREG->STCFSTAT)) {
            sl_stcREG->STCFSTAT = STC_STCFSTAT_TO_ERR;
            failInfostc->TimeOutFailure = ST_PASS;
        }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (STC_STCFSTAT_TO_ERR == (STC_STCFSTAT_TO_ERR & stcREG1->STCFSTAT)) {
            stcREG1->STCFSTAT = STC_STCFSTAT_TO_ERR;
            failInfostc->TimeOutFailure = ST_PASS;
        }
#endif
        else {
            failInfostc->TimeOutFailure = ST_FAIL;
        }
    } else {
        /* Test did not complete */
    	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
    }

    return(retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/


boolean SL_SelfTest_PSCON(SL_SelfTestType testType, boolean mode, SL_PSCON_FailInfo* param1)
{
	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	register boolean retVal = FALSE;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
    register uint32 regBkupIntEnaSet, regBckupErrInfulence;
    uint32 regbackup;
#endif

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in rage */
    /*LDRA_INSPECTWINDOW 50 */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if ((_SELFTEST_PSCON_MIN > testType) || (_SELFTEST_PSCON_MAX < testType) || !CHECK_RANGE_RAM_PTR(param1)) {
        SL_Log_Error(FUNC_ID_ST_PSCON, ERR_TYPE_PARAM, 0U);
            return(retVal);
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /* Error if not in privilege mode */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
    if ((PSCON_PMA_TEST != testType) && (ARM_MODE_USR == _SL_Get_ARM_Mode())) {
        SL_Log_Error(FUNC_ID_ST_PSCON, ERR_TYPE_ENTRY_CON, 2U);
        return(retVal);
    }
    if ((PSCON_PMA_TEST == testType) && (ARM_MODE_USR != _SL_Get_ARM_Mode())) {
            SL_Log_Error(FUNC_ID_ST_PSCON, ERR_TYPE_ENTRY_CON, 2U);
            return(retVal);
    }
#endif

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((boolean)(TRUE) == SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_PSCON, ERR_TYPE_ENTRY_CON, 3U);
        return(retVal);
    }
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif
    /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE;
    switch(testType) {

        case PSCON_SELF_TEST:
            /*flag is set to indicate the current test which is ongoing and
            These flags are used in the sl_esm.c so as to mask the esm callback*/  
        	(void)SL_FLAG_SET(PSCON_SELF_TEST);
            pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_ST_MODE;
            if (mode) {
                /* Wait for the self-test to complete & get results*/
                /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
                while (!(SL_SelfTest_Status_PSCON(param1))) {
                }
            }
            break;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
        case PSCON_ERROR_FORCING:
        case PSCON_ERROR_FORCING_FAULT_INJECT:
            /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
        status functions to get the pass/failure information*/        
            regBkupIntEnaSet = esmREG->IESR4;
            regBckupErrInfulence = esmREG->IEPSR4;
            if(PSCON_ERROR_FORCING == testType){
                /*flag is set to indicate the current test which is ongoing and
               These flags are used in the sl_esm.c so as to mask the esm callback*/
            	(void)SL_FLAG_SET(PSCON_ERROR_FORCING);
                esmREG->IECR4 = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_COMPARE_ERR);
                esmREG->IEPCR4 = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_COMPARE_ERR);
                esmREG->IECR4 = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_SELTEST_ERR);
                esmREG->IEPCR4 = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_SELTEST_ERR);
            }
            else
            {
                /*flag is set to indicate the current test which is ongoing and
                These flags are used in the sl_esm.c so as to mask the esm callback*/
            	(void)SL_FLAG_SET(PSCON_ERROR_FORCING_FAULT_INJECT);
            }
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
			_SL_Barrier_Data_Access();
			pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_ERRFCG_MODE;
			_SL_Barrier_Data_Access();
            if(PSCON_ERROR_FORCING == testType)
            {
                if (GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_COMPARE_ERR) == (esmREG->SR4[0] & GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_COMPARE_ERR))) {
                    esmREG->SR4[0] = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_COMPARE_ERR);
                    esmREG->SR4[0] = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_SELTEST_ERR);
                    param1->stResult = ST_PASS;
                } else {
                    param1->stResult = ST_FAIL;
                }
                /*clear the flags which indicate tests ongoing*/
                SL_FLAG_CLEAR(PSCON_ERROR_FORCING);
            }
            else
            {
            /*clear the flags which indicate tests ongoing*/
            	SL_FLAG_CLEAR(PSCON_ERROR_FORCING_FAULT_INJECT);
            }
            /*Restore grp1 esm interrupt enable*/
            esmREG->IESR4 = regBkupIntEnaSet;
            esmREG->IEPSR4 = regBckupErrInfulence;
            /* Revert to normal operation */
            pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_LKSTEP_MODE;
            break;
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
        case PSCON_SELF_TEST_ERROR_FORCING:
        case PSCON_SELF_TEST_ERROR_FORCING_FAULT_INJECT:
            /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
             /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
            status functions to get the pass/failure information*/        
            regBkupIntEnaSet = esmREG->IESR4;
            regBckupErrInfulence = esmREG->IEPSR4;
            if(PSCON_SELF_TEST_ERROR_FORCING ==  testType){
                /*flag is set to indicate the current test which is ongoing and
                These flags are used in the sl_esm.c so as to mask the esm callback*/
            	(void)SL_FLAG_SET(PSCON_SELF_TEST_ERROR_FORCING);
                esmREG->IECR4 = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_SELTEST_ERR);
                esmREG->IEPCR4 = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_SELTEST_ERR);
            }
            else
            {
                /*flag is set to indicate the current test which is ongoing and
                These flags are used in the sl_esm.c so as to mask the esm callback*/ 
            	(void)SL_FLAG_SET(PSCON_SELF_TEST_ERROR_FORCING_FAULT_INJECT);
            }
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
			_SL_Barrier_Data_Access();
			pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_STERRF_MODE;
			_SL_Barrier_Data_Access();
            if(PSCON_SELF_TEST_ERROR_FORCING == testType)
            {
                if (GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_SELTEST_ERR) == (esmREG->SR4[0] & GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_SELTEST_ERR))) {
                    esmREG->SR4[0] = GET_ESM_BIT_NUM(ESM_G1ERR_PSCON_SELTEST_ERR);
                    param1->stResult = ST_PASS;
                } else {
                    param1->stResult = ST_FAIL;
                }
                /*clear the flags which indicate tests ongoing*/
                SL_FLAG_CLEAR(PSCON_SELF_TEST_ERROR_FORCING);
            }
            else
            {
            /*clear the flags which indicate tests ongoing*/
            	SL_FLAG_CLEAR(PSCON_SELF_TEST_ERROR_FORCING_FAULT_INJECT);
            }
            /*Restore grp1 esm interrupt enable*/
            esmREG->IESR4 = regBkupIntEnaSet;
            esmREG->IEPSR4 = regBckupErrInfulence;
            /* Revert to normal operation */
            pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_LKSTEP_MODE;
            break;
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
        case PSCON_PMA_TEST:
            /* Make sure we are in non-privilege (user) mode */
            if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
            	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
            	retVal = TRUE;
                regbackup = pmmREG->PRCKEYREG;
                /*flag is set to indicate the current test which is ongoing and
                These flags are used in the sl_esm.c so as to mask the esm callback*/
                (void)SL_FLAG_SET(PSCON_PMA_TEST);
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
                /* Try writing to GLOBALCTRL1.AUTO_CLK_WAKE_ENA, which should result in an abort*/
                pmmREG->PRCKEYREG = 0x0000000FU;
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                if(regbackup == pmmREG->PRCKEYREG)
                {
                    param1->stResult = ST_PASS;
                }
                else
                {
                    param1->stResult = ST_FAIL;
                }
            } else {
                /* Error, must be in user mode to run this test */
            	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
            }
            /*clear the flags which indicate tests ongoing*/
            SL_FLAG_CLEAR(PSCON_PMA_TEST);
            /* Revert to normal operation */
            pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_LKSTEP_MODE;
            break;
#endif
        default:
            /* Revert to normal operation */
            pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_LKSTEP_MODE;
            break;
    }
#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_PSCON, testType, param1->stResult , 0U);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/

boolean SL_SelfTest_Status_PSCON (SL_PSCON_FailInfo* param1)
{
	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	boolean retVal = FALSE;
    /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    /*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
    if ((PMM_PSCON_LPDDCSTAT1_LSTC != ((uint32)(PMM_PSCON_LPDDCSTAT1_LSTC & pmmREG->LPDDCSTAT1))) &&
            (PMM_PSCON_MPDDCSTAT1_MSTC != ((uint32)(PMM_PSCON_MPDDCSTAT1_MSTC & pmmREG->MPDDCSTAT1)))){
        retVal = FALSE;
    } else { /*enter here if any of the two tests is complete*/
        /* If one of the Power or Memory domains have failed, mark the ST as failure */
    	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if  ((0u == (PMM_PSCON_LPDDCSTAT2_LSTE & pmmREG->LPDDCSTAT2)) &&
                (0u == (PMM_PSCON_MPDDCSTAT2_MSTE & pmmREG->MPDDCSTAT2))) {
            param1->stResult = ST_PASS; /*both tests passed*/
        } else {
            param1->stResult = ST_FAIL;
        }

        param1->pdFailure = pmmREG->LPDDCSTAT2;
        param1->mdFailure = pmmREG->MPDDCSTAT2;
        /*clear the flags which indicate tests ongoing*/
        SL_FLAG_CLEAR(PSCON_SELF_TEST);
        /* Revert to normal operation */
        pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_LKSTEP_MODE;
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE;
    }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    if (PMM_PSCON_LPDDCSTAT1_LSTC != ((uint32)(PMM_PSCON_LPDDCSTAT1_LSTC & pmmREG->LPDDCSTAT1))){
        retVal = FALSE;
    } else { /*enter here if any of the two tests is complete*/
        /* If one of the Power domains  have failed, mark the ST as failure */
    	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    	/*SAFETYMCUSW 408 S MR:12.4 <APPROVED> Comment_12*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if  (0u == (PMM_PSCON_LPDDCSTAT2_LSTE & pmmREG->LPDDCSTAT2)) {
            param1->stResult = ST_PASS; /*test passed*/
        } else {
            param1->stResult = ST_FAIL;
        }

        param1->pdFailure = pmmREG->LPDDCSTAT2;
        /*clear the flags which indicate tests ongoing*/
        SL_FLAG_CLEAR(PSCON_SELF_TEST);
        /* Revert to normal operation */
        pmmREG->PRCKEYREG = PMM_PRCKEYREG_MKEY_LKSTEP_MODE;
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE;
    }
#endif
    return(retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_EFuse (SL_SelfTestType testType, boolean bMode, SL_EFuse_Config* config)
{
    uint32 regBkup;
    volatile uint32 regBkupIntEnaSet, regBckupErrInfulence;
    volatile boolean _sl_fault_injection = FALSE;
    /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	boolean retVal = FALSE;
#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */
    /*LDRA_INSPECTWINDOW 50 */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if ((_SELFTEST_EFUSE_MIN > testType) || (_SELFTEST_EFUSE_MAX < testType) || !CHECK_RANGE_RAM_PTR(config)){
        SL_Log_Error(FUNC_ID_ST_EFUSE, ERR_TYPE_PARAM, 0U);
        return(retVal);
    }
#endif
#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_EFUSE, ERR_TYPE_ENTRY_CON, 2U);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return(retVal);
    }
    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((boolean)(TRUE) == SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_EFUSE, ERR_TYPE_ENTRY_CON, 3U);
        return(retVal);
    }
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    switch(testType) {
    case EFUSE_SELF_TEST_AUTOLOAD:

    	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
        break;

    case EFUSE_SELF_TEST_ECC:
    	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE; /* Inputs are fine. Tests are run :) */
        /*flag is set to indicate the current test which is ongoing and
        These flags are used in the sl_esm.c so as to mask the esm callback*/
	(void)SL_FLAG_SET(EFUSE_SELF_TEST_ECC);
        /* Configure the self-test parameters */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        efcREG->SELF_TEST_CYCLES = config->numPatterns;
        efcREG->SELF_TEST_SIGN   = config->seedSignature;
        /* Kick off the self-test */
        efcREG->BOUNDARY = EFC_BOUNDARY_INPUT_EN | EFC_BOUNDARY_SELF_TEST_EN;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        efcREG->ST_CY = config->numPatterns;
        efcREG->ST_SIG   = config->seedSignature;
        /* Kick off the self-test */
        efcREG->BOUND = EFC_BOUNDARY_INPUT_EN | EFC_BOUNDARY_SELF_TEST_EN;
#endif
        /* If mode is synchronous, update the result now*/
        if(bMode){
            /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
            while(!(SL_SelfTest_Status_EFuse(&config->failInfo))){}
        }
        break;

    case EFUSE_SELF_TEST_STUCK_AT_ZERO:

    	/* set test flag */
    	(void)SL_FLAG_SET(EFUSE_SELF_TEST_STUCK_AT_ZERO);

    	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE; /* Inputs are fine. Tests are run :) */
        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
        status functions to get the pass/failure information*/        
        regBkupIntEnaSet = esmREG->IESR4;
        regBckupErrInfulence = esmREG->IEPSR4;
        esmREG->IECR4 = GET_ESM_BIT_NUM(ESM_G1ERR_EFUSE_SELTEST_ERR);
           esmREG->IEPCR4 = GET_ESM_BIT_NUM(ESM_G1ERR_EFUSE_SELTEST_ERR);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBkup = (efcREG->BOUNDARY);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBkup = (efcREG->BOUND);
#endif
        /* Enable the output for errors */
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        efcREG->BOUNDARY = (ST_BOUNDARY_VALUE | EFC_BOUNDARY_OUTPUT_EN);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        efcREG->BOUND = (ST_BOUNDARY_VALUE | EFC_BOUNDARY_OUTPUT_EN);
#endif
        /* Check the efuse controller output for the above error outputs */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (ST_PIN_VALUE == (ST_PIN_VALUE & efcREG->PINS)) {
            /* If all outputs are set, then test has passed */
            config->failInfo.stResult = ST_PASS;
        } else {
            /* One or more ouputs is/are stuck at zero !! return error */
            config->failInfo.stResult = ST_FAIL;
        }
        /* Clear interrupts & Error Pins only for this event but only if this is the only event */
        if ((uint32)(1u << ESM_G3ERR_EFUSE_AUTOLOAD_ERR) == (uint32)(esmREG->SR1[2] & (uint32)(1u << ESM_G3ERR_EFUSE_AUTOLOAD_ERR))) {
            _SL_HoldNClear_nError();
        }

        /* Clear any ESM interrupts */
        esmREG->SR4[0] = GET_ESM_BIT_NUM(ESM_G1ERR_EFUSE_ERR_OTHER)|GET_ESM_BIT_NUM(ESM_G1ERR_EFUSE_SELTEST_ERR);
        esmREG->SR1[2] = GET_ESM_BIT_NUM(ESM_G3ERR_EFUSE_AUTOLOAD_ERR);
        /*Restore grp1 esm interrupt enable*/
        esmREG->IESR4 = regBkupIntEnaSet;
        esmREG->IEPSR4 = regBckupErrInfulence;

        /* Restore boundary register */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        efcREG->BOUNDARY = regBkup;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        efcREG->BOUND = regBkup;
#endif
    	/* clear test flag */
    	SL_FLAG_CLEAR(EFUSE_SELF_TEST_STUCK_AT_ZERO);

        break;
    default:
    	/* nothing here - comment to avoid misra-c warning */
        break;
    }
#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_EFUSE, testType, config->failInfo.stResult , 0U);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_Status_EFuse (SL_EFuse_FailInfo* failInfoefuse)
{
	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	boolean retVal = FALSE;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if ((uint32) (EFC_BOUNDARY_INPUT_EN | EFC_BOUNDARY_SELF_TEST_EN)
            == (uint32) (efcREG->BOUNDARY & (EFC_BOUNDARY_INPUT_EN | EFC_BOUNDARY_SELF_TEST_EN))) 
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if ((uint32) (EFC_BOUNDARY_INPUT_EN | EFC_BOUNDARY_SELF_TEST_EN)
            == (uint32) (efcREG->BOUND & (EFC_BOUNDARY_INPUT_EN | EFC_BOUNDARY_SELF_TEST_EN))) 
#endif
    {
        /** ECC Self test has been enabled, check the results accordingly */
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (EFC_PINS_SELF_TEST_DONE == (uint32) (efcREG->PINS & EFC_PINS_SELF_TEST_DONE)) {
            /* self test is complete */
            retVal = TRUE;
            /* Now check the status of the self test */
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            if ((EFC_PINS_SELF_TEST_ERROR != (uint32) (efcREG->PINS & EFC_PINS_SELF_TEST_ERROR)))
            {
                /*EFUSE ECC*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
            	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                if(EFC_NO_ERROR == (efcREG->ERROR & EFC_ERROR_CODE))
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                if(EFC_NO_ERROR == (efcREG->ERR_STAT & EFC_ERROR_CODE))
#endif
                {
                    if(0==(esmREG->SR4[0] &
                            (GET_ESM_BIT_NUM(ESM_G1ERR_EFUSE_ERR_OTHER)|GET_ESM_BIT_NUM(ESM_G1ERR_EFUSE_SELTEST_ERR)))){
                        failInfoefuse->stResult = ST_PASS;
                    }
                }
            } else {
                /* Self test failed */
                failInfoefuse->stResult = ST_FAIL;
            }
            /* Load the error info anyways */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            failInfoefuse->failInfo= (SL_EFuse_Error)(efcREG->ERROR & EFC_ERROR_CODE);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            failInfoefuse->failInfo= (SL_EFuse_Error)(efcREG->ERR_STAT & EFC_ERROR_CODE);
#endif
            /*clear the flags which indicate tests ongoing*/
            SL_FLAG_CLEAR(EFUSE_SELF_TEST_ECC);
        } else {
            /* self test is not complete */
        	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
        }
    } else {
        /* Since no test was triggered, return FALSE */
    	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
    }

    return (retVal);
}

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_CCMR4F (SL_SelfTestType testType, boolean bMode, SL_CCMR4F_FailInfo* config)
{
	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	boolean retVal = FALSE;
    volatile boolean _sl_fault_injection = FALSE;
    volatile uint32 regBkupIntEnaSet, regBckupErrInfulence;
    if(CCMR4F_ERROR_FORCING_TEST_FAULT_INJECT == testType){
        _sl_fault_injection=TRUE;
    }
#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_20*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_20*/
    if (FALSE == (boolean)CHECK_RANGE_RAM_PTR(config)) {
        SL_Log_Error(FUNC_ID_ST_CCMR4F, ERR_TYPE_PARAM, 0u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return retVal;
    }
    if ((_SELFTEST_CCMR4F_MIN > testType) || (_SELFTEST_CCMR4F_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_CCMR4F, ERR_TYPE_PARAM, 1u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
        return retVal;
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_CCMR4F, ERR_TYPE_ENTRY_CON, 0u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return(retVal);
    }


    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((boolean)(TRUE) == SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_CCMR4F, ERR_TYPE_ENTRY_CON, 2u);
        return(retVal);
    }
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (CCMR4F_CCMKEYR_MODE_LOCKSTEP != (uint32) (ccmr4fREG1->_CCMKEYR & CCMR4F_CCMKEYR_MODE)) {
            retVal =  FALSE;
    }
    else
    {
        switch(testType) {
            case CCMR4F_SELF_TEST:
                /* Set mode to self_test but first ensure that a test is not in process*/
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
                  /*flag is set to indicate the current test which is ongoing and
                These flags are used in the sl_esm.c so as to mask the esm callback*/
            	(void)SL_FLAG_SET(CCMR4F_SELF_TEST);
                ccmr4fREG1->_CCMKEYR = CCMR4F_CCMKEYR_MODE_SELFTEST;
                /* If sync mode loop for the test to complete */
                if (bMode) {
                    /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
                    while (!(SL_SelfTest_Status_CCMR4F(config)))
                    {

                    }
                    /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE;
                } else {
                /* Since the test is running return TRUE (though this func., call might have not started it) */
                	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE;
            }
        break;
        case CCMR4F_ERROR_FORCING_TEST:
        case CCMR4F_ERROR_FORCING_TEST_FAULT_INJECT:
        	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE;
            regBkupIntEnaSet = esmREG->IESR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            regBckupErrInfulence = esmREG->EEPAPR1;
             if(CCMR4F_ERROR_FORCING_TEST ==testType) {
                 /*flag is set to indicate the current test which is ongoing and
                   These flags are used in the sl_esm.c so as to mask the esm callback*/
            	 (void)SL_FLAG_SET(CCMR4F_ERROR_FORCING_TEST);
                 esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR4_SELFTEST);
                    esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR4_SELFTEST);
             }
             /* Set the mode to error forcing mode */
             /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
             /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
             ccmr4fREG1->_CCMKEYR = CCMR4F_CCMKEYR_MODE_ERRFORCING;
             /* Error is injected & mode is switched back to lockstep (takes 1 cpu cycle).
             Wait for the mode to switch */
             /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
             /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
             /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
             while (CCMR4F_CCMKEYR_MODE_LOCKSTEP != (uint32) (ccmr4fREG1->_CCMKEYR & CCMR4F_CCMKEYR_MODE)) {
             }

             /* This section is applicable only for the CCMR4F_ERROR_FORCING_TEST case and not applicable for the
              * CCMR4F_ERROR_FORCING_TEST_FAULT_INJECT, where the application handler clears the error bits that
              * were set */
             if(CCMR4F_ERROR_FORCING_TEST ==testType) {
             /* Since this raises a G2 ESM Error, it will be cleared by the ESM handler. So check the shadow register */
             /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
              /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
              if(((uint32)1u == BF_GET(esmREG->SSR2, BF_CCMR4_CMP_ERROR, BF_CCMR4_CMP_ERROR_LENGTH))) {
            	  config->stResult = ST_PASS;
              } else {
                  config->stResult = ST_FAIL;
              }

             /* Clear nERROR & Interrupt only if in normal self test mode */
                 /* Clear nERROR */
                 _SL_HoldNClear_nError();
                 /* Clear the interrupt */
                 /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
                 esmREG->SR1[1] = GET_ESM_BIT_NUM(ESM_G2ERR_CCMR4_COMP);
                 esmREG->SSR2 = GET_ESM_BIT_NUM(ESM_G2ERR_CCMR4_COMP);
                 esmREG->SR1[0] = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR4_SELFTEST);
                 /*clear the flags which indicate tests ongoing*/
                 SL_FLAG_CLEAR(CCMR4F_ERROR_FORCING_TEST);
             }
             else
             {
                 /* Nothing to do incase of fault injection */
             }
             esmREG->IESR1 = regBkupIntEnaSet;
             esmREG->EEPAPR1 = regBckupErrInfulence;
             break;

        case CCMR4F_SELF_TEST_ERROR_FORCING:
        	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE;
             /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
             /*the esm interrupts for selftests which generate group 1 interrupts is blocked.Users will have to rely on
             status functions to get the pass/failure information*/        
             regBkupIntEnaSet = esmREG->IESR1;
             /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
             regBckupErrInfulence = esmREG->EEPAPR1;
             esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR4_SELFTEST);
                esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR4_SELFTEST);
             /*flag is set to indicate the current test which is ongoing and
             These flags are used in the sl_esm.c so as to mask the esm callback*/
                (void)SL_FLAG_SET(CCMR4F_SELF_TEST_ERROR_FORCING);
             /* Set the mode to self test error forcing mode */
             /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
             ccmr4fREG1->_CCMKEYR = CCMR4F_CCMKEYR_MODE_STERRFORCG;
             /* Error is injected & mode is switched back to lockstep
             (takes 1 cpu cycle). Wait for the mode to switch */
             /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
             /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
             /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
             while (CCMR4F_CCMKEYR_MODE_LOCKSTEP != (uint32) (ccmr4fREG1->_CCMKEYR & CCMR4F_CCMKEYR_MODE)) {
             }

             /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
             if(GET_ESM_BIT_NUM(ESM_G1ERR_CCMR4_SELFTEST) == (esmREG->SR1[0] & GET_ESM_BIT_NUM(ESM_G1ERR_CCMR4_SELFTEST))){
                 config->stResult = ST_PASS;
             }
             else
             {
                 config->stResult = ST_FAIL;
             }
             /* Clear nERROR */
             _SL_HoldNClear_nError();
             /* Clear the interrupt */
             /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
             esmREG->SR1[0] = (uint32)(1u << ESM_G1ERR_CCMR4_SELFTEST);
             /*clear the flags which indicate tests ongoing*/
             SL_FLAG_CLEAR(CCMR4F_ERROR_FORCING_TEST);
             /*Restore grp1 esm interrupt enable*/
             esmREG->IESR1 = regBkupIntEnaSet;
             esmREG->EEPAPR1 = regBckupErrInfulence;
             break;
        default:
        	/* nothing here - comment to avoid misra-c warning */
             break;
        }
    }
    /* If the test was fault injection and not self test, then do not mask the call back & clear the *
     *  FAULT_INJECTION flag for subsequent runs*/
     _sl_fault_injection=FALSE;

#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_CCMR4F, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_CCMR5F (SL_SelfTestType testType, boolean bMode, SL_CCMR5F_FailInfo * config)
{
    boolean retVal = FALSE;
    volatile boolean _sl_fault_injection = FALSE;
    volatile uint32 regBkupIntEnaSet, regBckupErrInfulence;
    volatile uint32* ccmr5fKeyReg = (uint32*)0;

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_20*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_20*/
    if (FALSE == (boolean)CHECK_RANGE_RAM_PTR(config)) {
        SL_Log_Error(FUNC_ID_ST_CCMR5F, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return retVal;
    }
    if ((_SELFTEST_CCMR5F_MIN > testType) || (_SELFTEST_CCMR5F_MAX < testType))
        {
        SL_Log_Error(FUNC_ID_ST_CCMR5F, ERR_TYPE_PARAM, 1u);
        retVal = FALSE;
        return retVal;
    }
#endif
    if ((testType >= CCMR5F_CPUCOMP_SELF_TEST) && 
            (testType <= CCMR5F_CPUCOMP_SELF_TEST_ERROR_FORCING)) 
        ccmr5fKeyReg = &(ccmr5fREG1->_CCMKEYR1);
    else if ((testType >= CCMR5F_VIMCOMP_SELF_TEST) && 
            (testType <= CCMR5F_VIMCOMP_SELF_TEST_ERROR_FORCING))
        ccmr5fKeyReg = &(ccmr5fREG1->_CCMKEYR2);
    else if ((testType >= CCMR5F_PDCOMP_SELF_TEST) && 
            (testType <= CCMR5F_PDCOMP_SELF_TEST_ERROR_FORCING)) 
        ccmr5fKeyReg = &(ccmr5fREG1->_CCMKEYR4);
    else if ((testType >= CCMR5F_INMCOMP_SELF_TEST) && 
            (testType <= CCMR5F_INMCOMP_SELF_TEST_ERROR_FORCING))
        ccmr5fKeyReg = &(ccmr5fREG1->_CCMKEYR3);

    if (ccmr5fKeyReg == (uint32*)0)
    {
        retVal = FALSE;
        return retVal;
    }

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_CCMR5F, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(FALSE);
    }


    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((TRUE) == SL_ESM_nERROR_Active()){
        SL_Log_Error(FUNC_ID_ST_CCMR5F, ERR_TYPE_ENTRY_CON, 1u);
        return(FALSE);
    }

    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    if (CCMR5F_CCMKEYR_MODE_LOCKSTEP != (*ccmr5fKeyReg & CCMR5F_CCMKEYR_MODE)) {
        SL_Log_Error(FUNC_ID_ST_CCMR5F, ERR_TYPE_ENTRY_CON, 2u);
        return(FALSE);
    }
#endif
    if((CCMR5F_CPUCOMP_ERROR_FORCING_TEST_FAULT_INJECT == testType) ||
            (CCMR5F_VIMCOMP_ERROR_FORCING_TEST_FAULT_INJECT == testType) || 
            (CCMR5F_PDCOMP_ERROR_FORCING_TEST_FAULT_INJECT == testType)  || 
            (CCMR5F_INMCOMP_ERROR_FORCING_TEST_FAULT_INJECT == testType)) { 
        _sl_fault_injection=TRUE;
    }
#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif
    switch(testType) {
        case CCMR5F_CPUCOMP_SELF_TEST:
        case CCMR5F_VIMCOMP_SELF_TEST:
        case CCMR5F_PDCOMP_SELF_TEST:
        case CCMR5F_INMCOMP_SELF_TEST:
            /* Set mode to self_test but first ensure that a test is not in process*/
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
           /* flag is set to indicate the current test which is ongoing and
            * These flags are used in the sl_esm.c so as to mask the esm
            * callback.
            * NOTE: even if the CCMR4F Selftest fails (selftest ESM occurs), the
            * application callback will not be made; application must use the
            * failinfo structure to get the status of selftest diagnostic */
            SL_FLAG_SET(testType);
            *ccmr5fKeyReg = CCMR5F_CCMKEYR_MODE_SELFTEST;
            /* If sync mode loop for the test to complete */
            if ((TRUE) == bMode) {
                /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
                while (FALSE == SL_SelfTest_Status_CCMR5F(testType, config)) {
                }
                retVal = TRUE;
            } else {
                /* Since the test is running return TRUE */
                retVal = TRUE;
            }
            break;

    case CCMR5F_CPUCOMP_ERROR_FORCING_TEST:
    case CCMR5F_CPUCOMP_ERROR_FORCING_TEST_FAULT_INJECT:
    case CCMR5F_VIMCOMP_ERROR_FORCING_TEST:
    case CCMR5F_VIMCOMP_ERROR_FORCING_TEST_FAULT_INJECT:
    case CCMR5F_PDCOMP_ERROR_FORCING_TEST:
    case CCMR5F_PDCOMP_ERROR_FORCING_TEST_FAULT_INJECT:
    case CCMR5F_INMCOMP_ERROR_FORCING_TEST:
    case CCMR5F_INMCOMP_ERROR_FORCING_TEST_FAULT_INJECT:
        retVal = TRUE;
        regBkupIntEnaSet = esmREG->IESR1;
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBckupErrInfulence = esmREG->EEPAPR1;
        SL_FLAG_SET(testType);
        if(_sl_fault_injection == FALSE) {
             /*flag is set to indicate the current test which is ongoing and
               These flags are used in the sl_esm.c so as to mask the esm callback*/
             esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_SELFTEST);
             esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_SELFTEST);
         }
#if FUNCTION_PROFILING_ENABLED
    SL_Record_Errorcreationtick(testType);
#endif
         /* Set the mode to error forcing mode */
         /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
         /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
        *ccmr5fKeyReg = CCMR5F_CCMKEYR_MODE_ERRFORCING;
        /* Error is injected & mode is switched back to lockstep (takes 1 cpu
         * cycle).  
         * Wait for the mode to switch */

        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
        /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
        while (CCMR5F_CCMKEYR_MODE_LOCKSTEP != (*ccmr5fKeyReg & CCMR5F_CCMKEYR_MODE)) {
        }
        /* Since this raises a G2 ESM Error, it will be cleared by the ESM
         * handler. So check the shadow register */
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
        int g2interrupt;
        switch (testType) {
            case CCMR5F_CPUCOMP_ERROR_FORCING_TEST:
            case CCMR5F_CPUCOMP_ERROR_FORCING_TEST_FAULT_INJECT:
                g2interrupt = BF_CCMR5_CPUCOMP_ERROR;
                break;

            case CCMR5F_VIMCOMP_ERROR_FORCING_TEST:
            case CCMR5F_VIMCOMP_ERROR_FORCING_TEST_FAULT_INJECT:
                g2interrupt = BF_CCMR5_VIMCOMP_ERROR;
                break;

            case CCMR5F_PDCOMP_ERROR_FORCING_TEST:
            case CCMR5F_PDCOMP_ERROR_FORCING_TEST_FAULT_INJECT:
                g2interrupt = BF_CCMR5_PDCOMP_ERROR;
                break;

            case CCMR5F_INMCOMP_ERROR_FORCING_TEST:
            case CCMR5F_INMCOMP_ERROR_FORCING_TEST_FAULT_INJECT:
                g2interrupt = BF_CCMR5_INMCOMP_ERROR;
                break;
            default:
                break;
        }
        /* check if group 2 interrupt corresponding to error forced occured */
        if(((uint32)1u ==
                    BF_GET(esmREG->SSR2,g2interrupt,BF_CCMR5_CMP_ERROR_LENGTH)))
        {
            config->stResult = ST_PASS;
        } else {
            config->stResult = ST_FAIL;
        }

        /* Clear nERROR & Interrupt only if in normal self test mode */
        if (_sl_fault_injection == FALSE) {
            /* Clear nERROR */
            _SL_HoldNClear_nError();
            /* Clear the interrupt */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
            /* ESM Status for Group2 is automatically cleared upon reading of
             * INTOFFH register on entering the ESM High interrupt handler. */
            /*esmREG->SR1[1] = GET_ESM_BIT_NUM(ESM_G2ERR_CCMR5_COMP);*/
            esmREG->SR1[0] = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_SELFTEST);
            esmREG->SSR2 = GET_ESM_BIT_NUM(g2interrupt);
        } else {
             /* Nothing to do incase of fault injection */
        }
        /*clear the flags which indicate tests ongoing*/
        SL_FLAG_CLEAR(testType);
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;
    break;

    case CCMR5F_CPUCOMP_SELF_TEST_ERROR_FORCING:
    case CCMR5F_VIMCOMP_SELF_TEST_ERROR_FORCING:
    case CCMR5F_PDCOMP_SELF_TEST_ERROR_FORCING:
    case CCMR5F_INMCOMP_SELF_TEST_ERROR_FORCING:
         retVal = TRUE;
         /*Backup grp1 esm interrupt enable register and clear the interrupt
          * enable */
         /*the esm interrupts for selftests which generate group 1 interrupts is
          * blocked. Users will have to rely on status values to get the
          * pass/failure information*/
         regBkupIntEnaSet = esmREG->IESR1;
         /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
         regBckupErrInfulence = esmREG->EEPAPR1;
         esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_SELFTEST);
         esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_SELFTEST);
         /*flag is set to indicate the current test which is ongoing and
         These flags are used in the sl_esm.c so as to mask the esm callback*/
         SL_FLAG_SET(testType);
         /* Set the mode to self test error forcing mode */
         /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
#if FUNCTION_PROFILING_ENABLED
    SL_Record_Errorcreationtick(testType);
#endif
        *ccmr5fKeyReg = CCMR5F_CCMKEYR_MODE_STERRFORCING;
         /* Error is injected & mode is switched back to lockstep
         (takes 1 cpu cycle). Wait for the mode to switch */

         /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
        /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/

        while (CCMR5F_CCMKEYR_MODE_LOCKSTEP != (uint32) (*ccmr5fKeyReg & CCMR5F_CCMKEYR_MODE)) {
        }
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
        if(GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_SELFTEST) == (esmREG->SR1[0] & GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_SELFTEST))){
            config->stResult = ST_PASS;
        }
        else
        {
            config->stResult = ST_FAIL;
        }
        /* Clear nERROR */
        _SL_HoldNClear_nError();
        /* Clear the interrupt */
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_21*/
        esmREG->SR1[0] = (1u << ESM_G1ERR_CCMR5_SELFTEST);
        /*clear the flags which indicate tests ongoing*/
        SL_FLAG_CLEAR(testType);
        /*Restore grp1 esm interrupt enable*/
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;
    break;

    default:
    break;
    }

    if (GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_OPERATING_STATUS) == (esmREG->SR7[0] & GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_OPERATING_STATUS)))
    {
        esmREG->SR7[0] = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_OPERATING_STATUS);
    }
    /* If the test was fault injection and not self test, then do not mask the call back & clear the *
     *  FAULT_INJECTION flag for subsequent runs*/
    if((TRUE) == _sl_fault_injection){
            _sl_fault_injection=FALSE;
    }
#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_CCMR5F, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 91 S MR: 5.2,5.6,5.7 <APPROVED> Comment_22*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_Status_CCMR4F (SL_CCMR4F_FailInfo* failInfoccmr4f)
{
	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	boolean retVal = FALSE;
#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if (FALSE == (boolean)CHECK_RANGE_RAM_PTR(failInfoccmr4f)) {
        SL_Log_Error(FUNC_ID_ST_CCMR4F_STATUS, ERR_TYPE_PARAM, 0u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return retVal;
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_CCMR4F_STATUS, ERR_TYPE_ENTRY_CON, 0u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return(retVal);
    }
#endif
    /* Proceed only if in self-test mode else entry condition is wrong */
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (CCMR4F_CCMKEYR_MODE_SELFTEST != (uint32) (ccmr4fREG1->_CCMKEYR & CCMR4F_CCMKEYR_MODE)) {
            retVal =  FALSE;
    }
    else
    {
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (CCMR4F_CCMSR_STC == (uint32) (ccmr4fREG1->_CCMSR & CCMR4F_CCMSR_STC)) {
        /* Restore the CPU mode */
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
        ccmr4fREG1->_CCMKEYR = CCMR4F_CCMKEYR_MODE_LOCKSTEP;

        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE; /* Indicate that the self test was complete */
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (CCMR4F_CCMSR_STE == (uint32) (ccmr4fREG1->_CCMSR & CCMR4F_CCMSR_STE)) {
            /* self-test failed */
            failInfoccmr4f->stResult = ST_FAIL;
            /* retrieve the err info */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            if (CCMR4F_CCMSR_STET == (uint32) (ccmr4fREG1->_CCMSR & CCMR4F_CCMSR_STET)) {

                failInfoccmr4f->failInfo = CCMR4F_ST_ERR_COMPARE_MATCH;
            } else {
                failInfoccmr4f->failInfo = CCMR4F_ST_ERR_COMPARE_MISMATCH;
            }
        } else {
            failInfoccmr4f->stResult = ST_PASS;
        }
        /*clear the flags which indicate tests ongoing*/
        SL_FLAG_CLEAR(CCMR4F_SELF_TEST);
    } else {
    	/*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE; /* Indicate that the self test was not complete */
    }
#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_CCMR4F_STATUS, (SL_SelfTestType)0u, failInfoccmr4f->stResult, 0u);
#endif
    }
    return(retVal);
}
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
/*SAFETYMCUSW 91 S MR: 5.2,5.6,5.7 <APPROVED> Comment_22*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_Status_CCMR5F (SL_SelfTestType testType, SL_CCMR5F_FailInfo * failInfoccmr5f)
{
    boolean retVal = FALSE;
    volatile uint32* ccmr5fKeyReg = (uint32*)0;
    volatile uint32* ccmr5fStReg = (uint32*)0;
#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if (FALSE == (boolean)CHECK_RANGE_RAM_PTR(failInfoccmr5f)) {
        SL_Log_Error(FUNC_ID_ST_CCMR5F_STATUS, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return retVal;
    }

    if ((CCMR5F_CPUCOMP_SELF_TEST != testType) && 
            (CCMR5F_VIMCOMP_SELF_TEST != testType) &&
            (CCMR5F_PDCOMP_SELF_TEST != testType) &&
            (CCMR5F_INMCOMP_SELF_TEST != testType)) {
        SL_Log_Error(FUNC_ID_ST_CCMR5F, ERR_TYPE_PARAM, 1u);
        retVal = FALSE;
        return retVal;
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_CCMR5F_STATUS, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(FALSE);
    }
#endif

    if (testType == CCMR5F_CPUCOMP_SELF_TEST) {
        ccmr5fKeyReg = &(ccmr5fREG1->_CCMKEYR1);
        ccmr5fStReg =  &(ccmr5fREG1->_CCMSR1);
    } else if (testType == CCMR5F_VIMCOMP_SELF_TEST) {
        ccmr5fKeyReg = &(ccmr5fREG1->_CCMKEYR2);
        ccmr5fStReg =  &(ccmr5fREG1->_CCMSR2);
    } else if (testType == CCMR5F_PDCOMP_SELF_TEST)  {
        ccmr5fKeyReg = &(ccmr5fREG1->_CCMKEYR4);
        ccmr5fStReg =  &(ccmr5fREG1->_CCMSR4);
    }
    else if (testType == CCMR5F_INMCOMP_SELF_TEST) {
        ccmr5fKeyReg = &(ccmr5fREG1->_CCMKEYR3);
        ccmr5fStReg =  &(ccmr5fREG1->_CCMSR3);
    }

    /* Proceed only if in self-test mode else entry condition is wrong */
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    if (CCMR5F_CCMKEYR_MODE_SELFTEST != (uint32) (*ccmr5fKeyReg & CCMR5F_CCMKEYR_MODE)) {
               retVal =  FALSE;
    } else {
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (CCMR5F_CCMSR_STC == (uint32) (*ccmr5fStReg & CCMR5F_CCMSR_STC)) {
            /* Restore the CPU mode */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            *ccmr5fKeyReg = CCMR5F_CCMKEYR_MODE_LOCKSTEP;

            retVal = TRUE; /* Indicate that the self test was complete */
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
            if (CCMR5F_CCMSR_STE == (uint32) (*ccmr5fStReg & CCMR5F_CCMSR_STE)) {
                /* self-test failed */
                failInfoccmr5f->stResult = ST_FAIL;
                /* retrieve the err info */
                /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
                /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
                if (CCMR5F_CCMSR_STET == (uint32) (*ccmr5fStReg & CCMR5F_CCMSR_STET)) {

                    failInfoccmr5f->failInfo = CCMR5F_ST_ERR_COMPARE_MATCH;
                } else {
                    failInfoccmr5f->failInfo = CCMR5F_ST_ERR_COMPARE_MISMATCH;
                }
            } else {
                failInfoccmr5f->stResult = ST_PASS;
            }
            /*clear the flags which indicate tests ongoing*/
            SL_FLAG_CLEAR(testType);
        } else {
            retVal = FALSE; /* Indicate that the self test was not complete */
        }
    }

#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_CCMR5F_STATUS, (SL_SelfTestType)0u, failInfoccmr5f->stResult, 0u);
#endif
    return(retVal);
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
/** @fn void adcCalibration(adcBASE_t *adc)
*   @brief Computes offset error using Calibration mode
*   @param[in] adc Pointer to ADC module:
*              - adcREG1: ADC1 module pointer
*              - adcREG2: ADC2 module pointer
*   This function computes offset error using Calibration mode
*
*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_adcCalibration(adcBASE_t* adc, uint32* offset_error)
{
	uint32 calr_val[5]={0U,0U,0U,0U,0U};
    uint32 loop_index=0U;
    uint32 backup_mode;

    /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	boolean retVal =  FALSE;

#ifdef FUNCTION_PARAM_CHECK_ENABLED

    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    if(((adc != adcREG1) && (adc != adcREG2)) || (offset_error == NULL)) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_PARAM, 2u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
        return retVal;
    }

#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 0u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
        return retVal;
    }

    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if ((uint32)1u == adc->RSTCR) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 1u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
        return retVal;
    }
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(0u != (adc->OPMODECR & ADC_POWERDOWN_MODE)) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 2u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
        return retVal;
    }
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(0u == (adc->OPMODECR & ADC_ENABLE)) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 3u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = FALSE;
        return retVal;
    }

    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if (FALSE == (boolean)CHECK_RANGE_RAM_PTR(offset_error)) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_PARAM, 4u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return retVal;
    }
#endif

    /** - Backup Mode before Calibration  */
    backup_mode = adc->OPMODECR;

    /** - Enable 12-BIT ADC  */
    adc->OPMODECR |= (uint32)0x80000000U;

    /* Disable all channels for conversion */
    adc->GxSEL[0]=0x00U;
    adc->GxSEL[1]=0x00U;
    adc->GxSEL[2]=0x00U;

    for(loop_index=0U;loop_index<4U;loop_index++)
    {
        /* Disable Self Test and Calibration mode */
        adc->CALCR=0x0U;

        switch(loop_index)
        {
            case 0U :     /* Test 1 : Bride En = 0 , HiLo =0 */
                        adc->CALCR=0x0U;
                        break;

            case 1U :    /* Test 1 : Bride En = 0 , HiLo =1 */
                        adc->CALCR=0x0100U;
                        break;

            case 2U :     /* Test 1 : Bride En = 1 , HiLo =0 */
                        adc->CALCR=0x0200U;
                        break;

            case 3U :     /* Test 1 : Bride En = 1 , HiLo =1 */
                        adc->CALCR=0x0300U;
                        break;
            default :
            	/* nothing here - comment to avoid misra-c warning */
                        break;
        }

        /* Enable Calibration mode */
        adc->CALCR|=0x1U;

        /* Start calibration conversion */
        adc->CALCR|=0x00010000U;

        /* Wait for calibration conversion to complete */
        /*SAFETYMCUSW 28 D <APPROVED> Comment_13*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        while((adc->CALCR & 0x00010000U)==0x00010000U)
        {
        } /* Wait */

        /* Read converted value */
        calr_val[loop_index]= adc->CALR;
    }

    /* Disable Self Test and Calibration mode */
    adc->CALCR=0x0U;

    /* Compute the Offset error correction value */
    calr_val[4]=calr_val[0]+ calr_val[1] + calr_val[2] + calr_val[3];

    calr_val[4]=(calr_val[4]/4);

    *offset_error=calr_val[4]-0x7FFU;

    /*Write the offset error to the Calibration register */
    /* Load 2's complement of the computed value to ADCALR register */
    *offset_error=~(*offset_error);
    *offset_error=(*offset_error) & 0xFFFU;
    *offset_error=(*offset_error) + 1U;

    adc->CALR = *offset_error;

  /** - Restore Mode after Calibration  */
    adc->OPMODECR = backup_mode;

    /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal = TRUE;
    return retVal;

 /**   @note The function adcInit has to be called before using this function. */

}
#endif

/*SAFETYMCUSW 62 D MR: 16.7 <APPROVED> Comment_23*/
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define adc1RAMParLoc ((volatile uint8 *)(0xFF3E0004U + 0x1000U))
#define adc2RAMParLoc ((volatile uint8 *)(0xFF3A0008U + 0x1000U))
#else
#define adc1RAMParLoc ((volatile uint8 *)(0xFF3E0007U + 0x1000U))
#define adc2RAMParLoc ((volatile uint8 *)(0xFF3A000BU + 0x1000U))
#endif

#define adc1RAMLoc	 ((volatile uint32 *)0xFF3E0004U)
#define adc2RAMLoc	 ((volatile uint32 *)0xFF3A0008U)
#define adc1RAMLocVar	 ((uint32)0xFF3E0004U)
#define adc2RAMLocVar	 ((uint32)0xFF3A0008U)
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_ADC(SL_SelfTestType testType, boolean mode, SL_ADC_Config* config, SL_ADC_Pinstatus* pinstatus)
{
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    uint16 adRefHi = 0u;
    uint16 adRefLo = 0u;
    uint16 approximated_value;
    uint32 temp, offset;
    SL_ADC_Data Vd, Vu, Vn;
    uint8 channel = 0u;
    uint32 ModeCtrlRegWorkingCopy, IntEnaRegWorkingCopy, IntThrRegWorkingCopy;
#endif
    register uint32 regBackupPCR, regBkupIntEnaSet, regBckupErrInfulence;
    volatile uint32* adcRAMLoc;
    uint32 adcRAMLocVar;
    volatile uint8* adcRAMParLoc;
    adcBASE_t* sl_adcREG;
    uint32 esmErrorChannel;
    volatile uint32 ramRead;

    /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	boolean retVal =  FALSE;

#ifdef FUNCTION_PARAM_CHECK_ENABLED

    if ((_SELFTEST_ADC_MIN > testType) || (_SELFTEST_ADC_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    	}

    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    if(testType == ADC_SELFTEST_ALL)
    {
        if (FALSE == (boolean)CHECK_RANGE_RAM_PTR(pinstatus)) {
            SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_PARAM, 1u);
            /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
    	retVal =  FALSE;
            return retVal;
        }
    }
#endif

    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if (FALSE == (boolean)CHECK_RANGE_RAM_PTR(config)) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_PARAM, 2u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return retVal;
    }

    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    if((config->adcbase != adcREG1) && (config->adcbase != adcREG2)) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_PARAM, 3u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return retVal;
    }
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
/*ADC Channels 0 to 23 are available in TMS570LS31x, TMS570LS12x, RM48 and RM46x devices*/
    if((config->adc_channel >= 24u)) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_PARAM, 4u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return retVal;
    }
#endif
#if defined(_RM42x_) || defined(_TMS570LS04x_)
/*These ADC channels are not available in TMS570LS04x and RM42x devices*/
    if(((config->adc_channel > 11u) && (config->adc_channel < 16u)) ||
           ((config->adc_channel > 17u) && (config->adc_channel < 20u)) ||
           (config->adc_channel > 21u)){
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_PARAM, 4u);
        retVal = FALSE;
        return retVal;
    }
#endif

#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 0u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return retVal;
    }

    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if ((uint32)1u == config->adcbase->RSTCR) {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 1u);
        /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
	retVal =  FALSE;
        return retVal;
    }

    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    if(testType == ADC_SELFTEST_ALL)
    {
        if(0u != (config->adcbase->OPMODECR & ADC_POWERDOWN_MODE)) {
            SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 2u);
            /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
    	retVal =  FALSE;
            return retVal;
        }
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if(0u == (config->adcbase->OPMODECR & ADC_ENABLE)) {
            SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 3u);
            /*SAFETYMCUSW 57 S MR: 14.2 <APPROVED> Comment_25*/
    	retVal =  FALSE;
            return retVal;
        }
    }
#endif

	 /* If ADC parity check is not enabled, return error. */
    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
	 if((testType == ADC_SRAM_PARITY_TEST) &&
	   ((ADC_PAR_DIS) == BF_GET(config->adcbase->PARCR, ADC_PARCR_PAR_START, ADC_PARCR_PAR_LENGTH)))
	 {
		 SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 4u);
	     return(FALSE);
	 }

	    /* If nERROR is active then do not proceed with tests that trigger nERROR */
	    if((TRUE) == SL_ESM_nERROR_Active())
	    {
	        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 5U);
	        return(FALSE);
	    }

#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    sl_adcREG = config->adcbase;

    if(sl_adcREG == adcREG1)
    {
    	adcRAMLoc = adc1RAMLoc;
    	adcRAMLocVar = adc1RAMLocVar;
    	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    	adcRAMParLoc = adc1RAMParLoc;
    	esmErrorChannel = ESM_G1ERR_ADC1_PARITY_CORRERR;
    }
    else if(sl_adcREG == adcREG2)
    {
        adcRAMLoc = adc2RAMLoc;
        adcRAMLocVar = adc2RAMLocVar;
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
        adcRAMParLoc = adc2RAMParLoc;
        esmErrorChannel = ESM_G1ERR_ADC2_PARITY_CORRERR;
    }
    else /* default as adcREG1 */
    {
    	adcRAMLoc = adc1RAMLoc;
    	adcRAMLocVar = adc1RAMLocVar;
    	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    	adcRAMParLoc = adc1RAMParLoc;
    	esmErrorChannel = ESM_G1ERR_ADC1_PARITY_CORRERR;
    }

    switch(testType)
    {
    	case ADC_SRAM_PARITY_TEST:

    		(void)SL_FLAG_SET(testType);

        	/* Backup PCR register */
        	regBackupPCR = sl_adcREG->PARCR;

            /*Backup grp1 esm interrupt enable register */
            regBkupIntEnaSet = esmREG->IESR1;
            regBckupErrInfulence = esmREG->EEPAPR1;

        	/* Disable esm interrupt generation */
            esmREG->IECR1 = GET_ESM_BIT_NUM(esmErrorChannel);

            /* Disable esm error influence */
            esmREG->DEPAPR1 = GET_ESM_BIT_NUM(esmErrorChannel);

        	/* Enable parity test mode */
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
            BIT_SET(sl_adcREG->PARCR, ADC_TEST_MODE);

            /* flip bit of the parity location */
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
            BIT_FLIP((*adcRAMParLoc), 0x1U);

        	/* Disable parity test mode */
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
            BIT_CLEAR(sl_adcREG->PARCR, ADC_TEST_MODE);

        	/* Cause parity error */
        	ramRead = (*adcRAMLoc);

        	/* Check if ESM group1 channel 1 (or 19) is not flagged */
        	if ((esmREG->SR1[0U] & GET_ESM_BIT_NUM(esmErrorChannel)) == 0U)
        	{
        		/* ADC RAM parity error was not flagged to ESM. */
        		retVal = FALSE;
        	}
        	else
        	{
        		/* verify erronous address */
        		if(sl_adcREG->PARADDR == (uint32)(adcRAMLocVar & 0xFFFU))
        		{
        			retVal = TRUE;
        		}
        		else
        		{
        			retVal = FALSE;
        		}

        	}

        	/* clear ESM group1 channel flag */
        	esmREG->SR1[0U] = GET_ESM_BIT_NUM(esmErrorChannel);

        	/* Enable parity test mode */
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
            BIT_SET(sl_adcREG->PARCR, ADC_TEST_MODE);

    		/* Revert back to correct parity */
            /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
            if((boolean)adcRAMParLoc)
    		{
                /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
            	BIT_FLIP((*adcRAMParLoc), 0x1U);
    		}

        	/* Restrore PCR register */
    		sl_adcREG->PARCR = regBackupPCR;

            /* Restore ESM registers states */
            esmREG->IESR1 = regBkupIntEnaSet;
            esmREG->EEPAPR1 = regBckupErrInfulence;

    		break;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    	case ADC_SELFTEST_ALL:

    		(void)SL_FLAG_SET(testType);
    	    channel = config->adc_channel;
    	    /*initializing the Vd,Vu,Vn values to 0 on start*/
    	    Vd.value = 0u;
    	    Vu.value = 0u;
    	    Vn.value = 0u;

    	    /*setting pinstatus to default value*/
    	    *pinstatus = ADC_PIN_UNDETERMINED;
    	    /*backup copy of the interrupt and mode control registers of adc*/
    	    IntEnaRegWorkingCopy = sl_adcREG->GxINTENA[sl_adcGROUP1];
    	    IntThrRegWorkingCopy = sl_adcREG->GxINTCR[sl_adcGROUP1];
    	    ModeCtrlRegWorkingCopy = sl_adcREG->OPMODECR;

    	    /*Disable Interrupts*/
    	    sl_adcREG->GxINTENA[sl_adcGROUP1] = 0u;

    	    /*disable any self tst and Calib mode first */
    	    sl_adcREG->CALCR = 0u;

    	    /*first find out AdRefHi and AdRefLo by using Calibration method*/
    	    /** - Backup Mode before Calibration  */

    	    /** - Enable 12-BIT ADC  */
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	    (sl_adcREG->OPMODECR) |= ADC_12BIT_RESOLUTION;
    	    /* Disable all channels for conversion */
    	    /*Event select*/
    	    sl_adcREG->GxSEL[sl_adcGROUP1] = 0u;

    	    /* Test 1 : Bride En = 1 , HiLo =0 Calib Enabled, Start Calibration */
    	    /*SAFETYMCUSW 28 D MR: 16.7 <APPROVED> Comment_24*/
    	    sl_adcREG->CALCR = (ADC_CALIB_BRIDGE_ENABLE | ADC_START_CALIBRATION | ADC_ENABLE_CALIBRATION);

    	    /* Wait for calibration conversion to complete */
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	    while ((sl_adcREG->CALCR & ADC_START_CALIBRATION) == ADC_START_CALIBRATION) {

    	    }
    	    /* Read converted value */
    	    /* Since adc->CALR is volatile, it is recommended not to cast directly to uint16 */
    	    temp = sl_adcREG->CALR;
    	    adRefLo = (uint16)temp;

    	    /*  Bride En = 1 , HiLo =1 Calib Enabled, Start Calibration */
    	    sl_adcREG->CALCR = (ADC_CALIB_BRIDGE_ENABLE | ADC_CALIB_HILO_ENABLE | ADC_START_CALIBRATION | ADC_ENABLE_CALIBRATION);
    	    /* Wait for calibration conversion to complete */
    	    /*SAFETYMCUSW 28 D MR: 16.7 <APPROVED> Comment_24*/
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	    while ((sl_adcREG->CALCR & ADC_START_CALIBRATION) == ADC_START_CALIBRATION) {
    	    }
    	    /* Read converted value */
    	    /* Since adc->CALR is volatile, it is recommended not to cast directly to uint16 */
    	    temp = sl_adcREG->CALR;
    	    adRefHi = (uint16)temp;

    	    /** - Restore Mode after Calibration  */
    	    sl_adcREG->OPMODECR = ModeCtrlRegWorkingCopy;
    	    /* disable any self tst and Calib mode first*/
    	    sl_adcREG->CALCR = 0u;

    	    /*Calibrate the ADC so as to fill appropriate value in CALR register*/
    	    /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
    	    if(!(SL_adcCalibration(sl_adcREG, &offset)))
    	    {
    	    	return retVal;
    	    }

    	    /*Calculate Vn*/
    	    /*enable ADC channel which needs to be tested */
    	    (void)_SL_SelfTest_adcStartConversion_selChn(sl_adcREG, channel, ADC_FIFO_SIZE, (uint32) sl_adcGROUP1);
    	    /* check for the status of the group to see if it is finished*/
    	    /*SAFETYMCUSW 28 D MR: 16.7 <APPROVED> Comment_24*/
    	    while (!(_SL_SelfTest_adcIsConversionComplete(sl_adcREG, (uint32) sl_adcGROUP1))) {

    	    }
    	    /* copy the results into the Vn*/
    	    (void)_SL_SelfTest_adcGetSingleData(sl_adcREG, (uint32) sl_adcGROUP1, &Vn);

    	    /* for this self test put the adc group in single shot non streaming mode & later restore the setting to original */
    	    /* set the ADC hw in the self test mode and start the test.*/
    	    /*Calculate Vd*/
    	    sl_adcREG->CALCR |= ADC_ENABLE_SELFTEST;
    	    /* set HiLo to 0 to connect AdVref = low */
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	    sl_adcREG->CALCR = (sl_adcREG->CALCR)&(~ADC_CALIB_HILO_ENABLE);
    	    /*enable ADC channel which needs to be tested */
    	    (void)_SL_SelfTest_adcStartConversion_selChn(sl_adcREG, channel, ADC_FIFO_SIZE, (uint32) sl_adcGROUP1);
    	    /* check for the status of the group to see if it is finished*/
    	    /*SAFETYMCUSW 28 D MR: 16.7 <APPROVED> Comment_24*/
    	    while (!(_SL_SelfTest_adcIsConversionComplete(sl_adcREG, (uint32) sl_adcGROUP1))) {

    	    }
    	    /* copy the results into the Vd*/
    	    (void)_SL_SelfTest_adcGetSingleData(sl_adcREG, (uint32) sl_adcGROUP1, &Vd);

    	    /*Calculate Vu*/
    	    sl_adcREG->CALCR |= ADC_CALIB_HILO_ENABLE;
    	    /*enable ADC channel which needs to be tested */
    	    (void)_SL_SelfTest_adcStartConversion_selChn(sl_adcREG, channel, ADC_FIFO_SIZE, (uint32) sl_adcGROUP1);
    	    /* check for the status of the group to see if it is finished*/
    	    /*SAFETYMCUSW 28 D MR: 16.7 <APPROVED> Comment_24*/
    	    while (!(_SL_SelfTest_adcIsConversionComplete(sl_adcREG, (uint32) sl_adcGROUP1))) {

    	    }
    	    /* copy the results into the Vn*/
    	    (void)_SL_SelfTest_adcGetSingleData(sl_adcREG, (uint32) sl_adcGROUP1, &Vu);

    	    /*Approximation of the values is done here*/
    	    /* find the differences and set the voltages levels to ref_low or rel_hig for proper comparision */
    	    approximated_value = _SL_Approximate_value(Vn.value, adRefHi, (uint8)ADC_APPROXIMATION_THRESHOLD);
    	    if(approximated_value == Vn.value)
    	    {
    	        Vn.value = _SL_Approximate_value(Vn.value, adRefLo, (uint8)ADC_APPROXIMATION_THRESHOLD);
    	    }
    	    else
    	    {
    	        Vn.value = approximated_value;
    	    }
    	    approximated_value = _SL_Approximate_value(Vu.value, adRefHi, (uint8)ADC_APPROXIMATION_THRESHOLD);
    	    if(approximated_value == Vu.value)
    	    {
    	        Vu.value = _SL_Approximate_value(Vu.value, adRefLo, (uint8)ADC_APPROXIMATION_THRESHOLD);
    	        Vu.value = Vu.value; /* Avoid warning that function return value not used */
    	    }
    	    else
    	    {
    	        Vu.value = approximated_value;
    	    }
    	    approximated_value = _SL_Approximate_value(Vd.value, adRefHi, (uint8)ADC_APPROXIMATION_THRESHOLD);
    	    if(approximated_value == Vd.value)
    	    {
    	        Vd.value = _SL_Approximate_value(Vd.value, adRefLo, (uint8)ADC_APPROXIMATION_THRESHOLD);
    	        Vd.value = Vd.value; /* Avoid warning that function return value not used */
    	    }
    	    else
    	    {
    	        Vd.value = approximated_value;
    	    }

    	    if ((Vn.value == adRefHi) && (Vu.value == adRefHi) && (Vd.value == adRefHi)) {
    	        *pinstatus = ADC_PIN_SHORTED_TO_ADVREF_HIGH;
    	    } else if ((Vn.value == adRefLo) && (Vu.value == adRefLo) && (Vd.value == adRefLo)) {
    	        *pinstatus = ADC_PIN_SHORTED_TO_ADVREF_LOW;
    	    } else if (((Vu.value < adRefHi) && (Vu.value > Vn.value)) && ((Vd.value > adRefLo) && (Vd.value < Vn.value))) {
    	        *pinstatus = ADC_PIN_GOOD;
    	    } else if ((Vu.value == adRefHi) && (Vd.value == adRefLo)) {
    	        *pinstatus = ADC_PIN_OPEN;
    	    } else {
    	        *pinstatus = ADC_PIN_UNDETERMINED;
    	    }

    	    /*disable any self tst and Calib mode first*/
    	    sl_adcREG->CALCR = 0u;
    	    /* Disable all channels for conversion */
    	    /* Disable all channels for conversion */
    	    /*Event select*/
    	    sl_adcREG->GxSEL[(uint32) sl_adcGROUP1] = 0u;
    	    /*Enable Interrupts if it was enabled before the selftest*/
    	    sl_adcREG->GxINTENA[(uint32) sl_adcGROUP1] = IntEnaRegWorkingCopy;
    	    sl_adcREG->GxINTCR[(uint32) sl_adcGROUP1] = IntThrRegWorkingCopy;

    	    retVal = TRUE;
    		break;
#endif

    	default:
    	    retVal = FALSE;
    		break;
    }

    SL_FLAG_CLEAR(testType);

#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_ADC, testType, adc_stResult , 0u);
#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return retVal;
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/*a reserved location in L2 hierarchy (memory map between EMIF SDRAM and EEPROM)*/
#define SCR_RESERVED_LOCATION 0x88000000U

/*a reserved location in L3 hierarchy (memory map between perpheral frame2 and CRC)*/
#define PCR_RESERVED_LOCATION 0xFD000000U

/*a valid protected location in L2 hierarchy (HTU control register base)*/
#define SCR_PROTECTED_LOCATION 0xFFF7A400U

/*a valid protected location in L3 hierarchy (GIOINTDET control register)*/
#define PCR_PROTECTED_LOCATION 0xFFF7BC08U

/* PPROT[PS=22][QUAD=0] = PPROT2[24] for HTU control register base*/
#define SCR_PROTECTED_LOCATION_FRAMEBIT 24u

/* PPROT[PS=16][QUAD=0] for GIO */
#define PCR_PROTECTED_LOCATION_FRAMEBIT 0u

/* Sample value to be written on protected location */
#define INTERCONNECT_PROTECTED_LOCATION_NEW_DATA 0xAAAAAAAAU
uint32 g_L2L3_read_reserved_word = 0u; /* global variable in order to prevent from being optimised out */

/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_15*/
boolean SL_SelfTestL2L3Interconnect(SL_SelfTestType testType, volatile uint32* location, volatile uint32* protsetreg, uint32 protbit)
{
	boolean g_L2L3_retVal = FALSE;
	boolean g_L2L3_flag = FALSE;
	uint32 g_L2L3_read_protected_word = 0u;

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in rage */
    if ((_SELFTEST_L2L3INTERCONNECT_MIN > testType) || (_SELFTEST_L2L3INTERCONNECT_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_L2L3INTERCONNECT, ERR_TYPE_PARAM, 0u);
        g_L2L3_retVal =  FALSE;
        return(g_L2L3_retVal);
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED

    /*check for priveleged mode in case of reserved access*/
    if (((L2INTERCONNECT_RESERVED_ACCESS == testType) || 
		 (L3INTERCONNECT_RESERVED_ACCESS == testType)) && 
		 (ARM_MODE_USR == _SL_Get_ARM_Mode())) {
        SL_Log_Error(FUNC_ID_ST_L2L3INTERCONNECT, ERR_TYPE_ENTRY_CON, 0u);
        g_L2L3_retVal =  FALSE;
        return(g_L2L3_retVal);
    }

    /*check for user mode in case of unpriveleged access*/
    if (((L2INTERCONNECT_UNPRIVELEGED_ACCESS == testType) ||
		  (L3INTERCONNECT_UNPRIVELEGED_ACCESS == testType))  && 
		  (ARM_MODE_USR != _SL_Get_ARM_Mode())) {
        SL_Log_Error(FUNC_ID_ST_L2L3INTERCONNECT, ERR_TYPE_ENTRY_CON, 1u);
        g_L2L3_retVal =  FALSE;
        return(g_L2L3_retVal);
    }

    /* In case of unpriveleged access, verify PCR protection is set for the location used  */
    if(((L2INTERCONNECT_UNPRIVELEGED_ACCESS == testType) ||
		(L3INTERCONNECT_UNPRIVELEGED_ACCESS == testType)) && 
    	((uint32)(1u<<protbit) != ((uint32)(1u<<protbit) & (*protsetreg)))) {
        SL_Log_Error(FUNC_ID_ST_L2L3INTERCONNECT, ERR_TYPE_ENTRY_CON, 2u);
        g_L2L3_retVal =  FALSE;
        return(g_L2L3_retVal);
    }

#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif
    switch(testType)
    {
        case L3INTERCONNECT_RESERVED_ACCESS:
        	/*Access a reserved location in L3 hierarchy
			 * (in this case, reserved location in memory map between perpheral frame2 and CRC)
			 */
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        	(void)SL_FLAG_SET(testType);
        	_SL_Barrier_Data_Access();
            g_L2L3_read_reserved_word = *((uint32*)PCR_RESERVED_LOCATION);

            /* Verify Data Fault Status and Address register values */
        	if ((0x00000008u == (uint32)(0x00000008u & _SL_Get_DataFault_Status())) &&
        		((uint32)PCR_RESERVED_LOCATION == _SL_Get_DataFault_Address())) {
                g_L2L3_retVal = TRUE; /*test passed*/
        	}

            g_L2L3_read_reserved_word = g_L2L3_read_reserved_word; /* Avoid compiler warning. */

            break;

        case L2INTERCONNECT_RESERVED_ACCESS:
        	/*Access a reserved location in L2 hierarchy
        	 * (in this case, reserved location in memory map between EMIF SDRAM and EEPROM)
        	 */
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        	(void)SL_FLAG_SET(testType);
        	_SL_Barrier_Data_Access();
            g_L2L3_read_reserved_word = *((uint32*)SCR_RESERVED_LOCATION);
            _SL_Barrier_Data_Access(); /*added to avoid linker alignment issue*/

            /* Verify Data Fault Status and Address register values */
        	if ((0x00000008u == (uint32)(0x00000008u & _SL_Get_DataFault_Status())) &&
        		((uint32)SCR_RESERVED_LOCATION == _SL_Get_DataFault_Address())) {
                g_L2L3_retVal = TRUE; /*test passed*/
        	}

            g_L2L3_read_reserved_word = g_L2L3_read_reserved_word; /* Avoid compiler warning. */

            break;

        case L2INTERCONNECT_UNPRIVELEGED_ACCESS:
		case L3INTERCONNECT_UNPRIVELEGED_ACCESS:
        	/*Modify a protected location in L2/L3 hierarchy */
#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif
        	(void)SL_FLAG_SET(testType);

        	/* read initial value at the protected location */
        	g_L2L3_read_protected_word = *location;

            /* modify the value at the protected location and check if its unchanged */
            _SL_Barrier_Data_Access();
			*location = (g_L2L3_read_protected_word ^ (uint32)0xFFFFFFFFU);
			_SL_Barrier_Data_Access(); /*added to avoid linker alignment issue*/


			/* Verify that value at the protected location is unchanged */
			if(g_L2L3_read_protected_word == (*location))
			{
	            /* Verify Data Fault Status and Address register values -
	             * cannot be verified as reading DFSR register not allowed
	             * in user mode
	             */

	                g_L2L3_retVal = TRUE; /*test passed*/
			}


            break;

        default:
        	g_L2L3_retVal = FALSE;
            break;
    }
#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_L2L3INTERCONNECT, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    g_L2L3_flag = g_L2L3_flag; /*to avoid compiler warning*/
    SL_FLAG_CLEAR(testType);
    return(g_L2L3_retVal);
    /*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)

/*a reserved location in Memory Interconnect (L2SRAM)*/
#define MEMINTRCNT_RESERVED_LOCATION 0x08080000U

/* global variables for memory interconnect */
boolean g_MemIntrCnt_retVal;
uint32 g_MemIntrCnt_reserved_word;

/****************************************************************************************************
* Identifier:   SCOD-AMC01-316 (Trace to: SLD-AMC01-316)
* Function:     SL_SelfTest_MemoryInterconnect
* Description:  内存连接器诊断
* Input:        testType 测试类型
*               bmode    测试模式
*               config   配置参数
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
boolean SL_SelfTest_MemoryInterconnect(SL_SelfTestType testType)
{
	g_MemIntrCnt_retVal = FALSE;

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in rage */
    if ((_SELFTEST_MEMINTRCNT_MIN > testType) || (_SELFTEST_MEMINTRCNT_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_MEMINTRCNT, ERR_TYPE_PARAM, 0u);
        g_MemIntrCnt_retVal =  FALSE;
        return(g_MemIntrCnt_retVal);
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_MEMINTRCNT, ERR_TYPE_ENTRY_CON, 0u);
        g_MemIntrCnt_retVal =  FALSE;
        return(g_MemIntrCnt_retVal);
    }
#endif

	/* In case of selftest: Ensure SDC_MASK_SOFT_RESET bit is zero */
    if((testType == MEMINTRCNT_SELFTEST) &&
      (sdcmmrREG1->SDC_CONTROL & SDC_MASK_SOFT_RESET == SDC_MASK_SOFT_RESET))
    {
        SL_Log_Error(FUNC_ID_ST_MEMINTRCNT, ERR_TYPE_ENTRY_CON, 1u);
        g_MemIntrCnt_retVal =  FALSE;
        return(g_MemIntrCnt_retVal);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((TRUE) == SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 2U);
        return(FALSE);
    }


#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif
    switch(testType)
    {

    	case MEMINTRCNT_RESERVED_ACCESS:
        	/*Access a reserved location in Memory Interconnect
			 * (in this case, reserved location in memory map between perpheral frame2 and CRC)
			 */
    		(void)SL_FLAG_SET(testType);
            _SL_Barrier_Data_Access();
            g_MemIntrCnt_reserved_word = *((uint32*)MEMINTRCNT_RESERVED_LOCATION);
            _SL_Barrier_Data_Access(); /*added to avoid linker alignment issue*/

            /* Verify Data Fault Status and Address register values */
        	if ((0x00000008u == (0x00000008u & _SL_Get_DataFault_Status())) &&
        		(0x08080000 == _SL_Get_DataFault_Address())) {
        		g_MemIntrCnt_retVal = TRUE; /*test passed*/
        	}

            g_MemIntrCnt_reserved_word = g_MemIntrCnt_reserved_word; /* Avoid compiler warning. */
            break;

    	 case MEMINTRCNT_SELFTEST:
    		 (void)SL_FLAG_SET(testType);
			/* Initiate selftest sequence */
		    BF_SET(scmREG1->SCMCNTRL, SCM_DTC_SOFTRESET_EN, SCM_DTC_SOFTRESET_EN_START, SCM_DTC_SOFTRESET_EN_LENGTH);

		    /* Kick off the execution */
		    _SL_Kickoff_STC_execution();

		    /* Program never reaches here !!! */
		    g_MemIntrCnt_retVal = TRUE;

    		 break;

         default:
        	 g_MemIntrCnt_retVal = FALSE;
             break;
     }
 #if(FUNC_RESULT_LOG_ENABLED == 1)
     SL_Log_Result(FUNC_ID_ST_MEMINTRCNT, testType, (SL_SelfTest_Result)retVal , 0u);
 #endif
 #if FUNCTION_PROFILING_ENABLED
     SL_Stop_Profiling(testType);
 #endif

     SL_FLAG_CLEAR(testType);
     return(g_MemIntrCnt_retVal);
     /*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
}


boolean SL_SelfTest_Status_MemIntrcntSelftest(void)
{
	boolean retVal = FALSE;

	/* Check if CPU interconnect error not set in ESM */
	if(GET_ESM_BIT_NUM(ESM_G3ERR_CPU_INTRCNT_ERR) !=
	   (esmREG->SR1[2U] & GET_ESM_BIT_NUM(ESM_G3ERR_CPU_INTRCNT_ERR)))
		retVal = TRUE;

	return(retVal);
}
/*a reserved location in Main Perpheral Interconnect (Reserved location in CRC)*/
#define MAINPERIPHINTRCNT_RESERVED_LOCATION 0xFB000300U

/* global variables for Main Perpheral interconnect */
boolean g_MainPeriphIntrCnt_retVal, g_MainPeriphIntrCnt_flag;
uint32 g_MainPeriphIntrCnt_reserved_word;

boolean SL_SelfTest_MainPeripheralInterconnect(SL_SelfTestType testType)
{
	g_MainPeriphIntrCnt_retVal = FALSE;
	g_MainPeriphIntrCnt_flag = FALSE;

#ifdef FUNCTION_PARAM_CHECK_ENABLED

    /* Verify that the parameters are in rage */
    if ((_SELFTEST_MAINPERIPHINTRCNT_MIN > testType) || (_SELFTEST_MAINPERIPHINTRCNT_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_MAINPERIPHINTRCNT, ERR_TYPE_PARAM, 0u);
        g_MainPeriphIntrCnt_retVal =  FALSE;
        return(g_MainPeriphIntrCnt_retVal);
    }

#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED

    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_MAINPERIPHINTRCNT, ERR_TYPE_ENTRY_CON, 0u);
        g_MainPeriphIntrCnt_retVal =  FALSE;
        return(g_MainPeriphIntrCnt_retVal);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((TRUE) == SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 1U);
        return(FALSE);
    }
#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif
    switch(testType)
    {

    	case MAINPERIPHINTRCNT_RESERVED_ACCESS:
        	/*Access a reserved location in Main Perpheral Interconnect
			 * (in this case, reserved location in )
			 */
    		(void)SL_FLAG_SET(testType);
            _SL_Barrier_Data_Access();
            g_MainPeriphIntrCnt_reserved_word = *((uint32*)MAINPERIPHINTRCNT_RESERVED_LOCATION);
            _SL_Barrier_Data_Access(); /*added to avoid linker alignment issue*/

            /* Verify Data Fault Status and Address register values */
        	if ((0x00000008u == (0x00000008u & _SL_Get_DataFault_Status())) &&
        		(0xFB000300 == _SL_Get_DataFault_Address())) {
        		g_MainPeriphIntrCnt_retVal = TRUE; /*test passed*/
        	}

            g_MainPeriphIntrCnt_reserved_word = g_MainPeriphIntrCnt_reserved_word; /* Avoid compiler warning. */
            break;

         default:
        	 g_MainPeriphIntrCnt_retVal = FALSE;
             break;
     }
 #if(FUNC_RESULT_LOG_ENABLED == 1)
     SL_Log_Result(FUNC_ID_ST_MAINPERIPHINTRCNT, testType, (SL_SelfTest_Result)retVal , 0u);
 #endif
 #if FUNCTION_PROFILING_ENABLED
     SL_Stop_Profiling(testType);
 #endif
     SL_FLAG_CLEAR(testType);
     return(g_MainPeriphIntrCnt_retVal);
     /*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
}

/*a reserved location in Perpheral Segment1 Interconnect (unimplemented location between STC2 and SCM)*/
#define PERIPHSEG1INTRCNT_RESERVED_LOCATION 0xFFFF0900U

/*a reserved location in Perpheral Segment2 Interconnect (unimplemented location in ethernet ram)*/
#define PERIPHSEG2INTRCNT_RESERVED_LOCATION 0xFC522000U

/*a reserved location in Perpheral Segment3 Interconnect (Segment3 base address - unimplemented)*/
#define PERIPHSEG3INTRCNT_RESERVED_LOCATION 0xFF000000U

/*a protected location in Perpheral Segment1 Interconnect (DMA GCTRL register)*/
#define PERIPHSEG1INTRCNT_PROTECTED_LOCATION 0xFFFFF000U

/*a protected location in Perpheral Segment2 Interconnect (ePWM1)*/
#define PERIPHSEG2INTRCNT_PROTECTED_LOCATION 0xFCF78C00U

/*a protected location in Perpheral Segment3 Interconnect (GIO INTDET register)*/
#define PERIPHSEG3INTRCNT_PROTECTED_LOCATION 0xFFF7BC08U

/* PS[4] for DMA */
#define PERIPHSEG1INTRCNT_PCR_FRAMEBIT 16u
#define PERIPHSEG1INTRCNT_PCRSET_REG 0xFFFF1020u
#define PERIPHSEG1INTRCNT_PCRCLR_REG 0xFFFF1040u

/* PS[28] for ePWM1 */
#define PERIPHSEG2INTRCNT_PCR_FRAMEBIT 16u
#define PERIPHSEG2INTRCNT_PCRSET_REG 0xFCFF102Cu
#define PERIPHSEG2INTRCNT_PCRCLR_REG 0xFCFF104Cu

/* PS[16] for GIO */
#define PERIPHSEG3INTRCNT_PCR_FRAMEBIT 0u
#define PERIPHSEG3INTRCNT_PCRSET_REG 0xFFF78028u
#define PERIPHSEG3INTRCNT_PCRCLR_REG 0xFFF78048u

/* global variables for Perpheral Segment interconnect */
boolean g_PeriphSegIntrCnt_retVal, g_PeriphSegIntrCnt_flag;
uint32 g_PeriphSegIntrCnt_reserved_word_read, g_PeriphSegIntrCnt_protected_word_read, g_PeriphSegIntrCnt_mode, g_PeriphSegIntrCnt_pcrprot_bit;
volatile uint32 *g_PeriphSegIntrCnt_reserved_word_location, *g_PeriphSegIntrCnt_protected_word_location, *g_PeriphSegIntrCnt_pcrprotset_reg, *g_PeriphSegIntrCnt_pcrprotclr_reg;
pcrBASE_t* g_sl_pcrREG;

boolean SL_SelfTest_PeripheralSegmentInterconnect(SL_SelfTestType testType, SL_PeripheralSegment_Instance instance, volatile uint32* location, volatile uint32* protsetreg, uint32 protbit )
{
	g_PeriphSegIntrCnt_retVal = FALSE;


#ifdef FUNCTION_PARAM_CHECK_ENABLED

    /* Verify that the parameters are in rage */
    if ((_SELFTEST_PERIPHSEGINTRCNT_MIN > testType) || (_SELFTEST_PERIPHSEGINTRCNT_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_PERIPHSEGINTRCNT, ERR_TYPE_PARAM, 0u);
        g_PeriphSegIntrCnt_retVal =  FALSE;
        return(g_PeriphSegIntrCnt_retVal);
    }

    if ((PERIPHSEGINTRCNT_MIN > instance) || (PERIPHSEGINTRCNT_MAX < instance)) {
           SL_Log_Error(FUNC_ID_ST_PERIPHSEGINTRCNT, ERR_TYPE_PARAM, 1u);
           g_PeriphSegIntrCnt_retVal =  FALSE;
           return(g_PeriphSegIntrCnt_retVal);
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED

	/*check for priveleged mode in case of reserved access*/
    if ((PERIPHSEGINTRCNT_RESERVED_ACCESS == testType) && (ARM_MODE_USR == _SL_Get_ARM_Mode())) {
        SL_Log_Error(FUNC_ID_ST_PERIPHSEGINTRCNT, ERR_TYPE_ENTRY_CON, 0u);
        g_PeriphSegIntrCnt_retVal =  FALSE;
        return(g_PeriphSegIntrCnt_retVal);
        }
    /*check for user mode in case of unpriveleged access*/
    if ((PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS == testType) && (ARM_MODE_USR != _SL_Get_ARM_Mode())) {
        SL_Log_Error(FUNC_ID_ST_PERIPHSEGINTRCNT, ERR_TYPE_ENTRY_CON, 1u);
        g_PeriphSegIntrCnt_retVal =  FALSE;
        return(g_PeriphSegIntrCnt_retVal);
    }

    /* In case of unpriveleged access, verify PCR protection is set for the location used  */
    if((PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS == testType) && ((uint32)(1u<<protbit) != ((uint32)(1u<<protbit) & (*protsetreg)))) {
        SL_Log_Error(FUNC_ID_ST_PERIPHSEGINTRCNT, ERR_TYPE_ENTRY_CON, 2u);
        g_PeriphSegIntrCnt_retVal =  FALSE;
        return(g_PeriphSegIntrCnt_retVal);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((TRUE) == SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_ADC, ERR_TYPE_ENTRY_CON, 3U);
        return(FALSE);
    }
#endif

    switch(instance)
    {
        case SL_PERIPH_SEGMENT1:
            g_sl_pcrREG = pcrREG1;
            g_PeriphSegIntrCnt_reserved_word_location = (uint32*)PERIPHSEG1INTRCNT_RESERVED_LOCATION;
            g_PeriphSegIntrCnt_protected_word_location = (uint32*)PERIPHSEG1INTRCNT_PROTECTED_LOCATION;
            g_PeriphSegIntrCnt_pcrprotset_reg = (uint32*)PERIPHSEG1INTRCNT_PCRSET_REG;
            g_PeriphSegIntrCnt_pcrprotclr_reg = (uint32*)PERIPHSEG1INTRCNT_PCRCLR_REG;
            g_PeriphSegIntrCnt_pcrprot_bit = (uint32)(PERIPHSEG1INTRCNT_PCR_FRAMEBIT);
            break;

        case SL_PERIPH_SEGMENT2:
            g_sl_pcrREG = pcrREG2;
            g_PeriphSegIntrCnt_reserved_word_location = (uint32*)PERIPHSEG2INTRCNT_RESERVED_LOCATION;
            g_PeriphSegIntrCnt_protected_word_location = (uint32*)PERIPHSEG2INTRCNT_PROTECTED_LOCATION;
            g_PeriphSegIntrCnt_pcrprotset_reg = (uint32*)PERIPHSEG2INTRCNT_PCRSET_REG;
            g_PeriphSegIntrCnt_pcrprotclr_reg = (uint32*)PERIPHSEG2INTRCNT_PCRCLR_REG;
            g_PeriphSegIntrCnt_pcrprot_bit = (uint32)(PERIPHSEG2INTRCNT_PCR_FRAMEBIT);
            break;

        case SL_PERIPH_SEGMENT3:
            g_sl_pcrREG = pcrREG3;
            g_PeriphSegIntrCnt_reserved_word_location = (uint32*)PERIPHSEG3INTRCNT_RESERVED_LOCATION;
            g_PeriphSegIntrCnt_protected_word_location = (uint32*)PERIPHSEG3INTRCNT_PROTECTED_LOCATION;
            g_PeriphSegIntrCnt_pcrprotset_reg = (uint32*)PERIPHSEG3INTRCNT_PCRSET_REG;
            g_PeriphSegIntrCnt_pcrprotclr_reg = (uint32*)PERIPHSEG3INTRCNT_PCRCLR_REG;
            g_PeriphSegIntrCnt_pcrprot_bit = (uint32)(PERIPHSEG3INTRCNT_PCR_FRAMEBIT);
            break;
    }


#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif
    switch(testType)
    {
    	case PERIPHSEGINTRCNT_RESERVED_ACCESS:
        	/*Access a reserved location in Perpheral Segment Interconnect */
    		(void)SL_FLAG_SET(testType);
            _SL_Barrier_Data_Access();
            g_PeriphSegIntrCnt_reserved_word_read = *(g_PeriphSegIntrCnt_reserved_word_location);
            _SL_Barrier_Data_Access(); /*added to avoid linker alignment issue*/

            /* Verify Data Fault Status and Address register values */
        	if ((0x00000008u == (0x00000008u & _SL_Get_DataFault_Status())) &&
        		((uint32)g_PeriphSegIntrCnt_reserved_word_location == _SL_Get_DataFault_Address())) {
        		g_PeriphSegIntrCnt_retVal = TRUE; /*test passed*/
        	}

            g_PeriphSegIntrCnt_reserved_word_read = g_PeriphSegIntrCnt_reserved_word_read; /* Avoid compiler warning. */
            break;

    	 case PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS:
			/*Modify a protected location in Perpheral Segment Interconnect */

    		 (void)SL_FLAG_SET(testType);

			/* read initial value at the protected location */
			g_PeriphSegIntrCnt_protected_word_read = *location;

			/* Flip the bits at the protected location and check if its unchanged */
			_SL_Barrier_Data_Access();
			*(location) = (g_PeriphSegIntrCnt_protected_word_read ^ (uint32)0xFFFFFFFFU);
			_SL_Barrier_Data_Access(); /*added to avoid linker alignment issue*/


			/* Verify that value at the protected location is unchanged */
			if(g_PeriphSegIntrCnt_protected_word_read == (*(location)))
			{
				/* Verify Data Fault Status and Address register values - cannot access
				 * DFSR in non-priveleged mode */

					g_PeriphSegIntrCnt_retVal = TRUE; /*test passed*/


			}

			break;

         default:
        	 g_PeriphSegIntrCnt_retVal = FALSE;
             break;
     }
 #if(FUNC_RESULT_LOG_ENABLED == 1)
     SL_Log_Result(FUNC_ID_ST_PERIPHSEG2INTRCNT, testType, (SL_SelfTest_Result)retVal , 0u);
 #endif
 #if FUNCTION_PROFILING_ENABLED
     SL_Stop_Profiling(testType);
 #endif
     SL_FLAG_CLEAR(testType);
     return(g_PeriphSegIntrCnt_retVal);
     /*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
}

#endif


#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
void sl_dmaSetCtrlPacket(sl_dmaChannel_t channel, sl_g_dmaCTRL g_dmaCTRLPKT)
{
    uint8 i,j;

    dmaRAMREG->PCP[channel].ISADDR  =  g_dmaCTRLPKT.SADD;

    dmaRAMREG->PCP[channel].IDADDR  =  g_dmaCTRLPKT.DADD;

    dmaRAMREG->PCP[channel].ITCOUNT = (g_dmaCTRLPKT.FRCNT << 16U) | g_dmaCTRLPKT.ELCNT;

    dmaRAMREG->PCP[channel].CHCTRL  = (g_dmaCTRLPKT.RDSIZE    << 14U) | (g_dmaCTRLPKT.WRSIZE    << 12U) | (g_dmaCTRLPKT.TTYPE << 8U)| \
                                      (g_dmaCTRLPKT.ADDMODERD << 3U ) | (g_dmaCTRLPKT.ADDMODEWR << 1U ) | (g_dmaCTRLPKT.AUTOINIT);

    dmaRAMREG->PCP[channel].CHCTRL |= (g_dmaCTRLPKT.CHCTRL << 16U);

    dmaRAMREG->PCP[channel].EIOFF   = (g_dmaCTRLPKT.ELDOFFSET << 16U) | (g_dmaCTRLPKT.ELSOFFSET);

    dmaRAMREG->PCP[channel].FIOFF   = (g_dmaCTRLPKT.FRDOFFSET << 16U) | (g_dmaCTRLPKT.FRSOFFSET);

    i = channel / 8U;          /* Find the register to write */
    j = channel % 8U;          /* Find the offset            */
    j = (uint8)7U - j;         /* Reverse the order          */
    j = j * 4U;                /* Find the bit position      */

    dmaREG->PAR[i] &= ~((uint32)0xFU << j);
    dmaREG->PAR[i] |= (g_dmaCTRLPKT.PORTASGN << j);

}

void sl_dmaSetChEnable(sl_dmaChannel_t channel, sl_dmaTriggerType_t type)
{
	if(type == SL_DMA_HW)
	{
		dmaREG->HWCHENAS = (uint32)1U << channel;
	}
    else
    {
    	dmaREG->SWCHENAS = (uint32)1U << channel;
    }

}

void sl_dmaSetProtectedRegion(uint32* srcAddr)
{

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
	/*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
	dmaREG->DMAMPR0S = (uint32)(srcAddr);
	/*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    dmaREG->DMAMPR0E = (uint32)(srcAddr) + sizeof(uint32);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
	    dmaREG->DMAMPR_L[0U].STARTADD = (uint32)(srcAddr);
	    dmaREG->DMAMPR_L[0U].ENDADD = (uint32)(srcAddr) + sizeof(uint32);
#endif

    /* Enable region for protection  */
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    BIT_SET(dmaREG->DMAMPCTRL1, DMAPCTRL1_REG0AP_EN);

    /* Disable interrupt on protection violation (since diagnostic test)  */
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    BIT_CLEAR(dmaREG->DMAMPCTRL1, DMAPCTRL1_INT0_EN);
}

void sl_dmaConfigCtrlPacket(sl_g_dmaCTRL* dmaCTRLPKT, uint32* srcAddr, uint32* destAddr)
{
	/*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    dmaCTRLPKT->SADD = (uint32)(srcAddr);  		 /* Initial source address           */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    dmaCTRLPKT->DADD = (uint32)(destAddr);  	 /* Initial destination address      */
    dmaCTRLPKT->CHCTRL = (uint32)0u;     		 /* Next channel to be triggered + 1 */
    dmaCTRLPKT->FRCNT = (uint32)1u;     		 /* Frame   count                    */
    dmaCTRLPKT->ELCNT = (uint32)1u;     		 /* Element count                    */
    dmaCTRLPKT->ELDOFFSET = (uint32)0u; 		 /* Element destination offset       */
    dmaCTRLPKT->ELSOFFSET = (uint32)0u;  		 /* Element source offset            */
    dmaCTRLPKT->FRDOFFSET = (uint32)0u; 		 /* Frame destination offset         */
    dmaCTRLPKT->FRSOFFSET = (uint32)0u;		     /* Frame source offset              */
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    dmaCTRLPKT->PORTASGN = (uint32)0x2u;   		 /* DMA port A                       */
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    dmaCTRLPKT->PORTASGN = (uint32)0x4u;    	 /* DMA port B                       */
#endif
    dmaCTRLPKT->RDSIZE = (uint32)0x2u;     		 /* Read element size                */
    dmaCTRLPKT->WRSIZE = (uint32)0x2u;     		 /* Write element size               */
    dmaCTRLPKT->TTYPE = (uint32)0u;      		 /* Trigger type - frame/block       */
    dmaCTRLPKT->ADDMODERD = (uint32)0u;  		 /* Addressing mode for source       */
    dmaCTRLPKT->ADDMODEWR = (uint32)0u;  		 /* Addressing mode for destination  */
    dmaCTRLPKT->AUTOINIT = (uint32)0u;  		 /* Auto-init mode                   */
}

boolean sl_dmaSoftwrTestConfig(uint32* srcAddr, uint32* destAddr, uint32 permission, sl_g_dmaCTRL* dmaCTRLPKT)
{
	boolean retVal = FALSE;

    /* Clear source and destination memory locations */
    (*srcAddr) = (uint32)0U;
    (*destAddr) = (uint32)0U;

    /* Set access permissions for protected region */
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    BF_SET(dmaREG->DMAMPCTRL1, permission, DMAPCTRL1_REG0AP_START, DMAPCTRL1_REG0AP_LENGTH);

	/* set sample data on memory to be accessed */
    (*srcAddr) = (uint32)0x55555555U;

    /* Configure sample control packet */
    sl_dmaConfigCtrlPacket(dmaCTRLPKT,srcAddr, destAddr);

    /* Program control packet to DMA RAM */
    sl_dmaSetCtrlPacket(SL_DMA_CH0, *dmaCTRLPKT);

    /* Enable channel for software triggering */
    sl_dmaSetChEnable(SL_DMA_CH0, SL_DMA_SW);

    /* Enable DMA to initiate transfer */
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    BIT_SET(dmaREG->GCTRL, DMA_GCTRL_DMA_EN);

    /* Wait while bus busy */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    while(DMA_GCTRL_BUS_BUSY == (dmaREG->GCTRL & DMA_GCTRL_BUS_BUSY))
    {
    	/*wait*/
    }

    /* Verify data at source and destination memory locations */
    if((*srcAddr) == (*destAddr))
    {
    	retVal = TRUE;
    }

    /* Check region0 error status bit */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(dmaREG->DMAMPST1 & DMAMPST1_REG0FT == DMAMPST1_REG0FT)
    {
    	retVal = FALSE;

    	/* Clear region0 error status bit */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(dmaREG->DMAMPST1, DMAMPST1_REG0FT);
    }

    /* Disable DMA transfer */
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    BIT_CLEAR(dmaREG->GCTRL, DMA_GCTRL_DMA_EN);


    return(retVal);

}
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
#define dmaRAMParLoc		((volatile uint32 *)(0xFFF80A00U))
#define dmaRAMBaseAddr		((volatile uint32 *)(0xFFF80004U))
#define dmaRAMBaseAddrVar		((uint32)0xFFF80004U)
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define dmaRAMBaseAddr    0xFFF80000u
#define dmaBadECC        0xFFF80010u
#endif

#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
#define DMA_PERMISSION_ALL_ACCESS	(uint32)0x0U
#define DMA_PERMISSION_WRITE_ACCESS	(uint32)0x2U
#define DMA_PERMISSION_READ_ACCESS	(uint32)0x1U
#define DMA_PERMISSION_NO_ACCESS	(uint32)0x3U

boolean SL_SelfTest_DMA(SL_SelfTestType testType)
{
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    volatile boolean _sl_fault_injection = FALSE;
    register uint64 regBackupSECCTL, ramread64;
    uint32 ram1erraddr, dataVal;
    uint32 *dmaRead = (uint32 *)dmaBadECC;
#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    volatile uint32 ramRead;
    register uint32 regBackupPCR, regBkupIntEnaSet, regBckupErrInfulence, regBackupDMAMPCTRL1, regBackupPR0S, regBackupPR0E, regBackupGCTRL;
    boolean retVal = FALSE;

    sl_g_dmaCTRL dmaCTRLPKT;

    /*Variables for DMA Software Test
     * (dma_test_varC used to avoid adjacent memory allocation for
     * dma_test_varA and dma_test_varB)
     */
    uint32 dma_test_varA = 0U, dma_test_varC = 0U, dma_test_varB = 0U;

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    regBackupSECCTL = dmaREG->DMASECCCTRL;
#endif

    regBackupPCR = dmaREG->DMAPCR;

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */
    if ((_SELFTEST_DMA_MIN > testType) || (_SELFTEST_DMA_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_DMA, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_DMA, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    /*verify dma is not in reset state*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (DMA_RES_BIT == (dmaREG->GCTRL & DMA_RES_BIT)) {
        SL_Log_Error(FUNC_ID_ST_DMA, ERR_TYPE_ENTRY_CON, 1u);
        retVal =  FALSE;
        return retVal;
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((TRUE) == SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_DMA, ERR_TYPE_ENTRY_CON, 2U);
        return(FALSE);
    }



#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    if((DMA_ECC_TEST_MODE_1BIT_FAULT_INJECT == testType)||
        (DMA_ECC_TEST_MODE_2BIT_FAULT_INJECT == testType)||
        (DMA_ECC_TEST_MODE_1BIT == testType)||
        (DMA_ECC_TEST_MODE_2BIT == testType))
    {
        /* If DMA ECC is not enabled, return error. */
        if((DMA_SRAM_ECC_DISABLED) == BF_GET(dmaREG->DMAPCR, DMA_PARCTRL_ECC_DETECT_EN_START, DMA_PARCTRL_ECC_DETECT_EN_LENGTH))
        {

            SL_Log_Error(FUNC_ID_ST_DMA, ERR_TYPE_ENTRY_CON, 3U);
            return(FALSE);
        }

        /* If fault inject set global variable to flag to the ESM handler that it is a fault injection */
        if((DMA_ECC_TEST_MODE_1BIT_FAULT_INJECT == testType)||
            (DMA_ECC_TEST_MODE_2BIT_FAULT_INJECT == testType)){
            _sl_fault_injection=TRUE;
        }

        /* backup DATA stored at this location */
        dataVal = *dmaRead;

        /* Enable ECC */
        BF_SET(dmaREG->DMAPCR, DMA_PARCTRL_ECC_EN, DMA_PARCTRL_ECC_EN_START, DMA_PARCTRL_ECC_EN_LENGTH);

        /* rewrite the data back to the DMA RAM, this will ensure that the ECC is
         * correctly computed for this location. */
        *dmaRead = dataVal;

        /* Enable ECC test mode */
        BIT_SET(dmaREG->DMAPCR, DMA_ECC_TST_EN);
    }
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

    /* If DMA parity check is not enabled, return error. */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(((DMA_SRAM_PARITY_DISABLED) == BF_GET(dmaREG->DMAPCR, DMA_PARCTRL_PARITY_EN_START, DMA_PARCTRL_PARITY_EN_LENGTH)) &&
	   (testType == DMA_SRAM_PARITY_TEST))
    {
        SL_Log_Error(FUNC_ID_ST_DMA, ERR_TYPE_ENTRY_CON, 3u);
        retVal = FALSE;
        return(retVal);
    }
#endif

#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
    if(DMA_SOFTWARE_TEST == testType)
    {
    	/* Ensure DMA external bus is not busy */
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (DMA_GCTRL_BUS_BUSY == (dmaREG->GCTRL & DMA_GCTRL_BUS_BUSY)) {
            SL_Log_Error(FUNC_ID_ST_DMA, ERR_TYPE_ENTRY_CON, 4u);
            retVal =  FALSE;
            return retVal;
        }

    	/* Ensure DMA is not enabled for transfers */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (DMA_GCTRL_DMA_EN == (dmaREG->GCTRL & DMA_GCTRL_DMA_EN)) {
            SL_Log_Error(FUNC_ID_ST_DMA, ERR_TYPE_ENTRY_CON, 5u);
            retVal =  FALSE;
            return retVal;
        }
    }
#endif

#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Record_Errorcreationtick(testType);
#endif



    switch(testType)
    {

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    case DMA_ECC_TEST_MODE_1BIT:
    case DMA_ECC_TEST_MODE_1BIT_FAULT_INJECT:

        (void)SL_FLAG_SET(testType);
        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        regBkupIntEnaSet = esmREG->IESR7;
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBckupErrInfulence = esmREG->IEPSR7;

        if((FALSE) == _sl_fault_injection){
            /* Disable ESM notification */
            esmREG->IECR7 = GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_SBERR);
            esmREG->IEPCR7 = GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_SBERR);
        }

        /* Enable generation of single bit event to the ESM (will set the ESM
         * Status flag upon detection of single bit error.) If ESM is enabled
         * (fault injection), will cause ESM interrupt to occur.  */
        BF_SET(dmaREG->DMASECCCTRL, DMA_SBE_EVT_EN, DMA_SBE_EVT_START, DMA_SBE_EVT_LENGTH);
        /* Write clear any pending single bit error flag */
        BIT_SET(dmaREG->DMASECCCTRL, DMA_ECC_SBERR);
        /* Read clear the error address offset */
        ramRead = dmaREG->DMAECCSBE;

        /* Check syndrome */
        /* Flip bit 0 to introduce a single bit error */
        BIT_FLIP((*dmaRead), DMA_FLIP_DATA_1BIT);

        /*required when running with optimisation*/
        _SL_Barrier_Data_Access();

        /* Cause 1 bit ECC error */
        ramRead = (*dmaRead);

        ram1erraddr = (uint32)(dmaREG->DMAECCSBE | dmaRAMBaseAddr);

        if(DMA_ECC_TEST_MODE_1BIT == testType) {
            if((BIT(DMA_ECC_SBERR) == (dmaREG->DMASECCCTRL & BIT(DMA_ECC_SBERR))) &&
                    (dmaBadECC == ram1erraddr) &&
                    (((esmREG->SR7[0]) & GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_SBERR))
                     == GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_SBERR))) {
                /* Clear ESM error status */
                esmREG->SR7[0] =  GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_SBERR);
                retVal = TRUE;
            } else {
                retVal = FALSE;
            }
        }  else {
            /*nothing to be done for fault injection */
            retVal = TRUE;
        }

        /* Clear flags and read error address */
        BIT_SET(dmaREG->DMASECCCTRL, BIT(DMA_ECC_SBERR));

        /* Disable ECC */
        BF_SET(dmaREG->DMAPCR, DMA_PARCTRL_ECC_DIS, DMA_PARCTRL_ECC_EN_START, DMA_PARCTRL_ECC_EN_LENGTH);

        /* Restore data. ECC should remain the same as before. */
        *dmaRead = dataVal;

        SL_FLAG_CLEAR(testType);

        /* Clear the TEST bit of Parity control register */
        BIT_SET(dmaREG->DMAPCR, DMA_ECC_TST_EN);

        /* Restore DMA register states */
        dmaREG->DMAPCR = regBackupPCR;
        dmaREG->DMASECCCTRL = regBackupSECCTL;

        /* Restore ESM registers states */
        esmREG->IESR7 = regBkupIntEnaSet;
        esmREG->IEPSR7 = regBckupErrInfulence;

        break;

    case DMA_ECC_TEST_MODE_2BIT:
    case DMA_ECC_TEST_MODE_2BIT_FAULT_INJECT:

        (void)SL_FLAG_SET(testType);

        regBkupIntEnaSet = esmREG->IESR1;
        regBckupErrInfulence = esmREG->EEPAPR1;

        if((FALSE) == _sl_fault_injection){
            /* Disable ESM notification */
            esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_UNCORR);
            esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_UNCORR);
        }

        /* Clear previous ECC error */
        BIT_SET(dmaREG->DMAPAR, DMA_ECC_ERR);

        /* Read clear the error address */
         ramRead= dmaREG->DMAPAR;

        /* Introduce a double bit error */
        BIT_FLIP(*dmaRead, DMA_FLIP_DATA_2BIT);

        /*required when running with optimisation*/
        _SL_Barrier_Data_Access();

        ramRead = *dmaRead;

        ram1erraddr = (BF_GET(dmaREG->DMAPAR, DMA_ECC_UNCORERR_ADDR_START, DMA_ECC_UNCORERR_ADDR_LEN)| dmaRAMBaseAddr);

        if(DMA_ECC_TEST_MODE_2BIT == testType) {
            if(((dmaREG->DMAPAR & BIT(DMA_ECC_ERR)) == BIT(DMA_ECC_ERR)) &&
                (dmaBadECC == ram1erraddr)    &&
                (GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_UNCORR) == 
                 (esmREG->SR1[0] & GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_UNCORR)))) {
                retVal = TRUE;
                /* Clear ESM error status */
                esmREG->SR1[0] =  GET_ESM_BIT_NUM(ESM_G1ERR_DMA_ECC_UNCORR);
            } else {
                retVal = FALSE;
            }
        } else {
            retVal = TRUE;
        }

        /* Clear flags and read error address */
        BIT_SET(dmaREG->DMAPAR, BIT(DMA_ECC_ERR));

        /* Disable ECC */
        BF_SET(dmaREG->DMAPCR, DMA_PARCTRL_ECC_DIS, DMA_PARCTRL_ECC_EN_START, DMA_PARCTRL_ECC_EN_LENGTH);

        /* Restore data */
        *dmaRead = dataVal;

        SL_FLAG_CLEAR(testType);

        /* Disable ECC test mode */
        BIT_SET(dmaREG->DMAPCR, DMA_ECC_TST_EN);

        /* Restore DMA register states */
        dmaREG->DMAPCR = regBackupPCR;
        dmaREG->DMASECCCTRL = regBackupSECCTL;

        /* Restore ESM registers states */
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;

        break;
#endif

    case DMA_SOFTWARE_TEST:

    	/* Set test flag */
    	(void)SL_FLAG_SET(testType);
      

        /* Backup suitable values (no backing up of dma RAM,
         *  app should take care after boot time test) */
        regBackupDMAMPCTRL1 = dmaREG->DMAMPCTRL1;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
		regBackupPR0S = dmaREG->DMAMPR0S;
		regBackupPR0E = dmaREG->DMAMPR0E;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
		regBackupPR0S = dmaREG->DMAMPR_L[0U].STARTADD;
		regBackupPR0E = dmaREG->DMAMPR_L[0U].ENDADD;
#endif
		regBackupGCTRL = dmaREG->GCTRL;

        /* Set up protected region */
		sl_dmaSetProtectedRegion(&dma_test_varA);

        /* Perform configurations and initiate transfer - with no access permission for dma_test_varA */
        if(!sl_dmaSoftwrTestConfig(&dma_test_varA, &dma_test_varB, DMA_PERMISSION_NO_ACCESS, &dmaCTRLPKT))
        {
            /* Perform configurations and initiate transfer - with no access permission for dma_test_varA */
            if(!sl_dmaSoftwrTestConfig(&dma_test_varB, &dma_test_varA, DMA_PERMISSION_NO_ACCESS, &dmaCTRLPKT))
            {
                /* Perform configurations and initiate transfer - with read only access permission for dma_test_varA */
                if(sl_dmaSoftwrTestConfig(&dma_test_varA, &dma_test_varB, DMA_PERMISSION_READ_ACCESS, &dmaCTRLPKT))
                {
                    /* Perform configurations and initiate transfer - with read only access permission for dma_test_varA */
                    if(!sl_dmaSoftwrTestConfig(&dma_test_varB, &dma_test_varA, DMA_PERMISSION_READ_ACCESS, &dmaCTRLPKT))
                    {
                        /* Perform configurations and initiate transfer - with write only access permission for dma_test_varA */
                        if(!sl_dmaSoftwrTestConfig(&dma_test_varA, &dma_test_varB, DMA_PERMISSION_WRITE_ACCESS, &dmaCTRLPKT))
                        {
                            /* Perform configurations and initiate transfer - with write only access permission for dma_test_varA */
                            if(sl_dmaSoftwrTestConfig(&dma_test_varB, &dma_test_varA, DMA_PERMISSION_WRITE_ACCESS, &dmaCTRLPKT))
                            {
                                /* Perform configurations and initiate transfer - with all access permission for dma_test_varA */
                                if(sl_dmaSoftwrTestConfig(&dma_test_varA, &dma_test_varB, DMA_PERMISSION_ALL_ACCESS, &dmaCTRLPKT))
                                {
                                    /* Perform configurations and initiate transfer - with all access permission for dma_test_varA */
                                    if(sl_dmaSoftwrTestConfig(&dma_test_varB, &dma_test_varA, DMA_PERMISSION_ALL_ACCESS, &dmaCTRLPKT))
                                    	{
                                    		retVal = TRUE;
                                    	}
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Restore backed up values */
         dmaREG->DMAMPCTRL1 = regBackupDMAMPCTRL1;
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
		 dmaREG->DMAMPR_L[0U].STARTADD = regBackupPR0S;
		 dmaREG->DMAMPR_L[0U].ENDADD = regBackupPR0E;
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
		 dmaREG->DMAMPR0S = regBackupPR0S;
		 dmaREG->DMAMPR0E = regBackupPR0E;
#endif
		 dmaREG->GCTRL = regBackupGCTRL;

		 /* To avoid misra-c warning */
		 dma_test_varC = dma_test_varC;

		 SL_FLAG_CLEAR(testType);
    	break;


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case DMA_SRAM_PARITY_TEST:

    	(void)SL_FLAG_SET(testType);

        /*Backup grp1 esm interrupt enable register */
        regBkupIntEnaSet = esmREG->IESR1;
        regBckupErrInfulence = esmREG->EEPAPR1;

    	/* Disable esm interrupt generation */
        esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_DMA_PARITY_CORRERR);

        /* Disable esm error influence */
        esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_DMA_PARITY_CORRERR);

    	/* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(dmaREG->DMAPCR, DMA_TEST_MODE);

    	/* Flip a bit in DMA RAM parity location */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_FLIP((*dmaRAMParLoc), 0x10U);

    	/* Disable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_CLEAR(dmaREG->DMAPCR, DMA_TEST_MODE);

        /* read DMAPAR register to allow it to get updatted on error generation */
    	ramRead = dmaREG->DMAPAR;

    	/* Cause parity error */
    	ramRead = (*dmaRAMBaseAddr);

    	/* Check if ESM group1 channel 3 is not flagged */
    	if ((esmREG->SR1[0U] & GET_ESM_BIT_NUM(ESM_G1ERR_DMA_PARITY_CORRERR)) == 0U)
    	{
    		/* DMA RAM parity error was not flagged to ESM. */
    		retVal = FALSE;
    	}
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	else if((dmaREG->DMAPAR & DMA_PARITY_EDFLAG) == 0U)
    	{
    		/* DMA RAM parity error was not flagged to ESM. */
    		retVal = FALSE;
    	}
    	else
    	{
    		/* verify erronous address */
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    		if((uint32)(dmaRAMBaseAddrVar & 0xFFFU) ==
    			/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    			BF_GET(dmaREG->DMAPAR, DMA_PAR_ADDR_START, DMA_PAR_ADDR_LENGTH))
    			{
    				retVal = TRUE;
    			}
    		else
    			{
    				retVal = FALSE;
    			}
    	}

		/* clear DMA parity error flag in DMA */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(dmaREG->DMAPAR, DMA_PARITY_EDFLAG);

		/* clear ESM group1 channel 3 flag */
		esmREG->SR1[0U] = GET_ESM_BIT_NUM(ESM_G1ERR_DMA_PARITY_CORRERR);

		/* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
		BIT_SET(dmaREG->DMAPCR, DMA_TEST_MODE);

		/* Revert back to correct parity */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
		BIT_FLIP((*dmaRAMParLoc), 0x10U);

    	/* Restrore Parity Control register */
    	dmaREG->DMAPCR = regBackupPCR;

        /* Restore ESM registers states */
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;

        SL_FLAG_CLEAR(testType);
    	break;
#endif
    default:
        retVal = FALSE;
        break;
    }

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    /* If the test was fault injection and not self test, then do not mask the call back & clear the *
    *  FAULT_INJECTION flag for subsequent runs*/
    if((TRUE) == _sl_fault_injection){
            _sl_fault_injection=FALSE;
    }

    ramread64 = ramread64; /* Avoid compiler warning. */
#endif

#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_DMA, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define vimRAMParLoc	((volatile uint8 *)0xFFF82408U)
#else
#define vimRAMParLoc	((volatile uint8 *)0xFFF8240BU)
#endif

#define vimRAMLoc		((volatile uint32 *)0xFFF82008U)
#define vimRAMLocVar		((uint32)0xFFF82008U)
#endif
/*SAFETYMCUSW 25 D MR:8.7 <APPROVED> "Statically allocated memory needs to be available to entire application." */
boolean vimTestFlag=FALSE;

boolean SL_SelfTest_VIM(SL_SelfTestType testType)
{

#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    volatile uint32 ramRead;
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    SL_CCMR5F_FailInfo failInfoCCMR5F;	/* CCMR5 Self Test fail info - for VIM software test */
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

    register uint32 regBackupPCR, regBkupIntEnaSet, regBckupErrInfulence;
    SL_SelfTest_Result failInfoTCMRAM;     /* TCM RAM Failure  information - for VIM software test */

#endif
    boolean retVal = FALSE;
    boolean tmp;

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */
    if ((_SELFTEST_VIM_MIN > testType) || (_SELFTEST_VIM_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_VIM, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_VIM, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }



#if FUNCTION_PROFILING_ENABLED
    SL_Record_Errorcreationtick(testType);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

    /* If VIM parity check is not enabled, return error */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(((VIM_SRAM_PARITY_DISABLED) == BF_GET(sl_vimParREG->PARCTL, VIM_PARCTRL_PARITY_EN_START, VIM_PARCTRL_PARITY_EN_LENGTH)) &&
       (testType == VIM_SRAM_PARITY_TEST))
    {
        SL_Log_Error(FUNC_ID_ST_VIM, ERR_TYPE_ENTRY_CON, 1u);
        retVal =  FALSE;
        return(retVal);
    }
#endif

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if(SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_VIM, ERR_TYPE_ENTRY_CON, 2U);
        return(FALSE);
    }

#endif

    switch(testType)
    {
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
   case VIM_SRAM_PARITY_TEST:

       (void)SL_FLAG_SET(testType);

    	/* Backup PCR register */
        regBackupPCR = sl_vimParREG->PARCTL;

        /*Backup grp1 esm interrupt enable register */
        regBkupIntEnaSet = esmREG->IESR1;
        regBckupErrInfulence = esmREG->EEPAPR1;

    	/* Disable esm interrupt generation */
        esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_VIM_PARITY_CORRERR);

        /* Disable esm error influence */
        esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_VIM_PARITY_CORRERR);

    	/* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_vimParREG->PARCTL, VIM_TEST_MODE);

    	/* Flip a bit in VIM RAM parity location */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        BIT_FLIP((*vimRAMParLoc), 0x1U);

    	/* Disable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_CLEAR(sl_vimParREG->PARCTL, VIM_TEST_MODE);

    	/* Cause parity error */
    	ramRead = (*vimRAMLoc);

    	/* Check if ESM group1 channel 15 is not flagged */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if (((sl_vimParREG->PARFLG & VIM_PAR_ERR_FLG) == 0U) ||
    		((esmREG->SR1[0U] & GET_ESM_BIT_NUM(ESM_G1ERR_VIM_PARITY_CORRERR)) == 0U))
    	{
    		/* VIM RAM parity error was not flagged to ESM. */
    		retVal = FALSE;
    	}
    	else
    	{
    		/* verify erronous address */
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    		if(sl_vimParREG->ADDERR == vimRAMLocVar)
    		{
    			retVal = TRUE;
    		}
    		else
    		{
    			retVal = FALSE;
    	}
    	}

		/* clear VIM parity error flag in VIM */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(sl_vimParREG->PARFLG, VIM_PAR_ERR_FLG);

		/* clear ESM group1 channel 15 flag */
		esmREG->SR1[0U] = GET_ESM_BIT_NUM(ESM_G1ERR_VIM_PARITY_CORRERR);

		/* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
		BIT_SET(sl_vimParREG->PARCTL, VIM_TEST_MODE);

		/* Revert back to correct parity */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
		BIT_FLIP((*vimRAMParLoc), 0x1U);

    	/* Restrore Parity Control register */
    	sl_vimParREG->PARCTL = regBackupPCR;

        /* Restore ESM registers states */
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;

    	break;
#endif
    case VIM_SOFTWARE_TEST:

    	/* Set test type */
    	(void)SL_FLAG_SET(testType);

   	    /* set indicator variable as false */
        vimTestFlag = FALSE;

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    	/* cause interrupt (ESM_G2ERR_CCMR5_CPUCOMP_ERR interrupt in this case) */
		tmp = SL_SelfTest_CCMR5F(CCMR5F_CPUCOMP_ERROR_FORCING_TEST, TRUE, &failInfoCCMR5F);

		tmp = tmp; /* to avoid compiler warning */
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
		/* cause interrupt (SRAM_RADECODE_DIAGNOSTICS interrupt in this case) - storing retval to avoid misra-c violation */
		tmp = SL_SelfTest_SRAM(SRAM_RADECODE_DIAGNOSTICS, TRUE, &failInfoTCMRAM);

		tmp = tmp; /* to avoid compiler warning */
#endif

		/* check if the ISR called */
		if(vimTestFlag)
		{
			retVal = TRUE; /*Test passed*/
		}

		/* Clear global flag */
		vimTestFlag = FALSE;

		/* Clear test type */
		SL_FLAG_CLEAR(testType);
    	break;

    default:
        retVal = FALSE;
        break;
    }

#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_VIM, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif

    SL_FLAG_CLEAR(testType);
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_hetEnableLoopback(sl_hetBASE_t* hetREG, uint32 pin)
{
	uint8 bit;
	uint32 tmp1, tmp2; /* to handle misra-c violation */

	tmp1 = pin >> 1U;
	tmp2 = tmp1 + 16U;

	if((pin % 2U) == 0U)
	{
		bit = 1U;
	}
	else
	{
		bit = 0U;
	}

	/* loopback setting - select hr pair and loopback type */
	hetREG->LBPSEL = (uint32)(1U << tmp1)
	                   | (uint32)(1U << tmp2);

	/* loopback setting - select loopback direction and enable loopback test */
	hetREG->LBPDIR = (uint32)(bit << ((uint32)(pin>>1U)))
	                   | (uint32)(0x000A0000U);
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_capGetSignal(sl_hetRAMBASE_t* hetRAM, sl_hetSIGNAL_t* signal)
{
    uint32    pwmDuty   = (hetRAM->Instruction[5U].Data) >> 7U;
    uint32    pwmPeriod = (hetRAM->Instruction[6U].Data) >> 7U;

    signal->duty = pwmDuty;
    signal->period = (float64)pwmPeriod;

}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define het1RAMParLoc	((volatile uint8 *)0xFF462004U)
#else
#define het1RAMParLoc	((volatile uint8 *)0xFF462007U)
#endif
#define het1RAMLoc		((volatile uint32 *)0xFF460004U)
#define het1RAMLocVar		((uint32)0xFF460004U)

#if !(defined(_RM42x_) || defined(_TMS570LS04x_))

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define het2RAMParLoc	((volatile uint8 *)0xFF442008U)
#else
#define het2RAMParLoc	((volatile uint8 *)0xFF44200BU)
#endif
#define het2RAMLoc		((volatile uint32 *)0xFF440008U)
#define het2RAMLocVar		((uint32)0xFF440008U)

#endif


#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
boolean SL_SelfTest_HET(SL_SelfTestType testType, SL_HET_Instance instance, uint32 pinOut, uint32 pinIn)
{
   volatile uint32 ramRead;
    volatile uint32* hetRAMLoc;
    volatile uint32* regIntEnaSet;
    volatile uint32* regErrInfulenceSet;
    volatile uint32* regIntEnaClr;
    volatile uint32* regErrInfulenceClr;
    volatile uint32* regEsmStatus;
    volatile uint8* hetRAMParLoc;
    uint32 hetRAMLocVar;
    register uint32 regBackupPCR, regBkupIntEnaSet, regBckupErrInfulenceSet;
    register uint32 regBackupLBPSEL, regBackupLBPDIR, regBackupHETPFR, regBackupHetIntEnaSet, regBackupHetReqEnaSet;
	uint32 tmp_count, esmErrorChannel;
	sl_hetSIGNAL_t received_signal;
	sl_hetBASE_t* sl_hetREG;
	sl_hetRAMBASE_t* sl_hetRAM;
	sl_hetINSTRUCTION_t hetRAMBackup[7U];


#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    boolean retVal = FALSE;

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */

    if ((_SELFTEST_HET_MIN > testType) || (_SELFTEST_HET_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    if ((HET_MIN > instance) || (HET_MAX < instance)) {
           SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_PARAM, 1u);
           retVal =  FALSE;
           return(retVal);
       }

#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }
#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Record_Errorcreationtick(testType);
#endif

    switch(instance)
    {
        case SL_HET1:
        	sl_hetREG       = sl_hetREG1;
        	sl_hetRAM       = sl_hetRAM1;
        	hetRAMLoc       = het1RAMLoc;
        	hetRAMLocVar	= het1RAMLocVar;
        	hetRAMParLoc    = het1RAMParLoc;
        	esmErrorChannel = ESM_G1ERR_HET1_PARITY_CORRERR;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regIntEnaSet    = &esmREG->IESR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regErrInfulenceSet = &esmREG->EEPAPR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regIntEnaClr    = &esmREG->IECR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regErrInfulenceClr = &esmREG->DEPAPR1;
			regEsmStatus =  &esmREG->SR1[0U];

            break;
#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
        case SL_HET2:
        	sl_hetREG       = sl_hetREG2;
        	sl_hetRAM       = sl_hetRAM2;
        	hetRAMLoc       = het2RAMLoc;
        	hetRAMLocVar	= het2RAMLocVar;
        	hetRAMParLoc = het2RAMParLoc;
#if defined(_TMS570LS12x_) || defined(_RM46x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
        	esmErrorChannel = ESM_G1ERR_HET2_PARITY_CORRERR;
        	regIntEnaSet    = &esmREG->IESR4;
			regErrInfulenceSet = &esmREG->IEPSR4;
        	regIntEnaClr    = &esmREG->IECR4;
			regErrInfulenceClr = &esmREG->IEPCR4;
			regEsmStatus =  &esmREG->SR4[0U];
#else
        	esmErrorChannel = ESM_G1ERR_HET1_PARITY_CORRERR;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regIntEnaSet    = &esmREG->IESR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regErrInfulenceSet = &esmREG->EEPAPR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regIntEnaClr    = &esmREG->IECR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regErrInfulenceClr = &esmREG->DEPAPR1;
			regEsmStatus =  &esmREG->SR1[0U];
#endif

            break;
#endif
        default:
        	sl_hetREG       = sl_hetREG1;
        	sl_hetRAM       = sl_hetRAM1;
        	hetRAMLoc       = het1RAMLoc;
        	hetRAMLocVar	= het1RAMLocVar;
        	hetRAMParLoc    = het1RAMParLoc;
        	esmErrorChannel = ESM_G1ERR_HET1_PARITY_CORRERR;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regIntEnaSet    = &esmREG->IESR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regErrInfulenceSet = &esmREG->EEPAPR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regIntEnaClr    = &esmREG->IECR1;
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	regErrInfulenceClr = &esmREG->DEPAPR1;
			regEsmStatus =  &esmREG->SR1[0U];

            break;
    }

    /*verify HET is turned off (since if timer is on, the HET RAM will get read by the HET CPU)*/
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if((boolean)sl_hetREG)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if (HET_TO_BIT == (sl_hetREG->GCR & HET_TO_BIT)) {
            SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_ENTRY_CON, 1u);
            retVal =  FALSE;
            return retVal;
        }

    }

	 /* If HET parity check is not enabled in case of parity test, return error. */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(((HET_SRAM_PARITY_DISABLED) == BF_GET(sl_hetREG->PCR, HET_PARCTRL_PARITY_EN_START, HET_PARCTRL_PARITY_EN_LENGTH)) &&
		(testType == HET_SRAM_PARITY_TEST))
	 {
		 SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_ENTRY_CON, 2u);
	     return(FALSE);
	 }

	/* If pins passed as argument are not in range for the case of loopback test, return error. */
	if(testType == HET_ANALOG_LOOPBACK_TEST)
	{
		if((pinOut == (uint32)NULL) || (pinIn == (uint32)NULL))
		{
			SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_PARAM, 2u);
			retVal =  FALSE;
			return(retVal);
		}

		if(instance == SL_HET1)
		{
			if ((HET1_PIN_MAX < pinOut) || (HET1_PIN_MAX < pinIn))
			{
				SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_PARAM, 3u);
				retVal =  FALSE;
				return(retVal);
			}
		}
#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
		if(instance == SL_HET2)
		{
			if ((HET2_PIN_MAX < pinOut) || (HET2_PIN_MAX < pinIn))
			{
				SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_PARAM, 3u);
				retVal =  FALSE;
				return(retVal);
			}
		}
#endif
	}

	 /* If pins passed as argument are not a valid pair in case of het loopback test, return error. */
	 if(testType == HET_ANALOG_LOOPBACK_TEST)
	 {
		 if((pinOut > pinIn) && (((pinIn%2U) != 0U) || ((pinIn+1U) != pinOut)))
		 {
			 SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_PARAM, 4u);
			 return(FALSE);
		 }
		 else if((pinOut < pinIn) && (((pinOut%2U) != 0U) || ((pinOut+1U) != pinIn)))
		 {
			 SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_PARAM, 4u);
			 return(FALSE);
		 }
		 else if(pinOut == pinIn)
		 {
			 SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_PARAM, 4u);
			 return(FALSE);
		 }
		 else
		 {

		 } /*To avoid warning*/
	 }

     /* If nERROR is active then do not proceed with tests that trigger nERROR */
     if(SL_ESM_nERROR_Active())
     {
         SL_Log_Error(FUNC_ID_ST_HET, ERR_TYPE_ENTRY_CON, 3U);
         return(FALSE);
     }


    switch(testType)
    {

    case HET_SRAM_PARITY_TEST:

    	(void)SL_FLAG_SET(testType);

    	/* Backup PCR register */
    	regBackupPCR = sl_hetREG->PCR;

        /*Backup esm interrupt enable register */
        regBkupIntEnaSet = *regIntEnaSet;
        regBckupErrInfulenceSet = *regErrInfulenceSet;

    	/* Disable esm interrupt generation */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET((*regIntEnaClr), GET_ESM_BIT_NUM(esmErrorChannel));

        /* Disable error influence */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET((*regErrInfulenceClr), GET_ESM_BIT_NUM(esmErrorChannel));

    	/* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_hetREG->PCR, HET_TEST_MODE);

    	/* Flip a bit in HET RAM parity location */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        BIT_FLIP((*hetRAMParLoc), 0x1U);

    	/* Disable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_CLEAR(sl_hetREG->PCR, HET_TEST_MODE);

        /* read HETPAR register to allow it to get updatted on error generation */
    	ramRead = sl_hetREG->PAR;

    	/* Cause parity error by reading ram */
    	ramRead = (*hetRAMLoc);

    	/* Check if ESM group1 channel is not flagged */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if(((*regEsmStatus) & GET_ESM_BIT_NUM(esmErrorChannel)) == 0U)
    	{
    		/* HET RAM parity error was not flagged to ESM. */
    		retVal = FALSE;
    	}
    	else
    	{
    		/* verify erronous address */
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    		if((uint32)(hetRAMLocVar & 0xFFFU) == sl_hetREG->PAR)
    		{
    			retVal = TRUE;
    		}
    		else
    		{
    			retVal = FALSE;
    	}
    	}

		/* clear ESM group1 flag */
    	(*regEsmStatus) = GET_ESM_BIT_NUM(ESM_G1ERR_HET1_PARITY_CORRERR);

		/* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(sl_hetREG->PCR, HET_TEST_MODE);

		/* Revert back to correct parity */
		/*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
		if((boolean)hetRAMParLoc)
		{
	        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
			BIT_FLIP((*hetRAMParLoc), 0x1U);
    	}

    	/* Restrore Parity Control register */
    	sl_hetREG->PCR = regBackupPCR;

        /* Restore ESM registers states */
    	*regIntEnaSet = regBkupIntEnaSet;
    	*regErrInfulenceSet = regBckupErrInfulenceSet;


    	break;

    case HET_ANALOG_LOOPBACK_TEST:

    	(void)SL_FLAG_SET(testType);

    	/* Backup ram (first 7 instructions) */
    	/*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    	if((boolean)sl_hetRAM)
    	{
        	for(tmp_count=0U; tmp_count<7U; tmp_count++)
        	{
        		hetRAMBackup[tmp_count].Program = sl_hetRAM->Instruction[tmp_count].Program ;
        		hetRAMBackup[tmp_count].Control = sl_hetRAM->Instruction[tmp_count].Control;
        		hetRAMBackup[tmp_count].Data = sl_hetRAM->Instruction[tmp_count].Data;
        		hetRAMBackup[tmp_count].rsvd1 = sl_hetRAM->Instruction[tmp_count].rsvd1;
        	}

    	}

    	/*  Backup loopback registers and LRP value */
    	regBackupLBPSEL = sl_hetREG->LBPSEL;
    	regBackupLBPDIR = sl_hetREG->LBPDIR;
    	regBackupHETPFR = sl_hetREG->PFR;

    	/* Backup interrupt enable set & request enable set registers */
    	regBackupHetIntEnaSet = sl_hetREG->INTENAS;
    	regBackupHetReqEnaSet = sl_hetREG->REQENS;

    	/* Disable interrupts & requests for first 7 instructions */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BF_SET(sl_hetREG->INTENAC, HET_INST7_SET, HET_INST7_SET_START, HET_INST7_SET_LENGTH);
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BF_SET(sl_hetREG->REQENC, HET_INST7_SET, HET_INST7_SET_START, HET_INST7_SET_LENGTH);

    	/* initialise receive signal */
    	received_signal.duty = 0U;
    	received_signal.period = 0.0F;

    	/* Set LRP to 8 */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BF_SET(sl_hetREG->PFR, HET_LRP8, HET_LRP_START, HET_LRP_LENGTH);

    	/* Set up the ram program for conducting the test */

        /* CNT: Timebase
        *       - Instruction                  = 0
        *       - Next instruction             = 1
        *       - Conditional next instruction = na
        *       - Interrupt                    = na
        *       - Pin                          = na
        *       - Reg                          = T
        */
    	sl_hetRAM->Instruction[0U].Program = 0x00002C81U; /*enabling maxcount interrupt*/
    	sl_hetRAM->Instruction[0U].Control = 0x00000140U; /*setting maxcount = 40LRP*/
    	sl_hetRAM->Instruction[0U].Data = 0x00000000U; /*setting initial count = 0*/
    	sl_hetRAM->Instruction[0U].rsvd1 = 0x00000000U;

        /* PWCNT: PWM -> Duty Cycle
        *         - Instruction                  = 1
        *         - Next instruction             = 2
        *         - Conditional next instruction = 2
        *         - Interrupt                    = 1
        *         - Pin                          = pinOut
        */
    	sl_hetRAM->Instruction[1U].Program = 0x000055C0U;
    	sl_hetRAM->Instruction[1U].Control = (0x00004006U | (uint32)((uint32)pinOut << 8U) | (uint32)((uint32)3U << 3U));
    	sl_hetRAM->Instruction[1U].Data = 0x00000000U;
    	sl_hetRAM->Instruction[1U].rsvd1 = 0x00000000U;

        /* DJZ: PWM -> Period
        *         - Instruction                  = 2
        *         - Next instruction             = 5
        *         - Conditional next instruction = 3
        *         - Interrupt                    = 2
        *         - Pin                          = na
        */
    	sl_hetRAM->Instruction[2U].Program = 0x0000B480U;
    	sl_hetRAM->Instruction[2U].Control = 0x00006006U;
    	sl_hetRAM->Instruction[2U].Data = 0x00000000U;
    	sl_hetRAM->Instruction[2U].rsvd1 = 0x00000000U;

        /* MOV64: PWM -> Duty Cycle Update
        *         - Instruction                  = 3
        *         - Next instruction             = 4
        *         - Conditional next instruction = 2
        *         - Interrupt                    = 1
        *         - Pin                          = pinOut
        */
    	sl_hetRAM->Instruction[3U].Program = 0x00008201U;
    	sl_hetRAM->Instruction[3U].Control = (0x04004007U | (uint32)((uint32)1U << 22U) | (uint32)((uint32)pinOut << 8U) | (uint32)((uint32)3U << 3U));
    	sl_hetRAM->Instruction[3U].Data = (uint32)(7U << 7U);
    	sl_hetRAM->Instruction[3U].rsvd1 = 0x00000000U;

        /* MOV64: PWM -> Period Update
        *         - Instruction                  = 4
        *         - Next instruction             = 5
        *         - Conditional next instruction = 3
        *         - Interrupt                    = 2
        *         - Pin                          = na
        */
    	sl_hetRAM->Instruction[4U].Program = 0x0000A202U;
    	sl_hetRAM->Instruction[4U].Control = (0x00006007U);
    	sl_hetRAM->Instruction[4U].Data = (uint32)(10U << 7U);
    	sl_hetRAM->Instruction[4U].rsvd1 = 0x00000000U;

        /* PCNT: Capture Duty
        *         - Instruction                  = 5
        *         - Next instruction             = 6
        *         - Conditional next instruction = na
        *         - Interrupt                    = na
        *         - Pin                          = pinOut
        */
    	sl_hetRAM->Instruction[5U].Program = 0x0000CE00U | (uint32)((uint32)1U << 6U)  | (uint32)(pinOut);
    	sl_hetRAM->Instruction[5U].Control = 0x00000000U;
    	sl_hetRAM->Instruction[5U].Data = 0x00000000U;
    	sl_hetRAM->Instruction[5U].rsvd1 = 0x00000000U;

        /* PCNT: Capture Period
        *         - Instruction                  = 6
        *         - Next instruction             = 0
        *         - Conditional next instruction = na
        *         - Interrupt                    = na
        *         - Pin                          = pinIn
        */
    	sl_hetRAM->Instruction[6U].Program = 0x00000E80U | (uint32)((uint32)0U << 6U)  | (uint32)(pinIn);
    	sl_hetRAM->Instruction[6U].Control = 0x00000000U;
    	sl_hetRAM->Instruction[6U].Data = 0x00000000U;
    	sl_hetRAM->Instruction[6U].rsvd1 = 0x00000000U;

    	/* loopback setting - select hr pair, loopback type, loopback direction and enable loopback */
    	SL_hetEnableLoopback(sl_hetREG, pinOut);

    	/* turn on het */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(sl_hetREG->GCR, HET_TO_BIT);

    	/* add delay (2 * period of pwm) before capture */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(sl_hetREG->FLG, HET_INTR0_BIT); /* clear maxcount flag if already set */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	/*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    	while(HET_INTR0_BIT != (sl_hetREG->FLG & HET_INTR0_BIT))
    	{

    	} /*wait until flag set again*/


    	/* capGetSignal to capture the duty cycle and time period */
    	SL_capGetSignal(sl_hetRAM, &received_signal);

    	/* compare in and out signals with allowable threshold */
    	if(((received_signal.duty <= (7U+2U)) &&
    		(received_signal.duty >= (7U-2U))) &&
    		((received_signal.period < (10.0F+1.1F)) &&
    		(received_signal.period > (10.0F-1.1F))))
    	{
    		retVal = TRUE;
    	}
    	else
    	{
    		retVal = FALSE;
    	}

    	/* turn off het */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_CLEAR(sl_hetREG->GCR, HET_TO_BIT);

    	/* Restore ram first 7 instructions */
    	for(tmp_count=0U; tmp_count<7U; tmp_count++)
    	{
    		sl_hetRAM->Instruction[tmp_count].Program = hetRAMBackup[tmp_count].Program;
    		sl_hetRAM->Instruction[tmp_count].Control = hetRAMBackup[tmp_count].Control;
    		sl_hetRAM->Instruction[tmp_count].Data = hetRAMBackup[tmp_count].Data;
    		sl_hetRAM->Instruction[tmp_count].rsvd1 = hetRAMBackup[tmp_count].rsvd1;
    	}

    	/* Clear interrupt flags set due to the test */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BF_SET(sl_hetREG->FLG, HET_INST7_SET, HET_INST7_SET_START, HET_INST7_SET_LENGTH);

    	/* Restore loopback registers and LRP value */
    	sl_hetREG->LBPSEL = regBackupLBPSEL;
    	sl_hetREG->LBPDIR = regBackupLBPDIR;
    	sl_hetREG->PFR = regBackupHETPFR;

    	/* Restore interrupt enable set & request enable set registers */
    	sl_hetREG->INTENAS = regBackupHetIntEnaSet;
    	sl_hetREG->REQENS = regBackupHetReqEnaSet;

    	break;

    default:
        retVal = FALSE;
        break;
    }

#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_HET, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif

	SL_FLAG_CLEAR(testType);
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define htu1RAMParLoc	((volatile uint8 *)0xFF4E0207U)
#else
#define htu1RAMParLoc	((volatile uint8 *)0xFF4E0204U)
#endif
#define htu1RAMLocVar		((uint32)0xFF4E0004U)
#define htu1RAMLoc		((volatile uint32 *)0xFF4E0004U)

#if !(defined(_RM42x_) || defined(_TMS570LS04x_))

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define htu2RAMParLoc	((volatile uint8 *)0xFF4C020BU)
#else
#define htu2RAMParLoc	((volatile uint8 *)0xFF4C0208U)
#endif
#define htu2RAMLocVar		((uint32)0xFF4C0008U)
#define htu2RAMLoc		((volatile uint32 *)0xFF4C0008U)
#endif

#endif

/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
boolean SL_SelfTest_HTU(SL_SelfTestType testType, SL_HTU_Instance instance)
{

    volatile uint32 ramRead;
    volatile uint32* htuRAMLoc;
    uint32 htuRAMLocVar;
    volatile uint8* htuRAMParLoc;
    register uint32 regBackupPCR, regBkupIntEnaSet, regBckupErrInfulence;
    sl_htuBASE_t* sl_htuREG;


#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    boolean retVal = FALSE;

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */

    if ((_SELFTEST_HTU_MIN > testType) || (_SELFTEST_HTU_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_HTU, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    if ((HTU_MIN > instance) || (HTU_MAX < instance)) {
           SL_Log_Error(FUNC_ID_ST_HTU, ERR_TYPE_PARAM, 1u);
           retVal =  FALSE;
           return(retVal);
       }
#endif

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for priveleged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_HTU, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }

#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Record_Errorcreationtick(testType);
#endif

    switch(instance)
    {
        case SL_HTU1:
        	sl_htuREG       = sl_htuREG1;
        	htuRAMLoc       = htu1RAMLoc;
        	htuRAMLocVar       = htu1RAMLocVar;
        	htuRAMParLoc = htu1RAMParLoc;
            break;
#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
        case SL_HTU2:
        	sl_htuREG       = sl_htuREG2;
        	htuRAMLoc       = htu2RAMLoc;
        	htuRAMLocVar       = htu2RAMLocVar;
        	htuRAMParLoc = htu2RAMParLoc;
            break;
#endif
        default:
        	sl_htuREG       = sl_htuREG1;
        	htuRAMLoc       = htu1RAMLoc;
        	htuRAMLocVar       = htu1RAMLocVar;
        	htuRAMParLoc = htu1RAMParLoc;
            break;
    }

    /* verify htu is enabled */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if((boolean)sl_htuREG)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if ((uint32)0u == (sl_htuREG->GC & HTU_EN_BIT)) {
            SL_Log_Error(FUNC_ID_ST_HTU, ERR_TYPE_ENTRY_CON, 1u);
            retVal =  FALSE;
            return retVal;
        }
    }


	 /* If HTU parity check is not enabled, return error. */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(((HTU_SRAM_PARITY_DISABLED) == BF_GET(sl_htuREG->PCR, HTU_PARCTRL_PARITY_EN_START, HTU_PARCTRL_PARITY_EN_LENGTH)) &&
		(testType == HTU_SRAM_PARITY_TEST))
	 {
		 SL_Log_Error(FUNC_ID_ST_HTU, ERR_TYPE_ENTRY_CON, 2u);
	     return(FALSE);
	 }

     /* If nERROR is active then do not proceed with tests that trigger nERROR */
     if(SL_ESM_nERROR_Active())
     {
         SL_Log_Error(FUNC_ID_ST_HTU, ERR_TYPE_ENTRY_CON, 3U);
         return(FALSE);
     }


    switch(testType)
    {

   case HTU_SRAM_PARITY_TEST:

	   (void)SL_FLAG_SET(testType);

    	/* Backup PCR register */
    	regBackupPCR = sl_htuREG->PCR;

        /*Backup grp1 esm interrupt enable register */
        regBkupIntEnaSet = esmREG->IESR1;
        regBckupErrInfulence = esmREG->EEPAPR1;

    	/* Disable esm interrupt generation */
        esmREG->IECR1 = GET_ESM_BIT_NUM(ESM_G1ERR_HTU_PARITY_CORRERR);

        /* Disable esm error influence */
        esmREG->DEPAPR1 = GET_ESM_BIT_NUM(ESM_G1ERR_HTU_PARITY_CORRERR);

    	/* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_htuREG->PCR, HTU_TEST_MODE);

    	/* Flip a bit in HTU RAM parity location */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        BIT_FLIP((*htuRAMParLoc), 0x1U);

    	/* Disable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_CLEAR(sl_htuREG->PCR, HTU_TEST_MODE);

        /* read HTUPAR register to allow it to get updatted on error generation */
    	ramRead = sl_htuREG->PAR;

    	/* Cause parity error */
    	ramRead = (*htuRAMLoc);

    	/* Check if ESM group1 channel 8 is not flagged */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if(((sl_htuREG->PAR & HTU_PAR_ERR_FLG) == 0U) ||
    	   ((esmREG->SR1[0U] & GET_ESM_BIT_NUM(ESM_G1ERR_HTU_PARITY_CORRERR)) == 0U))
    	{
    		/* HTU RAM parity error was not flagged to ESM. */
    		retVal = FALSE;
    	}
    	else
    	{
    		/* verify erronous address */
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    		if(BF_GET(sl_htuREG->PAR, HTU_PAR_ADDR_START, HTU_PAR_ADDR_LENGTH) ==
    		  (uint32)(htuRAMLocVar & 0xFFFU))
    		{
    			retVal = TRUE;
    		}
    		else
    		{
    			retVal = FALSE;
    		}
    	}

    	/* clear ESM group1 channel 8 flag */
    	esmREG->SR1[0U] = GET_ESM_BIT_NUM(ESM_G1ERR_HTU_PARITY_CORRERR);

        /* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(sl_htuREG->PCR, HTU_TEST_MODE);

    	/* Revert back to correct parity */
        /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
        if((boolean)htuRAMParLoc)
        {
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        	BIT_FLIP((*htuRAMParLoc), 0x1U);
        }

    	/* Restrore Parity Control register */
    	sl_htuREG->PCR = regBackupPCR;

        /* Restore ESM registers states */
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;

    	break;

    default:
        retVal = FALSE;
        break;
    }

#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_HTU, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif

	SL_FLAG_CLEAR(testType);
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
void SL_mibspiEnableLoopback(mibspiBASE_t* mibspi, loopBackType_t Loopbacktype)
{

    /* Clear Loopback incase enabled already */
    mibspi->IOLPKTSTCR = 0U;

    /* Enable Loopback either in Analog or Digital Mode */
    mibspi->IOLPKTSTCR = (uint32)0x00000A00U
                       | (uint32)((uint32)Loopbacktype << 1U);

}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
void SL_mibspiSetData(mibspiBASE_t* mibspi, mibspiRAM_t* ram, uint32 group, uint16* data)
{
    uint32 start  = (mibspi->TGCTRL[group] >> 8U) & 0xFFU;
    uint32 end;

    if (group == 7U)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	end = (mibspi->LTGPEND & 0x00007F00U) >> 8U;
    	end = end + 1U;
    }
    else
    {
    	group = group + 1U;
    	end = (mibspi->TGCTRL[group] >> 8U) & 0xFFU;
    }

    if (end == 0U)
    {
    	end = 128U;
    }

    while (start < end)
    {
    /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        ram->tx[start].data = *data;
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        data++;
        start++;
    }

}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
void SL_mibspiTransfer(mibspiBASE_t* mibspi, uint32 group)
{
    mibspi->TGCTRL[group] |= 0x80000000U;
}


boolean SL_mibspiIsTransferComplete(mibspiBASE_t* mibspi, uint32 group)
{
    boolean status;
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if((((uint32)((uint32)(mibspi->TGINTFLG & 0xFFFF0000U) >> 16U) >> group) & 1U) != 0U)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	mibspi->TGINTFLG = (mibspi->TGINTFLG & 0x0000FFFFU) | ((uint32)((uint32)1U << group) << 16U);
       status = TRUE;
    }
    else
    {
       status = FALSE;
    }

    return (status);
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
uint32 SL_mibspiGetData(mibspiBASE_t* mibspi, mibspiRAM_t* ram, uint32 group, uint16* data)
{
    uint32 start  = (mibspi->TGCTRL[group] >> 8U) & 0xFFU;
    uint32 end;
    uint16 mibspiFlags  = 0U;
	uint32 ret;

    if (group == 7U)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	end = (mibspi->LTGPEND & 0x00007F00U) >> 8U;
    	end = end + 1U;
    }
    else
    {
    	group = group + 1U;
    	end = (mibspi->TGCTRL[group] >> 8U) & 0xFFU;
    }

    if (end == 0U)
    {
    	end = 128U;
    }

    while (start < end)
    {
        mibspiFlags  |= ram->rx[start].flags;
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        *data = ram->rx[start].data;
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        data++;
        start++;
    }

	ret = ((uint32)mibspiFlags >> 8U) & 0x5FU;

    return ret;
}
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define mibspi1uerrESMReg (volatile uint32*)&(esmREG->SR1[0])
#define mibspi1iesrESMReg (volatile uint32*)&(esmREG->IESR1)
#define mibspi1iecrESMReg (volatile uint32*)&(esmREG->IECR1)
#define mibspi1eepaprESMReg (volatile uint32*)&(esmREG->EEPAPR1)
#define mibspi1depaprESMReg (volatile uint32*)&(esmREG->DEPAPR1)

#define mibspi2uerrESMReg (volatile uint32*)&(esmREG->SR4[0])
#define mibspi2iesrESMReg (volatile uint32*)&(esmREG->IESR4)
#define mibspi2iecrESMReg (volatile uint32*)&(esmREG->IECR4)
#define mibspi2eepaprESMReg (volatile uint32*)&(esmREG->IEPSR4)
#define mibspi2depaprESMReg (volatile uint32*)&(esmREG->IEPCR4)

#define mibspi3uerrESMReg (volatile uint32*)&(esmREG->SR1[0])
#define mibspi3iesrESMReg (volatile uint32*)&(esmREG->IESR1)
#define mibspi3iecrESMReg (volatile uint32*)&(esmREG->IECR1)
#define mibspi3eepaprESMReg (volatile uint32*)&(esmREG->EEPAPR1)
#define mibspi3depaprESMReg (volatile uint32*)&(esmREG->DEPAPR1)

#define mibspi4uerrESMReg (volatile uint32*)&(esmREG->SR4[0])
#define mibspi4iesrESMReg (volatile uint32*)&(esmREG->IESR4)
#define mibspi4iecrESMReg (volatile uint32*)&(esmREG->IECR4)
#define mibspi4eepaprESMReg (volatile uint32*)&(esmREG->IEPSR4)
#define mibspi4depaprESMReg (volatile uint32*)&(esmREG->IEPCR4)

#define mibspi5uerrESMReg (volatile uint32*)&(esmREG->SR1[0])
#define mibspi5iesrESMReg (volatile uint32*)&(esmREG->IESR1)
#define mibspi5iecrESMReg (volatile uint32*)&(esmREG->IECR1)
#define mibspi5eepaprESMReg (volatile uint32*)&(esmREG->EEPAPR1)
#define mibspi5depaprESMReg (volatile uint32*)&(esmREG->DEPAPR1)
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

#define mibspi1RAMParLoc ((volatile uint8 *)(0xFF0E0004U + 0x00000400U))

#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
#define mibspi3RAMParLoc ((volatile uint8 *)(0xFF0C0008U + 0x00000400U))
#define mibspi5RAMParLoc ((volatile uint8 *)(0xFF0A000CU + 0x00000400U))
#endif

#define mibspi1RAMLoc	 ((volatile uint32 *)0xFF0E0004U)
#define mibspi1RAMLocVar	 ((uint32)0xFF0E0004U)

#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
#define mibspi3RAMLoc	 ((volatile uint32 *)0xFF0C0008U)
#define mibspi5RAMLoc	 ((volatile uint32 *)0xFF0A000CU)
#define mibspi3RAMLocVar	 ((uint32)0xFF0C0008U)
#define mibspi5RAMLocVar	 ((uint32)0xFF0A000CU)
#endif

#endif


boolean SL_SelfTest_MibSPI(SL_SelfTestType testType, SL_MIBSPI_Instance instance)
{
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    boolean testPassed = FALSE;
    volatile boolean _sl_fault_injection = FALSE;
    register uint32 ramread32, regBackupEccCtl, regBckupErrInfulence, regBkupIntEnaSet, regBackuMIBSPIE;
    volatile uint32* data;
    uint32           dataVal;
    uint32 offset;


    uint32 esm1bitecc, esm2bitecc;
    volatile uint32* esm2bitStatReg;
    volatile uint32* esm2bitEEPAPR;
    volatile uint32* esm2bitDEPAPR; 
    volatile uint32* esm2bitIESR;   
    volatile uint32* esm2bitIECR;

    register uint32 regBackupLBP;
    register uint32 regBackupINT0, regBackupTGITENST;
    uint32 regBackupTGCTRL[2U];
    uint16 ramBackupMIBSPI[2U];
	uint16 rxdata = 0U;
	uint16 txdata = 0xAAU;
	uint32 tGroup = 0U; /*transfer group0 used for loopback test*/
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    register uint32 regBackupPCR, regBackupLBP, regBackupSPIE, regBkupIntEnaSet, regBckupErrInfulence;
    register uint32 regBackupINT0, regBackupTGITENST;
    uint32 regBackupTGCTRL[2U];
    uint16 ramBackupMIBSPI[2U] = {0u, 0u};
    volatile uint32* mibspiRAMLoc = 0u;
    uint32 mibspiRAMLocVar = 0u;
    volatile uint8* mibspiRAMParLoc = 0u;
    uint32 esmErrorChannel;
	uint16 rxdata = 0U;
	uint16 txdata = 0xAAU;
	uint32 tGroup = 0U; /* transfer group0 used for loopback test */
#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    mibspiBASE_t* mibspiREG = 0u;
    mibspiRAM_t* mibspiRAM = 0u;
    volatile uint32 ramRead;
    boolean retVal = FALSE;

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for privileged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_MIBSPI, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if((TRUE) == SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_MIBSPI, ERR_TYPE_ENTRY_CON, 1U);
        return(FALSE);
    }
#endif

#ifdef FUNCTION_PARAM_CHECK_ENABLED

    /* Verify that the parameters are in range */

    if ((_SELFTEST_MIBSPI_MIN > testType) || (_SELFTEST_MIBSPI_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_MIBSPI, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    	}

    if ((MIBSPI_MIN > instance) || (MIBSPI_MAX < instance)) {
           SL_Log_Error(FUNC_ID_ST_MIBSPI, ERR_TYPE_PARAM, 1u);
           retVal =  FALSE;
           return(retVal);
       }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)


    /* If fault inject set global variable to flag to the ESM handler that it is a fault injection */
    if((MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT == testType)||
        (MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT == testType)){
        _sl_fault_injection=TRUE;
    }
#endif


#if FUNCTION_PROFILING_ENABLED
    SL_Record_Errorcreationtick(testType);
#endif

    switch(instance)
    {
        case SL_MIBSPI1:
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
            mibspiREG = mibspiREG1;
            mibspiRAM = mibspiRAM1;
            mibspiRAMLoc = mibspi1RAMLoc;
            mibspiRAMLocVar = mibspi1RAMLocVar;
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            mibspiRAMParLoc = mibspi1RAMParLoc;
            esmErrorChannel = ESM_G1ERR_MIBSPI1_PARITY_CORRERR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            mibspiREG = mibspiREG1;
            mibspiRAM = mibspiRAM1;
            if((testType == MIBSPI_ECC_TEST_MODE_1BIT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT) ||
               (testType == MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT))
            {
                esm1bitecc     = ESM_G1ERR_SPI1_ECC_SBERR;
                esm2bitecc     = ESM_G1ERR_SPI1_ECC_UNCORR;
                esm2bitStatReg = mibspi1uerrESMReg;
                esm2bitEEPAPR  = mibspi1eepaprESMReg;
                esm2bitDEPAPR  = mibspi1depaprESMReg;
                esm2bitIESR    = mibspi1iesrESMReg;
                esm2bitIECR    = mibspi1iecrESMReg;
            }

#endif
            break;

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        case SL_MIBSPI2:

            mibspiREG = mibspiREG2;
            mibspiRAM = mibspiRAM2;
            if((testType == MIBSPI_ECC_TEST_MODE_1BIT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT) ||
               (testType == MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT))
            {
                esm1bitecc     = ESM_G1ERR_SPI2_ECC_SBERR;
                esm2bitecc     = ESM_G1ERR_SPI2_ECC_UNCORR;
                esm2bitStatReg = mibspi2uerrESMReg;
                esm2bitEEPAPR  = mibspi2eepaprESMReg;
                esm2bitDEPAPR  = mibspi2depaprESMReg;
                esm2bitIESR    = mibspi2iesrESMReg;
                esm2bitIECR    = mibspi2iecrESMReg;
            }

            break;
#endif
        case SL_MIBSPI3:
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
            mibspiREG = mibspiREG3;
            mibspiRAM = mibspiRAM3;
            mibspiRAMLoc = mibspi3RAMLoc;
            mibspiRAMLocVar = mibspi3RAMLocVar;
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            mibspiRAMParLoc = mibspi3RAMParLoc;
            esmErrorChannel = ESM_G1ERR_MIBSPI3_PARITY_CORRERR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            mibspiREG = mibspiREG3;
            mibspiRAM = mibspiRAM3;
            if((testType == MIBSPI_ECC_TEST_MODE_1BIT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT) ||
               (testType == MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT))
            {
                esm1bitecc     = ESM_G1ERR_SPI3_ECC_SBERR;
                esm2bitecc     = ESM_G1ERR_SPI3_ECC_UNCORR;
                esm2bitStatReg = mibspi3uerrESMReg;
                esm2bitEEPAPR  = mibspi3eepaprESMReg;
                esm2bitDEPAPR  = mibspi3depaprESMReg;
                esm2bitIESR    = mibspi3iesrESMReg;
                esm2bitIECR    = mibspi3iecrESMReg;
            }

#endif
            break;

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        case SL_MIBSPI4:
            mibspiREG = mibspiREG4;
            mibspiRAM = mibspiRAM4;
            if((testType == MIBSPI_ECC_TEST_MODE_1BIT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT) ||
               (testType == MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT))
            {
                esm1bitecc     = ESM_G1ERR_SPI4_ECC_SBERR;
                esm2bitecc     = ESM_G1ERR_SPI4_ECC_UNCORR;
                esm2bitStatReg = mibspi4uerrESMReg;
                esm2bitEEPAPR  = mibspi4eepaprESMReg;
                esm2bitDEPAPR  = mibspi4depaprESMReg;
                esm2bitIESR    = mibspi4iesrESMReg;
                esm2bitIECR    = mibspi4iecrESMReg;
            }

            break;
#endif

        case SL_MIBSPI5:
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
            mibspiREG = mibspiREG5;
            mibspiRAM = mibspiRAM5;
            mibspiRAMLoc = mibspi5RAMLoc;
            mibspiRAMLocVar = mibspi5RAMLocVar;
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            mibspiRAMParLoc = mibspi5RAMParLoc;
            esmErrorChannel = ESM_G1ERR_MIBSPI5_PARITY_CORRERR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            mibspiREG = mibspiREG5;
            mibspiRAM = mibspiRAM5;
            if((testType == MIBSPI_ECC_TEST_MODE_1BIT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT) ||
               (testType == MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT))
            {
                esm1bitecc     = ESM_G1ERR_SPI5_ECC_SBERR;
                esm2bitecc     = ESM_G1ERR_SPI5_ECC_UNCORR;
                esm2bitStatReg = mibspi5uerrESMReg;
                esm2bitEEPAPR  = mibspi5eepaprESMReg;
                esm2bitDEPAPR  = mibspi5depaprESMReg;
                esm2bitIESR    = mibspi5iesrESMReg;
                esm2bitIECR    = mibspi5iecrESMReg;
            }

#endif
            break;

        default:
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
            mibspiREG = mibspiREG1;
            mibspiRAM = mibspiRAM1;
            mibspiRAMLoc = mibspi1RAMLoc;
            mibspiRAMLocVar = mibspi1RAMLocVar;
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            mibspiRAMParLoc = mibspi1RAMParLoc;
            esmErrorChannel = ESM_G1ERR_MIBSPI1_PARITY_CORRERR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            mibspiREG = mibspiREG1;
            mibspiRAM = mibspiRAM1;
            if((testType == MIBSPI_ECC_TEST_MODE_1BIT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT) ||
               (testType == MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT))
            {
                esm1bitecc     = ESM_G1ERR_SPI1_ECC_SBERR;
                esm2bitecc     = ESM_G1ERR_SPI1_ECC_UNCORR;
                esm2bitStatReg = mibspi1uerrESMReg;
                esm2bitEEPAPR  = mibspi1eepaprESMReg;
                esm2bitDEPAPR  = mibspi1depaprESMReg;
                esm2bitIESR    = mibspi1iesrESMReg;
                esm2bitIECR    = mibspi1iecrESMReg;
            }

#endif
            break;
    }

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    if((testType == MIBSPI_ECC_TEST_MODE_1BIT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT) ||
       (testType == MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT) || (testType == MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT))
    {
        if(BF_GET(mibspiREG->MIBSPIE, SPI_MIBSPIE_EX_BUF_START, SPI_MIBSPIE_EX_BUF_LENGTH) == SPI_MIBSPIE_EX_BUF_DIS) {
            offset = DEFAULT;
        } else {
            offset = EXTENDED;
        }

        /* backUp registers */
        regBackupEccCtl = mibspiREG->PAR_ECC_CTRL;
        regBackuMIBSPIE = mibspiREG->MIBSPIE;

        /* enable ECC memory test */
        BIT_SET(mibspiREG->PAR_ECC_CTRL, SPI_ECC_MEMTST_EN);

        /* enable RXRAM access */
        BIT_SET(mibspiREG->MIBSPIE, SPI_MIBSPI_RXRAM_ACCESS);

        /* enable ECC diagnostic mode */
        BF_SET(mibspiREG->ECCDIAG_CTRL, SPI_DIAG_CTRL_ECC_EN, SPI_DIAG_CTRL_ECC_START, SPI_DIAG_CTRL_ECC_LENGTH);

        /* initialize data */
        data = (volatile uint32 *)((uint32)mibspiRAM + offset + 4);

        /* backUp the data */
        dataVal = (uint32)(*data);
    }

#endif

#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif


    /* verify SPI is out of reset */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if((boolean)mibspiREG)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if (SPI_GCR0_RESET != (mibspiREG->GCR0 & SPI_GCR0_RESET)) {
            SL_Log_Error(FUNC_ID_ST_MIBSPI, ERR_TYPE_ENTRY_CON, 2u);
            retVal =  FALSE;
            return retVal;
        }
    }

    /* verify mibSPI is enabled */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (SPI_MIBSPIE_MSPIENA != (mibspiREG->MIBSPIE & SPI_MIBSPIE_MSPIENA)) {
        SL_Log_Error(FUNC_ID_ST_MIBSPI, ERR_TYPE_ENTRY_CON, 3u);
        retVal =  FALSE;
        return retVal;
    }

    /* verify mibSPI ram is initialized */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (SPI_MIBSPIE_BUFINITACTIVE == (mibspiREG->FLG & SPI_MIBSPIE_BUFINITACTIVE)) {
        SL_Log_Error(FUNC_ID_ST_MIBSPI, ERR_TYPE_ENTRY_CON, 4u);
        retVal =  FALSE;
        return retVal;
    }

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

	 /* If MIBSPI parity check is not enabled, return error. */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(((SPI_PAR_CTRL_ECC_DIS) == BF_GET(mibspiREG->PAR_ECC_CTRL, SPI_PAR_CTRL_ECC_START, SPI_PAR_CTRL_ECC_LENGTH)) &&
		(testType == MIBSPI_SRAM_PARITY_TEST))
	 {
		 SL_Log_Error(FUNC_ID_ST_MIBSPI, ERR_TYPE_ENTRY_CON, 5u);
	     return(FALSE);
	 }
#endif

    switch(testType)
    {
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    case MIBSPI_ECC_TEST_MODE_1BIT:
    case MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT:

    	(void)SL_FLAG_SET(testType);

        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        regBkupIntEnaSet = esmREG->IESR7;

        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBckupErrInfulence = esmREG->IEPSR7;

        if((FALSE) == _sl_fault_injection) {
            esmREG->IECR7 = GET_ESM_BIT_NUM(esm1bitecc);
            esmREG->IEPCR7 = GET_ESM_BIT_NUM(esm1bitecc);
        } 

        /* enable single bit error event */
        BF_SET(mibspiREG->PAR_ECC_CTRL, SPI_PAR_ECC_CTRL_SBE_EVT_EN, SPI_PAR_ECC_CTRL_SBE_EVT_START, SPI_PAR_ECC_CTRL_SBE_EVT_LENGTH);

        /* corrupt the data */
        BIT_FLIP(*data, SPI_FLIP_DATA_1BIT);

        /* enable SECDED */
        BF_SET(mibspiREG->PAR_ECC_CTRL, SPI_PAR_CTRL_ECC_EN, SPI_PAR_CTRL_ECC_START, SPI_PAR_CTRL_ECC_LENGTH);

        /*required when running with optimisation*/
        _SL_Barrier_Data_Access();

        ramread32 = *data;

        if(MIBSPI_ECC_TEST_MODE_1BIT == testType) {
            if(GET_ESM_BIT_NUM(esm1bitecc) == 
                    (esmREG->SR7[0] & GET_ESM_BIT_NUM(esm1bitecc))) {
                testPassed = TRUE;
                /* clear ESM error status */
                esmREG->SR7[0] = GET_ESM_BIT_NUM(esm1bitecc);
            }

            
            if((testPassed == TRUE) && 
                    ((mibspiREG->UERRSTAT & BIT(SPI_PAR_ECC_STAT_SBEFLGRX)) == BIT(SPI_PAR_ECC_STAT_SBEFLGRX)) &&
                    ((mibspiREG->ECCDIAG_STAT & BIT(SPI_ECCDIAG_STAT_SBEFLGRX)) == BIT(SPI_ECCDIAG_STAT_SBEFLGRX)) &&
                    ((offset + 4) == (uint32)BF_GET(mibspiREG->SBERRADDR1, SPI_SBERR_ADDRRX_START, SPI_SBERR_ADDRRX_LENGTH))){


                retVal = TRUE;

            } else {
                retVal = FALSE;
            }
        }
        else {
            /* Fault injected */
            retVal = TRUE;
        }

        /* clear the single bit error status */
        mibspiREG->UERRSTAT = BIT(SPI_PAR_ECC_STAT_SBEFLGRX);
        mibspiREG->ECCDIAG_STAT = BIT(SPI_ECCDIAG_STAT_SBEFLGRX);
        /* Restore data */
        *data = dataVal;

        /* disable diagnostic mode */
        BF_SET(mibspiREG->ECCDIAG_CTRL, SPI_DIAG_CTRL_ECC_DIS, SPI_DIAG_CTRL_ECC_START, SPI_DIAG_CTRL_ECC_LENGTH);

        /* Restore SPI registers states */
        mibspiREG->PAR_ECC_CTRL = regBackupEccCtl;
        mibspiREG->MIBSPIE = regBackuMIBSPIE;

        /* Restore grp1 esm interrupt enable */
        esmREG->IESR7 = regBkupIntEnaSet;
        esmREG->IEPSR7 = regBckupErrInfulence;

        break;

    case MIBSPI_ECC_TEST_MODE_2BIT:
    case MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT:

    	(void)SL_FLAG_SET(testType);

        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        regBkupIntEnaSet = *esm2bitIESR;

        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBckupErrInfulence = *esm2bitEEPAPR;

        if((FALSE) == _sl_fault_injection) {
            *esm2bitIECR = GET_ESM_BIT_NUM(esm2bitecc);
            *esm2bitDEPAPR = GET_ESM_BIT_NUM(esm2bitecc);
        } 

        /* corrupt the data */
        *data ^= SPI_FLIP_DATA_2BIT;

        /* enable SECDED */
        BF_SET(mibspiREG->PAR_ECC_CTRL, SPI_PAR_CTRL_ECC_EN, SPI_PAR_CTRL_ECC_START, SPI_PAR_CTRL_ECC_LENGTH);

        /*required when running with optimisation*/
        _SL_Barrier_Data_Access();

        ramread32 = *data;

        if(MIBSPI_ECC_TEST_MODE_2BIT == testType) {
            if(GET_ESM_BIT_NUM(esm2bitecc) == 
                    (*esm2bitStatReg & GET_ESM_BIT_NUM(esm2bitecc))) {
                        testPassed = TRUE;
                        /* clear ESM error status */
                        *esm2bitStatReg = GET_ESM_BIT_NUM(esm2bitecc);
            }

            
            if((testPassed == TRUE) && 
                    ((mibspiREG->UERRSTAT & BIT(SPI_PAR_ECC_STAT_UERRFLGRX)) == BIT(SPI_PAR_ECC_STAT_UERRFLGRX)) &&
                    ((mibspiREG->ECCDIAG_STAT & BIT(SPI_ECCDIAG_STAT_DEFLGRX)) == BIT(SPI_ECCDIAG_STAT_DEFLGRX)) &&
                    ((offset + 4) == (uint32)BF_GET(mibspiREG->UERRADDRRX, SPI_UERR_ADDRRX_START, SPI_UERR_ADDRRX_LENGTH))) {


                retVal = TRUE;
            } else {
                retVal = FALSE;
            }
        }
        else {
            /* Fault injected */
            retVal = TRUE;
        }

        /* clear the single bit error status */
        mibspiREG->UERRSTAT = BIT(SPI_PAR_ECC_STAT_UERRFLGRX);
        mibspiREG->ECCDIAG_STAT = BIT(SPI_PAR_ECC_STAT_UERRFLGRX);

        /* Restore data */
        *data = dataVal;

        /* disable diagnostic mode */
        BF_SET(mibspiREG->ECCDIAG_CTRL, SPI_DIAG_CTRL_ECC_DIS, SPI_DIAG_CTRL_ECC_START, SPI_DIAG_CTRL_ECC_LENGTH);

        /* Restore SPI registers states */
        mibspiREG->PAR_ECC_CTRL = regBackupEccCtl;
        mibspiREG->MIBSPIE = regBackuMIBSPIE;

        /* restore grp1 ESM interrupt enable */
        *esm2bitIESR = regBkupIntEnaSet;
        *esm2bitEEPAPR = regBckupErrInfulence;

        break;
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case MIBSPI_SRAM_PARITY_TEST:

    	(void)SL_FLAG_SET(testType);

    	/* Backup PCR and SPIE registers */
    	regBackupPCR = mibspiREG->PAR_ECC_CTRL;
    	regBackupSPIE = mibspiREG->MIBSPIE;

        /*Backup grp1 esm interrupt enable register */
        regBkupIntEnaSet = esmREG->IESR1;
        regBckupErrInfulence = esmREG->EEPAPR1;

    	/* Disable esm interrupt generation */
        esmREG->IECR1 = GET_ESM_BIT_NUM(esmErrorChannel);

    	/* Disable esm error influence */
        esmREG->DEPAPR1 = GET_ESM_BIT_NUM(esmErrorChannel);

    	/* Enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(mibspiREG->PAR_ECC_CTRL, MIBSPI_TEST_MODE);

        /* flip bit of the parity location */
        /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
        if((boolean)mibspiRAMParLoc)
        {
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        	BIT_FLIP((*mibspiRAMParLoc), 0x1U);
    	}

    	/* Disable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_CLEAR(mibspiREG->PAR_ECC_CTRL, MIBSPI_TEST_MODE);

        /* read clear UERRADDRTX register */
        ramRead = mibspiREG->UERRADDRTX;

    	/* Cause parity error */
    	ramRead = (*mibspiRAMLoc);

    	/* Check if ESM group1 channel 18 is not flagged */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if (((mibspiREG->UERRSTAT & MIBSPI_PAR_ERR_FLG) == 0U) ||
    		((esmREG->SR1[0U] & GET_ESM_BIT_NUM(esmErrorChannel)) == 0U))
    	{
    		/* MIBSPI RAM parity error was not flagged to ESM. */
    		retVal = FALSE;
    	}
    	else
    	{
    		/* verify erronous address */
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    		if(mibspiREG->UERRADDRTX == (uint32)(mibspiRAMLocVar & 0xFFFU))
    		{
    			retVal = TRUE;
    		}
    		else
    		{
    			retVal = FALSE;
    		}

    	}

    	/* clear MIBSPI parity error flag (precautionary for emulation mode) */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(mibspiREG->UERRSTAT, MIBSPI_PAR_ERR_FLG);

    	/* clear ESM group1 channel flag */
    	esmREG->SR1[0U] = GET_ESM_BIT_NUM(esmErrorChannel);

		/* enable parity test mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(mibspiREG->PAR_ECC_CTRL, MIBSPI_TEST_MODE);

		/* Revert back to correct parity */
		/*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
		if((boolean)mibspiRAMParLoc)
		{
			/*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
			BIT_FLIP((*mibspiRAMParLoc), 0x1U);
    	}

    	/* Restrore PCR and SPIE registers */
    	mibspiREG->PAR_ECC_CTRL = regBackupPCR;
    	mibspiREG->MIBSPIE = regBackupSPIE;

        /* Restore ESM registers states */
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;

    	break;
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
    case MIBSPI_ANALOG_LOOPBACK_TEST:

    	(void)SL_FLAG_SET(testType);

    	/* Backup LOOPBACK, INT0 and TGITENST registers */
    	regBackupLBP = mibspiREG->IOLPKTSTCR;
    	regBackupINT0 = mibspiREG->INT0;
    	regBackupTGITENST = mibspiREG->TGITENST;

    	/* Backup TG0CTRL & TG1CTRL registers */
    	regBackupTGCTRL[0U] = mibspiREG->TGCTRL[0U];
    	regBackupTGCTRL[1U] = mibspiREG->TGCTRL[1U];

    	/* Backup mibspi ram tx[0] & tx[1] control fields */
    	/*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    	if((boolean)mibspiRAM)
    	{
        	ramBackupMIBSPI[0U] = mibspiRAM->tx[0U].control;
        	ramBackupMIBSPI[1U] = mibspiRAM->tx[1U].control;

    	}

    	/* Disable interrupts and dma requests */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BF_SET(mibspiREG->INT0, SPI_INTR_DIS, SPI_INTR_DIS_START, SPI_INTR_DIS_LENGTH);
    	mibspiREG->TGITENCR = (uint32)0xFFFFFFFFu;

    	/* transfer group settings for the test -
    	 *
    	 * Set Group0 PSTART bits to 0u
    	 * Set Group0 Trigger source to 0u
    	 * Set Group0 Trigger event to 7h
    	 * Enable one shot for Group0
    	 * Set Group1 PSTART bits to desired tgLength (1u in this test)
    	 * Settings for mibspi ram tx[0] and tx[1] control fields
    	 */

    	/* Set Group0 PSTART bits to 0u */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    	BF_SET(mibspiREG->TGCTRL[tGroup], MIBSPI_TG0CTRL_PSTART_VAL, MIBSPI_TGCTRL_PSTART_START, MIBSPI_TGCTRL_PSTART_LENGTH);

    	/* Set Group0 Trigger source to 0u */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    	BF_SET(mibspiREG->TGCTRL[tGroup], MIBSPI_TG0CTRL_TRIGSRC_VAL, MIBSPI_TGCTRL_TRIGSRC_START, MIBSPI_TGCTRL_TRIGSRC_LENGTH);

    	/* Set Group0 Trigger event to 7h */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    	BF_SET(mibspiREG->TGCTRL[tGroup], MIBSPI_TG0CTRL_TRIGEVT_VAL, MIBSPI_TGCTRL_TRIGEVT_START, MIBSPI_TGCTRL_TRIGEVT_LENGTH);

    	/* Enable one shot for Group0 */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    	BIT_SET(mibspiREG->TGCTRL[tGroup], MIBSPI_TGCTRL_ONESHOT);

    	/* Set Group1 PSTART bits to desired tgLength (1u in this test) */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
    	BF_SET(mibspiREG->TGCTRL[tGroup+1U], MIBSPI_TG1CTRL_PSTART_VAL, MIBSPI_TGCTRL_PSTART_START, MIBSPI_TGCTRL_PSTART_LENGTH);

    	/* Settings for mibspi ram tx[0] control field */
    	mibspiRAM->tx[0U].control = (uint16)((uint16)4U << 13U)  /*continuous buffer mode */
                                  | (uint16)((uint16)1U << 12U) /* no need to deactivate chip select after first buffer transmit */
                                  | (uint16)((uint16)0U << 10U)  /* no WDELAY */
                                  | (uint16)((uint16)0U << 8U) /* dfsel */
                                  | (uint16)(CS_0);  /* chip select for next buffer */

    	/* Settings for mibspi ram tx[1] control field */
    	mibspiRAM->tx[1U].control = (uint16)((uint16)4U << 13U)  /*continuous buffer mode */
                                  | (uint16)((uint16)0U << 12U) /* chip select hold must be zero for last buffer */
                                  | (uint16)((uint16)0U << 10U)  /* no WDELAY */
                                  | (uint16)((uint16)0U << 8U)  /* dfsel*/
                                  | (uint16)(CS_NONE);  /* chip select - no next buffer */

        /* Enable External Loopback */
    	SL_mibspiEnableLoopback(mibspiREG, Analog_Lbk);

    	/* set up message */
    	SL_mibspiSetData(mibspiREG, mibspiRAM, tGroup, &txdata);

    	/* transmit message */
    	SL_mibspiTransfer(mibspiREG, tGroup);

    	/* wait till transfer complete */
    	/*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    	while(!(SL_mibspiIsTransferComplete(mibspiREG, tGroup)))
    	{

    	}/*wait*/

    	/* recieve message successfully */
    	if(0U != SL_mibspiGetData(mibspiREG, mibspiRAM, tGroup, &rxdata))
    	{
    	    retVal = FALSE;
    	}
    	else
    	{
        	/* compare messages */
        	if(rxdata == txdata)
        	{
        		retVal = TRUE;
        	}
        	else
        	{
        		retVal = FALSE;
    	}
    	}

    	/* Restore mibspi ram tx[0] & tx[1] control fields */
    	mibspiRAM->tx[0U].control = ramBackupMIBSPI[0U];
    	mibspiRAM->tx[1U].control = ramBackupMIBSPI[1U];

    	/* Restore TG0CTRL & TG1CTRL registers */
    	mibspiREG->TGCTRL[0U] = regBackupTGCTRL[0U];
    	mibspiREG->TGCTRL[1U] = regBackupTGCTRL[1U];

    	/* restore LOOPBACK, INT0 and TGITENST registers */
    	mibspiREG->IOLPKTSTCR = regBackupLBP;
    	mibspiREG->INT0 = regBackupINT0;
    	mibspiREG->TGITENCR = (uint32)0xFFFFFFFFu; /* clear all bits first */
    	mibspiREG->TGITENST = regBackupTGITENST;

    	break;

#endif

    default:
        retVal = FALSE;
        break;
    }

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    /* If the test was fault injection and not self test, then do not mask the call back & clear the *
    *  FAULT_INJECTION flag for subsequent runs*/
    if((TRUE) == _sl_fault_injection){
            _sl_fault_injection=FALSE;
    }
    SL_FLAG_CLEAR(testType);

    ramread32 = ramread32; /* Avoid compiler warning. */
#endif

    #if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_SPI, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif

	SL_FLAG_CLEAR(testType);
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_spiEnableLoopback(sl_spiBASE_t* spi, loopBackType_t Loopbacktype)
{
    /* Clear Loopback incase enabled already */
    spi->IOLPKTSTCR = 0U;

    /* Enable Loopback either in Analog or Digital Mode */
    spi->IOLPKTSTCR = (uint32)0x00000A00U
                    | (uint32)((uint32)Loopbacktype << 1U);
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

uint32 SL_spiTransmitAndReceiveData(sl_spiBASE_t* spi, sl_spiDAT1_t* dataconfig_t, uint32 blocksize, uint16* srcbuff, uint16* destbuff)
{
    uint16 Tx_Data;
    uint32 Rx_Data;
    uint32 blk_size = blocksize;
    uint32 Chip_Select_Hold = (dataconfig_t->CS_HOLD != 0u) ? 0x10000000U : 0U;
    uint32 WDelay = (dataconfig_t->WDEL != 0u) ? 0x04000000U : 0U;
    sl_SPIDATAFMT_t DataFormat = dataconfig_t->DFSEL;
    uint8 ChipSelect = dataconfig_t->CSNR;

    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    while((((spi->FLG) & (uint32)0x000000FFU) == 0U) && (blk_size != 0U))
    {

        if(blk_size == 1U)
        {
           Chip_Select_Hold = 0U;
        }
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        Tx_Data = *srcbuff;

        spi->DAT1 =(uint32)(((uint32)DataFormat) << 24U) |
                   (uint32)((uint32)ChipSelect << 16U) |
                   (WDelay)           |
                   (Chip_Select_Hold) |
				   (uint32)Tx_Data;

        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        srcbuff++;
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        while((spi->FLG & 0x00000100U) != 0x00000100U)
        {
        } /* Wait */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        Rx_Data = spi->BUF;
        *destbuff = (uint16)Rx_Data;
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        destbuff++;

        blk_size--;
    }

    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    return ((uint32)(spi->FLG & 0xFFU));
}

#endif

/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
boolean SL_SelfTest_SPI(SL_SelfTestType testType, SL_SPI_Instance instance)
{

	uint16 rxdata=0U,txdata=0xAAAAU;
	sl_spiDAT1_t dataconfig_t;
	sl_spiBASE_t* sl_spiREG;
	uint32 tmp, blocksize = 1U;
	register uint32 regBackupLBP, regBackupINT0, regBackupDAT1;


#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    boolean retVal = FALSE;

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for privileged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_SPI, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }
#endif

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */

    if ((_SELFTEST_SPI_MIN > testType) || (_SELFTEST_SPI_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_SPI, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    if ((SPI_MIN > instance) || (SPI_MAX < instance)) {
           SL_Log_Error(FUNC_ID_ST_SPI, ERR_TYPE_PARAM, 1u);
           retVal =  FALSE;
           return(retVal);
       }
#endif

    switch(instance)
    {
        case SL_SPI1:
            sl_spiREG = sl_spiREG1;
            break;

        case SL_SPI2:
            sl_spiREG = sl_spiREG2;
            break;

        case SL_SPI3:
            sl_spiREG = sl_spiREG3;
            break;

        case SL_SPI4:
            sl_spiREG = sl_spiREG4;
            break;

        case SL_SPI5:
            sl_spiREG = sl_spiREG5;
            break;

        default:
            sl_spiREG = sl_spiREG1;
            break;
    }

#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif

        /* verify SPI is out of reset */
        /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
        if((boolean)sl_spiREG)
        {
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        	if (SPI_GCR0_RESET != (sl_spiREG->GCR0 & SPI_GCR0_RESET)) {
                SL_Log_Error(FUNC_ID_ST_SPI, ERR_TYPE_ENTRY_CON, 1u);
                retVal =  FALSE;
                return retVal;
            }

        }

        /* verify SPI is enabled for transfers (required for loopback test) */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (SPI_GCR1_SPIENA != (sl_spiREG->GCR1 & SPI_GCR1_SPIENA)) {
            SL_Log_Error(FUNC_ID_ST_SPI, ERR_TYPE_ENTRY_CON, 2u);
            retVal =  FALSE;
            return retVal;
        }

        /* If nERROR is active then do not proceed with tests that trigger nERROR */
        if(SL_ESM_nERROR_Active())
        {
            SL_Log_Error(FUNC_ID_ST_SPI, ERR_TYPE_ENTRY_CON, 3U);
            return(FALSE);
        }


    switch(testType)
    {


    case SPI_ANALOG_LOOPBACK_TEST:

    	(void)SL_FLAG_SET(testType);

    	/* Backup LOOPBACK, DAT1 and INT0 registers */
    	regBackupLBP = sl_spiREG->IOLPKTSTCR;
    	regBackupINT0 = sl_spiREG->INT0;
    	regBackupDAT1 = sl_spiREG->DAT1;

    	/* Disable interrupts and dma requests */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BF_SET(sl_spiREG->INT0, SPI_INTR_DIS, SPI_INTR_DIS_START, SPI_INTR_DIS_LENGTH);

        /* Configure message */
    	dataconfig_t.CSNR = SL_SPI_CS_NONE;
    	dataconfig_t.CS_HOLD = FALSE;
    	dataconfig_t.WDEL = FALSE; /* wont matter for a single word transmit */
    	dataconfig_t.DFSEL = SL_SPI_FMT_0;

        /* Enable External Loopback */
    	SL_spiEnableLoopback(sl_spiREG, SL_Analog_Lbk);

    	/* Transmit And Receive message - storing return value to avoid misra-c violation */
    	tmp = SL_spiTransmitAndReceiveData(sl_spiREG, &dataconfig_t, blocksize, &txdata, &rxdata);

    	tmp = tmp; /* to avoid compiler warning */

    	/* compare messages */
    	if(rxdata == txdata)
    	{
    		retVal = TRUE;
    	}
    	else
    	{
    		retVal = FALSE;
    	}

    	/* restore LOOPBACK, DAT1 and INT0 registers */
    	sl_spiREG->IOLPKTSTCR = regBackupLBP;
    	sl_spiREG->INT0 = regBackupINT0;
    	sl_spiREG->DAT1 = regBackupDAT1;

    	break;



    default:
        retVal = FALSE;
        break;
    }

    #if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_SPI, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif


	SL_FLAG_CLEAR(testType);
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define msgNo            2u
#define eccOffset        0x00001000u
#define dcan1RAMBase    (volatile uint32*)0xFF1E0000
#define dcan2RAMBase    (volatile uint32*)0xFF1C0000
#define dcan3RAMBase    (volatile uint32*)0xFF1A0000
#define dcan4RAMBase    (volatile uint32*)0xFF180000

/* ESM 2 bit registers  specific to each DCAN instance. */
#define dcan1uerrESMReg (volatile uint32*)&(esmREG->SR1[0])
#define dcan1iesrESMReg (volatile uint32*)&(esmREG->IESR1)
#define dcan1iecrESMReg (volatile uint32*)&(esmREG->IECR1)
#define dcan1eepaprESMReg (volatile uint32*)&(esmREG->EEPAPR1)
#define dcan1depaprESMReg (volatile uint32*)&(esmREG->DEPAPR1)

#define dcan2uerrESMReg dcan1uerrESMReg
#define dcan2iesrESMReg dcan1iesrESMReg
#define dcan2iecrESMReg dcan1iecrESMReg
#define dcan2eepaprESMReg dcan1eepaprESMReg
#define dcan2depaprESMReg dcan1depaprESMReg

#define dcan3uerrESMReg dcan1uerrESMReg
#define dcan3iesrESMReg dcan1iesrESMReg
#define dcan3iecrESMReg dcan1iecrESMReg
#define dcan3eepaprESMReg dcan1eepaprESMReg
#define dcan3depaprESMReg dcan1depaprESMReg

#define dcan4uerrESMReg (volatile uint32*)&(esmREG->SR4[0])
#define dcan4iesrESMReg (volatile uint32*)&(esmREG->IESR4)
#define dcan4iecrESMReg (volatile uint32*)&(esmREG->IECR4)
#define dcan4eepaprESMReg (volatile uint32*)&(esmREG->IEPSR4)
#define dcan4depaprESMReg (volatile uint32*)&(esmREG->IEPCR4)
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

#define dcan1RAMLoc ((volatile uint32 *)0xFF1E000CU)
#define dcan2RAMLoc ((volatile uint32 *)0xFF1C000CU)
#define dcan1RAMLocVar ((uint32)0xFF1E000CU)
#define dcan2RAMLocVar ((uint32)0xFF1C000CU)

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define dcan1RAMParLoc ((volatile uint8*)(0xFF1E0000U + 0x11U))
#define dcan2RAMParLoc ((volatile uint8 *)(0xFF1C0000U + 0x11U))
#else
#define dcan1RAMParLoc ((volatile uint8*)(0xFF1E0000U + 0x12U))
#define dcan2RAMParLoc ((volatile uint8 *)(0xFF1C0000U + 0x12U))
#endif



#if !(defined(_RM42x_) || defined(_TMS570LS04x_))

#define dcan3RAMLoc ((volatile uint32 *)0xFF1A000CU)
#define dcan3RAMLocVar ((uint32)0xFF1A000CU)

#if ((__little_endian__ == 1) || (__LITTLE_ENDIAN__ == 1))
#define dcan3RAMParLoc ((volatile uint8 *)(0xFF1A0000U + 0x11U))
#else
#define dcan3RAMParLoc ((volatile uint8 *)(0xFF1A0000U + 0x12U))
#endif

#endif

#endif

boolean SL_SelfTest_CAN(SL_SelfTestType testType, SL_DCAN_Instance instance)
{
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    volatile boolean _sl_fault_injection = FALSE;
    register uint32 ramread32, regBackupEccCS, regBackupCtl, regBckupErrInfulence, regBkupIntEnaSet;
    volatile uint32* data;
    boolean testPassed = FALSE;
    uint32           dataVal;

    volatile uint32 *dcanRAMBase;
    uint32 esm1bitecc, esm2bitecc;
    volatile uint32* esm2bitStatReg;
    volatile uint32* esm2bitEEPAPR;
    volatile uint32* esm2bitDEPAPR; 
    volatile uint32* esm2bitIESR;   
    volatile uint32* esm2bitIECR;   
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    register uint32 regBackupCTL, regBkupIntEnaSet, regBckupErrInfulence;
    volatile uint32* dcanRAMLoc = 0u;
    uint32 dcanRAMLocVar = 0u;
    volatile uint8* dcanRAMParLoc = 0u;
    uint32 esmErrorChannel;

#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    canBASE_t* sl_canREG;
    volatile uint32 ramRead;
    boolean retVal = FALSE;

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for privileged mode*/
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_CAN, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }
#endif

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in rage */
    if ((_SELFTEST_CAN_MIN > testType) || (_SELFTEST_CAN_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_CAN, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    if ((instance < DCAN_MIN) || (instance > DCAN_MAX)) {
        SL_Log_Error(FUNC_ID_ST_CAN, ERR_TYPE_PARAM, 1u);
        retVal =  FALSE;
        return(retVal);
    }

#endif

    switch(instance)
    {
        case SL_DCAN1:
            sl_canREG      = canREG1;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
            dcanRAMLoc    =    dcan1RAMLoc;
            dcanRAMLocVar =    dcan1RAMLocVar;
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            dcanRAMParLoc = dcan1RAMParLoc;
            esmErrorChannel = ESM_G1ERR_DCAN1_PARITY_CORRERR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            dcanRAMBase    = dcan1RAMBase;
            esm1bitecc     = ESM_G1ERR_CAN1_ECC_SBERR;
            esm2bitecc     = ESM_G1ERR_CAN1_ECC_UNCORR;
            esm2bitStatReg = dcan1uerrESMReg;
            esm2bitEEPAPR  = dcan1eepaprESMReg;
            esm2bitDEPAPR  = dcan1depaprESMReg;
            esm2bitIESR    = dcan1iesrESMReg;
            esm2bitIECR    = dcan1iecrESMReg;
#endif
            break;
        case SL_DCAN2:
            sl_canREG      = canREG2;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
            dcanRAMLoc    =    dcan2RAMLoc;
            dcanRAMLocVar =    dcan2RAMLocVar;
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            dcanRAMParLoc = dcan2RAMParLoc;
            esmErrorChannel = ESM_G1ERR_DCAN2_PARITY_CORRERR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            dcanRAMBase    = dcan2RAMBase;
            esm1bitecc     = ESM_G1ERR_CAN2_ECC_SBERR;
            esm2bitecc     = ESM_G1ERR_CAN2_ECC_UNCORR;
            esm2bitStatReg = dcan2uerrESMReg;
            esm2bitEEPAPR  = dcan2eepaprESMReg;
            esm2bitDEPAPR  = dcan2depaprESMReg;
            esm2bitIESR    = dcan2iesrESMReg;
            esm2bitIECR    = dcan2iecrESMReg;
#endif
            break;
        case SL_DCAN3:
            sl_canREG      = canREG3;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
            dcanRAMLoc    =    dcan3RAMLoc;
            dcanRAMLocVar =    dcan3RAMLocVar;
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            dcanRAMParLoc = dcan3RAMParLoc;
            esmErrorChannel = ESM_G1ERR_DCAN3_PARITY_CORRERR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            dcanRAMBase    = dcan3RAMBase;
            esm1bitecc     = ESM_G1ERR_CAN3_ECC_SBERR;
            esm2bitecc     = ESM_G1ERR_CAN3_ECC_UNCORR;
            esm2bitStatReg = dcan3uerrESMReg;
            esm2bitEEPAPR  = dcan3eepaprESMReg;
            esm2bitDEPAPR  = dcan3depaprESMReg;
            esm2bitIESR    = dcan3iesrESMReg;
            esm2bitIECR    = dcan3iecrESMReg;
#endif
            break;


#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        case SL_DCAN4:
            sl_canREG      = canREG4;
            dcanRAMBase    = dcan4RAMBase;
            esm1bitecc     = ESM_G1ERR_CAN4_ECC_SBERR;
            esm2bitecc     = ESM_G1ERR_CAN4_ECC_UNCORR;
            esm2bitStatReg = dcan4uerrESMReg;
            esm2bitEEPAPR  = dcan4eepaprESMReg;
            esm2bitDEPAPR  = dcan4depaprESMReg;
            esm2bitIESR    = dcan4iesrESMReg;
            esm2bitIECR    = dcan4iecrESMReg;
            break;
#endif

        default:
            sl_canREG      = canREG1;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
            dcanRAMLoc    =    dcan1RAMLoc;
            dcanRAMLocVar =    dcan1RAMLocVar;
            /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_18*/
            dcanRAMParLoc = dcan1RAMParLoc;
            esmErrorChannel = ESM_G1ERR_DCAN1_PARITY_CORRERR;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
            esm1bitecc     = ESM_G1ERR_CAN1_ECC_SBERR;
            esm2bitecc     = ESM_G1ERR_CAN1_ECC_UNCORR;
            esm2bitStatReg = dcan1uerrESMReg;
            esm2bitEEPAPR  = dcan1eepaprESMReg;
            esm2bitDEPAPR  = dcan1depaprESMReg;
            esm2bitIESR    = dcan1iesrESMReg;
            esm2bitIECR    = dcan1iecrESMReg;
#endif
            break;
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if(SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_CAN, ERR_TYPE_ENTRY_CON, 1U);
        return(FALSE);
    }

    /* verify DCAN is initialised */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if((boolean)sl_canREG)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if ((uint32)CAN_INIT_BIT == (sl_canREG->CTL & CAN_INIT_BIT)) {
            SL_Log_Error(FUNC_ID_ST_CAN, ERR_TYPE_ENTRY_CON, 3u);
            retVal =  FALSE;
            return retVal;
        }
    }


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

	 /* If DCAN parity check is not enabled, return error. */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(((CAN_CTL_PMD_DIS) == BF_GET(sl_canREG->CTL, CAN_CTL_PMD_START, CAN_CTL_PMD_LENGTH)) &&
		(testType == CAN_SRAM_PARITY_TEST))
	 {
		 SL_Log_Error(FUNC_ID_ST_CAN, ERR_TYPE_ENTRY_CON, 4u);
	     return(FALSE);
	 }
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    if((CAN_SRAM_ECC_DISABLED) ==
            BF_GET(sl_canREG->CTL, CAN_CTRL_SECDED_START, CAN_CTRL_SECDED_LENGTH))
    {
        SL_Log_Error(FUNC_ID_ST_CAN, ERR_TYPE_ENTRY_CON, 2U);
        return(FALSE);
    }

    /* If fault inject set global variable to flag to the ESM handler that it is a fault injection */
    if((CAN_ECC_TEST_MODE_1BIT_FAULT_INJECT == testType)||
        (CAN_ECC_TEST_MODE_2BIT_FAULT_INJECT == testType)){
        _sl_fault_injection = TRUE;
    }

    /* DCAN1/2/3 uncorrectable ECC is in SR1  */


    regBackupEccCS = sl_canREG->ECC_CS;
    regBackupCtl = sl_canREG->CTL;

    /* initialize data */
    data = (volatile uint32 *)((((uint32)dcanRAMBase) + (msgNo*0x20u)));
    /* Offset the pointer by 4 bytes as the memory map for the MESSAGE RAM
     * is different based on RDA or debug, and offsetting this by 4 points
     * to an unreserved location regardless of RDA or debug mode access of
     * message RAM */
    data++;

    /* disable SECDED - write to PMD in CANCTL */
    BF_SET(sl_canREG->CTL, CAN_CTRL_SECDED_DIS, CAN_CTRL_SECDED_START, CAN_CTRL_SECDED_LENGTH);

    /* set Test bit to enable Test Mode (required for selecting test mode - RDA) */
    BIT_SET(sl_canREG->CTL, BIT(CAN_CTRL_TEST_EN));

    /* set Init bit (enter software initialization mode) and avoid conflicts with Message Handler
     * This step is required before entering RDA mode */
    BIT_SET(sl_canREG->CTL, BIT(CAN_CTRL_INIT));

    /* enable Ram Direct Access (RDA) */
    BIT_SET(sl_canREG->TEST, BIT(CAN_TEST_RDA_EN));

    /* backup DATA stored at this location */
    dataVal = *data;

    /* enable SECDED diagnostic mode */
    BF_SET(sl_canREG->ECCDIAG, CAN_ECCDIAG_SECDED_EN, 0, 4);
#endif

#if FUNCTION_PROFILING_ENABLED
    SL_Record_Errorcreationtick(testType);
#endif

    switch(testType)
    {
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    case CAN_ECC_TEST_MODE_1BIT:
    case CAN_ECC_TEST_MODE_1BIT_FAULT_INJECT:

        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        regBkupIntEnaSet = esmREG->IESR7;

        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBckupErrInfulence = esmREG->IEPSR7;

        if((FALSE) == _sl_fault_injection) {
            esmREG->IECR7 = GET_ESM_BIT_NUM(esm1bitecc);
            esmREG->IEPCR7 = GET_ESM_BIT_NUM(esm1bitecc);
        } 

        /* enable ECC single bit error event */
        BF_SET(sl_canREG->ECC_CS , CAN_ECC_CS_SBE_EVT_EN, CAN_ECC_CS_SBE_EVT_START, CAN_ECC_CS_SBE_EVT_LENGTH);
        /* corrupt the data */
        *data ^= CAN_FLIP_DATA_1BIT;

        /* enable SECDED */
        BF_SET(sl_canREG->CTL, CAN_CTRL_SECDED_EN, CAN_CTRL_SECDED_START, CAN_CTRL_SECDED_LENGTH);

        /* set the SL Test flag as close as possible to the creation of error.
         * This minimizes the chance of a race condition in the reading of the
         * test flag incorrectly. */
        (void)SL_FLAG_SET(testType);
        
        /* create fault */
        ramread32 = *data;

        if(CAN_ECC_TEST_MODE_1BIT == testType) {
            /* Correctable ESM event for all DCAN instances is registered in ESM
             * status SR7[0]  */
            if(GET_ESM_BIT_NUM(esm1bitecc) == 
                    (esmREG->SR7[0] & GET_ESM_BIT_NUM(esm1bitecc))) {
                testPassed = TRUE;
                /* clear ESM error status */
                esmREG->SR7[0] = GET_ESM_BIT_NUM(esm1bitecc);
            }

            if(testPassed && 
                ((sl_canREG->ECC_CS & BIT(CAN_ECC_SBERR)) == BIT(CAN_ECC_SBERR)) && 
                ((sl_canREG->ECCDIAG_STAT & BIT(CAN_ECC_SBERR)) == BIT(CAN_ECC_SBERR)) &&
                (msgNo == (sl_canREG->ECC_SERR & CAN_ECC_SERR_MSG_NO))) {
                if(CAN_ECC_CS_ECC_MODE_DIS == 
                        BF_GET(sl_canREG->ECC_CS, CAN_ECC_CS_ECC_MODE_START, CAN_ECC_CS_ECC_MODE_LENGTH)) {
                    if((sl_canREG->ES & BIT(CAN_ECC_ES_PER)) == BIT(CAN_ECC_ES_PER)) {
                        ramRead = sl_canREG->ES;
                        retVal = TRUE;
                    } else {
                        retVal = FALSE;
                    }
                } else {
                    retVal = TRUE;
                }


            } else {
                retVal = FALSE;
            }
        }
        else {
            /* fault injected */
            retVal = TRUE;
        }


        /* clear the single bit error status */
        /*BIT_SET(sl_canREG->ECC_CS, BIT(CAN_ECC_SBERR));*/
        /*BIT_SET(sl_canREG->ECCDIAG_STAT, BIT(CAN_ECC_SBERR));*/
        sl_canREG->ECC_CS = BIT(CAN_ECC_SBERR);
        sl_canREG->ECCDIAG_STAT = BIT(CAN_ECC_SBERR);

        /* disable SECDED - write to PMD in CANCTL */
        BF_SET(sl_canREG->CTL, CAN_CTRL_SECDED_DIS, CAN_CTRL_SECDED_START, CAN_CTRL_SECDED_LENGTH);
        /* Restore data; in other cases data should be auto corrected. */
        *data = dataVal;
        /* enable SECDED */
        BF_SET(sl_canREG->CTL, CAN_CTRL_SECDED_EN, CAN_CTRL_SECDED_START, CAN_CTRL_SECDED_LENGTH);


        /* disable diagnostic mode */
        BF_SET(sl_canREG->ECCDIAG, CAN_ECCDIAG_SECDED_DIS, 0, 4);
        /* Disable Ram Direct Access (RDA) */
        BIT_CLEAR(sl_canREG->TEST, BIT(CAN_TEST_RDA_EN));

        /* Restore CAN registers states */
        sl_canREG->CTL = regBackupCtl;
        sl_canREG->ECC_CS = regBackupEccCS;

        /* Restore grp1 esm interrupt enable */
        esmREG->IESR7 = regBkupIntEnaSet;
        esmREG->IEPSR7 = regBckupErrInfulence;

        break;

    case CAN_ECC_TEST_MODE_2BIT:
    case CAN_ECC_TEST_MODE_2BIT_FAULT_INJECT:

        /*Backup grp1 esm interrupt enable register and clear the interrupt enable */
        regBkupIntEnaSet = *esm2bitIESR;

        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        regBckupErrInfulence = *esm2bitEEPAPR;

        if((FALSE) == _sl_fault_injection) {
            *esm2bitIECR    = GET_ESM_BIT_NUM(esm2bitecc);
            *esm2bitDEPAPR = GET_ESM_BIT_NUM(esm2bitecc);
        }

        /* doubt - corrupt the data/ecc */
        *data ^= CAN_FLIP_DATA_2BIT;

        /* enable SECDED */
        BF_SET(sl_canREG->CTL, CAN_CTRL_SECDED_EN, CAN_CTRL_SECDED_START, CAN_CTRL_SECDED_LENGTH);

        /* set the SL Test flag as close as possible to the creation of error.
         * This minimizes the chance of a race condition in the reading of the
         * test flag incorrectly. */
        SL_FLAG_SET(testType);
        ramread32 = *data;

        if(CAN_ECC_TEST_MODE_2BIT == testType) {
            if(GET_ESM_BIT_NUM(esm2bitecc) == 
                    ((*esm2bitStatReg) & GET_ESM_BIT_NUM(esm2bitecc))) {
                testPassed = TRUE;
                /* clear ESM error status */
                *esm2bitStatReg = GET_ESM_BIT_NUM(esm2bitecc);
            }

            if(testPassed  && 
                    ((sl_canREG->ECC_CS & BIT(CAN_ECC_UNCORR_ERR)) == BIT(CAN_ECC_UNCORR_ERR)) &&
                    ((sl_canREG->ECCDIAG_STAT & BIT(CAN_ECC_UNCORR_ERR)) == BIT(CAN_ECC_UNCORR_ERR)) &&
                    ((sl_canREG->ES & BIT(CAN_ECC_UNCORR_ERR)) == BIT(CAN_ECC_UNCORR_ERR))) {
                retVal = TRUE;
            } else {
                retVal = FALSE;
            }
        }
        else {
            /* fault injected */
            retVal = TRUE;
        }
        /* disable SECDED - write to PMD in CANCTL */
        BF_SET(sl_canREG->CTL, CAN_CTRL_SECDED_DIS, CAN_CTRL_SECDED_START, CAN_CTRL_SECDED_LENGTH);
        /* Restore data */
        *data = dataVal;
        /* enable SECDED */
        BF_SET(sl_canREG->CTL, CAN_CTRL_SECDED_EN, CAN_CTRL_SECDED_START, CAN_CTRL_SECDED_LENGTH);

        /* clear the double bit error status */
        /*BIT_SET(canREG1->ECC_CS, BIT(CAN_ECC_UNCORR_ERR));*/
        /*BIT_SET(canREG1->ECCDIAG_STAT, BIT(CAN_ECC_UNCORR_ERR));*/
        sl_canREG->ECC_CS = BIT(CAN_ECC_UNCORR_ERR);
        sl_canREG->ECCDIAG_STAT = BIT(CAN_ECC_UNCORR_ERR);

        /* disable diagnostic mode */
        BF_SET(sl_canREG->ECCDIAG, CAN_ECCDIAG_SECDED_DIS, 0, 4);
        /* Disable Ram Direct Access (RDA) */
        BIT_CLEAR(sl_canREG->TEST, BIT(CAN_TEST_RDA_EN));

        /* Restore CAN registers states */
        sl_canREG->ECC_CS = regBackupEccCS;
        sl_canREG->CTL = regBackupCtl;

        /* Restore grp1 esm interrupt enable */
        *esm2bitIESR = regBkupIntEnaSet;
        *esm2bitEEPAPR = regBckupErrInfulence;
        break;
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    case CAN_SRAM_PARITY_TEST:

    	(void)SL_FLAG_SET(testType);

    	/* Backup CTL register */
    	regBackupCTL = sl_canREG->CTL;

        /*Backup grp1 esm interrupt enable register */
        regBkupIntEnaSet = esmREG->IESR1;
        regBckupErrInfulence = esmREG->EEPAPR1;

    	/* Disable esm interrupt generation */
        esmREG->IECR1 = GET_ESM_BIT_NUM(esmErrorChannel);

    	/* Disable esm error influence */
        esmREG->DEPAPR1 = GET_ESM_BIT_NUM(esmErrorChannel);

    	/* enable TEST mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_canREG->CTL, CAN_TEST_BIT);

        /* Disable parity checking */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BF_SET(sl_canREG->CTL, CAN_CTL_PMD_DIS, CAN_CTL_PMD_START, CAN_CTL_PMD_LENGTH);

    	/* enable init mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_canREG->CTL, CAN_INIT_BIT);

        /* Enable RAM Direct Access mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_SET(sl_canREG->TEST, CAN_RDA_BIT);

        /* flip the parity bit */
        /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
        if((boolean)dcanRAMParLoc)
        {
            /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        	BIT_FLIP((*dcanRAMParLoc), 0x80U);
    	}

        /* disable init mode (but not TEST mode) */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_CLEAR(sl_canREG->CTL, CAN_INIT_BIT);

        /* Enable parity checking */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BF_SET(sl_canREG->CTL, CAN_CTL_PMD_EN, CAN_CTL_PMD_START, CAN_CTL_PMD_LENGTH);

        /* Read location with parity error */
        ramRead = (*dcanRAMLoc);

    	/* Check if ESM group1 channel 18 is not flagged */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if (((sl_canREG->ES & CAN_PAR_ERR_FLG) == 0U) ||
    		((esmREG->SR1[0U] & GET_ESM_BIT_NUM(esmErrorChannel)) == 0U))
    	{
    		/* DCAN RAM parity error was not flagged to ESM. */
    		retVal = FALSE;
    	}
    	else
    	{
    		/* verify erronous address */
    	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    		if( (uint32)(((uint32)((uint32)(sl_canREG->PERR) >> 8U) & 0xFU) * 3U) ==
    			(uint32)(dcanRAMLocVar & 0xFFFU) )
    		{
    			retVal = TRUE;
    		}
    		else
    		{
    			retVal = FALSE;
    	}
    	}

		/* clear ESM group1 channel flag */
    	esmREG->SR1[0U] = GET_ESM_BIT_NUM(esmErrorChannel);

        /* Disable parity checking */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BF_SET(sl_canREG->CTL, CAN_CTL_PMD_DIS, CAN_CTL_PMD_START, CAN_CTL_PMD_LENGTH);

    	/* enable init mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BIT_SET(sl_canREG->CTL, CAN_INIT_BIT);

		/* Revert back to correct parity */
        /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
        if((boolean)dcanRAMParLoc)
        {
        	/*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        	BIT_FLIP((*dcanRAMParLoc), 0x80U);
    	}

        /* Disable RAM Direct Access mode */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        BIT_CLEAR(sl_canREG->TEST, CAN_RDA_BIT);

        /* Restore CTL register */
    	sl_canREG->CTL = regBackupCTL;

        /* Restore ESM registers states */
        esmREG->IESR1 = regBkupIntEnaSet;
        esmREG->EEPAPR1 = regBckupErrInfulence;

    	break;
#endif

    default:
        retVal = FALSE;
        break;
    }
    SL_FLAG_CLEAR(testType);

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    /* If the test was fault injection and not self test, then do not mask the call back & clear the *
    *  FAULT_INJECTION flag for subsequent runs*/
    if((TRUE) == _sl_fault_injection){
        _sl_fault_injection = FALSE;
    }
    ramread32 = ramread32; /* Avoid compiler warning. */
#endif

#if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_CAN, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_gioSetDirection(sl_gioPORT_t* port, uint32 dir)
{
    port->DIR = dir;
}


void SL_gioSetPort(sl_gioPORT_t* port, uint32 value)
{
    port->DOUT = value;
}


uint32 SL_gioGetPort(sl_gioPORT_t* port)
{
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	return ((uint32)(port->DIN));
}


void SL_gioSetBit(sl_gioPORT_t* port, uint32 bit, uint32 value)
{
    if (value != 0U)
    {
        port->DSET = (uint32)1U << bit;
    }
    else
    {
        port->DCLR = (uint32)1U << bit;
    }
}


uint32 SL_gioGetBit(sl_gioPORT_t* port, uint32 bit)
{
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	return ((uint32)(port->DIN) >> bit) & 1U;
}

#endif

/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
boolean SL_SelfTest_GIO(SL_SelfTestType testType, SL_gioPORT_Instance instance, uint32 pin)
{

	register uint32 regBackupENASET;
	sl_gioPORT_t* sl_gioPORT = 0u;
	uint32 rxdata=0U, txdata=0U;
	uint32 gio_ENACLR_start;


#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    boolean retVal = FALSE;

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for privileged mode */
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_GIO, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    /*verify gio is not in reset state */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if (GIO_RESET_BIT != (sl_gioREG->GCR0 & GIO_RESET_BIT)) {
        SL_Log_Error(FUNC_ID_ST_GIO, ERR_TYPE_ENTRY_CON, 1u);
        retVal =  FALSE;
        return retVal;
    }
#endif

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */

    if ((_SELFTEST_GIO_MIN > testType) || (_SELFTEST_GIO_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_GIO, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    if ((instance < GIO_PORT_MIN) || (instance > GIO_PORT_MAX)) {
        SL_Log_Error(FUNC_ID_ST_GIO, ERR_TYPE_PARAM, 1u);
        retVal =  FALSE;
        return(retVal);
    }
#endif

    switch(instance)
    {

    case SL_GIOPORTA:
    	sl_gioPORT = sl_gioPORTA;
    	gio_ENACLR_start = GIO_ENACLR_PORTA_START;
    	break;

    case SL_GIOPORTB:
#if defined(_TMS570LC43x_) || defined(_RM57Lx_) || defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
    	sl_gioPORT = sl_gioPORTB;
    	gio_ENACLR_start = GIO_ENACLR_PORTB_START;
#endif
    	break;

    default:
    	sl_gioPORT = sl_gioPORTA;
    	gio_ENACLR_start = GIO_ENACLR_PORTA_START;
    	break;

    }


	/* If loopback test, ensure the selected pin is an output pin */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if((boolean)sl_gioPORT)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if(((uint32)(1U << pin) != (sl_gioPORT->DIR & ((uint32)(1U << pin)))) &&
   		 (testType == GIO_ANALOG_LOOPBACK_TEST))
    	{
    		SL_Log_Error(FUNC_ID_ST_GIO, ERR_TYPE_ENTRY_CON, 2u);
    		return(FALSE);
    	}

    }

	    /* If nERROR is active then do not proceed with tests that trigger nERROR */
	    if(SL_ESM_nERROR_Active())
	    {
	        SL_Log_Error(FUNC_ID_ST_GIO, ERR_TYPE_ENTRY_CON, 3U);
	        return(FALSE);
	    }

#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif

    switch(testType)
    {

    case GIO_ANALOG_LOOPBACK_TEST:

    	/* set test flag */
    	(void)SL_FLAG_SET(testType);

    	/* Backup Interrupt Set register */
    	regBackupENASET = sl_gioREG->ENASET;

    	/* Disable interrupts */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	BF_SET(sl_gioREG->ENACLR, GIO_INTR_SET, gio_ENACLR_start, GIO_ENACLR_LENGTH);

    	/* Get current state of pin */
    	txdata = SL_gioGetBit(sl_gioPORT, pin);

    	/* Copy to rxdata and flip txdata */
    	rxdata = txdata;
    	txdata ^= (uint32)0x1U;

    	/* set txdata on the pin */
    	SL_gioSetBit(sl_gioPORT, pin, txdata);

    	/* read back the value */
    	rxdata = SL_gioGetBit(sl_gioPORT, pin);

    	/* compare values */
    	if(rxdata == txdata)
    	{
    		retVal = TRUE;
    	}
    	else
    	{
    		retVal = FALSE;
    	}

    	/* Restore Port Direction and Interrupt Set registers */
    	sl_gioREG->ENASET = regBackupENASET;

    	break;


    default:
        retVal = FALSE;
        break;
    }

    #if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_GIO, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif

	SL_FLAG_CLEAR(testType);
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_sciSendByte(sl_sciBASE_t* sci, uint8 byte)
{
	sci->TD = byte;
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
uint32 SL_sciReceiveByte(sl_sciBASE_t* sci)
{
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	return (sci->RD & (uint32)0x000000FFU);
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_sciEnableLoopback(sl_sciBASE_t* sci, loopBackType_t Loopbacktype)
{

    /* Clear Loopback incase enabled already */
    sci->IODFTCTRL = 0U;

    /* Enable Loopback either in Analog or Digital Mode */
    sci->IODFTCTRL = (uint32)0x00000A00U
                   | (uint32)((uint32)Loopbacktype << 1U);

    /* Enable loopback in GCR1 */
    sci->GCR1 |= (uint32)0x10000u;

}
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
boolean SL_SelfTest_SCI(SL_SelfTestType testType, SL_SCI_Instance instance)
{

	uint8 rxdata=0U, txdata=0xAAU;
	register uint32 regBackupLPB, regBackupENASET, regBackupFORMAT;
	sl_sciBASE_t* sl_sciREG;

#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    boolean retVal = FALSE;

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for privileged mode */
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_SCI, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }

#endif

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */
    if ((_SELFTEST_SCI_MIN > testType) || (_SELFTEST_SCI_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_SCI, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }

    if ((SCI_MIN > instance) || (SCI_MAX < instance)) {
           SL_Log_Error(FUNC_ID_ST_SCI, ERR_TYPE_PARAM, 1u);
           retVal =  FALSE;
           return(retVal);
       }
#endif

    switch(instance)
    {
        case SL_SCI1:
            sl_sciREG = sl_sciREG1;
            break;
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        case SL_SCI2:
            sl_sciREG = sl_sciREG2;
            break;

        case SL_SCI3:
            sl_sciREG = sl_sciREG3;
            break;

        case SL_SCI4:
            sl_sciREG = sl_sciREG4;
            break;
#endif

        default:
            sl_sciREG = sl_sciREG1;
            break;
    }

    /*verify sci is not in reset state */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if((boolean)sl_sciREG)
    {    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
        if((SCI_GCR1_SWRST_BIT|SCI_GCR0_RESET_BIT) != ((sl_sciREG->GCR1 & SCI_GCR1_SWRST_BIT)|(sl_sciREG->GCR0 & SCI_GCR0_RESET_BIT))) {
            SL_Log_Error(FUNC_ID_ST_SCI, ERR_TYPE_ENTRY_CON, 1u);
            retVal =  FALSE;
            return retVal;
        }

    }


    /* In case of loopback test ensure TXENA and RXENA bits are set */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if((SCI_GCR1_TXRXENA_BIT != (sl_sciREG->GCR1 & SCI_GCR1_TXRXENA_BIT)) &&
    	(SCI_ANALOG_LOOPBACK_TEST == testType)) {

        SL_Log_Error(FUNC_ID_ST_SCI, ERR_TYPE_ENTRY_CON, 2u);
        retVal =  FALSE;
        return retVal;
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if(SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_SCI, ERR_TYPE_ENTRY_CON, 3U);
        return(FALSE);
    }


#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif

    switch(testType)
    {

    case SCI_ANALOG_LOOPBACK_TEST:

    	/* set test flag */
    	(void)SL_FLAG_SET(testType);

	    /*SAFETYMCUSW 28 D MR: 16.7 <APPROVED> Comment_24*/
    	/* Backup LOOPBACK, SETINT and FORMAT registers */
    	regBackupLPB = sl_sciREG->IODFTCTRL;
    	regBackupENASET = sl_sciREG->SETINT;
    	regBackupFORMAT = sl_sciREG->FORMAT;

    	/* wait until idle */
	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	while ((sl_sciREG->FLR & SCI_FLR_IDLE_BIT) == SCI_FLR_IDLE_BIT)
	    {
	    } /* Wait */

		/* wait until TX ready */
	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	/*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    	while ((sl_sciREG->FLR & (uint32)SL_SCI_TX_INT) != (uint32)SL_SCI_TX_INT)
	    {
	    } /* Wait */

	    /* Disable interrupts and dma requests */
	    sl_sciREG->CLEARINT = (uint32)0xFFFFFFFFu;

	    /* Enable Loopback in Analog Mode */
	    SL_sciEnableLoopback(sl_sciREG, SL_Analog_Lbk);

	    /* set data format length to 8 bits */
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_10*/
	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	    BF_SET(sl_sciREG->FORMAT, SCI_FORMAT_CHAR8, SCI_FORMAT_CHAR_START, SCI_FORMAT_CHAR_LENGTH);

    	/* send byte (polling method) */
		SL_sciSendByte(sl_sciREG, txdata);

		/* wait until RX ready */
	    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
		/*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
		while ((sl_sciREG->FLR & (uint32)SL_SCI_RX_INT) != (uint32)SL_SCI_RX_INT)
		{
		} /* Wait */

		/* get byte (polling method) */
		rxdata = (uint8)SL_sciReceiveByte(sl_sciREG);

		/* compare values */
		if(rxdata == txdata)
		{
			retVal = TRUE;
		}
		else
		{
			retVal = FALSE;
		}

		/* Restore LOOPBACK, SETINT and FORMAT registers */
    	sl_sciREG->IODFTCTRL = regBackupLPB;
    	sl_sciREG->SETINT = regBackupENASET;
    	sl_sciREG->FORMAT = regBackupFORMAT;

	break;



    default:
        retVal = FALSE;
        break;
    }

    #if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_GIO, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif

	SL_FLAG_CLEAR(testType);
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_linEnableLoopback(sl_linBASE_t* lin, loopBackType_t Loopbacktype)
{

	/* Clear Loopback incase enabled already */
	lin->IODFTCTRL = 0U;

	/* Enable Loopback either in Analog or Digital Mode */
    lin->IODFTCTRL = ((uint32)(0x00000A00U)
                   | (uint32)((uint32)Loopbacktype << 1U));

    /* Enable loopback in GCR1 */
    lin->GCR1 |= (uint32)0x10000u;

}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_linSetLength(sl_linBASE_t* lin, uint32 length)
{
	length = length - 1U;
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	lin->FORMAT = ((lin->FORMAT & 0xFFF8FFFFU) | (length << 16U));
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_linSendHeader(sl_linBASE_t* lin, uint8 identifier)
{
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
	lin->ID = ((lin->ID & 0xFFFFFF00U) | (uint32)identifier);
}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_linSend(sl_linBASE_t* lin, uint8* data)
{
    uint32           tmp_count;
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    uint32           length = (uint32)((uint32)(lin->FORMAT & 0x00070000U) >> 16U);
	/*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
    /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
	uint8* pData = data + length;

    for (tmp_count=0U; tmp_count<=length; tmp_count++)
    {
	    /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        lin->TDx[length-tmp_count] = *pData;
		/*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
		/*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
		pData--;
    }

}
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void SL_linGetData(sl_linBASE_t* lin, uint8* const data)
{
    uint32      tmp_count;
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    uint32      length = (uint32)((uint32)(lin->FORMAT & 0x00070000U) >> 16U);
    uint8* pData = data;

    for (tmp_count = 0U; tmp_count <= length; tmp_count++)
    {
	    /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        *pData = lin->RDx[tmp_count];
		/*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
		/*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
		pData++;
    }

}
#endif

/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#define  TSIZE 8U
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
boolean SL_SelfTest_LIN(SL_SelfTestType testType, SL_LIN_Instance instance)
{
	uint8  TEXT1[TSIZE]= {'H','E','R','C','U','L','E','S'};
	uint8 TEXT2[TSIZE]= {0};
	uint32 i = 0u;
	register uint32 regBackupLPB, regBackupENASET;
	sl_linBASE_t* sl_linREG;


#if FUNCTION_PROFILING_ENABLED
    SL_Start_Profiling(testType);
#endif

    boolean retVal = FALSE;

#ifdef FUNC_ENTRY_COND_CHECK_ENABLED
    /*check for privileged mode */
    if (ARM_MODE_USR == _SL_Get_ARM_Mode()) {
        SL_Log_Error(FUNC_ID_ST_LIN, ERR_TYPE_ENTRY_CON, 0u);
        retVal =  FALSE;
        return(retVal);
    }

#endif

#ifdef FUNCTION_PARAM_CHECK_ENABLED
    /* Verify that the parameters are in range */
    if ((_SELFTEST_LIN_MIN > testType) || (_SELFTEST_LIN_MAX < testType)) {
        SL_Log_Error(FUNC_ID_ST_LIN, ERR_TYPE_PARAM, 0u);
        retVal =  FALSE;
        return(retVal);
    }
#endif

    switch(instance)
    {
        case SL_LIN1:
            sl_linREG = sl_linREG1;
            break;

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        case SL_LIN2:
            sl_linREG = sl_linREG2;
            break;
#endif

        default:
            sl_linREG = sl_linREG1;
            break;

    }

    /*verify LIN is not in reset state */
    /*SAFETYMCUSW 439 S MR:11.3 <APPROVED> Comment_4*/
    if((boolean)sl_linREG)
    {
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	if((LIN_GCR0_RESET_BIT|LIN_GCR1_SWRST_BIT) != (sl_linREG->GCR0 & (LIN_GCR0_RESET_BIT|LIN_GCR1_SWRST_BIT)))
        {
            SL_Log_Error(FUNC_ID_ST_LIN, ERR_TYPE_ENTRY_CON, 1u);
            retVal =  FALSE;
            return retVal;
        }

    }


    /* In case of loopback test ensure TXENA and RXENA bits are set */
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    if(((LIN_GCR1_TXENA_BIT|LIN_GCR1_RXENA_BIT) != (sl_linREG->GCR1 & (LIN_GCR1_TXENA_BIT|LIN_GCR1_RXENA_BIT))) &&
    	(LIN_ANALOG_LOOPBACK_TEST == testType)) {

        SL_Log_Error(FUNC_ID_ST_LIN, ERR_TYPE_ENTRY_CON, 2u);
        retVal =  FALSE;
        return retVal;
    }

    /* If nERROR is active then do not proceed with tests that trigger nERROR */
    if(SL_ESM_nERROR_Active())
    {
        SL_Log_Error(FUNC_ID_ST_LIN, ERR_TYPE_ENTRY_CON, 3U);
        return(FALSE);
    }

#if FUNCTION_PROFILING_ENABLED
        SL_Record_Errorcreationtick(testType);
#endif

    switch(testType)
    {


    case LIN_ANALOG_LOOPBACK_TEST:

    	/* set test flag */
    	(void)SL_FLAG_SET(testType);

    	/* Backup LOOPBACK and SETINT registers */
    	regBackupLPB = sl_linREG->IODFTCTRL;
    	regBackupENASET = sl_linREG->SETINT;

    	/* Checking that Tx is ready */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	/*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    	while((sl_linREG->FLR & LIN_FLR_TX_READY) != LIN_FLR_TX_READY)
    	{

    	} /* wait */

    	/* Checking if the LIN bus is free (Checking BUSY flag) */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	/*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    	while((sl_linREG->FLR & LIN_FLR_BUSY_BIT) == LIN_FLR_BUSY_BIT)
    	{

    	} /* wait */

	    /* Disable interrupts and dma requests */
	    sl_linREG->CLEARINT = (uint32)0xFFFFFFFFu;

    	/* Enable Loopback in Analog Mode */
    	SL_linEnableLoopback(sl_linREG, SL_Analog_Lbk);

    	/* Set length of data response in bytes (1 byte) */
    	SL_linSetLength(sl_linREG, TSIZE);

    	/*Send lin header including sync break field, sync field and identifier. */
    	SL_linSendHeader(sl_linREG, (uint8)0x28U); /* id value doesnt matter since mask should be set to always match */

    	/*Send data TEXT1 */
    	SL_linSend(sl_linREG, TEXT1);

		/* wait until RX ready */
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_5*/
    	/*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    	while((sl_linREG->FLR & (uint32)SL_LIN_RX_INT) != (uint32)SL_LIN_RX_INT)
		{
		} /* Wait */


    	/* Data is read from RD register and stored in TEXT2. */
    	SL_linGetData(sl_linREG, TEXT2);

		/* compare values */
    	for(i = 0u; i < TSIZE; i++)
    	{
    		if(TEXT1[i] != TEXT2[i])
    		{
    			break;
    		}
    	}
		if(TSIZE == i)
		{
			retVal = TRUE;
		}
		else
		{
			retVal = FALSE;
		}

		/* Restore LOOPBACK and SETINT registers */
    	sl_linREG->IODFTCTRL = regBackupLPB;
    	sl_linREG->SETINT = regBackupENASET;

	break;



    default:
        retVal = FALSE;
        break;
    }

    #if(FUNC_RESULT_LOG_ENABLED == 1)
    SL_Log_Result(FUNC_ID_ST_GIO, testType, (SL_SelfTest_Result)retVal , 0u);
#endif
#if FUNCTION_PROFILING_ENABLED
    SL_Stop_Profiling(testType);
#endif

    SL_FLAG_CLEAR(testType);
    return(retVal);
}
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_3*/
#endif



/*Comment_1:
 *  "Reason - used for creation of 16 byte aligned sram data array"*/

/*Comment_2:
 *  "Reason -  used for creation of aligned data section for sramEccTestBuff"*/

/*Comment_3:
 *  "Reviewed - The entry condition and parameter checks shall exit immediately"*/

/*Comment_4:
 *  "Reason -  This is an advisory by MISRA.Verified validity of operation by review"*/

/*Comment_5:
 * "Reason -  FALSE_POSITIVE The rule requires that the expression should be
 * guaranteed to have the same value regardless of the order in which the operands within the expression are evaluated.
 * This usually means no more than one volatile access occuring in the expression.The recommendation that simple
 * assignments should be used is intended to be restricted to simple assignments of the form x = v and not
 * "simple assignments" according to the ISO definition. However, the recommendation does not have to be followed
 * in order to be compliant with the rule. You should implement the rule (the value should be independent of the
 * evaluation order) for compliance purposes and you might choose to issue a warning for non-simple assignments
 * or you might choose to ignore them."*/

/*Comment_6:
 *  "Reason -  This can never be NULL as addresss points to an array"*/

/*Comment_7:
 *  "Reason -  This can never be NULL"*/

/*Comment_8:
 * "Address of volatile variable used, not the variable itself"*/

/*Comment_9:
 * "Address of volatile variable used, not the variable itself"*/

/*Comment_10:
 * "Assignment in an expression is valid here"*/

/*Comment_11:
 * "Needed to generate error"*/

/*Comment_12:
 * "Address of volatile variable accessed, not the variable itself"*/

/*Comment_13:
 * "Reason - Design change necessary to remove the infinite while loop"*/

/*Comment_14:
 * "Reason -  CHECK_RANGE_RAM_PTR is used to check the valid range"*/

/*Comment_15:
 * "This function will be called by application so not static" */

/*Comment_16:
 * "Reason -  Needed for the test"*/

/*Comment_17:
 * "changes may be made in the usage of the testType" */

/*Comment_18:
 * "The base address of the peripheral registers are actually macros" */

/*Comment_19:
 * "Reason - FALSE POSITIVE _SELFTEST_PBIST_MIN and testType are object pointers of same type*/

/*Comment_20:
 * "Reason -  This is an advisory by MISRA.Verified validity of operation by review"*/

/*Comment_21:
 * "The base address of the peripheral registers are actually macros"*/

/*Comment_22:
 * "Name declared in another C name space is deviated"*/

/*Comment_23:
 * "changes may be made in the usage of the config"*/

/*Comment_24:
 * "Overall design change needed" */

/*Comment_25:
 * "False positive" */
