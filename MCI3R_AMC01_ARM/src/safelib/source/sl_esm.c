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
 * @file        sl_esm.c
 * @version     2.3.1
 * @brief       File contains esm handling and interrupt handlers for esm errors
 *
 * @details This file implements interrupt handlers for the esmHigh and esmLow interrupts\n
 * the esm group 2 and group 1 handlers have been implemented which can be further extended\n
 * depending on the end application.It provides the Init function which updates the application\n
 * callback function and the interrupt handlers for esmHigh and esmLow interrupt.It also provides\n
 * API's which provide the information on the nerror pin status
 */

/* Comments regarding various justificatios for deviation from MISRA-C coding guidelines
 * are provided at the bottom of the file
 */

#include <sl_api.h>
#include <sl_priv.h>
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#include <sl_reg_flash.h>
#endif
#if FUNCTION_PROFILING_ENABLED
#include <sl_sys_pmu.h>
#endif

#ifdef __IAR_SYSTEMS_ICC__
__fiq
#endif
static
void sl_esm_high_intr_handler(void);
#ifdef __IAR_SYSTEMS_ICC__
__irq
#endif
static
void sl_esm_low_intr_handler(void);
static
void esmGroup1Handler(uint8 esmChannel);
static
void esmGroup2Handler(uint8 esmChannel);

/*SAFETYMCUSW 298 S MR:- <APPROVED> Comment_1*/
static ESM_Application_Callback ESM_AppCallback;

static uint32 callbackCancelCount = 0u;

/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_2*/
void SL_ESM_Init(ESM_Application_Callback appCallback)
{
	/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_2*/
	sl_vimRAM->ISR[1]  = &sl_esm_high_intr_handler;
	sl_vimRAM->ISR[21] = &sl_esm_low_intr_handler;

	ESM_AppCallback = appCallback;
}


boolean SL_ESM_nERROR_Active(void)
{
    boolean retval = FALSE;
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
    if(ESM_ERROR_PIN_OFF == BF_GET(esmREG->EPSR, BF_EPSF_START, BF_EPSF_LENGTH)) {
        retval = FALSE;
    }
    else {
        retval = TRUE;
    }
	return(retval);
}


/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_3*/
#ifdef __TI_COMPILER_VERSION__
#pragma CODE_STATE(sl_esm_high_intr_handler, 32)
#endif
#ifdef __IAR_SYSTEMS_ICC__
__arm
#endif
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_3*/
#ifdef __TI_COMPILER_VERSION__
#pragma INTERRUPT(sl_esm_high_intr_handler, FIQ)
#endif
#ifdef __IAR_SYSTEMS_ICC__
__fiq
#endif
static void sl_esm_high_intr_handler(void)
{
    register uint8  esmOffH,esmChannel;
#if FUNCTION_PROFILING_ENABLED
    register uint32 entrytick=0u;
    entrytick = _sl_pmuGetCycleCount_();
#endif
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
    esmOffH = (uint8) esmREG->IOFFHR;

    if (0u != esmOffH) {
        esmOffH--;
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        if (esmOffH >= 128u && esmOffH < 160u) {
            /* Group 1 channel 64 to 95 */
        	esmChannel = esmOffH - 64u;
        	esmGroup1Handler(esmChannel);
        } else 
#endif
        if (esmOffH >= 96u) {
            /* Shouldn't be here... */
			/* Misra C fix 477S added to make the else part non empty */
            esmOffH = esmOffH;
        }
        else if (esmOffH >= 64u) {
            /* Group 1 channel 32 to 63 */
        	esmChannel = esmOffH - 32u;
        	esmGroup1Handler(esmChannel);
        }
        else if (esmOffH >= 32u) {
            /* Group 2 channel 0 to 31 */
        	esmChannel = esmOffH - 32u;
        	esmGroup2Handler(esmChannel);
        }
        else if (esmOffH > 0u) {
            /* Group 1 channel 0 to 31 */
        	esmChannel = esmOffH;
        	esmGroup1Handler(esmChannel);
        }
        else {
            /* Shouldn't be here... */
			/* Misra C fix 477S added to make the else part non empty */
			esmOffH++;
        }
    }
    else {
        /* Shouldn't get here... */
		/* Misra C fix 477S added to make the else part non empty */
    	esmOffH++;
    }
#if FUNCTION_PROFILING_ENABLED
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    if ((esmChannel != ESM_G2ERR_B0TCM_ADDPAR) &&
    		(esmChannel != ESM_G2ERR_B1TCM_ADDPAR)) {
#endif
    	if(SL_Profile_Struct[SL_Active_Profile_Testtype-TESTTYPE_MIN].esm_entrytick == 0U )
    	{
        SL_Profile_Struct[SL_Active_Profile_Testtype-TESTTYPE_MIN].esm_entrytick = entrytick;
        SL_Profile_Struct[SL_Active_Profile_Testtype-TESTTYPE_MIN].esm_exittick = _sl_pmuGetCycleCount_();
    	}
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    }
#endif
#endif
}


