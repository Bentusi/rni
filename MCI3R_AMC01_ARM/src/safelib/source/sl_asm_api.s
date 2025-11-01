;/*******************************************************************************
;**+--------------------------------------------------------------------------+**
;**|                            ****                                          |**
;**|                            ****                                          |**
;**|                            ******o***                                    |**
;**|                      ********_///_****                                   |**
;**|                      ***** /_//_/ ****                                   |**
;**|                       ** ** (__/ ****                                    |**
;**|                           *********                                      |**
;**|                            ****                                          |**
;**|                            ***                                           |**
;**|                                                                          |**
;**|         Copyright (c) 2012 Texas Instruments Incorporated                |**
;**|                        ALL RIGHTS RESERVED                               |**
;**|                                                                          |**
;**| Permission is hereby granted to licensees of Texas Instruments           |**
;**| Incorporated (TI) products to use this computer program for the sole     |**
;**| purpose of implementing a licensee product based on TI products.         |**
;**| No other rights to reproduce, use, or disseminate this computer          |**
;**| program, whether in part or in whole, are granted.                       |**
;**|                                                                          |**
;**| TI makes no representation or warranties with respect to the             |**
;**| performance of this computer program, and specifically disclaims         |**
;**| any responsibility for any damages, special or consequential,            |**
;**| connected with the use of this program.                                  |**
;**|                                                                          |**
;**+--------------------------------------------------------------------------+**
;*******************************************************************************/
;/**
; * @file        sl_asm_api.inc
; * @version     2.3.1
; * @brief       Safety Library APIs in Assembly
; *
; * @notes		1. All functions are redirected to test_sl section
; *
; * <\todo Detailed description goes here>
; */

.equ ARM_MODE_SYS, (0x1F)

.equ PMCR_EVNTEXP, (0x00000010)

	.text
    .arm
    .global _SL_Get_ARM_Mode
_SL_Get_ARM_Mode:
    MRS     R0, CPSR                //Get CPSR Value
    AND     R0, R0, #ARM_MODE_SYS   //Clear rest of the information
	BX      LR
	
/* Barriers to ensure the the data accesses are complete & instr., are executed */
.global _SL_Barrier_Data_Access
_SL_Barrier_Data_Access:
	DMB
	DSB
	BX		LR

/* Kick off the STC execution */
.global  _SL_Kickoff_STC_execution
_SL_Kickoff_STC_execution:
	WFI
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	NOP // Added for allowing debug breakpoints.
	
	BX     LR //Though not required, to be safe adding branch here
	
.global _SL_Get_DataFault_Address
_SL_Get_DataFault_Address:
    MRC     p15, #0, r0, c6, c0, #0
    BX      LR
    
.global _SL_Get_DataFault_Status
_SL_Get_DataFault_Status:
    MRC     p15, #0, r0, c5, c0, #0
    BX      LR

    
.global _SL_Init_EnableEventExport
_SL_Init_EnableEventExport:
	STMFD   SP!, {R0}

    MRC     P15, #0, R0, C9, C12, #0
    ORR     R0, R0, #PMCR_EVNTEXP
    MCR     P15, #0, R0, C9, C12, #0

    LDMFD   SP!, {R0}
    BX      LR
    