/** @file HL_sys_startup.c 
*   @brief Startup Source File
*   @date 08-Feb-2017
*   @version 04.06.01
*
*   This file contains:
*   - Include Files
*   - Type Definitions
*   - External Functions
*   - VIM RAM Setup
*   - Startup Routine
*   .
*   which are relevant for the Startup.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */


/* Include Files */

#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_sys_vim.h"
#include "HL_sys_core.h"
#include "HL_esm.h"
#include "HL_sys_mpu.h"

#include "sl_types.h"
#include "sl_api.h"
#include "sl_priv.h"

/* USER CODE BEGIN (2) */
/* USER CODE END */

/* External Functions */
/*SAFETYMCUSW 354 S MR:NA <APPROVED> " Startup code(main should be declared by the user)" */
extern void main(void);
/*SAFETYMCUSW 122 S MR:20.11 <APPROVED> "Startup code(exit and abort need to be present)" */
/*SAFETYMCUSW 354 S MR:NA <APPROVED> " Startup code(Extern declaration present in the library)" */
extern void exit(int _status);


static void afterSTC(void);

/* Startup Routine */
void _c_int00(void) __attribute__((noreturn));
/* USER CODE BEGIN (4) */
/* USER CODE END */

__attribute__ ((naked))

/* SourceId : STARTUP_SourceId_001 */
/* DesignId : STARTUP_DesignId_001 */
/* Requirements : HL_CONQ_STARTUP_SR1 */
void _c_int00(void)
{
    static resetSource_t        s_resetReason;      /* Reset reason */
    static SL_STC_Config         s_stcSelfTestConfig;  /* STC Configuration */

    /* Initialize L2RAM to avoid ECC errors right after power on */
    _memInit_();

    /* Initialize Core Registers to avoid CCM Error */
    _coreInitRegisters_();

    /* Initialize Stack Pointers */
    _coreInitStackPointer_();

    /* Enable CPU Event Export */
    /* This allows the CPU to signal any single-bit or double-bit errors detected
     * by its ECC logic for accesses to program flash or data RAM.
     */
    /* NOTE - needs to be called on every reset */
    _coreEnableEventBusExport_();

    /* Reset handler: the following instructions read from the system exception status register
     * to identify the cause of the CPU reset.
     */
    if ((SYS_EXCEPTION & (uint32)INTERCONNECT_RESET) != 0U)
    {
        if(!SL_SelfTest_Status_MemIntrcntSelftest()) /*Memory Interconnect Error*/
        {
            while(1)
            {
                ; /*for misra2004*/
            }
        }
    }

    switch(getResetSource())
    {
        case POWERON_RESET:
        case DEBUG_RESET:
        case EXT_RESET:
            /* Check if there were ESM group3 errors during power-up.
             * These could occur during eFuse auto-load or during reads from flash OTP
             * during power-up. Device operation is not reliable and not recommended
             * in this case. */
            if ((esmREG->SR1[2]) != 0U)
            {
               esmGroup3Notification(esmREG,esmREG->SR1[2]);
            }

            /* Initialize System - Clock, Flash settings with Efuse self check */
            systemInit();
            break;

        case OSC_FAILURE_RESET:
            break;

        case WATCHDOG_RESET:
        case WATCHDOG2_RESET:
            break;

        case CPU0_RESET:
            if ((uint8_t)0xAU == (uint8_t)(stcREG1->STCSCSCR & (uint8_t)0xFU))/* check if this was an stcSelfCheck run */
            {
                if ((uint8_t)0x3U != (uint8_t)(stcREG1->STCGSTAT & (uint8_t)0x3U))/* check if the self-test fail bit is set */
                {
                    for(;;)
                    {
                        ; /*for misra2004*/
                    }
                }
                else/* STC self-check has passed */
                {
                    stcREG1->STCSCSCR = (uint32_t)0x05U;/* clear self-check mode */
                    stcREG1->STCGSTAT = (uint32_t)0x3U;/* clear STC global status flags */
                    esmREG->SR1[0U] = (uint32_t)0x08000000U;/* clear ESM group1 channel 27 status flag */

                    /* Start CPU Self-Test */
                    s_stcSelfTestConfig.stcClockDiv      = (uint8_t)0U;           /* STC Clock divider = 1 */
                    s_stcSelfTestConfig.intervalCount    = (uint16_t)1U;              /* One interval only */
                    s_stcSelfTestConfig.restartInterval0     = TRUE;         /* Start from interval 0 */
                    s_stcSelfTestConfig.timeoutCounter   = (uint32_t)0xFFFFFFFFU;      /* Timeout counter*/
                    _SL_HoldNClear_nError();

                    /* mask vim interrupts before running STC */
                    vimREG->REQMASKCLR0 = (uint32_t)0xFFFFFFFFu;
                    vimREG->REQMASKCLR1 = (uint32_t)0xFFFFFFFFu;
                    vimREG->REQMASKCLR2 = (uint32_t)0xFFFFFFFFu;
                    vimREG->REQMASKCLR3 = (uint32_t)0xFFFFFFFFu;

                    if((uint32_t)0u == esmREG->SSR2)/* ensure no pending ESM GRP2 errors before running STC */
                    {
                        (void)SL_SelfTest_STC(STC1_RUN, TRUE, &s_stcSelfTestConfig);
                    }
                }
            }
            else if ((uint8_t)0x1U == (uint8_t)(stcREG1->STCGSTAT & (uint8_t)0x1U))/* CPU reset caused by CPU self-test completion */
            {
                if ((uint8_t)0x2U == (uint8_t)(stcREG1->STCGSTAT & (uint8_t)0x2U))/* Self-Test Fail flag is set */
                {
                    for(;;)
                    {
                        ; /*for misra2004*/
                    }
                }
                /* CPU self-test completed successfully */
                else
                {

                    stcREG1->STCGSTAT = (uint32_t)0x1U;/* clear STC global status flag */
                    afterSTC();/* Continue start-up sequence after CPU STC completed */
                }
            }
            /* CPU reset caused by software writing to CPU RESET bit */
            else
            {
                _coreEnableEventBusExport_();/* Enable CPU Event Export */
            }
            break;
    
        case SW_RESET:
        break;

        default:
            break;
    }

    if(DEBUG_RESET != s_resetReason)
    {
        if ((SYS_EXCEPTION & (uint32)INTERCONNECT_RESET) != 0U)
        {
            (void)SL_SelfTest_MemoryInterconnect(MEMINTRCNT_SELFTEST); /* Memory interconnect selftest */
        }

        /* Make sure that the CPU self-test controller can actually detect a fault inside CPU */
        s_stcSelfTestConfig.stcClockDiv      = (uint8_t)0U;             /* STC Clock divider = 1 */
        s_stcSelfTestConfig.intervalCount   = (uint16_t)1U;             /* One interval only */
        s_stcSelfTestConfig.restartInterval0       = TRUE;              /* Start from interval 0 */
        s_stcSelfTestConfig.timeoutCounter   = (uint32_t)0xFFFFFFFFU;   /* Timeout counter*/
        _SL_HoldNClear_nError();

        /* mask vim interrupts before running STC */
        vimREG->REQMASKCLR0 = (uint32_t)0xFFFFFFFFu;
        vimREG->REQMASKCLR1 = (uint32_t)0xFFFFFFFFu;
        vimREG->REQMASKCLR2 = (uint32_t)0xFFFFFFFFu;
        vimREG->REQMASKCLR3 = (uint32_t)0xFFFFFFFFu;

        if((uint32_t)0u == esmREG->SSR2)/* ensure no pending ESM GRP2 errors before running STC */
        {
            (void)SL_SelfTest_STC(STC1_COMPARE_SELFCHECK, TRUE, &s_stcSelfTestConfig);
        }
        else
        {
            while(1); /* ESM Group2 error */
        }
    }
    else
    {
        afterSTC();
    }
}

/****************************************************************************************************
* Identifier:   SCOD-LOAD-007 (Trace to: SLD-LOAD-007)
* Function:     afterSTC
* Description:  cache、esm等功能初始化
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
static void afterSTC(void)
{
    /* Enable IRQ offset via Vic controller */
    _coreEnableIrqVicOffset_();

    vimInit();

    esmInit();/* This function can be configured from the ESM tab of HALCoGen */

    _mpuInit_();

    /* initialize global variable and constructors */
    {
    	extern uint32 _sidata, _sdata, _edata;
    	extern uint32 _start_flash_rom, _start_flash_ram, _end_flash_ram;
    	uint32 *src, *dst;

    	src = &_sidata;
    	dst = &_sdata;
       	while (dst < &_edata)
       	{
    		*dst++ = *src++;
    	}

       	src = &_start_flash_rom;
        dst = &_start_flash_ram;
        while (dst < &_end_flash_ram)
        {
            *dst++ = *src++;
        }
    }

    _cacheEnable_();

    /* call the application */
    main();

    exit(0);
}

void _exit(int state)
{
    while(1);
}