/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_3*/
#ifdef __TI_COMPILER_VERSION__
#pragma CODE_STATE(sl_esm_low_intr_handler, 32)
#endif
#ifdef __IAR_SYSTEMS_ICC__
__arm
#endif
/*SAFETYMCUSW 69 S MR:3.4 <APPROVED> Comment_3*/
#ifdef __TI_COMPILER_VERSION__
#pragma INTERRUPT(sl_esm_low_intr_handler, IRQ)
#endif
#ifdef __IAR_SYSTEMS_ICC__
__irq
#endif
static void sl_esm_low_intr_handler(void) {
    register uint8  esmOffL,esmChannel;
#if FUNCTION_PROFILING_ENABLED
    register uint32 entrytick=0u;
    entrytick = _sl_pmuGetCycleCount_();
#endif
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
    esmOffL = ((uint8)esmREG->IOFFLR - (uint8)1u);

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    if ((esmOffL >= 128U) && (esmOffL < 160U))
    {
        /* Group 1 channel 64 to 95 */
    	esmChannel = esmOffL-64u;
    	esmGroup1Handler(esmChannel);
    }
    else {
#endif
    if ((esmOffL >= 64U) && (esmOffL < 96U))
    {
        /* Group 1 channel 32 to 63 */
    	esmChannel = esmOffL-32u;
    	esmGroup1Handler(esmChannel);
    }
    else if( esmOffL < 32U){
        /* Group 1 channel 0 to 31 */
    	esmChannel = esmOffL;
    	esmGroup1Handler(esmChannel);
    }
    else {
        /* Shouldn't be here... */
		/* Misra C fix 477S added to make the else part non empty */
    	esmOffL++;
    }
	#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
	}
	#endif
#if FUNCTION_PROFILING_ENABLED
   	if(SL_Profile_Struct[SL_Active_Profile_Testtype-TESTTYPE_MIN].esm_entrytick == 0U )
    	{
        SL_Profile_Struct[SL_Active_Profile_Testtype-TESTTYPE_MIN].esm_entrytick = entrytick;
        SL_Profile_Struct[SL_Active_Profile_Testtype-TESTTYPE_MIN].esm_exittick = _sl_pmuGetCycleCount_();
    	}
#endif
    return;
}


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#include "sl_reg_flash.h"
#endif

