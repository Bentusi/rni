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
 * @file        sl_api.h
 * @version     2.3.1
 * @brief       Main API files for Hercules SafeTI&trade; Hercules&trade; Diagnostic Library
 *
 * @details The function prototypes of the of the API's which are exposed by the\n
 * SafeTI&trade; Hercules&trade; Diagnostic Library are declared here.The file also includes the depandancies\n
 * of the SafeTI&trade; Hercules&trade; Diagnostic Library API's This single file can be included by\n
 * application which needs to make use of the SafeTI&trade; Hercules&trade; Diagnostic Library in their\n
 * application.
 */

#ifndef __SL_API_H__
#define __SL_API_H__

/** \defgroup RegInclud Types & Module register definition files */
/**@{*/
#include <sl_types.h>
#include <sl_config.h>
#include <sl_log.h>
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#include <reg_bitdefn_ccmr4f.h>
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#include <reg_bitdefn_ccmr5f.h>
#include <reg_bitdefn_pmm.h>
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#include <reg_bitdefn_dcc.h>
#endif
#include <reg_bitdefn_efc.h>
#include <reg_bitdefn_esm.h>
#include <reg_bitdefn_f021flash.h>
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#include <reg_bitdefn_iomm.h>
#endif
#include <reg_bitdefn_pbist.h>

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#include <reg_bitdefn_pmm.h>
#endif

#include <reg_bitdefn_rti.h>
#include <reg_bitdefn_stc.h>
#include <reg_bitdefn_system.h>

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#include <reg_bitdefn_tcmram.h>
#include <reg_bitdefn_dma.h>
#include <reg_bitdefn_het.h>
#include <reg_bitdefn_htu.h>
#include <reg_bitdefn_gio.h>
#include <reg_bitdefn_mibspi.h>
#include <reg_bitdefn_sci.h>
#include <reg_bitdefn_can.h>
#include <reg_bitdefn_lin.h>
#include <reg_bitdefn_pcr.h>
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#include <reg_bitdefn_l2ramw.h>
#include <reg_bitdefn_dma.h>
#include <reg_bitdefn_can.h>
#include <reg_bitdefn_mibspi.h>
#include <reg_bitdefn_sci.h>
#include <reg_bitdefn_epc.h>
#include <reg_bitdefn_scm.h>
#include <reg_bitdefn_sdcmmr.h>
#include <reg_bitdefn_adc.h>
#include <reg_bitdefn_pcr.h>
#endif

#include <reg_bitdefn_vim.h>
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#include <reg_bitdefn_adc.h>
#endif
/**@}*/

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief   Initializes Cortex-R4F CPU's internal registers.
 *
 * @description Hercules series of MCU include dual Cortex-R4F CPUs in lock-step. At power up registers are not\n
 * guaranteed to be in same state for both the CPUs. The CPU pushes the internal registers on to the stack on\n
 * a function call, which could lead to the detection of a core compare error.  Therefore, the CPU internal\n
 * core registers need to be initialized to a predefined state before any function call is made. This function\n
 * is recommended to be executed right after a reset.
 *
 * \if imageenabled
 * @image html  SL_Init_R4Registers.jpg
 * @image rtf  SL_Init_R4Registers.jpg
 * @image latex  SL_Init_R4Registers.eps
 * \endif
 * @param [in]  None
 * @param [out] None
 *
 * @return      None
 *
 * Example Usage:
 * @code
 *      SL_Init_R4Registers();
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    SVC Mode
 *
 * @seealso     @ref SL_Init_StackPointers
 *
 * **Note: This function does not initialize Stack Pointers.SL_Init_StackPointers must be called to initialize the stack pointers.**
 *
 * \if destagenabled
 * @destag        DES_01
 * @reqtag        SL_SR213
 * \endif
 */
void SL_Init_R4Registers (void);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
/**
 * @brief   Initializes Cortex-R5F CPU's internal registers.
 *
 * @description Hercules series of MCU include dual Cortex-R5F CPUs in lock-step. At power up registers are not\n
 * guaranteed to be in same state for both the CPUs. The CPU pushes the internal registers on to the stack on\n
 * a function call, which could lead to the detection of a core compare error.  Therefore, the CPU internal\n
 * core registers need to be initialized to a predefined state before any function call is made. This function\n
 * is recommended to be executed right after a reset.
 *
 * \if imageenabled
 * @image html  SL_Init_R5Registers.jpg
 * @image rtf  SL_Init_R5Registers.jpg
 * @image latex  SL_Init_R5Registers.eps
 * \endif
 * @param [in]  None
 * @param [out] None
 *
 * @return      None
 *
 * Example Usage:
 * @code
 *      SL_Init_R5Registers();
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    SVC Mode
 *
 * @seealso     @ref SL_Init_StackPointers
 *
 * **Note: This function does not initialize Stack Pointers. @ref SL_Init_StackPointers must be called to initialize the stack pointers.**
 *
 * \if destagenabled
 * @destag        DES_01
 * @reqtag        SL_SR213
 * \endif
 */
void SL_Init_R5Registers (void);
#endif

#ifndef EXTERNAL_SP_INIT
/**
 * @brief   Initializes CPU's stack pointer for SVC, FIQ, IRQ, ABT, UND & SYS modes.
 *
 * @description This function initializes the stack pointers for the privilege modes of the CPU. Following\n
 * variables are used as stack pointers and hence must be defined by the application. StackModeSVC,\n
 * StackModeFIQ, StackModeIRQ, StackModeABT, StackModeUND, StackModeSYS. Function uses the\n
 * macros @ref grp_armmodes from sl_config.h file.
 *
 * \if imageenabled
 * @image html  SL_Init_StackPointers.jpg
 * @image rtf  SL_Init_StackPointers.jpg
 * @image latex  SL_Init_StackPointers.eps
 * \endif
 *
 * @param [in]  None
 * @param [out] None
 *
 * @return      None
 *
 * Example Usage:
 * @code
 *      SL_Init_StackPointers();
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    SVC Mode
 *
 * @seealso     @ref SL_Init_R4Registers
 *
 * **Note:        Application can define the macro EXTERNAL_SP_INIT to disable
 * the use of this API and use its own implementation to initialize the stack
 * pointers. This provision is provided because the diagnostic library expects
 * the application to define global variables pointing to the Stack pointer
 * locations, and if not using this API, need to be removed.
 *
 * \if destagenabled
 * @destag        DES_02
 * @reqtag        SL_SR213
 * \endif
 */
void SL_Init_StackPointers (void);
#endif




/**
 * @brief   Hardware intialization of memories
 *
 * @description This function auto-initializes memories using the hardware initialization on Hercules devices.\n
 * Hercules devices support auto-initialization on most memories on the system bus and on the\n
 * peripheral bus. The memory used for the FlexRay message objects is not directly CPU addressable,\n
 * hence there is no memory auto-initialization support for this memory.\n
 * \n
 * The system module on the Hercules microcontroller allows all on-chip SRAMs to be initialized in hardware.\n
 * This is especially essential since all the on-chip memories support some form of error detection. The CPU\n
 * data RAM supports ECC while the peripheral memories support parity error detection. The auto initialization\n
 * mechanism also initializes the ECC or parity memories, as required.
 *\n
 * For possible values of the ramSet please refer to @ref _SL_AutoInitRamId.
 *
 *
 * \if imageenabled
 * @image html  SL_Init_Memory.jpg
 * @image rtf  SL_Init_Memory.jpg
 * @image latex SL_Init_Memory.eps
 * \endif
 *
 * @param [in]  ramSet Specifies the memories to be auto-initialized.
 * @param [out] None
 *
 * @return      TRUE if function executed successfully else FALSE
 *
 * Example Usage:
 * @code
 *      boolean retVal;
 *      retVal = SL_Init_Memory((RAMTYPE_RAM | RAMTYPE_VIM));
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    Same as at entry
 *
 * @seealso     @ref _SL_AutoInitRamId\n
 *              Refer to the document "Initialization of Hercules?ARM  Cortex -R4F Microcontrollers"\n
 *              for more information(refer to section 2.27)
 *
 * **Note:The ECC or parity should be enabled on the RAMs before hardware auto-initialization starts if parity or ECC is being used.**
 *
 * \if destagenabled
 * @destag        DES_03
 *
 * @reqtag        SL_SR213
 * \endif
 */
boolean SL_Init_Memory (SL_AutoInitRamId ramSet);

/* Not needed for R5 based devices, as ECC is always enabled for memory accesses
 * in R5. */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Enables detection of Flash ECC errors on Cortex-R4F
 *
 * @implementation The on-chip Flash memory (ATCM) supports SECDED diagnositcs. This function enables this\n
 * mechanism by exporting error detection events from the CPU to the Flash wrapper and then\n
 * from the Flash wrapper to the ESM. This functionality is not enabled by default and must\n
 * be configured by the software. The R4F PMU is first set to export events to external monitor\n
 * and then the Flash wrapper is confiugred to export the correctable and uncorrectable events\n
 * to the ESM.
 *
 * \if imageenabled
 * @image html  SL_Init_ECCFlash.jpg
 * @image rtf  SL_Init_ECCFlash.jpg
 * @image latex  SL_Init_ECCFlash.eps
 * \endif
 *
 * @param [in]  secThreshold - Single Error Correction Threshold(updates the fields for EE_CTRL2/FEDACCTRL2 register in flash module)
 * @param [in]    ctrlFlags - Error Detection and Correction flags (updates the fields for EE_CTRL1/FEDACCTRL1 register in flash module)\n
 *              **Note : please refer the section for F021 Flash Module Controller of the TRM for more information**
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *            boolean retVal;
 *          retVal = SL_Init_ECCFlash(10, FLASHECC_DEFAULT);
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    No change
 *
 * @seealso     @ref SL_Init_ECCTCMRAM, @ref FLASHECCConfig
 *
 *
 * \if destagenabled
 * @destag        DES_04
 * @reqtag        5.8.1.a, SL_SR213
 * \endif
 */
boolean SL_Init_ECCFlash (uint16 secThreshold, uint16 ctrlFlags);
#endif

/* Not needed for R5 based devices, as ECC is always enabled for memory accesses
 * in R5. */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Enables/Disables detection of ECC errors on TCM SRAM on Cortex-R4F
 *
 * @description The on-chip TCM SRAM is supported by SECDED ECC diagnostics. In this SECDED scheme, an 8-bit\n
 * code word is used to store the ECC data as calculated over the 64-bit data bus. This function\n
 * eports error detection events from the CPU to the SRAM wrapper, and then from the SRAM wrapper\n
 * to the ESM.
 *
 * \if imageenabled
 * @image html  SL_Init_ECCTCMRAM.jpg
 * @image rtf  SL_Init_ECCTCMRAM.jpg
 * @image latex SL_Init_ECCTCMRAM.eps
 * \endif
 *
 * @param [in]  threshold - Number of single-bit error corrections by the SECDED
                logic inside the Cortex-R4F CPU before generating a single-bit error interrupt.
 * @param [in] enableErrIntGen - When TRUE enables the generation of the single-bit error interrupt
 *              when the count reaches the programmed threshold. If the interrupt is not enabled,
 *              the single-bit error counter continues to count by resetting back to zero without
 *                 generating any error interrupt.
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *        boolean retVal;
 *      //Set the threshold to 1 & generarte ESM interrupt on each error
 *      retVal = SL_Init_ECCTCMRAM(10, TRUE);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso     @ref SL_Init_ECCFlash
 *
 * \if destagenabled
 * @destag        DES_05
 * @reqtag        5.10.1.a, SL_SR213
 * \endif
 */
boolean SL_Init_ECCTCMRAM (uint16 threshold, boolean enableErrIntGen);
#endif


