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
 * @file        sl_init.h
 * @version     2.3.1
 * @brief       File contains the intialization APIs exposed by Safety Library
 *
 *
 */

/* Comments regarding various justificatios for deviation from MISRA-C coding guidelines
 * are provided at the bottom of the file */

#include <sl_api.h>
#include <sl_priv.h>
#include <sl_config.h>
   
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
boolean SL_Init_ECCTCMRAM (uint16 threshold, boolean enableErrIntGen)
{
	boolean retVal = TRUE;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    /* Enable the ECC Check in CP Auxillary Control registers */
    _SL_Init_ECCTCMRAM();

    /** First configure and then enable the ECC Checks **/
    /* RAMOCCUR register must be cleared before setting the threshold */
    sl_tcram1REG->RAMOCCUR = 0u;
    sl_tcram2REG->RAMOCCUR = 0u;
    sl_tcram1REG->RAMTHRESHOLD = threshold;
    sl_tcram2REG->RAMTHRESHOLD = threshold;

    if ((boolean)(TRUE) == enableErrIntGen) {
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        BF_SET(sl_tcram1REG->RAMINTCTRL, SERR_ENABLE, BF_SERR_EN_START, BF_SERR_EN_LEN);
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        BF_SET(sl_tcram2REG->RAMINTCTRL, SERR_ENABLE, BF_SERR_EN_START, BF_SERR_EN_LEN);
    } else {
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        BF_SET(sl_tcram1REG->RAMINTCTRL, SERR_DISABLE, BF_SERR_EN_START, BF_SERR_EN_LEN);
        /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
        BF_SET(sl_tcram2REG->RAMINTCTRL, SERR_DISABLE, BF_SERR_EN_START, BF_SERR_EN_LEN);
    }
    /* Current regVal disables writes to ECC Memory & Enables ECC detect */
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    BF_SET( sl_tcram1REG->RAMCTRL, ECC_DETECT_DISABLE, BF_ECC_DETECT_EN_START, BF_ECC_DETECT_EN_LEN);
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    BF_SET( sl_tcram1REG->RAMCTRL, ECC_WR_DISABLE, BF_ECC_WR_EN_START, BF_ECC_WR_EN_LEN);

    /* Current regVal disables writes to ECC Memory & Enables ECC detect */
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    BF_SET( sl_tcram2REG->RAMCTRL, ECC_DETECT_DISABLE, BF_ECC_DETECT_EN_START, BF_ECC_DETECT_EN_LEN);
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    BF_SET(sl_tcram2REG->RAMCTRL, ECC_WR_DISABLE, BF_ECC_WR_EN_START, BF_ECC_WR_EN_LEN);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)

	 /* Enabled by default at reset */