static void esmGroup2Handler (uint8 esmChannel)
{
	
    boolean cancelCallback = FALSE;
    register uint32 callbkParam1 = 0u, callbkParam2 = 0u, callbkParam3 = 0u;

    /* No need to clear ESM bit since reading ESMOFFH automatically clears the status bit */
    switch (esmChannel) {
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    	case ESM_G2ERR_L2RAMW_UNCORR_B:
    		 if (TRUE == SL_FLAG_GET(SRAM_ECC_ERROR_FORCING_1BIT) ||
    			 TRUE == SL_FLAG_GET(SRAM_ECC_ERROR_FORCING_2BIT) ) {
    	    	callbackCancelCount++;
    	    	cancelCallback = TRUE;
    		 }

    	break;

        case ESM_G2ERR_CCMR5_CPUCOMP_ERR:
            /* No additional parameters available */
        	callbkParam1 = 0u;
            /* Error is intentional due to diagnostic test - Clear the module
             * error */
            ccmr5fREG1->_CCMSR1 = CCMR5F_CCMSR_CMPE;
            /* Cancel callback if this was intentional */
            if (TRUE == SL_FLAG_GET(CCMR5F_CPUCOMP_ERROR_FORCING_TEST)) {
            	if (TRUE == SL_FLAG_GET(VIM_SOFTWARE_TEST)){
            		vimTestFlag = TRUE;
            	}
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
            break;

        case ESM_G2ERR_CCMR5_VIMCOMP_ERR:
            /* No additional parameters available */
        	callbkParam1 = 0u;
            /* Error is intentional due to diagnostic test - Clear the module
             * error */
            ccmr5fREG1->_CCMSR2 = CCMR5F_CCMSR_CMPE;
            /* Cancel callback if this was intentional */
            if (TRUE == SL_FLAG_GET(CCMR5F_VIMCOMP_ERROR_FORCING_TEST)) {
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
            break;

        case ESM_G2ERR_CCMR5_PDCOMP_ERR:
            /* No additional parameters available */
        	callbkParam1 = 0u;
            /* Error is intentional due to diagnostic test - Clear the module
             * error */
            ccmr5fREG1->_CCMSR3 = CCMR5F_CCMSR_CMPE;
            /* Cancel callback if this was intentional */
            if (TRUE == SL_FLAG_GET(CCMR5F_PDCOMP_ERROR_FORCING_TEST)) {
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
            break;

        case ESM_G2ERR_CCMR5_INMCOMP_ERR:
            /* No additional parameters available */
        	callbkParam1 = 0u;
            /* Error is intentional due to diagnostic test - Clear the module
             * error */
            ccmr5fREG1->_CCMSR4 = CCMR5F_CCMSR_CMPE;
            /* Cancel callback if this was intentional */
            if (TRUE == SL_FLAG_GET(CCMR5F_INMCOMP_ERROR_FORCING_TEST)) {
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
            break;
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        case ESM_G2ERR_CCMR4_COMP:
            /* No additional parameters available */
        	callbkParam1 = 0u;
            /* Clear the module error */
            ccmr4fREG1->_CCMSR = CCMR4F_CCMSR_CMPE;
            /* Cancel callback if this was intentional */
            if (TRUE == SL_FLAG_GET(CCMR4F_ERROR_FORCING_TEST)) {

            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
        break;

        case ESM_G2ERR_FMC_UNCORR:
        	if (TRUE == SL_FLAG_GET(FLASH_ADDRESS_PARITY_SELF_TEST)) {
        		sl_flashWREG->FPAROVR = 0x5400U;
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
            /* Flash Un-correctable error, capture the error address */
        	callbkParam1 = sl_flashWREG->FUNCERRADD;
            /* No status to clear here */
        break;

        case ESM_G2ERR_B0TCM_UNCORR:
        	if (TRUE == SL_FLAG_GET(SRAM_RADECODE_DIAGNOSTICS)) {
            	if (TRUE == SL_FLAG_GET(VIM_SOFTWARE_TEST)){
            		vimTestFlag = TRUE;
            	}
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
        	callbkParam1 = sl_tcram1REG->RAMUERRADDR;
            /* No status to clear here */
        break;

        case ESM_G2ERR_B1TCM_UNCORR:
        	if (TRUE == SL_FLAG_GET(SRAM_RADECODE_DIAGNOSTICS)) {
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
        	callbkParam1 = sl_tcram2REG->RAMUERRADDR;
            /* No status to clear here */
        break;

        case ESM_G2ERR_B0TCM_ADDPAR:
        	if (TRUE == SL_FLAG_GET(SRAM_PAR_ADDR_CTRL_SELF_TEST)) {
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
        	callbkParam1 = sl_tcram1REG->RAMPERADDR;
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
            callbkParam2 = (sl_tcram1REG->RAMERRSTATUS
                                & (TCRAM_RAMERRSTAT_WADDRPAR_FAIL | TCRAM_RAMERRSTAT_RADDRPAR_FAIL));
            callbkParam3 = (uint32) 0u;
            /* Clear the WADDR/RADDR PAR Fail bits in TCM Regs */
            break;

        case ESM_G2ERR_B1TCM_ADDPAR:
        	if (TRUE == SL_FLAG_GET(SRAM_PAR_ADDR_CTRL_SELF_TEST)) {
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
        	callbkParam1 = sl_tcram2REG->RAMPERADDR;
            /* No status to clear here */
            break;

        case ESM_G2ERR_FLASH_LIVLCK:
        	callbkParam1 = 0u;
            /* No status to clear here */
            break;

        case ESM_G2ERR_RTI_WWD_NMI:
        	callbkParam1 = 0u;
            /* No status to clear here */
        break;
#endif
        default:
        	/* nothing here - comment to avoid misra-c warning */
            break;
    }
    /* Call application callback only if it's registered & if it's not masked */
	/* the MSB of contains information regarding group and LSB contains the esm channel number*/
    if ((0u != ESM_AppCallback) && ((boolean)(FALSE) == cancelCallback)) {
    	(*(ESM_AppCallback))((uint32)(ESM_GRP2_MASK | esmChannel), callbkParam1, callbkParam2, callbkParam3);
    }
    return;
}


static
void esmGroup1Handler(uint8 esmChannel)
{

	boolean cancelCallback = FALSE;
    register uint32 callbkParam1 = 0u, callbkParam2 = 0u, callbkParam3 = 0u;

	switch (esmChannel) {
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)

        /** F021 Flash Correctabe error (bus1 & bus2) & non-EEPROM **/
		case ESM_G1ERR_FMC_CORR:

			if (TRUE == SL_FLAG_GET(FLASH_ECC_TEST_MODE_1BIT)) {
				cancelCallback  = TRUE;
				callbackCancelCount++;
			}
			/* Retrieve the callback parameters */
			/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
			callbkParam1 = (sl_flashWREG->FCORERRCNT & 0x0000FFFFu);
			/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
			callbkParam2 = (sl_flashWREG->FCORERRADD);
			/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
			callbkParam3 = (sl_flashWREG->FCORERRPOS & 0x000003FFu);
			break;

        /** PLL1 Slip error **/
        case ESM_G1ERR_PLL1_SLIP:
            /** Store the callback parameters & reset the status registers */
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
            callbkParam1 = (uint32)((uint32)(systemREG1->GBLSTAT & 0x00000300u) >> 8u);
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
            systemREG1->GBLSTAT |= (systemREG1->GBLSTAT & 0x00000300u);
            break;

        /** LPOCLK Detect error */
        case ESM_G1ERR_CLOCKMON_INTR:
            break;

        /** VIM Parity Error **/
        case ESM_G1ERR_VIMRAM_PARITY:

			if (TRUE == SL_FLAG_GET(VIM_SRAM_PARITY_TEST)) {
				cancelCallback  = TRUE;
				callbackCancelCount++;
			}
            /* Extract the parameters & clear the flag */
            callbkParam1 = SL_VIM_ADDERR;
            SL_VIM_PARFLG = 1u;
            break;

        /** B0TCM ECC Correctable error **/
        case ESM_G1ERR_B0TCM_CORRERR:

			if (TRUE == SL_FLAG_GET(SRAM_ECC_ERROR_FORCING_1BIT)) {
				cancelCallback  = TRUE;
				callbackCancelCount++;
			}
            /* Call application if this error was _not_ intentional (self-test) */
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
        	callbkParam1 = (sl_tcram1REG->RAMSERRADDR);
            break;

        /** B1TCM ECC Correctable error **/
        case ESM_G1ERR_B1TCM_CORRERR:

			if (TRUE == SL_FLAG_GET(SRAM_ECC_ERROR_FORCING_1BIT)) {
				cancelCallback  = TRUE;
				callbackCancelCount++;
			}
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
        	callbkParam1 = (sl_tcram2REG->RAMSERRADDR);
            /* Call application if this error was _not_ intentional (self-test) */
            break;

        /** DCC Error */
        case ESM_G1ERR_DCC1_ERR:
            callbkParam1 = sl_dccREG1->CNT0;
            callbkParam2 = sl_dccREG1->CNT1;
            sl_dccREG1->STAT |= DCC_DCCSTAT_ERR_FLG;
            break;
        /** CCMR4 Self-Test Compare error */
        case ESM_G1ERR_CCMR4_SELFTEST:
			/*ccmr4f selftests have has synchronous and asynchronous modes,the efuse esm errors are not blocked by disabling interrupt during selftest*/
		    /*blocking the esm application call back if the interrupt was due to a selftest*/
        	if (TRUE == SL_FLAG_GET(CCMR4F_ERROR_FORCING_TEST)) {
            	callbackCancelCount++;
            	cancelCallback = TRUE;
            }
        	break;

        case ESM_G1ERR_EEPROM_CORRERR:

			if (TRUE == SL_FLAG_GET(FEE_ECC_TEST_MODE_1BIT)) {
				cancelCallback  = TRUE;
				callbackCancelCount++;
			}
			/* Retrieve the callback parameters */
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
			callbkParam1 = (sl_flashWREG->EECORRERRADD);
			/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
			callbkParam2 = (sl_flashWREG->EECORRERRCNT);
			/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
			callbkParam3 = (sl_flashWREG->EECORRERRPOS & 0x000001FFu);

        	break;

        case ESM_G1ERR_EEPROM_UNCORRERR:

			if (TRUE == SL_FLAG_GET(FEE_ECC_TEST_MODE_1BIT)) {
				cancelCallback  = TRUE;
				callbackCancelCount++;
			}
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
        	callbkParam1 = (sl_flashWREG->EEUNCERRADD);
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
        	callbkParam2 = (sl_flashWREG->EESTATUS);

        	break;
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
		case ESM_G1ERR_PSCON_COMPARE_ERR:
     		/*pscon selftests have has synchronous and asynchronous modes,the efuse esm errors are not blocked by disabling interrupt during selftest*/
		    /*blocking the esm application call back if the interrupt was due to a selftest*/
			/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
			if ((PMM_PRCKEYREG_MKEY_ERRFCG_MODE == pmmREG->PRCKEYREG) && (TRUE == SL_FLAG_GET(PSCON_ERROR_FORCING))) {
				cancelCallback = TRUE;
			} else {
				cancelCallback = FALSE;
			}
			/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
			callbkParam1 = (((pmmREG->LPDDCSTAT1 & PMM_PSCON_LPDDCSTAT1_LCMPE) >> 16)
					/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
								| (pmmREG->MPDDCSTAT1 & PMM_PSCON_MPDDCSTAT1_MCMPE));
			break;

		case ESM_G1ERR_PSCON_SELTEST_ERR:
			/*pscon selftests have has synchronous and asynchronous modes,the efuse esm errors are not blocked by disabling interrupt during selftest*/
		    /*blocking the esm application call back if the interrupt was due to a selftest*/
			if (TRUE == SL_FLAG_GET(PSCON_SELF_TEST)){
				/* Since this is a self test related error, it's always intentionally
				 * created by kicking off the ST in PRCKEYREG. Selftest API takes
				 * care of clearing this error nothing needs to be done here */
				
				cancelCallback  = TRUE;
				callbackCancelCount++;
				SL_FLAG_CLEAR(PSCON_SELF_TEST);
			}
			break;
#endif

		case ESM_G1ERR_EFUSE_SELTEST_ERR:
			/*efuse selftests have synchronous and asynchronous modes.the efuse esm errors are not blocked by disabling interrupt during selftest*/
			if ((TRUE ==  SL_FLAG_GET(EFUSE_SELF_TEST_STUCK_AT_ZERO))||(TRUE ==  SL_FLAG_GET(EFUSE_SELF_TEST_ECC))){
				/* Since this is a self test related error, it's always intentionally
				 * created by kicking off the ST in PRCKEYREG. Selftest API takes
				 * care of clearing this error nothing needs to be done here */
				
				cancelCallback  = TRUE;
				callbackCancelCount++;
				SL_FLAG_CLEAR(EFUSE_SELF_TEST_ECC);
				SL_FLAG_CLEAR(EFUSE_SELF_TEST_STUCK_AT_ZERO);
			}
			break;

		case ESM_G1ERR_EFUSE_ERR_OTHER:
			/*efuse has synchronous and asynchronous modes,the efuse esm errors are not blocked by disabling interrupt during selftest*/
		    /*blocking the esm application call back if the interrupt was due to a selftest*/
			if (TRUE ==  SL_FLAG_GET(EFUSE_SELF_TEST_ECC)){
				/* Since this is a self test related error, it's always intentionally
				 * created by kicking off the ST in PRCKEYREG. Selftest API takes
				 * care of clearing this error nothing needs to be done here */
				
				cancelCallback  = TRUE;
				callbackCancelCount++;
				SL_FLAG_CLEAR(EFUSE_SELF_TEST_ECC);
			}
			break;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
        /** PLL2 Slip error **/
        case ESM_G1ERR_PLL2_SLIP:
            /** Store the callback parameters & reset the status registers */
        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
            callbkParam1 = (uint32)((uint32)(systemREG1->GBLSTAT & 0x00000300u) >> 8u);
            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
            systemREG1->GBLSTAT |= (systemREG1->GBLSTAT & 0x00000300u);
            break;

        /** DCC Error */
        case ESM_G1ERR_DCC2_ERR:
            callbkParam1 = sl_dccREG2->CNT0;
            callbkParam2 = sl_dccREG2->CNT1;
            sl_dccREG2->STAT |= DCC_DCCSTAT_ERR_FLG;
            break;
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
      	/* CCMR5 Self-Test Compare error */
        case ESM_G1ERR_CCMR5_SELFTEST:
            /*ccmr5f selftests have has synchronous and asynchronous modes,the
             * esm errors are not blocked by disabling interrupt during
             * selftest*/
            /* blocking the esm application call back if the interrupt was due to
             * a selftest.
             * NOTE: Getting here implies that the CCMR5F has an error! */
            if ((TRUE == SL_FLAG_GET(CCMR5F_CPUCOMP_SELF_TEST)) || 
                        (TRUE == SL_FLAG_GET(CCMR5F_VIMCOMP_SELF_TEST)) ||
                        (TRUE == SL_FLAG_GET(CCMR5F_PDCOMP_SELF_TEST)) ||
                        (TRUE == SL_FLAG_GET(CCMR5F_INMCOMP_SELF_TEST))) {
                /* clear the ESM Status flag */
				BIT_SET(esmREG->SR1[0], BIT(ESM_G1ERR_CCMR5_SELFTEST));
                callbackCancelCount++;
              	cancelCallback = TRUE;
            }
            break;

        case ESM_G1ERR_CCMR5_OPERATING_STATUS:
        	callbackCancelCount++;
            cancelCallback = TRUE;
        	esmREG->SR7[0] = GET_ESM_BIT_NUM(ESM_G1ERR_CCMR5_OPERATING_STATUS);
            break;

        case ESM_G1ERR_DMA_ECC_SBERR:
        	if(TRUE == SL_FLAG_GET(DMA_ECC_TEST_MODE_1BIT)) {
        		callbackCancelCount++;
        		cancelCallback = TRUE;
        	}
        	break;

        case ESM_G1ERR_DMA_ECC_UNCORR:
        	if(TRUE == SL_FLAG_GET(DMA_ECC_TEST_MODE_2BIT)) {
        		callbackCancelCount++;
        		cancelCallback = TRUE;
        	}
        	break;

        case ESM_G1ERR_CAN1_ECC_SBERR:
        case ESM_G1ERR_CAN2_ECC_SBERR:
        case ESM_G1ERR_CAN3_ECC_SBERR:
        case ESM_G1ERR_CAN4_ECC_SBERR:
        	if(TRUE == SL_FLAG_GET(CAN_ECC_TEST_MODE_1BIT)) {
        		callbackCancelCount++;
                cancelCallback = TRUE;
            }
            break;

        case ESM_G1ERR_CAN1_ECC_UNCORR:
        case ESM_G1ERR_CAN2_ECC_UNCORR:
        case ESM_G1ERR_CAN3_ECC_UNCORR:
        case ESM_G1ERR_CAN4_ECC_UNCORR:
        	if(TRUE == SL_FLAG_GET(CAN_ECC_TEST_MODE_2BIT)) {
        		callbackCancelCount++;
        		cancelCallback = TRUE;
        	}
        	break;

        case ESM_G1ERR_SPI1_ECC_SBERR:
        case ESM_G1ERR_SPI2_ECC_SBERR:
        case ESM_G1ERR_SPI3_ECC_SBERR:
        case ESM_G1ERR_SPI4_ECC_SBERR:
        case ESM_G1ERR_SPI5_ECC_SBERR:
        	if(TRUE == SL_FLAG_GET(MIBSPI_ECC_TEST_MODE_1BIT)) {
        		callbackCancelCount++;
                cancelCallback = TRUE;
            }
            break;

        case ESM_G1ERR_SPI1_ECC_UNCORR:
        case ESM_G1ERR_SPI2_ECC_UNCORR:
        case ESM_G1ERR_SPI3_ECC_UNCORR:
        case ESM_G1ERR_SPI4_ECC_UNCORR:
        case ESM_G1ERR_SPI5_ECC_UNCORR:
        	if(TRUE == SL_FLAG_GET(MIBSPI_ECC_TEST_MODE_2BIT)) {
        		callbackCancelCount++;
        		cancelCallback = TRUE;
        	}
        	break;

		case ESM_G1ERR_PSCON_COMPARE_ERR:
     		/*pscon selftests have has synchronous and asynchronous modes,the efuse esm errors are not blocked by disabling interrupt during selftest*/
		    /*blocking the esm application call back if the interrupt was due to a selftest*/

			if ((PMM_PRCKEYREG_MKEY_ERRFCG_MODE == pmmREG->PRCKEYREG) && (TRUE == SL_FLAG_GET(PSCON_ERROR_FORCING))) {
				cancelCallback = TRUE;
			} else {
				cancelCallback = FALSE;
			}
			callbkParam1 = ((pmmREG->LPDDCSTAT1 & PMM_PSCON_LPDDCSTAT1_LCMPE) >> 16);
			break;

		case ESM_G1ERR_PSCON_SELTEST_ERR:
			/*pscon selftests have has synchronous and asynchronous modes,the efuse esm errors are not blocked by disabling interrupt during selftest*/
		    /*blocking the esm application call back if the interrupt was due to a selftest*/
			if (TRUE == SL_FLAG_GET(PSCON_SELF_TEST)){
				/* Since this is a self test related error, it's always intentionally
				 * created by kicking off the ST in PRCKEYREG. Selftest API takes
				 * care of clearing this error nothing needs to be done here */

				cancelCallback  = TRUE;
				callbackCancelCount++;
				SL_FLAG_CLEAR(PSCON_SELF_TEST);
			}
			break;

	        /** PLL1 Slip error **/
	        case ESM_G1ERR_PLL1_SLIP:
	            /** Store the callback parameters & reset the status registers */
	        	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
	            callbkParam1 = (uint32)((systemREG1->GBLSTAT & 0x00000300u) >> 8u);
	            /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_4*/
	            systemREG1->GBLSTAT |= (systemREG1->GBLSTAT & 0x00000300u);
	            break;

	            /** PLL2 Slip error **/
	            case ESM_G1ERR_PLL2_SLIP:
	                /** No callback parameters - error cause not defined in device TRM */
	            	
	                break;

#endif

        default:
        	/* nothing here - comment to avoid misra-c warning */
			break;
	}
    /* Call application callback only if it's registered & if it's not masked */
	/* the MSB of contains information regarding group and LSB contains the esm channel number*/
    if ((0u != ESM_AppCallback) && ((boolean)(FALSE) == cancelCallback)) {
        (*(ESM_AppCallback))((uint32)(ESM_GRP1_MASK|esmChannel), callbkParam1, callbkParam2, callbkParam3);
    }
return;
}

/*Comment_1:
 * "Reason -  Needed"*/

/*Comment_2:
 * "This function will be called by application so not static"*/

/*Comment_3:
 * "Reason -  used for creation of code section for interrupt handler"*/

/*Comment_4:
 * "Reason -  FALSE_POSITIVE The rule requires that the expression should be
 * guaranteed to have the same value regardless of the order in which the operands within the expression are evaluated.
 * This usually means no more than one volatile access occuring in the expression.The recommendation that simple
 * assignments should be used is intended to be restricted to simple assignments of the form x = v and not
 * "simple assignments" according to the ISO definition. However, the recommendation does not have to be followed
 * in order to be compliant with the rule. You should implement the rule (the value should be independent of the
 * evaluation order) for compliance purposes and you might choose to issue a warning for non-simple assignments
 * or you might choose to ignore them."*/