/**
 * @brief       Provides source of last reset
 *
 * @description The Hercules microcontroller can be reset by either of the conditions described below. This function returns\n
 * reset condition indicated in the System Exception Status Register (SYSESR).\n
 *
 * - **Driving nPORRST pin low :**            Cold reset, or power-on reset. This reset signal is typically driven by an\n
 *                                             external voltage externally supervisor.
 *
 * - **Voltage Monitor reset :**             The microcontroller has an embedded voltage monitor that generates a power-on\n
 *                                             reset when the core voltage gets out of a valid range, or when the I/O voltage\n
 *                                            falls below a threshold.
 *
 * - **Warm reset :**                         This reset input is typically used in a system with multiple ICs and which\n
 *                                            requires that the microcontroller also gets reset whenever the other IC detects\n
 *                                            a fault condition.
 *
 * - **Oscillator failure :**                This reset is generated by the system module when the clock monitor detects an\n
 *                                             oscillator fail condition.
 *
 * - **Software reset :**                     This reset is generated by the application software\n
 *
 * - **CPU reset :**                         This reset is generated by the CPU self-test controller (LBIST) or by changing\n
 *                                             the memory protection (MMU/MPU) configuration in the CPURSTCR register.
 *
 * - **Debug reset :**                         The ICEPICK logic implemented on the microcontroller allows a system reset to\n
 *                                             be generated via the debug logic.
 *
 * - **Watchdog reset :**                     This reset is generated by the digital windowed watchdog (DWWD) module on the\n
 *                                             microcontroller. The DWWD can generate a reset whenever the watchdog service\n
 *                                             window is violated.
 *
 * \if imageenabled
 * @image html  SL_Init_ResetReason.jpg
 * @image rtf  SL_Init_ResetReason.jpg
 * @image latex  SL_Init_ResetReason.eps
 * \endif
 *
 * @param [in]    None
 * @param [out]     None
 *
 * @return      SL_ResetReason - Reset reason
 *
 * Example Usage:
 * @code
 *        SL_ResetReason rstReason;
 *         rstReason = SL_Init_ResetReason();
 * @endcode
 *
 * @entrymode   Can be called in any operating mode of CPU
 * @exitmode    No change in the operating mode
 *
 * @seealso     Type:@ref _SL_ResetReason
 *
 * \if destagenabled
 * @destag        DES_06
 * @reqtag        SL_SR17
 * \endif
 */
SL_ResetReason SL_Init_ResetReason(void);


/**
 * @brief       Executes diagnostic tests on TCM RAM (for R4 devices) or L2RAM (for R5 devices) SECDED logic
 *
 * @description The on-chip SRAM is supported by SECDED ECC diagnostics. This function forces 1Bit/2Bit\n
 * errors and checks for proper error responses.
 *
 * \if imageenabled
 * @image html  SL_SelfTest_SRAM.jpg
 * @image rtf  SL_SelfTest_SRAM.jpg
 * @image latex  SL_SelfTest_SRAM.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * | testType                                                           |
 * |--------------------------------------------------------------------|
 * |@ref SRAM_ECC_ERROR_FORCING_1BIT                                    |
 * |@ref SRAM_ECC_1BIT_FAULT_INJECTION  (only for R4 devices)           |
 * |@ref SRAM_ECC_ERROR_FORCING_2BIT                                    |
 * |@ref SRAM_ECC_2BIT_FAULT_INJECT (only for R4 devices)               |
 * |@ref SRAM_PAR_ADDR_CTRL_SELF_TEST (only for R4 devices)             |
 * |@ref SRAM_ECC_ERROR_PROFILING (only for R4 devices)                 |
 * |@ref SRAM_ECC_ERROR_PROFILING_FAULT_INJECT (only for R4 devices)    |
 * |@ref SRAM_RADECODE_DIAGNOSTICS                                      |
 * |@ref SRAM_LIVELOCK_DIAGNOSTICS (only for R4 devices)                |
 *
 * @param [in]     bMode - Reserved. Application must set it to TRUE.
 * @param [in]    param1 - Pointer to structure that is used to return the test status & results\n
 * **Note: contents of structure param1 is valid only for self-test. Not valid for Fault Injection modes.**
 * @return      TRUE if function executed successfully else FALSE
 *
 * NOTE: ECC Error Profiling tests should be conducted preferably at boot time only to avoid\n
 *       conflicts in RAMOCCUR values.\n
 *
 * Example Usage:
 * @code
 *        boolean retVal;
 *      //Execute 1Bit test
 *      SL_SelfTest_Result sram_stResult;
 *      retVal = SL_SelfTest_SRAM(SRAM_ECC_ERROR_FORCING_1BIT, TRUE, &sram_stResult);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso     Type:@ref _SL_SelfTest_Type
 *
 * \if profilingenabled
 * @profiling
 *
 * TestType                     | Profiling Time in us
 * -----------------------------| -------------
 * SRAM_ECC_ERROR_FORCING_1BIT  | 12.4245
 * SRAM_ECC_ERROR_FORCING_2BIT  | 10.6655
 * SRAM_PAR_ADDR_CTRL_SELF_TEST | 17.566
 * SRAM_RADECODE_DIAGNOSTICS    | 11.065
 * \endif
 *
 * \if destagenabled
 * @destag        DES_07
 * @reqtag        SL_SR57,SL_SR60,SL_SR61, SL_SR59, SLC_73, SLC_85
 * \endif
 */
boolean SL_SelfTest_SRAM(SL_SelfTestType testType, boolean bMode, SL_SelfTest_Result * sram_stResult);


/**
 * @brief       Executes diagnostic tests on F021 Flash
 *
 * @description F021 Flash wrapper provides to verify various logic. The flash wrapper provides multiple diagnostic modes.\n
 * Thisfunction executes the selected diagnostic mode.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType\n
 * \n
 * **Note : Refer to Safety Manual for TMS570LS31x/21x and RM48x Hercules ARM Safety Critical Microcontrollers
 * (section on Primary Embedded Flash and RM48x/TMS570LS31x 16/32-Bit RISC Flash Microcontroller TRM
 * (section F021 Flash Module Controller)**
 *\n
 * **Note: For FLASH_ECC_TEST_MODE_2BIT_FAULT_INJECT the fault is injected in the subsequent read
 * of the corrupted flash area by the application. The application must take care to restore the flash diagnostic control registers .**
 * **Note: FLASH_HARD_ERROR_LIVELOCK is not supported in this release.**
 *
 *
 * \if imageenabled
 * @image html  SL_SelfTest_Flash.jpg
 * @image rtf  SL_SelfTest_Flash.jpg
 * @image latex  SL_SelfTest_Flash.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                                            |
 * |--------------------------------------------------------------------|
 * |@ref FLASH_ECC_TEST_MODE_1BIT                                       |
 * |@ref FLASH_ECC_TEST_MODE_2BIT        |
 * |@ref FLASH_ECC_ADDR_TAG_REG_MODE (only for R4 devices)        |
 * |@ref FLASH_ADDRESS_ECC_SELF_TEST (only for R4 devices)        |
 * |@ref FLASH_ADDRESS_ECC_FAULT_INJECT (only for R4 devices)        |
 * |@ref FLASH_ADDRESS_PARITY_SELF_TEST (only for R4 devices)        |
 * |@ref FLASH_ADDRESS_PARITY_FAULT_INJECT (only for R4 devices)        |
 * |@ref FLASH_ECC_TEST_MODE_2BIT_FAULT_INJECT (only for R4 devices)        |
 *
 * @param [in]     bMode - NOT USED
 * @param [in]    flash_stResult - Pointer to structure that is used to return the test status & results\n
 * **Note: contents of structure param1 is valid only for self-test.Not valid for Fault Injection mode.**
 *
 * @return      TRUE if function executed successfully else FALSE
 *
 * Example Usage:
 * @code
 *        boolean retVal;
 *      //Execute 1Bit test
 *      SL_SelfTest_Result flash_stResult;
 *      retVal = SL_SelfTest_Flash(FLASHECC_ECC_TEST_MODE_1BIT, TRUE, &flash_stResult);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso     Type:@ref _SL_SelfTest_Type
 *
 * \if profilingenabled
 * @profiling
 *
 * TestType                               | Profiling Time in us
 * ---------------------------------------| -------------
 * FLASH_ECC_ADDR_TAG_REG_MODE            | 8.437
 * FLASH_ECC_TEST_MODE_1BIT               | 7.3605
 * FLASH_ECC_TEST_MODE_2BIT               | 13.3885
 * \endif
 *
 * \if destagenabled
 * @destag        DES_08
 * @reqtag        SL_SR40,SL_SR42,SL_SR43, SLC_53, SLC_63
 * \endif
 */