#endif
    _SL_Init_EnableEventExport();

    return (retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
boolean SL_Init_ECCFlash (uint16 secThreshold, uint16 ctrlFlags)
{
    boolean retVal = TRUE;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    uint32 fedacctrl1,eectrl1;
    /* Enable the ECC Check in CP Auxillary Control registers */
    _SL_Init_ECCFlash();

    fedacctrl1 = sl_flashWREG->FEDACCTRL1;
    /*Select Correction Mode */
	/*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    BF_SET(fedacctrl1, EDACMODE_DEF, BF_EDACMODE_START, BF_EDACMODE_LEN);
    /*Enable SECDED */
	/*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    BF_SET(fedacctrl1, EDAC_ENABLE_DEF, BF_EDACEN_START, BF_EDACEN_LEN);
    /*Set control flags*/
    fedacctrl1 |= ctrlFlags;
    sl_flashWREG->FEDACCTRL1 = fedacctrl1;
    /*Single Error Correction Threshold*/
    sl_flashWREG->FEDACCTRL2 = secThreshold;
	/* Enable ECC detection for FEE */
    eectrl1 = sl_flashWREG->EECTRL1;
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    BF_SET(eectrl1, EDACMODE_DEF, BF_EDACMODE_START, BF_EDACMODE_LEN);
    /*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    BF_SET(eectrl1, EDAC_ENABLE_DEF, BF_EDACEN_START, BF_EDACEN_LEN);
    sl_flashWREG->EECTRL1 = eectrl1;
    sl_flashWREG->EECTRL1 |= ctrlFlags;
    sl_flashWREG->EECTRL2 = secThreshold;
#endif

    _SL_Init_EnableEventExport();
    return (retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_4*/
boolean SL_Init_Memory (SL_AutoInitRamId ramSet)
{
	boolean retVal = FALSE;
#if (FUNCTION_PARAM_CHECK_ENABLED == 1)
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
	if (0u != ((uint32)(~(ALL_RAMTYPE_IDS)) & ramSet))
#elif defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
	if (0u != ((uint32)(~((uint32)(RAMTYPE_RAM) | (uint32)(RAMTYPE_DMA_RAM) | (uint32)(RAMTYPE_VIM) | (uint32)(RAMTYPE_NHET1) | (uint32)(RAMTYPE_HET_TU1) | (uint32)(RAMTYPE_NHET2) | (uint32)(RAMTYPE_HET_TU2) | (uint32)(RAMTYPE_MIBSPI1_RAM) | (uint32)(RAMTYPE_MIBSPI3_RAM) | (uint32)(RAMTYPE_MIBSPI5_RAM) | (uint32)(RAMTYPE_DCAN1_RAM) | (uint32)(RAMTYPE_DCAN2_RAM) | (uint32)(RAMTYPE_DCAN3_RAM))) & (uint32)(ramSet)))
#else
	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    if (0u != ((uint32)(~((uint32)(RAMTYPE_RAM) | (uint32)(RAMTYPE_DMA_RAM) | (uint32)(RAMTYPE_VIM) | (uint32)(RAMTYPE_NHET1) | (uint32)(RAMTYPE_HET_TU1))) & (uint32)(ramSet)))
#endif
    {
        return(retVal);
    }
#endif
    /* Enable Memory Hardware Initialization */
	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
	/*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    BF_SET(systemREG1->MINITGCR, ENABLE_GLOB_MEM_HW_INIT, BF_MINITGENA_START, BF_MINITGENA_LENGTH);

    /* Enable Memory Hardware Initialization for selected RAM's */
	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
    systemREG1->MSINENA  = ramSet;


	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
	/*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    /*SAFETYMCUSW 28 D <APPROVED> Comment_6*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    while (MEM_HW_INIT_COMPLETE != BF_GET(systemREG1->MSTCGSTAT, BF_MINIDONE_START, BF_MINIDONE_LENGTH)) {

    }
    /* Disable Memory Hardware Initialization */
	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
	/*SAFETYMCUSW 9 S MR: 12.2 <APPROVED> Comment_2*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    /*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    BF_SET(systemREG1->MINITGCR, DISABLE_MEM_STEST_CTRL, BF_MINITGENA_START, BF_MINITGENA_LENGTH);

    retVal = TRUE;
    return(retVal);
}


/*SAFETYMCUSW 7 C MR: 14.7 <APPROVED> Comment_4*/

/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
SL_ResetReason SL_Init_ResetReason(void)
{
    register uint32 sysEsr;
    register SL_ResetReason retVal = RESET_TYPE_UNKNOWN;

	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
    sysEsr = systemREG1->SYSESR;

    /* Check for nPORRST pin low - Cold reset, or power-on reset. This reset
     * signal is typically driven by an external voltage supervisor. This reset
     * is flagged by the PO RST bit in the SYSESR register, SYSESR[15]
     */
     if (SYSESR_PORRST == (sysEsr & SYSESR_PORRST)) {
        retVal = RESET_TYPE_POWERON;
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
        systemREG1->SYSESR = SYSESR_PORRST;
     } else if (SYSESR_OSCRST == (sysEsr & SYSESR_OSCRST)) {
        /*
         * This reset is generated by the system module when the clock monitor
         * detects an oscillator fail condition. Whether or not a reset is
         * generated is also dictated by a register in the system module. This
         * reset is flagged by the OSC RST bit in the SYSESR register, SYSESR[14]
         */
        retVal = RESET_TYPE_OSC_FAILURE;
        /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
        systemREG1->SYSESR = SYSESR_OSCRST;
     }

     else if (SYSESR_WDRST == (sysEsr & SYSESR_WDRST)) {
       /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
    	 /*SAFETYMCUSW 96 S MR: 6.2,10.1,10.2,12.1,12.6 <APPROVED> Comment_7*/
        if (RTI_RTIWDSTATUS_DWD_ST == (uint32)(rtiREG1->WDSTATUS & RTI_RTIWDSTATUS_DWD_ST)) {
            /*
             * This reset is generated by the digital windowed watchdog (DWWD) module
             * on the microcontroller. The DWWD can generate a reset whenever the
             * watchdog service window is violated. This reset is also flagged by
             * bit 13 of the SYSESR register, SYSESR[13]
             */
	       /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
             systemREG1->SYSESR = SYSESR_WDRST;
            retVal = RESET_TYPE_WATCHDOG;
        }
        else {
            /*
             * The ICEPICK logic implemented on the microcontroller allows a
             * system reset to be generated via the debug logic. This reset is
             * flagged by the DBG RST bit of the SYSESR register, SYSESR[13].
             */
        	systemREG1->SYSESR = SYSESR_WDRST;
            retVal = RESET_TYPE_DEBUG;
        }
     }
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
     else if (SYSESR_CPURST == (sysEsr & SYSESR_CPURST)) {
        /*
         * This reset is generated by the CPU self-test controller (LBIST) or by changing
         * the memory protection (MMU/MPU) configuration in the CPURSTCR register. This
         * reset is flagged by the CPU RST bit of the SYSESR register, SYSESR[5].
         */
        retVal = RESET_TYPE_CPU;
	   /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
        systemREG1->SYSESR = SYSESR_CPURST;
     }
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
     else if (SYSESR_CPURST1 == (sysEsr & SYSESR_CPURST1)) {
             /*
              * This reset is generated by the CPU self-test controller (LBIST) or by changing
              * the memory protection (MMU/MPU) configuration in the CPURSTCR register. This
              * reset is flagged by the CPU RST bit of the SYSESR register, SYSESR[5].
              */
             retVal = RESET_TYPE_CPU1;
     	   /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
             systemREG1->SYSESR = SYSESR_CPURST1;
     }
     else if (SYSESR_CPURST0 == (sysEsr & SYSESR_CPURST0)) {
             /*
              * This reset is generated by the CPU self-test controller (LBIST) or by changing
              * the memory protection (MMU/MPU) configuration in the CPURSTCR register. This
              * reset is flagged by the CPU RST bit of the SYSESR register, SYSESR[5].
              */
             retVal = RESET_TYPE_CPU0;
     	   /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
             systemREG1->SYSESR = SYSESR_CPURST0;
     }
#endif
     else if (SYSESR_SWRST == (sysEsr & SYSESR_SWRST)) {
        /*
         * This reset is generated by the application software writing a 1 to bit 15
         * of System Exception Control Register (SYSECR) or a 0 to bit 14 of SYSECR.
         * It is typically used by a bootloader type of code that uses a software reset
         * to allow the code execution to branch to the application code once it is
         * programmed into the program memory. This reset is flagged by the SW RST bit
         * of the SYSESR register, SYSESR[4]
         */
        retVal = RESET_TYPE_SWRST;
		/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
        systemREG1->SYSESR = SYSESR_SWRST;
		/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
     } else if (SYSESR_EXTRST == (sysEsr & SYSESR_EXTRST)) {
        /*
         * Driving nRST pin low externally.  This reset is flagged
         * by the EXT RST bit in the SYSESR, register SYSESR[3].
         * The nRST could also be driven in response to a nERROR generated or
         * for other reasons (async to this device).
         */
     	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/

        if (TRUE == SL_ESM_nERROR_Active()) {
            retVal = RESET_TYPE_EXTRST_NERROR;
       } else {
            retVal = RESET_TYPE_EXTRST;
       }

	   /*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
        systemREG1->SYSESR = SYSESR_EXTRST;
     }
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
     else if (SYSESR_ICSTRST == (sysEsr & SYSESR_ICSTRST)) {
         /*
          * This reset is generated by the cpu interconnect diagnostic error
          */
         retVal = RESET_TYPE_ICSTRST;
         systemREG1->SYSESR = SYSESR_ICSTRST;
      }
#endif
     else
     {
    	 /*default reset type*/
    	 retVal = RESET_TYPE_DEBUG;
     }
     return(retVal);
}


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> Comment_1*/
boolean SL_Init_ResetReason_XInfo (SL_ResetReason_XInfo* rrXInfo)
{
    /* Read the status from ESM registers */
	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
	boolean retVal = TRUE;
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
	/*SAFETYMCUSW 134 S MR: 12.2 <APPROVED> Comment_3*/
    if((sl_esmREG->EPSR & ESM_ESMEPSR_ESF) == 0u )
    {
        rrXInfo->nError = TRUE;
    }
    else
    {
        rrXInfo->nError = FALSE;
    }
	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
    rrXInfo->esmGrp2Status  = sl_esmREG->SSR2;

    /* Clear ESM Grp2 shadow register */
	/*SAFETYMCUSW 440 S MR: 11.3 <APPROVED> Comment_5*/
    sl_esmREG->SSR2 = rrXInfo->esmGrp2Status;
#endif
    return(retVal);
}

/*Comment_1:
 * "This function will be called by application so not static"*/

/*Comment_2:
 * "Assignment in an expression is valid here"*/

/*Comment_3:
 * "Reason -  FALSE_POSITIVE The rule requires that the expression should be
 * guaranteed to have the same value regardless of the order in which the operands within the expression are evaluated.
 * This usually means no more than one volatile access occuring in the expression.The recommendation that simple
 * assignments should be used is intended to be restricted to simple assignments of the form x = v and not
 * "simple assignments" according to the ISO definition. However, the recommendation does not have to be followed
 * in order to be compliant with the rule. You should implement the rule (the value should be independent of the
 * evaluation order) for compliance purposes and you might choose to issue a warning for non-simple assignments
 * or you might choose to ignore them."*/

 /*Comment_4:
 * "Reviewed - The entry condition and parameter checks shall exit immediately" */
 
 /*Comment_5:
 * "The base address of the peripheral registers are actually macros" */
 
 /*Comment_6:
 * "Reason - Design change necessary to remove the infinite while loop" */

/*Comment_7:
 * "False positive" */
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