boolean SL_SelfTest_Flash (SL_SelfTestType testType, boolean bMode, SL_SelfTest_Result * flash_stResult);


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Executes diagnostic tests on F021 Flash
 *
 * @description F021 Flash wrapper provides to verify various logic. The flash wrapper provides multiple diagnostic modes.\n
 * This function executes the selected diagnostic mode.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.\n
 *
 * **Note : Refer to Safety Manual for TMS570LS31x/21x and RM48x Hercules ARM Safety Critical Microcontrollers
 * (section on Primary Embedded Flash and RM48x/TMS570LS31x 16/32-Bit RISC Flash Microcontroller TRM
 * (section F021 Flash Module Controller)**
 *
 * \if imageenabled
 * @image html  SL_SelfTest_FEE.jpg
 * @image rtf  SL_SelfTest_FEE.jpg
 * @image latex  SL_SelfTest_FEE.eps
 * \endif
 *
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                     |
 * |---------------------------------------------|
 * |@ref FEE_ECC_DATA_CORR_MODE                  |
 * |@ref FEE_ECC_SYN_REPORT_MODE        |
 * |@ref FEE_ECC_MALFUNCTION_MODE1        |
 * |@ref FEE_ECC_MALFUNCTION_MODE2        |
 * |@ref FEE_ECC_ADDR_TAG_REG_MODE        |
 * |@ref FEE_ECC_TEST_MODE_1BIT        |
 * |@ref FEE_ECC_TEST_MODE_1BIT_FAULT_INJECT        |
 * |@ref FEE_ECC_TEST_MODE_2BIT        |
 * |@ref FEE_ECC_TEST_MODE_2BIT_FAULT_INJECT        |
 *
 * @param [in]     bMode - NOT USED
 * @param [in]    fee_stResult - Pointer to structure that is used to return the test status & results\n
 * **Note: contents of structure param1 is valid only for self-test.Not valid for Fault Injection modes.**
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *        boolean retVal;
 *      //Execute 1Bit test
 *      SL_SelfTest_Result flash_stResult;
 *      retVal = SL_SelfTest_FEE(FLASH_ECC_SYN_REPORT_MODE, TRUE, &flash_stResult);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso     Type:@ref _SL_SelfTest_Type
 *
 * \if destagenabled
 * @destag        DES_09
 * @reqtag        SL_SR50
 * \endif
 */
boolean SL_SelfTest_FEE (SL_SelfTestType testType, boolean bMode, SL_SelfTest_Result * fee_stResult);
#endif


/**
 * @brief       Executes PBIST tests on RAM groups with different algorithms
 *
 * @description Hercules microcontrollers support run-time-programmable memory BIST engine for varying levels of\n
 * coverage across many embedded memory instance. This function executes the selected PBIST Algorithms\n
 * on the selected RAM Groups.This API just initiates the PBIST for the provide  ramgroups and algorithms \n
 * The API @ref SL_SelfTest_Status_PBIST should be used to get the status of the PBIST execution which \n
 * which has completed.
 *
 * Please refer to
 *     - @ref _SL_SelfTest_Type for the values to be used for testType.
 *     - @ref PBIST_Algorithm   for the PBIST Algorithm types(possible values for algoInfo).
 *     - @ref PBIST_RAMGroup    for the PBIST Algorithm types(possible values for ramGroup).
 *
 * \if imageenabled
 * @image html  SL_SelfTest_PBIST.jpg
 * @image rtf  SL_SelfTest_PBIST.jpg
 * @image latex  SL_SelfTest_PBIST.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                            |
 * |------------------------------------|
 * |@ref PBIST_EXECUTE                  |
 * |@ref PBIST_EXECUTE_OVERRIDE         |
 *
 * @param [in]     ramGroup - List of RAM Groups to be used for testing.  ** NOTE: This parameter is of type uint64 for RM57Lx and TMS570LC43x devices **
 * @param [in]    algoInfo - List of algorithms to be used for testing.
 *
 * **Note: Refer to Device TRM for supported combinations of RAM Group & Algorithms.**\n
 * **Note: The PBIST on RAM memories is destructive and corrupts the memories.**\n
 * **Note: PBIST Algorithm should not be used on SRAM when code/data resides in SRAM.The application.**\n
 * **needs to appropriately branch to non volatile location without the use of the data variables during this test.**
 *
 *
 * @return      TRUE if function executed successfully else FALSE
 *
 * Example Usage:
 * @code
 *        boolean retVal;
 *      retVal = SL_SelfTest_PBIST( PBIST_EXECUTE,
 *                                 (PBIST_RAMGROUP_07_MIBSPI1  |
 *                                  PBIST_RAMGROUP_08_MIBSPI3   |
 *                                  PBIST_RAMGROUP_09_MIBSPI5   |
 *                                  PBIST_RAMGROUP_10_VIM       |
 *                                  PBIST_RAMGROUP_11_MIBADC1   |
 *                                  PBIST_RAMGROUP_18_MIBADC2),
 *                                  PBISTALGO_MARCH13N_RED_2PORT);
 *        while (TRUE != SL_SelfTest_PBIST_ExecStatus(&pbistStatus));
  *          retVal = SL_SelfTest_PBIST_StopExec();
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso     @ref _SL_SelfTest_Type, @ref PBIST_RAMGroup,@ref PBIST_Algorithm,@ref SL_SelfTest_Status_PBIST & @ref SL_SelfTest_PBIST_StopExec
 *
 * \if profilingenabled
 * @profiling
 *
 * RAMGROUP                      | ALGORITHM                        | Profiling Time in us
 * ----------------------------- | ---------------------------------|-----------
 * PBIST_RAMGROUP_01_PBIST_ROM     |  PBISTALGO_TRIPLE_READ_SLOW_READ |    261.2105
 * PBIST_RAMGROUP_01_PBIST_ROM     |  PBISTALGO_TRIPLE_READ_FAST_READ |    97.295
 * PBIST_RAMGROUP_02_STC_ROM       |  PBISTALGO_TRIPLE_READ_SLOW_READ |    211.308
 * PBIST_RAMGROUP_02_STC_ROM       |  PBISTALGO_TRIPLE_READ_FAST_READ |    80.765
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_MARCH13N_RED_2PORT    |    332.0085
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_DOWN1A_RED_2PORT      |    85.404
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_MAPCOLUMN_2PORT       |    110.2615
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_PRECHARGE_2PORT       |    78.144
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_DTXN2_2PORT           |    115.3145
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_PMOS_OPEN_2PORT       |    268.895
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    269.143
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_FLIP10_2PORT          |    78.932
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_IDDQ_2PORT            |    28.445
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_RETENTION_2PORT       |    28.655
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_IDDQ2_2PORT           |    28.415
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_RETENTION2_2PORT      |    28.655
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    28.895
 * PBIST_RAMGROUP_03_DCAN1         |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    28.895
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_MARCH13N_RED_2PORT    |    331.7705
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_DOWN1A_RED_2PORT      |    85.535
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_MAPCOLUMN_2PORT       |    102.253
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_PRECHARGE_2PORT       |    78.125
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_DTXN2_2PORT           |    115.295
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_PMOS_OPEN_2PORT       |    268.895
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    269.07
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_FLIP10_2PORT          |    70.925
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_IDDQ_2PORT            |    28.415
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_RETENTION_2PORT       |    28.655
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_IDDQ2_2PORT           |    28.415
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_RETENTION2_2PORT      |    28.655
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    28.895
 * PBIST_RAMGROUP_04_DCAN2         |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    28.895
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_MARCH13N_RED_2PORT    |    331.7745
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_DOWN1A_RED_2PORT      |    85.418
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_MAPCOLUMN_2PORT       |    102.365
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_PRECHARGE_2PORT       |    78.095
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_DTXN2_2PORT           |    115.295
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_PMOS_OPEN_2PORT       |    268.895
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    269.018
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_FLIP10_2PORT          |    70.925
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_IDDQ_2PORT            |    28.415
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_RETENTION_2PORT       |    28.655
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_IDDQ2_2PORT           |    28.415
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_RETENTION2_2PORT      |    28.655
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    28.895
 * PBIST_RAMGROUP_05_DCAN3         |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    28.895
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_MARCH13N_RED_2PORT    |    395.552
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_DOWN1A_RED_2PORT      |    93.005
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_MAPCOLUMN_2PORT       |    115.775
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_PRECHARGE_2PORT       |    83.615
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_DTXN2_2PORT           |    174.335
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_PMOS_OPEN_2PORT       |    379.178
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    379.325
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_FLIP10_2PORT          |    82.415
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_IDDQ_2PORT            |    26.015
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_RETENTION_2PORT       |    13.945
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_IDDQ2_2PORT           |    26.015
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_RETENTION2_2PORT      |    14.185
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    27.305
 * PBIST_RAMGROUP_07_MIBSPI1       |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    26.554
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_MARCH13N_RED_2PORT    |    395.408
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_DOWN1A_RED_2PORT      |    92.975
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_MAPCOLUMN_2PORT       |    115.775
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_PRECHARGE_2PORT       |    83.615
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_DTXN2_2PORT           |    174.464
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_PMOS_OPEN_2PORT       |    379.325
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    379.382
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_FLIP10_2PORT          |    82.445
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_IDDQ_2PORT            |    26.015
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_RETENTION_2PORT       |    13.945
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_IDDQ2_2PORT           |    26.015
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_RETENTION2_2PORT      |    14.185
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    26.495
 * PBIST_RAMGROUP_08_MIBSPI3       |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    26.495
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_MARCH13N_RED_2PORT    |    395.506
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_DOWN1A_RED_2PORT      |    93.005
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_MAPCOLUMN_2PORT       |    115.775
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_PRECHARGE_2PORT       |    83.615
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_DTXN2_2PORT           |    174.335
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_PMOS_OPEN_2PORT       |    379.195
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    379.325
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_FLIP10_2PORT          |    82.415
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_IDDQ_2PORT            |    26.015
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_RETENTION_2PORT       |    14.185
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_IDDQ2_2PORT           |    26.015
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_RETENTION2_2PORT      |    14.185
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    27.302
 * PBIST_RAMGROUP_09_MIBSPI5       |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    26.6135
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_MARCH13N_RED_2PORT    |    75.248
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_DOWN1A_RED_2PORT      |    27.695
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_MAPCOLUMN_2PORT       |    30.815
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_PRECHARGE_2PORT       |    26.495
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_DTXN2_2PORT           |    37.775
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_PMOS_OPEN_2PORT       |    58.175
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    58.175
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_FLIP10_2PORT          |    23.135
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_IDDQ_2PORT            |    15.935
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_RETENTION_2PORT       |    16.175
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_IDDQ2_2PORT           |    15.935
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_RETENTION2_2PORT      |    16.175
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    16.175
 * PBIST_RAMGROUP_11_MIBADC1       |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    16.175
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_MARCH13N_RED_2PORT    |    163.955
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_DOWN1A_RED_2PORT      |    52.022
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_MAPCOLUMN_2PORT       |    58.382
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_PRECHARGE_2PORT       |    49.235
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_DTXN2_2PORT           |    62.915
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_PMOS_OPEN_2PORT       |    15.895
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    84.275
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_FLIP10_2PORT          |    39.875
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_IDDQ_2PORT            |    23.795
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_RETENTION_2PORT       |    24.035
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_IDDQ2_2PORT           |    23.795
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_RETENTION2_2PORT      |    24.035
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    24.035
 * PBIST_RAMGROUP_12_DMA           |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    24.035
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_MARCH13N_RED_2PORT    |    452.376
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_DOWN1A_RED_2PORT      |    120.365
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_MAPCOLUMN_2PORT       |    142.415
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_PRECHARGE_2PORT       |    111.215
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_DTXN2_2PORT           |    157.066
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_PMOS_OPEN_2PORT       |    19.255
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    260.975
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_FLIP10_2PORT          |    93.695
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_IDDQ_2PORT            |    40.415
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_RETENTION_2PORT       |    40.895
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_IDDQ2_2PORT           |    40.415
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_RETENTION2_2PORT      |    40.895
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    41.375
 * PBIST_RAMGROUP_13_N2HET1        |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    41.365
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_MARCH13N_RED_2PORT    |    144.6005
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_DOWN1A_RED_2PORT      |    50.975
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_MAPCOLUMN_2PORT       |    56.255
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_PRECHARGE_2PORT       |    49.295
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_DTXN2_2PORT           |    57.695
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_PMOS_OPEN_2PORT       |    80.735
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    80.975
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_FLIP10_2PORT          |    36.335
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_IDDQ_2PORT            |    25.535
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_RETENTION_2PORT       |    25.775
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_IDDQ2_2PORT           |    25.535
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_RETENTION2_2PORT      |    32.4295
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    26.0375
 * PBIST_RAMGROUP_14_HETTU1        |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    26.015
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_MARCH13N_RED_2PORT    |    75.215
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_DOWN1A_RED_2PORT      |    27.695
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_MAPCOLUMN_2PORT       |    30.815
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_PRECHARGE_2PORT       |    26.495
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_DTXN2_2PORT           |    37.775
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_PMOS_OPEN_2PORT       |    58.175
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    58.175
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_FLIP10_2PORT          |    23.135
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_IDDQ_2PORT            |    15.935
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_RETENTION_2PORT       |    16.175
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_IDDQ2_2PORT           |    15.935
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_RETENTION2_2PORT      |    16.175
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    16.175
 * PBIST_RAMGROUP_18_MIBADC2       |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    16.175
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_MARCH13N_RED_2PORT    |    452.329
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_DOWN1A_RED_2PORT      |    120.365
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_MAPCOLUMN_2PORT       |    142.415
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_PRECHARGE_2PORT       |    111.215
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_DTXN2_2PORT           |    157.017
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_PMOS_OPEN_2PORT       |    19.495
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    260.975
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_FLIP10_2PORT          |    93.695
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_IDDQ_2PORT            |    40.415
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_RETENTION_2PORT       |    40.895
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_IDDQ2_2PORT           |    40.415
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_RETENTION2_2PORT      |    40.895
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    41.365
 * PBIST_RAMGROUP_19_N2HET2        |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    49.15
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_MARCH13N_RED_2PORT    |    144.605
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_DOWN1A_RED_2PORT      |    50.975
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_MAPCOLUMN_2PORT       |    56.255
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_PRECHARGE_2PORT       |    49.295
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_DTXN2_2PORT           |    57.695
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_PMOS_OPEN_2PORT       |    80.735
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    80.975
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_FLIP10_2PORT          |    36.335
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_IDDQ_2PORT            |    25.535
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_RETENTION_2PORT       |    25.775
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_IDDQ2_2PORT           |    28.457
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_RETENTION2_2PORT      |    25.7975
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    26.015
 * PBIST_RAMGROUP_20_HETTU2        |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    26.015
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_MARCH13N_RED_2PORT    |    138.815
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_DOWN1A_RED_2PORT      |    43.535
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_MAPCOLUMN_2PORT       |    49.535
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_PRECHARGE_2PORT       |    41.135
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_DTXN2_2PORT           |    53.855
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_PMOS_OPEN_2PORT       |    19.705
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    77.375
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_FLIP10_2PORT          |    35.135
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_IDDQ_2PORT            |    20.495
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_RETENTION_2PORT       |    20.495
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_IDDQ2_2PORT           |    20.495
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_RETENTION2_2PORT      |    20.498
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    20.519
 * PBIST_RAMGROUP_23_ETHERNET1     |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    20.495
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_MARCH13N_RED_2PORT    |    115.295
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_DOWN1A_RED_2PORT      |    39.215
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_MAPCOLUMN_2PORT       |    44.735
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_PRECHARGE_2PORT       |    38.015
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_DTXN2_2PORT           |    54.335
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_PMOS_OPEN_2PORT       |    77.375
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_PMOS_OPEN_SLICE1_2POR |    77.375
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_FLIP10_2PORT          |    30.815
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_IDDQ_2PORT            |    20.015
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_RETENTION_2PORT       |    20.255
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_IDDQ2_2PORT           |    20.015
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_RETENTION2_2PORT      |    20.255
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_IDDQROWSTRIPE_2PORT   |    20.52
 * PBIST_RAMGROUP_24_ETHERNET2     |  PBISTALGO_IDDQROWSTRIPE2_2PORT  |    20.522
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_MAPCOLUMN_1PORT       |    348.865
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_PRECHARGE_1PORT       |    225.255
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_DTXN2_1PORT           |    921.023
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_PMOS_OPEN_1PORT       |    2108.999
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_PMOS_OPEN_SLICE2_1POR |    14.6515
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_FLIP10_1PORT          |    261.245
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_IDDQ_1PORT            |    43.622
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_RETENTION_1PORT       |    35.937
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_IDDQ2_1PORT           |    35.835
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_RETENTION2_1PORT      |    35.835
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_IDDQROWSTRIPE_1PORT   |    36.035
 * PBIST_RAMGROUP_25_ETHERNET3     |  PBISTALGO_IDDQROWSTRIPE2_1PORT  |    36.035
 * \endif
 *
 * \if destagenabled
 * @destag      DES_10
 * @reqtag        SL_SR63,SL_SR64,SL_SR89,SL_SR90,SL_SR108,SL_SR109,SL_SR122,SL_SR123,SL_SR132,SL_SR133,SL_SR142,SL_SR143,SL_SR163,SL_SR164,SL_SR188,SL_SR189,SL_SR197,SL_SR198,SCL_49,SCL_50,SCL_79,SCL_80,SCL_158,SCL_159,SCL_180,SCL_181,SCL_197,SCL_198,SCL_210,SCL_211,SCL_237,SCL_238,SCL_262,SCL_263,SCL_275,SCL_276,SCL_290,SCL_291
 * \endif
 */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
boolean SL_SelfTest_PBIST (register SL_SelfTestType testType, register uint32 ramGroup, register uint32 algoInfo);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
boolean SL_SelfTest_PBIST (register SL_SelfTestType testType, register uint64 ramGroup, register uint32 algoInfo);
#endif


/**
 * @brief       Provides status information on the previous PBIST execution
 *
 * @description This API returns FALSE if the PBIST execution has not yet completed and if the PBIST execution is over the\n
 * the API updates the failinfo parameter with the relavant information on the status of the PBIST execution.This can be used\n
 * as apolling function after the call of the @ref SL_SelfTest_PBIST API in order to get the status of the PBIST selftest.
 *
 * \if imageenabled
 * @image html  SL_SelfTest_Status_PBIST.jpg
 * @image rtf  SL_SelfTest_Status_PBIST.jpg
 * @image latex  SL_SelfTest_Status_PBIST.eps
 * \endif
 *
 * @param [out]    param1     - Pointer to structure that is used to return the test status & results.
 * @param [in]     None
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *        boolean retVal;
 *      retVal = SL_SelfTest_PBIST( PBIST_EXECUTE,
 *                                 (PBIST_RAMGROUP_07_MIBSPI1  |
 *                                  PBIST_RAMGROUP_08_MIBSPI3   |
 *                                  PBIST_RAMGROUP_09_MIBSPI5   |
 *                                  PBIST_RAMGROUP_10_VIM       |
 *                                  PBIST_RAMGROUP_11_MIBADC1   |
 *                                  PBIST_RAMGROUP_18_MIBADC2),
 *                                  PBISTALGO_MARCH13N_RED_2PORT);
 *        while (TRUE != SL_SelfTest_Status_PBIST(&pbistStatus));
 *          retVal = SL_SelfTest_PBIST_StopExec();
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso        @ref SL_SelfTest_PBIST & @ref SL_SelfTest_PBIST_StopExec
 *
 * \if destagenabled
 * @destag      DES_11
 * @reqtag        SL_SR63,SL_SR64,SL_SR89,SL_SR90,SL_SR108,SL_SR109,SL_SR122,SL_SR123,SL_SR132,SL_SR133,SL_SR142,SL_SR143,SL_SR163,SL_SR164,SL_SR188,SL_SR189,SL_SR197,SL_SR198,SCL_49,SCL_50,SCL_79,SCL_80,SCL_158,SCL_159,SCL_180,SCL_181,SCL_197,SCL_198,SCL_210,SCL_211,SCL_237,SCL_238,SCL_262,SCL_263,SCL_275,SCL_276,SCL_290,SCL_291
 * \endif
 */
boolean SL_SelfTest_Status_PBIST(SL_PBIST_FailInfo * param1);

/**
 * @brief       Provides information regarding whether PBIST is active or not
 *
 * \if imageenabled
 * @image html  SL_SelfTest_WaitCompletion_PBIST.jpg
 * @image rtf  SL_SelfTest_WaitCompletion_PBIST.jpg
 * @image latex SL_SelfTest_WaitCompletion_PBIST.eps
 * \endif
 *
 * @param [out]    None
 * @param [in]     Pointer to structure that is used to return the test status & results.
 *
 * @return      TRUE if PBIST is running(wait for completion)
 *              FALSE if PBIST is not running
 *
 * Example Usage:
 * @code
 *        boolean retVal;
 *      retVal = SL_SelfTest_PBIST( PBIST_EXECUTE,
 *                                 (PBIST_RAMGROUP_07_MIBSPI1  |
 *                                  PBIST_RAMGROUP_08_MIBSPI3   |
 *                                  PBIST_RAMGROUP_09_MIBSPI5   |
 *                                  PBIST_RAMGROUP_10_VIM       |
 *                                  PBIST_RAMGROUP_11_MIBADC1   |
 *                                  PBIST_RAMGROUP_18_MIBADC2),
 *                                  PBISTALGO_MARCH13N_RED_2PORT);
 *        while (TRUE != SL_SelfTest_WaitCompletion_PBIST(&pbistStatus));
 *          retVal = SL_SelfTest_Status_PBIST(pbistfailinfo);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso        @ref SL_SelfTest_PBIST
 *
 * \if destagenabled
 * @destag      DES_12
 * @reqtag        SL_SR63,SL_SR64,SL_SR8,SL_SR90,SL_SR108,SL_SR109,SL_SR122,SL_SR123,SL_SR132,SL_SR133,SL_SR142,SL_SR143,SL_SR163,SL_SR164,SL_SR188,SL_SR189,SL_SR197,SL_SR198,SCL_49,SCL_50,SCL_79,SCL_80,SCL_158,SCL_159,SCL_180,SCL_181,SCL_197,SCL_198,SCL_210,SCL_211,SCL_237,SCL_238,SCL_262,SCL_263,SCL_275,SCL_276,SCL_290,SCL_291
 * \endif
 *
 */
boolean SL_SelfTest_WaitCompletion_PBIST(void);

/**
 * @brief       This function stops the PBIST controller from executing further
 *
 * \if imageenabled
 * @image html  SL_SelfTest_PBIST_StopExec.jpg
 * @image rtf  SL_SelfTest_PBIST_StopExec.jpg
 * @image latex  SL_SelfTest_PBIST_StopExec.eps
 * \endif
 *
 * @param [out]    None
 * @param [in]     None
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *        boolean retVal;
 *      retVal = SL_SelfTest_PBIST( PBIST_EXECUTE,
 *                                 (PBIST_RAMGROUP_07_MIBSPI1  |
 *                                  PBIST_RAMGROUP_08_MIBSPI3   |
 *                                  PBIST_RAMGROUP_09_MIBSPI5   |
 *                                  PBIST_RAMGROUP_10_VIM       |
 *                                  PBIST_RAMGROUP_11_MIBADC1   |
 *                                  PBIST_RAMGROUP_18_MIBADC2),
 *                                  PBISTALGO_MARCH13N_RED_2PORT);
 *        while (TRUE != SL_SelfTest_PBIST_ExecStatus(&pbistStatus));
 *          retVal = SL_SelfTest_PBIST_StopExec();
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso        @ref SL_SelfTest_PBIST & @ref SL_SelfTest_PBIST_StopExec
 *
 * \if destagenabled
 * @destag      DES_13
 * @reqtag        SL_SR63,SL_SR64,SL_SR89,SL_SR90,SL_SR108,SL_SR109,SL_SR122,SL_SR123,SL_SR132,SL_SR133,SL_SR142,SL_SR163,SL_SR164,SL_SR188,SL_SR189,SL_SR197,SL_SR198,SCL_49,SCL_50,SCL_79,SCL_80,SCL_158,SCL_159,SCL_180,SCL_181,SCL_197,SCL_198,SCL_210,SCL_211,SCL_237,SCL_238,SCL_262,SCL_263,SCL_275,SCL_276,SCL_290,SCL_291
 * \endif
 *
 */
boolean SL_SelfTest_PBIST_StopExec(void);

/**
 * @brief       Provides interface to run PSCON self tests
 *
 * @description contains the logic to place the PSCON diagnostic compare block in a self-test mode in order\n
 * to test the * failsafe feature. Three diagnostic features are supported by this API.\n
 *
 * - **Self-Test Mode :** The compare error signal output is disabled in self-test mode. The PSCON\n
 *                      diagnostic compare block generates two types of patterns during self-test mode:\n
 *                      compare match test followed by compare mismatch test. During the self-test,\n
 *                      each test pattern is applied on both PSCON signal ports of the PSCON diagnostic\n
 *                      compare block and then is clocked for one cycle.
 *
 * - **Error Forcing Mode :** This mode is designed specifically to ensure that the error signal output\n
 *                      from the PSCON compare block is not stuck inactive. In this mode, a test pattern\n
 *                      is applied to the PSCON related inputs of the compare logic to force an error.
 *
 * - **Self-test Error Forcing Mode :** In this mode, an error is forced so that the self-test error output\n
 *                      from the PSCON compare block is activated.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html  SL_SelfTest_PSCON.jpg
 * @image rtf  SL_SelfTest_PSCON.jpg
 * @image latex  SL_SelfTest_PSCON.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                            |
 * |----------------------------------------------------|
 * |@ref PSCON_SELF_TEST                                |
 * |@ref PSCON_ERROR_FORCING        |
 * |@ref PSCON_SELF_TEST_ERROR_FORCING        |
 * |@ref PSCON_PMA_TEST        |
 * |@ref PSCON_ERROR_FORCING_FAULT_INJECT        |
 * |@ref PSCON_SELF_TEST_ERROR_FORCING_FAULT_INJECT        |
 *
 * @param [in]     mode - When TRUE the function behaves synchronous (waits for test to complete & returns status)
 *                      When FALSE the test is started and user has to use the SL_SelfTest_PSCON_Status to
 *                      check the status & results.\n
 *                  **Note: Only self-test (#1) can be run in async mode (mode=FALSE). Rest of the tests are always synchronous**
 * @param [out] param1 - Pointer to structure that is used to return the test status & results.\n
 *                  **Note: param1.pdFailure & param1.mdFailure are valid only for self-test mode.**
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *         SL_PSCON_FailInfo failInfoPSCON;
 *        boolean retVal;
 *         retVal = SL_SelfTest_PSCON(PSCON_SELF_TEST, TRUE, &failInfoPSCON);
 *      retVal = SL_SelfTest_PBIST_StopExec();
 *
 *      if ((TRUE == retVal) && (ST_PASS == failInfoPSCON.stResult)) {
 *          //test passed
 *      } else {
 *          //test failed
 *      }
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso        @ref SL_SelfTest_PSCON_Status
 *
 *
 * \if profilingenabled
 * @profiling
 *
 * TestType                       | Profiling Time in us
 * -------------------------------| -------------
 * PSCON_SELF_TEST                | 2.2585
 * PSCON_ERROR_FORCING            | 1.741
 * PSCON_SELF_TEST_ERROR_FORCING  | 1.757
 * PSCON_PMA_TEST                 | 0.9365
 * \endif
 *
 * \if destagenabled
 * @destag        DES_14
 * @reqtag        SL_SR3, SL_SR4, SLC_8
 * \endif
 */
boolean SL_SelfTest_PSCON (SL_SelfTestType testType, boolean mode, SL_PSCON_FailInfo * param1);



/**
 * @brief       Returns the status for previous PSCON self-test execution\n
 *
 * **Note: Does not support status checking of error-forcing modes since these are synchronous in nature.**
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html  SL_SelfTest_Status_PSCON.jpg
 * @image rtf  SL_SelfTest_Status_PSCON.jpg
 * @image latex  SL_SelfTest_Status_PSCON.eps
 * \endif
 *
 * @param [out] param1 - Pointer to structure that is used to return the test status & results.\n
 * **Note: contents of structure param1 is valid only for self-test.Not valid for Fault Injection mode.**
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *      boolean retVal;
 *      while (TRUE != SL_SelfTest_PBIST_ExecStatus(&pbistStatus));
 *      retVal = SL_SelfTest_PBIST_StopExec();
 *      if (TRUE == pbistStatus.pbistStatus) {
 *          //test passed
 *      } else {
 *          //test failed
 *      }
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso     @ref SL_SelfTest_PSCON
 *
 * \if destagenabled
 * @destag        DES_15
 * @reqtag        SL_SR3,SL_SR4, SLC_8
 * \endif
 */
boolean SL_SelfTest_Status_PSCON (SL_PSCON_FailInfo * param1);




/**
 * @brief       Executes CPU Logic Built-In Self Test using Self-Test Controller
 *
 * @description The Hercules family architecture supports the use of a hardware logic BIST (LBIST) engine self-test\n
 * controller (STC). This logic is used to provide a very high diagnostic coverage on the lockstep CPUs\n
 * at a transistor level. This logic utilizes the same design for test (DFT) structures inserted into\n
 * the device for rapid execution of high quality manufacturing tests, but with an internal test engine\n
 * rather than external automated test equipment (ATE).\n
 * This function triggers the LBIST using the user provided parameters, thus providing runtime diagnostics\n
 * with execution of test time slices per safety critical loop as well as comprehensive CPU test during\n
 * initialization.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 *
 * \if imageenabled
 * @image html  SL_SelfTest_STC.jpg
 * @image rtf  SL_SelfTest_STC.jpg
 * @image latex  SL_SelfTest_STC.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                            |
 * |----------------------------------------------------|
 * |@ref STC_RUN (only for R4 devices)        |
 * |@ref STC_COMPARE_SELFCHECK (only for R4 devices)        |
 * |@ref STC1_RUN (only for R5 devices)        |
 * |@ref STC1_COMPARE_SELFCHECK (only for R5 devices)        |
 *
 *
 * @param [in]  bMode - NOT USED
 * @param [in]  config - Pointer to structure containing the configuration items. Refer to SL_STC_Config
 *                          for details of configurable items.
 * @return      FALSE - If the function failed to execute STC - the function terminated early from a parameter check error.
 * @return     TRUE - If the function failed to execute STC - parameters were good  but the WFI did not initiate LBIST.
 *
 * **Note: Function DOES NOT RETURN if STC was started**
 *
 * **Note: At the end of the STC execution a CPU reset is issued by the STC controller. So the application
 * startup must ensure that the STC is not-executed when reset type is RESET_TYPE_CPU & when the debugger
 * is connected.**
 *
 * Example Usage:
 * @code
 *      // Start if debugger not connected & reset type is not CPU or DEBUGGER
 *      if ((RESET_TYPE_CPU != resetReason) && (RESET_TYPE_DEBUG != resetReason)) {
 *          stcSelfTestConfig.stcClockDiv       = 0;            // STC Clock divider = 1
 *          stcSelfTestConfig.intervalCount     = 1;            // One interval only
 *          stcSelfTestConfig.restartInterval0  = TRUE;         // Start from interval 0
 *          stcSelfTestConfig.timeoutCounter    = 0xFFFFFFFF;   // Timeout counter
 *          SL_SelfTest_STC(STC_COMPARE_SELFCHECK, TRUE, &stcSelfTestConfig);
 *      }
 *      // If STC was executed, get STC self-test status
 *      if (RESET_TYPE_CPU == resetReason) {
 *          retVal = SL_SelfTest_Status_STC (&failInfoSTC);
 *      }
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    If STC executes, CPU Reset is issued at the end of STC execution, else same as entry mode.
 *
 * @seealso     @ref SL_SelfTest_Status_STC
 *
 * \if destagenabled
 * @destag        DES_16
 * @reqtag        SL_SR33,SL_SR34,SLC_37,SLC_38
 * \endif
 */
boolean SL_SelfTest_STC(register SL_SelfTestType testType, register boolean bMode, register SL_STC_Config * config);



/**
 * @brief       Returns the status of previously executed CPU self-test via STC
 *
 * \if imageenabled
 * @image html  SL_SelfTest_Status_STC.jpg
 * @image rtf  SL_SelfTest_Status_STC.jpg
 * @image latex  SL_SelfTest_Status_STC.eps
 * \endif
 *
 * @param [in]  failinfostc - Pointer to SL_STC_FailInfo. For more details on the return values refer to\n
 *              SL_STC_FailInfo structure documentation.
 * @param [out] None
 *
 * @return      TRUE - If the self test was completed, else FALSE (a failure occurred during self test).
 *
 * Example Usage:
 * @code
 *      // Start if debugger not connected & reset type is not CPU or DEBUGGER
 *      if ((RESET_TYPE_CPU != resetReason) && (RESET_TYPE_DEBUG != resetReason)) {
 *          stcSelfTestConfig.stcClockDiv       = 0;            // STC Clock divider = 1
 *          stcSelfTestConfig.intervalCount     = 1;            // One interval only
 *          stcSelfTestConfig.restartInterval0  = TRUE;         // Start from interval 0
 *          stcSelfTestConfig.timeoutCounter    = 0xFFFFFFFF;   // Timeout counter
 *          SL_SelfTest_STC(STC_COMPARE_SELFCHECK, TRUE, &stcSelfTestConfig);
 *      }
 *      // If STC was executed, get STC self-test status
 *      if (RESET_TYPE_CPU == resetReason) {
 *          retVal = SL_SelfTest_Status_STC (&failInfoSTC);
 *      }
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    SVC Mode
 *
 * @seealso     @ref SL_SelfTest_STC
 *
 * \if destagenabled
 * @destag        DES_17
 * @reqtag        SL_SR33,SL_SR34,SLC_37,SLC_38
 * \endif
 */
boolean SL_SelfTest_Status_STC (SL_STC_FailInfo * failInfostc);


/**
 * @brief       This function executes self tests on the eFuse structures.
 *
 * @description The Hercules platform devices support a manufacturing time configuration of certain functionality via a\n
 * one time programmable EFuse structures. The EFuses are read automatically after POR by an autoload function.\n
 * This function executes the diagnostic tests on detect incorrect configuration values.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html  SL_SelfTest_EFuse.jpg
 * @image rtf  SL_SelfTest_EFuse.jpg
 * @image latex  SL_SelfTest_EFuse.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                            |
 * |------------------------------------|
 * |@ref EFUSE_SELF_TEST_ECC        |
 * |@ref EFUSE_SELF_TEST_STUCK_AT_ZERO        |
 *
 * @param [in]  bMode - When TRUE function will wait for the test to complete (synchronous). Function
 *              will be async when FALSE & the use can must use SL_SelfTest_Status_EFuse to retrieve the result.
 * @param [in]  config - Configuration parameters for the self tests. Refer to SL_EFuse_Config structure
 *              documentation for details of each configuration parameter.
 *
 * @return      TRUE if the tests were executed else FALSE. (Does not indicate the test result).
 *
 * Example Usage:
 * @code
 *      // Run EFuse self tests
 *      stConfigEFuse.numPatterns      = 600u;
 *      stConfigEFuse.seedSignature    = 0x5362F97Fu;
 *      stConfigEFuse.failInfo.stResult= ST_FAIL;
 *      stConfigEFuse.failInfo.failInfo= EFUSE_ERROR_NONE;
 *      retVal = SL_SelfTest_EFuse(EFUSE_SELF_TEST_STUCK_AT_ZERO, TRUE, &stConfigEFuse);
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    SVC Mode
 *
 * @seealso     @ref SL_SelfTest_Status_EFuse
 *
 *
 *
 * \if profilingenabled
 * @profiling
 *
 * TestType                       | Profiling Time in us
 * -------------------------------| -------------
 * EFUSE_SELF_TEST_ECC            | 6.438
 * EFUSE_SELF_TEST_STUCK_AT_ZERO  | 5.402
 *
 * \endif
 * \if destagenabled
 * @destag        DES_18
 * @reqtag        SL_SR79,SL_SR80,SLC_143,SLC_142
 * \endif
 */
boolean SL_SelfTest_EFuse (SL_SelfTestType testType, boolean bMode, SL_EFuse_Config * config);


/**
 * @brief       Retrieves the status of the previous EFuse self-test execution.
 *
 * \if imageenabled
 * @image html  SL_SelfTest_Status_EFuse.jpg
 * @image rtf  SL_SelfTest_Status_EFuse.jpg
 * @image latex  SL_SelfTest_Status_EFuse.eps
 * \endif
 *
 * @param [in]  failInfo - Pointer to structure that will be used to return the execution results.
 *              For details on structure member refer to documentation on SL_EFuse_FailInfo documentation.
 * @param [out] None
 *
 * @return      TRUE if the self-test was completed. Else FALSE (self-test is still running).
 *
 * Example Usage:
 * @code
 *      SL_SelfTest_Status_EFuse(&failInfo);
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    SVC Mode
 *
 * @seealso     @ref SL_SelfTest_EFuse
 *
 * \if destagenabled
 * @destag      DES_19
 * @reqtag        SL_SR79,SL_SR80,SLC_143,SLC_142
 * \endif
 */
boolean SL_SelfTest_Status_EFuse (SL_EFuse_FailInfo * failInfoefuse);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief Executes self-tests on Core Compare Module (CCM).
 *
 * @description The Hercules product family includes a lockstep processor diagnostic. This feature includes the\n
 * addition of a diagnostic Cortex-R4F CPU that is combined into a 1oo1D (single channel with diagnostic\n
 * channel) configuration with the application CPU. The core compare module (CCM) compares the CPU\n
 * outputs and flags all mis-compares to the ESM.\n
 * The CCM logic provides self test and error forcing capability via software triggered hardware.\n
 * The self test ensures that the CCM compare logic is working properly. The error forcing capability\n
 * allows you to test the system level response to a lockstep miscompare.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType
 *
 * \if imageenabled
 * @image html  SL_SelfTest_CCMR4F.jpg
 * @image rtf  SL_SelfTest_CCMR4F.jpg
 * @image latex  SL_SelfTest_CCMR4F.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                            |
 * |----------------------------------------------------|
 * |@ref CCMR4F_SELF_TEST        |
 * |@ref CCMR4F_ERROR_FORCING_TEST        |
 * |@ref CCMR4F_ERROR_FORCING_TEST_FAULT_INJECT        |
 * |@ref CCMR4F_SELF_TEST_ERROR_FORCING        |
 *
 * @param [in] bMode -     TRUE for synchronous mode & FALSE for async mode
 *                          Parameter is valid only for CCMR4F_SELF_TEST selftest type.
 * @param [in] config -    Structure used for returning the test results. For details refer to
 *                          SL_CCMR4F_FailInfo documentation.**Note: contents of structure param1 is valid only for self-test.Not valid for Fault Injection mode.**
 *
 * @return     TRUE if the test was executed else FALSE
 *
 * Example Usage:
 * @code
 *      SL_CCMR4F_FailInfo failInfoCCMR4F;
 *      retVal = SL_SelfTest_CCMR4F(CCMR4F_SELF_TEST, TRUE, &failInfoCCMR4F);
 *      if (failInfoCCMR4F.stResult == ST_FAIL) {
 *          // Self test failed.. for details of failure info read failInfoCCMR4F.failInfo
 *      }
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    SVC Mode
 *
 * @seealso     @ref SL_SelfTest_Status_CCMR4F
 *
 *
 * \if profilingenabled
 * @profiling
 *
 * TestType                       | Profiling Time in us
 * -------------------------------| -------------
 * CCMR4F_SELF_TEST               | 10.405
 * CCMR4F_ERROR_FORCING_TEST      | 7.556
 * CCMR4F_SELF_TEST_ERROR_FORCING | 5.04
 *
 * \endif
 * \if destagenabled
 * @destag        DES_20
 * @reqtag        SL_SR32
 * \endif
 */
boolean SL_SelfTest_CCMR4F (SL_SelfTestType testType, boolean bMode, SL_CCMR4F_FailInfo * config);
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
/**
 * @brief Executes self-tests on Core Compare Module (CCM).
 *
 * @description The Hercules product family includes a lockstep processor diagnostic. This feature includes the\n
 * addition of a diagnostic Cortex-R5F CPU that is combined into a 1oo1D (single channel with diagnostic\n
 * channel) configuration with the application CPU. The core compare module (CCM) compares the CPU\n
 * outputs and flags all mis-compares to the ESM.\n
 * The CCM logic provides self test and error forcing capability via software triggered hardware.\n
 * The self test ensures that the CCM compare logic is working properly. The error forcing capability\n
 * allows you to test the system level response to a lockstep miscompare.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType
 *
 * \if imageenabled
 * @image html  SL_SelfTest_CCMR4F.jpg
 * @image rtf  SL_SelfTest_CCMR4F.jpg
 * @image latex  SL_SelfTest_CCMR4F.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                                    |
 * |------------------------------------------------------------|
 * |@ref CCMR5F_CPUCOMP_SELF_TEST        |
 * |@ref CCMR5F_CPUCOMP_ERROR_FORCING_TEST        |
 * |@ref CCMR5F_CPUCOMP_ERROR_FORCING_TEST_FAULT_INJECT        |
 * |@ref CCMR5F_CPUCOMP_SELF_TEST_ERROR_FORCING        |
 * |@ref CCMR5F_VIMCOMP_SELF_TEST        |
 * |@ref CCMR5F_VIMCOMP_ERROR_FORCING_TEST        |
 * |@ref CCMR5F_VIMCOMP_ERROR_FORCING_TEST_FAULT_INJECT        |
 * |@ref CCMR5F_VIMCOMP_SELF_TEST_ERROR_FORCING        |
 * |@ref CCMR5F_PDCOMP_SELF_TEST        |
 * |@ref CCMR5F_PDCOMP_ERROR_FORCING_TEST        |
 * |@ref CCMR5F_PDCOMP_ERROR_FORCING_TEST_FAULT_INJECT        |
 * |@ref CCMR5F_PDCOMP_SELF_TEST_ERROR_FORCING        |
 * |@ref CCMR5F_INMCOMP_SELF_TEST        |
 * |@ref CCMR5F_INMCOMP_ERROR_FORCING_TEST        |
 * |@ref CCMR5F_INMCOMP_ERROR_FORCING_TEST_FAULT_INJECT        |
 * |@ref CCMR5F_INMCOMP_SELF_TEST_ERROR_FORCING        |
 *
 *
 * @param [in] bMode -     TRUE for synchronous mode & FALSE for async mode
 *                          Parameter is valid only for CCMR5F_SELF_TEST selftest type.
 * @param [in] config -    Structure used for returning the test results. For details refer to
 *                          SL_CCMR5F_FailInfo documentation.**Note: contents of structure param1 is valid only for self-test.Not valid for Fault Injection mode.**
 *
 * @return     TRUE if the test was executed else FALSE
 *
 * Example Usage:
 * @code
 *      SL_CCMR5F_FailInfo failInfoCCMR5F;
 *      retVal = SL_SelfTest_CCMR5F(CCMR5F_SELF_TEST, TRUE, &failInfoCCMR5F);
 *      if (failInfoCCMR5F.stResult == ST_FAIL) {
 *          // Self test failed.. for details of failure info read failInfoCCMR5F.failInfo
 *      }
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    SVC Mode
 *
 * @seealso     @ref SL_SelfTest_Status_CCMR5F
 *
 *
 * \if profilingenabled
 * @profiling
 *
 * TestType                       | Profiling Time in us
 * -------------------------------| -------------
 * CCMR4F_SELF_TEST               | 10.405
 * CCMR4F_ERROR_FORCING_TEST      | 7.556
 * CCMR4F_SELF_TEST_ERROR_FORCING | 5.04
 *
 * \endif
 * \if destagenabled
 * @destag        DES_20
 * @reqtag        SLC_46,SLC_9
 * \endif
 */
boolean SL_SelfTest_CCMR5F (SL_SelfTestType testType, boolean bMode, SL_CCMR5F_FailInfo * config);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Returns the self test result for the previous CCRM4F selftest execution(SL_SelfTest_CCMR4F).To be used whenever the API SL_SelfTest_CCMR4F\n
 * is called in asynchronous mode i.e. when the parameter  bMode of SL_SelfTest_CCMR4F is FALSE.
 *
 * The CCMR4F selftest API SL_SelfTest_CCMR4F can run in asynchronous and in synchronous mode for the testType\n
 * CCMR4F_SELF_TEST.Whenever the API SL_SelfTest_CCMR4F is called in asynchronous mode the results of the previous CCMR4F\n
 * selftest can be accessed or got by calling this function.
 *
 * \if imageenabled
 * @image html  SL_SelfTest_Status_CCMR4F.jpg
 * @image rtf  SL_SelfTest_Status_CCMR4F.jpg
 * @image latex SL_SelfTest_Status_CCMR4F.eps
 * \endif
 *
 * @param [in]  failInfo - Pointer to structure used for returning the test result.\n
 * **Note: contents of structure param1 is valid only for self-test.Not valid for Fault Injection mode.**
 * @param [out] None
 *
 * @return      TRUE indicates that the test was complete and failInfo contents are valid. .
 *              FALSE indicates that the test is still executing.
 *
 * Example Usage:
 * @code
 *              SL_CCMR4F_FailInfo failInfoCCMR4F;
 *              retVal = SL_SelfTest_CCMR4F(CCMR4F_SELF_TEST, FALSE, &failInfoCCMR4F);
 *              while (TRUE == SL_SelfTest_CCMR4F(&faileInfoCCMR4F));
 *              if (failInfoCCMR4F.stResult == ST_FAIL) {
 *                  // Self test failed.. for details of failure info read failInfoCCMR4F.failInfo
 *              }
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    Same as entry
 *
 * @seealso     @ref SL_SelfTest_CCMR4F
 *
 * **Note : This API is only valid when SL_SelfTest_CCMR4F is called in async mode(with bMode=FALSE)**
 *
 * \if destagenabled
 *@destag       DES_21
 *@reqtag        SL_SR32
 *\endif
 */
boolean SL_SelfTest_Status_CCMR4F (SL_CCMR4F_FailInfo * failInfoccmr4f);
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
/**
 * @brief       Returns the self test result for the previous CCRM4F selftest execution(SL_SelfTest_CCMR5F).To be used whenever the API SL_SelfTest_CCMR5F\n
 * is called in asynchronous mode i.e. when the parameter  bMode of SL_SelfTest_CCMR5F is FALSE.
 *
 * The CCMR5F selftest API SL_SelfTest_CCMR5F can run in asynchronous and in synchronous mode for the testType\n
 * CCMR5F_CPUCOMP_SELF_TEST.Whenever the API SL_SelfTest_CCMR5F is called in
 * asynchronous mode the results of the previous CCMR5F\n
 * selftest can be accessed or got by calling this function.
 *
 * \if imageenabled
 * @image html  SL_SelfTest_Status_CCMR5F.jpg
 * @image rtf  SL_SelfTest_Status_CCMR5F.jpg
 * @image latex SL_SelfTest_Status_CCMR5F.eps
 * \endif
 *
 * @param [in]  failInfo - Pointer to structure used for returning the test result.\n
 * **Note: contents of structure param1 is valid only for self-test.Not valid for Fault Injection mode.**
 * @param [out] None
 *
 * @return      TRUE indicates that the test was complete and failInfo contents are valid. .
 *              FALSE indicates that the test is still executing.
 *
 * Example Usage:
 * @code
 *              SL_CCMR5F_FailInfo failInfoCCMR5F;
 *              retVal = SL_SelfTest_CCMR5F(CCMR5F_CPUCOMP_SELF_TEST, FALSE,
 *              &failInfoCCMR5F);
 *              while (TRUE == SL_SelfTest_CCMR5F(&faileInfoCCMR5F));
 *              if (failInfoCCMR5F.stResult == ST_FAIL) {
 *                  // Self test failed.. for details of failure info read
 *                  failInfoCCMR5F.failInfo
 *              }
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    Same as entry
 *
 * @seealso     @ref SL_SelfTest_CCMR5F
 *
 * **Note : This API is only valid when SL_SelfTest_CCMR5F is called in async mode(with bMode=FALSE)**
 *
 * \if destagenabled
 *@destag       DES_21
 *@reqtag       SLC_46,SLC_9
 *\endif
 */
boolean SL_SelfTest_Status_CCMR5F (SL_SelfTestType diagnostic, SL_CCMR5F_FailInfo * failInfoccmr5f);
#endif


/**
 * @brief       This function generates an internal warm reset(nRST). This is done by writing appropriate\n
 *              control bits in the SYSECR control register.
 *
 * \if imageenabled
 * @image html  SL_SW_Reset.jpg
 * @image rtf  SL_SW_Reset.jpg
 * @image latex  SL_SW_Reset.eps
 * \endif
 *
 * @param [in]  None
 * @param [out] None
 *
 * @return      None
 *
 * Example Usage:
 * @code
 *      SL_SW_Reset();
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    Not Applicable (since the device resets)
 *
 * @seealso     @ref SL_Init_ResetReason & @ref SL_Init_ResetReason_XInfo
 *
 * \if destagenabled
 * @destag        DES_22
 * @reqtag        SL_SR18,SLC_21
 * \endif
 */
void SL_SW_Reset(void);



/**
 * @brief       This function calculates the CRC of specified memory (& length) using the hardware CRC implementing the ISO CRC-64 standard polynomial.
 *
 *  @description This API can be used to test the integrity of memory contents by calculating a CRC for\n
 *  the memory and comparing this value to a previously generated golden CRC.
 *
 * \if imageenabled
 * @image html  SL_CRC_Calculate.jpg
 * @image rtf  SL_CRC_Calculate.jpg
 * @image latex  SL_CRC_Calculate.eps
 * \endif
 *
 *
 * @param [in]  startAddr - Starting address of the memory (aligned to 64bit address boundary).
 * @param [in]  count64 - Length of the memory (for calculating CRC) in multiple of 64bits.
 *
 * @return      Calculated CRC value
 *
 * Example Usage:
 * @code
 *              //Calculate the CRC for VIM RAM
 *              vimCRC = SL_CRC_Calculate(((uint64 *)0xFFF82000), 256);
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag      DES_23
 * @reqtag        SL_SR44,SL_SR45,SL_SR51,SL_SR52,SL_SR66,SL_SR92,SL_SR111,SL_SR145,SLC_57,SLC_58,SLC_67,SLC_68,SLC_82,SLC_161,SLC_183,SLC_200,SLC_213,SLC_240,SLC_265,SLC_278,SLC_293,SLC_298,SLC_299
 * \endif
 *
 *
 *
 */
uint64 SL_CRC_Calculate(uint64* startAddr, const uint32 count64);



/**
 * @brief       Initializes the ESM module handing in the SafeTI&trade; Hercules&trade; Diagnostic Library.
 *
 * @description Once initialized all ESM notifications are decoded & 'appCallback' is called with the\n
 * parameters indicating the ESM failure information. For details on the failure information,\n
 * provided by this API, refer to documentation on ESM_Application_Callback.
 *
 *
 * \if imageenabled
 * @image html  SL_ESM_Init.jpg
 * @image rtf  SL_ESM_Init.jpg
 * @image latex  SL_ESM_Init.eps
 * \endif
 *
 * @param [in]  appCallback - Application callback to be called for ESM notifications.
 * @param [out] None
 *
 * @return      None
 *
 * Example Usage:
 * @code
 * void ESM_Callback(uint32 channel, uint32 param1, uint32 param2, uint32 param3);
 * void ESM_Callback(uint32 channel, uint32 param1, uint32 param2, uint32 param3)
 * {
 *  Code for handling the ESM errors
 * }
 * SL_ESM_Init(ESM_Callback);
 * @endcode
 *
 * @entrymode   Any Privilege Mode
 * @exitmode    Same as at entry
 *
 * @seealso     @ref ESM_Application_Callback
 * \if destagenabled
 * @destag        DES_24
 * @reqtag        SL_SR213
 * \endif
 */
void SL_ESM_Init(ESM_Application_Callback appCallback);

/**
 * @brief       It does the selftest on the required adc pin and provides the information on status of the Pin.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * Below is the flowchart for the ADC self test
 * \if imageenabled
 * @image html  SL_SelfTest_ADC.jpg
 * @image rtf  SL_SelfTest_ADC.jpg
 * @image latex SL_SelfTest_ADC.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                    |
 * |----------------------------|
 * |@ref ADC_SELFTEST_ALL        |
 * |@ref ADC_SRAM_PARITY_TEST        |
 *
 * @param [in]  mode -     None  Only Synchronous mode is supported.
 * @param [in]  config - Pointer to structure that contains ADC base address and the Channel number.
 * @param [out] pinstatus - pointer to  SL_ADC_Pinstatus it is output parameter and stores the state of the adc pin tested.
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * **NOTE: The application must ensure that the sampling time of ADC event group is set to 1us before calling this API**
 *
 * Example Usage:
 * @code
 *      //use the channel 7 and adc1
 *       adcConfig.adc_channel = 7;
 *       adcConfig.adcbase = adcREG1;
 *       //pin is not connected to any analog input
 *       if( TRUE == SL_SefTest_ADC(ADC_SELFTEST_ALL, FALSE, &adcConfig, &adc_pinstatus)) {
 *           if (ADC_PIN_GOOD == adc_pinstatus) {
 *               //testcase pass
 *           } else {
 *               //testcase fail
 *           }
 *       } else {
 *            //testcase fail
 *       }
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * @seealso     @ref _SL_ADC_Config, @ref _SL_ADC_Pinstatus
 *
 *
 * \if destagenabled
 * @destag        DES_25
 * @reqtag        SL_SR127, SL_SR131, SLC_209
 * \endif
 */
boolean SL_SelfTest_ADC(SL_SelfTestType testType, boolean mode, SL_ADC_Config * config, SL_ADC_Pinstatus * pinstatus);



/**
 * @brief       Function clears the nERROR (de-activates) signal.
 *
 * **Note: Application must use the SL_ESM_nERROR_Active to wait for the signal to go in-active.**\n
 * **Note: The nERROR pin can remain active even after the function call and the duration for which it can be**\n
 * **active depends on the LTCRP register Low-Time Counter Preload register (LTCRP).**
 *
 * \if imageenabled
 * @image html  SL_Clear_nERROR.jpg
 * @image rtf  SL_Clear_nERROR.jpg
 * @image latex  SL_Clear_nERROR.eps
 * \endif
 *
 * @param [in]  None
 * @param [out] None
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as at entry
 *
 * @seealso     @ref SL_ESM_nERROR_Active
 *
 * \if destagenabled
 * @destag        DES_26
 * @reqtag
 * \endif
 */
void SL_Clear_nERROR(void);



/**
 * @brief       Function sets the nERROR pin active.
 *
 * \if imageenabled
 * @image html  SL_Set_nERROR.jpg
 * @image rtf  SL_Set_nERROR.jpg
 * @image latex  SL_Set_nERROR.eps
 * \endif
 *
 * @param [in]  None
 * @param [out] None
 *
 * @return      None
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as at entry
 *
 *
 * \if destagenabled
 * @destag        DES_27
 * @reqtag
 * \endif
 */
void SL_Set_nERROR(void);



/**
 * @brief       Function returns the status of the nERRORpin.
 *
 * \if imageenabled
 * @image html  SL_ESM_nERROR_Active.jpg
 * @image rtf  SL_ESM_nERROR_Active.jpg
 * @image latex  SL_ESM_nERROR_Active.eps
 * \endif
 *
 * @param [in]  None
 * @param [out] None
 *
 * @return      TRUE if nERROR pin is active (low) else FALSE.
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as at entry
 *
 *
 * \if destagenabled
 * @destag        DES_28
 * @reqtag
 * \endif
 */
boolean SL_ESM_nERROR_Active(void);



/**
 * @brief       Function is used to return extended information on the reset reason.
 *
 * **Note: Refer to section on Reset Behavior in the TRM.**
 *
 * \if imageenabled
 * @image html  SL_Init_ResetReason_XInfo.jpg
 * @image rtf  SL_Init_ResetReason_XInfo.jpg
 * @image latex  SL_Init_ResetReason_XInfo.eps
 * \endif
 *
 * @param [in]  rrXInfo - structure which holds the additional information on the reset reason.
 * @param [out] None
 *
 * @return      None
 *
 * @entrymode   Can be called in any operating mode of CPU.
 * @exitmode    No change in the operating mode.
 *
 * **Note: This function clears the ESM Group2 Shadow register (ESMSSR2).**
 *
 * @seealso     @ref _SL_ResetReason_XInfo
 *
 * \if destagenabled
 * @destag        DES_29
 * @reqtag        SL_SR17,SL_SR20,SL_SR30, SLC_23, SLC_34
 * \endif
 */
boolean SL_Init_ResetReason_XInfo (SL_ResetReason_XInfo * rrXInfo);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       It does the Fault injection on L2(Switched central resource) and L3(Peripheral central resource) interconnects.\n
 * Below is the flowchart for the SL_SelfTestL2L3Interconnect.
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html  SL_L2L3Interconnect_FI.jpg
 * @image rtf  SL_L2L3Interconnect_FI.jpg
 * @image latex SL_L2L3Interconnect_FI.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                    |
 * |--------------------------------------------|
 * |@ref L2L3INTERCONNECT_RESERVED_ACCESS        |
 * |@ref L2L3INTERCONNECT_UNPRIVELEGED_ACCESS        |
 *
 * @param [in]    location - Specifies the address of location to be used for test\n
 *
 * @param [in]    protsetreg - The pointer to PPROTSET register which holds the protection bit for location used\n
 *                              Applicable only for interconnect unpriveleged access\n
 *
 * @param [in]    protbit - The protection bit for location used, Applicable only for interconnect unpriveleged access\n
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *              //Trigger L2 interconnect reserved location error trap
 *              boolean retVal;
 *              retVal = SL_SelfTestL2L3Interconnect(L2INTERCONNECT_RESERVED_ACCESS);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 *
 * \if destagenabled
 * @destag        DES_30
 * @reqtag        SL_SR69, SL_SR76, SL_SR77
 * \endif
 */
boolean SL_SelfTestL2L3Interconnect(SL_SelfTestType testType, volatile uint32* location, volatile uint32* protsetreg, uint32 protbit);
#endif


/**
 * @brief  Computes offset error using Calibration mode
 * Below is the flowchart for the SL_adcCalibration.
 *
 *
 * \if imageenabled
 * @image html    SL_adcCalibration.jpg
 * @image rtf    SL_adcCalibration.jpg
 * @image latex    SL_adcCalibration.eps
 * \endif
 *
 *   @param[in] adc Pointer to ADC module:
 *              - adcREG1: ADC1 module pointer
 *              - adcREG2: ADC2 module pointer
 *   @param[in] offset_error - variable used to return calculated offset error value
 *
 * @return      TRUE if success
 *                FALSE if failure
 *                offset variable is updated with the computed offset error
 *
 * Example Usage:
 * @code
 *                 //Disable Interrupts
 *                 adc->GxINTENA[sl_adcGROUP1] = 0u;
 *                 //disable any self tst and Calib mode first
 *                 adc->CALCR = 0u;
 *                 //Enable 12-BIT ADC
 *                 (adc->OPMODECR) |= ADC_12BIT_RESOLUTION;
 *                 //Disable all channels for conversion
 *                 //Event select
 *                 adc->GxSEL[sl_adcGROUP1] = 0u;
 *                 //Calibrate the ADC so as to fill appropriate value in CALR register
 *                 SL_adcCalibration(adc, offset);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 *
 * \if destagenabled
 * @destag        DES_35
 * @reqtag        SL_SR128, SL_SR129, SLC_206, SLC_207
 * \endif
 */
boolean SL_adcCalibration(adcBASE_t * adc, uint32 * offset_error);

#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
/**
 * @brief       Performs diagnostic tests on DMA (test of the SECDED ECC in DMA, test of parity logic\n
 * in DMA).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 *
 * \if imageenabled
 * @image html    SL_SelfTest_DMA.jpg
 * @image rtf    SL_SelfTest_DMA.jpg
 * @image latex    SL_SelfTest_DMA.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                                            |
 * |--------------------------------------------------------------------|
 * |@ref DMA_ECC_TEST_MODE_1BIT (only for R5 devices)        |
 * |@ref DMA_ECC_TEST_MODE_2BIT (only for R5 devices)        |
 * |@ref DMA_ECC_TEST_MODE_1BIT_FAULT_INJECT (only for R5 devices)        |
 * |@ref DMA_ECC_TEST_MODE_2BIT_FAULT_INJECT (only for R5 devices)        |
 * |@ref DMA_SOFTWARE_TEST         |
 * |@ref DMA_SRAM_PARITY_TEST (only for R4 devices)        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *              retVal = SL_SelfTest_DMA(DMA_ECC_TEST_MODE_1BIT);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_36
 * @reqtag        SL_SR105, SL_SR107, SLC_179, SLC_185, SLC_186
 * \endif
 *
 */
boolean SL_SelfTest_DMA(SL_SelfTestType testType);
#endif

/**
 * @brief       Performs diagnostic tests on VIM (test of parity logic in VIM, software test of basic\n
 * functionality of VIM).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_VIM.jpg
 * @image rtf    SL_SelfTest_VIM.jpg
 * @image latex    SL_SelfTest_VIM.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                                            |
 * |----------------------------------------------------|
 * |@ref VIM_SRAM_PARITY_TEST (only for R4 devices)        |
 * |@ref VIM_SOFTWARE_TEST        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *              retVal = SL_SelfTest_VIM(VIM_SRAM_PARITY_TEST);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_37
 * @reqtag        SL_SR88, SL_SR93, SLC_162, SLC_163
 * \endif
 *
 */
boolean SL_SelfTest_VIM(SL_SelfTestType testType);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Performs diagnostic tests on HET (test of parity logic in HET, analog loopback test\n
 * on HET signal path).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_HET.jpg
 * @image rtf    SL_SelfTest_HET.jpg
 * @image latex    SL_SelfTest_HET.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType                            |
 * |------------------------------------|
 * |@ref HET_SRAM_PARITY_TEST        |
 * |@ref HET_ANALOG_LOOPBACK_TEST        |
 *
 * @param [in]  instance Indicates the HET module instance. Correct instance value as per the device\n
 *                 variant must be passed as parameter.
 *|instance         |
 *|--------------------------------------------|
 *|SL_HET1        |
 *|SL_HET2        |
 *
 * @param [in]  pinOut Indicates the HET pin to be used as output for analog loopback test. The value\n
 * does not matter in case of parity test.\n
 * @param [in]  pinIn Indicates the HET pin to be used as input for analog loopback test. The value\n
 * does not matter in case of parity test.\n
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * NOTE : Before running loopback test, ensure correct pinmux setting and no pending transfers.\n
 *           HET_ANALOG_LOOPBACK_TEST involves reprogramming of HET RAM which can be disruptive to\n
 *           application configuration of HET RAM. Thus HET_ANALOG_LOOPBACK_TEST should preferably be\n
 *           conducted at boot time only. Moreover the lower pin in the pin pair should be even (allowed\n
 *           pairs are 0-1, 2-3, and so on).
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_HET(HET_ANALOG_LOOPBACK_TEST, SL_HET2, 5U, 4U);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_38
 * @reqtag        SL_SR119, SL_SR120, SL_SR121
 * \endif
 *
 */
boolean SL_SelfTest_HET(SL_SelfTestType testType, SL_HET_Instance instance, uint32 pinOut, uint32 pinIn);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Performs diagnostic tests on HTU (test of parity logic in HTU).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_HTU.jpg
 * @image rtf    SL_SelfTest_HTU.jpg
 * @image latex    SL_SelfTest_HTU.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType       |
 * |-----------------------------|
 * |@ref HTU_SRAM_PARITY_TEST        |
 *
 * @param [in]  instance Indicates the HTU module instance. Correct instance value as per the device\n
 *                 variant must be passed as parameter.
 * |instance :        |
 * |----------------------------|
 * |SL_HTU1        |
 * |SL_HTU2        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_HTU(HTU_SRAM_PARITY_TEST, SL_HTU2);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_39
 * @reqtag        SL_SR121
 * \endif
 *
 */
boolean SL_SelfTest_HTU(SL_SelfTestType testType, SL_HTU_Instance instance);
#endif

/**
 * @brief       Performs diagnostic tests on MIBSPI (test of parity logic in HET, analog loopback\n
 * test on MIBSPI signal path).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_MIBSPI.jpg
 * @image rtf    SL_SelfTest_MIBSPI.jpg
 * @image latex    SL_SelfTest_MIBSPI.eps
 * \endif
 *
 * @param 
 * [in]    testType - Specifies the type of test\n
 * |TestType                                                           |
 * |-------------------------------------------------------------------|
 * |@ref MIBSPI_ECC_TEST_MODE_1BIT (only for R5 devices)               |
 * |@ref MIBSPI_ECC_TEST_MODE_2BIT (only for R5 devices)               |
 * |@ref MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT (only for R5 devices)  |
 * |@ref MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT (only for R5 devices)  |
 * |@ref MIBSPI_ANALOG_LOOPBACK_TEST                                   |
 * |@ref MIBSPI_SRAM_PARITY_TEST (only for R4 devices)                 |
 *
 * @param 
 * [in]  instance Indicates the MIBSPI module instance. Correct instance value as per the device\n
 *                 variant must be passed as parameter.
 * |instance          |
 * |-----------------------------|
 * |SL_MIBSPI1        |
 * |SL_MIBSPI2 (only for R5 devices)        |
 * |SL_MIBSPI3        |
 * |SL_MIBSPI4 (only for R5 devices)        |
 * |SL_MIBSPI5        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * NOTE : Before running loopback test, ensure correct pinmux setting and no pending transfers.\n
 *           In MIBSPI_ANALOG_LOOPBACK_TEST, the function uses Transfer Group 0 by default.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_MibSPI(MIBSPI_SRAM_PARITY_TEST, SL_MIBSPI3);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_40
 * @reqtag        SL_SR137, SL_SR138, SL_SR141, SLC_232, SLC_233, SLC_236
 * \endif
 *
 */
boolean SL_SelfTest_MibSPI(SL_SelfTestType testType, SL_MIBSPI_Instance instance);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Performs diagnostic tests on SPI (analog loopback test on SPI signal path).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_SPI.jpg
 * @image rtf    SL_SelfTest_SPI.jpg
 * @image latex    SL_SelfTest_SPI.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType         |
 * |----------------------------|
 * |@ref SPI_ANALOG_LOOPBACK_TEST        |
 *
 * @param [in]  instance Indicates the SPI module instance. Correct instance value as per the device\n
 *                 variant must be passed as parameter.
 * |instance         |
 * |----------------------------|
 * |SL_SPI1        |
 * |SL_SPI2        |
 * |SL_SPI3        |
 * |SL_SPI4        |
 * |SL_SPI5        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * NOTE : Before running loopback test, ensure correct pinmux setting and no pending transfers.\n
 *           New data format setting used in SPI_ANALOG_LOOPBACK_TEST may be disruptive to application\n
 *           settings of the data format. The test is carried out only in master mode. During testing,\n
 *            it should be ensured that none of the SPI pins are driven by any other device connected to them.\n
 *             Otherwise, if analog loopback is selected in I/O Loopback mode, then testing may damage the device.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_SPI(SPI_ANALOG_LOOPBACK_TEST, SL_SPI2);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_41
 * @reqtag        SL_SR147, SL_SR148
 * \endif
 *
 */
boolean SL_SelfTest_SPI(SL_SelfTestType testType, SL_SPI_Instance instance);
#endif


/**
 * @brief       Performs diagnostic tests on CAN (test of the SECDED ECC in CAN, test of parity logic\n
 * in CAN, analog loopback test on CAN signal path).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_CAN.jpg
 * @image rtf    SL_SelfTest_CAN.jpg
 * @image latex    SL_SelfTest_CAN.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 *
 * |testType                                                           |
 * |-------------------------------------------------------------------|
 * |@ref CAN_ECC_TEST_MODE_1BIT (only for R5 devices)                  |
 * |@ref CAN_ECC_TEST_MODE_2BIT (only for R5 devices)                  |
 * |@ref CAN_ECC_TEST_MODE_1BIT_FAULT_INJECT (only for R5 devices)     |
 * |@ref CAN_ECC_TEST_MODE_2BIT_FAULT_INJECT (only for R5 devices)     |
 * |@ref CAN_SRAM_PARITY_TEST (only for R4 devices)                    |
 *
 * @param [in]  instance Indicates the CAN module instance. Correct instance value as per the device\n
 *                 variant must be passed as parameter.
 * |instance                         |
 * |---------------------------------|
 * |SL_DCAN1                         |
 * |SL_DCAN2                         |
 * |SL_DCAN3                         |
 * |SL_DCAN4 (only for R5 devices)   |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * NOTE : Before running loopback test, ensure correct pinmux setting and no pending transfers.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_CAN(CAN_SRAM_PARITY_TEST, SL_DCAN1);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_42
 * @reqtag        SL_SR162, SLC_261
 * \endif
 *
 */
boolean SL_SelfTest_CAN(SL_SelfTestType testType, SL_DCAN_Instance instance);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Performs diagnostic tests on GIO (analog loopback test on GIO signal path).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_GIO.jpg
 * @image rtf    SL_SelfTest_GIO.jpg
 * @image latex    SL_SelfTest_GIO.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType         |
 * |----------------------------|
 * |@ref GIO_ANALOG_LOOPBACK_TEST        |
 *
 * @param [in]  instance Indicates the GIO port instance. Correct instance value as per the device\n
 *                 variant must be passed as parameter.
 * |instance         |
 * |----------------------------|
 * |SL_GIOPORTA        |
 * |SL_GIOPORTB        |
 *
 * @param [in]  pin Indicates the GIO pin to be tested [0-7].
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * NOTE : Before running loopback test, ensure correct pinmux setting and no pending transfers.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *                       retVal = SL_SelfTest_GIO(GIO_ANALOG_LOOPBACK_TEST, SL_GIOPORTA, 5U);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_43
 * @reqtag        SL_SR180, SL_SR181
 * \endif
 *
 */
boolean SL_SelfTest_GIO(SL_SelfTestType testType, SL_gioPORT_Instance instance, uint32 pin);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Performs diagnostic tests on SCI (analog loopback test on SCI signal path).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_SCI.jpg
 * @image rtf    SL_SelfTest_SCI.jpg
 * @image latex    SL_SelfTest_SCI.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType         |
 * |----------------------------|
 * |@ref SCI_ANALOG_LOOPBACK_TEST        |
 *
 * @param [in]  instance Indicates the SCI module instance. Correct instance value as per the device\n
 *                 variant must be passed as parameter.
 * |instance          |
 * |----------------------------|
 * |SL_SCI1        |
 * |SL_SCI2        |
 * |SL_SCI3 (only for R5 devices)        |
 * |SL_SCI4 (only for R5 devices)        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * NOTE : Before running loopback test, ensure correct pinmux setting and no pending transfers.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_SCI(SCI_ANALOG_LOOPBACK_TEST);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_44
 * @reqtag        SL_SR155, SL_SR156
 * \endif
 *
 */
boolean SL_SelfTest_SCI(SL_SelfTestType testType, SL_SCI_Instance instance);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief       Performs diagnostic tests on LIN (analog loopback test on LIN signal path).\n
 *
 * Please refer to @ref _SL_SelfTest_Type for the values to be used for testType.
 *
 * \if imageenabled
 * @image html    SL_SelfTest_LIN.jpg
 * @image rtf    SL_SelfTest_LIN.jpg
 * @image latex    SL_SelfTest_LIN.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType         |
 * |----------------------------|
 * |@ref LIN_ANALOG_LOOPBACK_TEST        |
 *
 * @param [in]  instance Indicates the LIN module instance. Correct instance value as per the device\n
 *                 variant must be passed as parameter.
 * |instance          |
 * |----------------------------|
 * |SL_LIN1        |
 * |SL_LIN2 (only for R5 devices)        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * NOTE : Before running loopback test, ensure correct pinmux setting and no pending transfers.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_LIN(LIN_ANALOG_LOOPBACK_TEST);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_45
 * @reqtag        SL_SR159, SL_SR160
 * \endif
 *
 */
boolean SL_SelfTest_LIN(SL_SelfTestType testType, SL_LIN_Instance instance);
#endif


#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
/**
 * @brief       Performs diagnostic tests on Memory Interconnect (test of reserved location access and\n
 * boot time self test of the interconnect).\n
 *
 * \if imageenabled
 * @image html    SL_SelfTest_MemoryInterconnect.jpg
 * @image rtf    SL_SelfTest_MemoryInterconnect.jpg
 * @image latex    SL_SelfTest_MemoryInterconnect.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType         |
 * |----------------------------|
 * |@ref MEMINTRCNT_RESERVED_ACCESS        |
 * |@ref MEMINTRCNT_SELFTEST        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * NOTE: In case of Memory Interconnect Selftest, ensure both GCLK1 and GCLK2 are running
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_MemoryInterconnect(MEMINTRCNT_RESERVED_ACCESS);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_100
 * @reqtag        SLC_92, SLC_93, SLC_99, SLC_100
 * \endif
 *
 */
boolean SL_SelfTest_MemoryInterconnect(SL_SelfTestType testType);
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
/**
 * @brief       Performs diagnostic tests on Main Peripheral Interconnect (test of reserved location access)\n
 *
 * \if imageenabled
 * @image html    SL_SelfTest_MainPeripheralInterconnect.jpg
 * @image rtf    SL_SelfTest_MainPeripheralInterconnect.jpg
 * @image latex    SL_SelfTest_MainPeripheralInterconnect.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType         |
 * |----------------------------|
 * |@ref MAINPERIPHINTRCNT_RESERVED_ACCESS        |
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_MainPeripheralInterconnect(MAINPERIPHINTRCNT_RESERVED_ACCESS);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_101
 * @reqtag        SLC_106, SLC_107
 * \endif
 *
 */
boolean SL_SelfTest_MainPeripheralInterconnect(SL_SelfTestType testType);

#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
/**
 * @brief       Performs diagnostic tests on Peripheral Segments Interconnects (test of reserved location access and \n
 * non privilege mode access)\n
 *
 * \if imageenabled
 * @image html    SL_SelfTest_PeripheralSegmentInterconnect.jpg
 * @image rtf    SL_SelfTest_PeripheralSegmentInterconnect.jpg
 * @image latex    SL_SelfTest_PeripheralSegmentInterconnect.eps
 * \endif
 *
 * @param [in]    testType - Specifies the type of test\n
 * |testType         |
 * |----------------------------|
 * |@ref PERIPHSEGINTRCNT_RESERVED_ACCESS        |
 * |@ref PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS        |
 *
 * @param [in]    instance - Specifies the Peripheral interconnect segment
 *
 * @param [in]    location - Specifies the address of location to be used for test\n
 *
 * @param [in]    protsetreg - The pointer to PPROTSET register which holds the protection bit for location used\n
 *                              Applicable only for interconnect unpriveleged access\n
 *
 * @param [in]    protbit - The protection bit for location used, Applicable only for interconnect unpriveleged access\n
 *
 * @return      TRUE if function executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *                 boolean retVal;
 *               retVal = SL_SelfTest_PeripheralSegmentInterconnect(PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS, SL_PERIPH_SEGMENT2);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 * \if destagenabled
 * @destag        DES_102
 * @reqtag        SLC_114, SLC_115, SLC_125, SLC_126, SLC_136, SLC_137
 * \endif
 *
 */
boolean SL_SelfTest_PeripheralSegmentInterconnect(SL_SelfTestType testType, SL_PeripheralSegment_Instance instance, volatile uint32* location, volatile uint32* protsetreg, uint32 protbit );
#endif

/** NOTE: for future enhancements. Do not use these APIs */
uint32 _SL_Get_DataFault_Status(void);
/** NOTE: for future enhancements. Do not use these APIs */
uint32 _SL_Get_DataFault_Address(void);
#endif /* __SL_API_H__ */
