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
 * @file        sl_types.h
 * @version     2.3.1
 * @brief       File contains the types used by the SafeTI&trade; Hercules&trade; Diagnostic Library APIs
 *
 * @details       The macros,enumerations,structures which are used within the\n
 sl_selftest.c file are to be declared here.This files serves as a type file for\n
 the SafeTI&trade; Hercules&trade; Diagnostic library the types specific for the SafeTI&trade; Hercules&trade; Diagnostic library API's are\n
 declared here.
 */
#ifndef __SL_TYPES_H__
#define __SL_TYPES_H__

#include "HL_sys_common.h"
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
#include "reg_bitdefn_adc.h"
#endif

/** \defgroup FLASHECCConfig Macros for F021Flash ECC Configuration
 *
 *  Following macros are to be used for the ctrlFlags parameter of SL_Init_ECCFlash API
 *
 */
/**@{*/
#define FLASHECC_E0FEN  (uint32)(1U << 10)    /**< Enable Error on Fail event */
#define FLASHECC_EZFEN  (uint32)(1U << 9)     /**< Enable Error on Zero Fail event */
#define FLASHECC_EPEN   (uint32)(1U << 8)     /**< Enable Error profiling */
#define FLASHECC_EOCV   (uint32)(1U << 5)     /**< Enable One Condition valid */
#define FLASHECC_EZCV   (uint32)(1U << 4)     /**< Enable Zero Condition Valid */
#define FLASHECC_DEFAULT  (uint32)(F021F_FEDACCTRL1_E0FEN | F021F_FEDACCTRL1_EZFEN)    /**< Default flag combination */
/**@}*/



/** \defgroup ESMChannels Macros for ESM Channel assignments
 *
 *  Following macros are used by the ESM Handler as a parameter (channel) to the application
 *  callback. Refer to ESM_Application_Callback definition for details
 *
 *  Naming Convention: ESM_G<number>ERR_<module>_<error type>
 */
/**@{*/
#define ESM_G3ERR_EFUSE_AUTOLOAD_ERR   (uint32)(1u)   /**< ESM Group 3 - eFuse
                                                        Auto Load error */

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define ESM_G3ERR_B0TCM_ECC_UNCORR     (uint32)(3u)   /**< ESM Group 3 - RAM
                                                        Even Bank (B0TCM)
                                                        Uncorrectable ECC error
                                                       */
#define ESM_G3ERR_B1TCM_ECC_UNCORR     (uint32)(5u)   /**< ESM Group 3 - RAM Odd
                                                        bank (B1TCM)
                                                        Uncorrectable ECC error
                                                       */
#define ESM_G3ERR_FMC_UNCORR           (uint32)(7u)   /**< ESM Group 3 - Flash
                                                        Uncorrectable error
                                                        (bus1 & bus2) */

#define ESM_G2ERR_CCMR4_COMP           (uint32)(2u)    /**< ESM Group 2 - CCMR4
                                                         Compare error */
#define ESM_G2ERR_B0TCM_UNCORR           (uint32)(6u)    /**< ESM Group 2 - TCM
                                                           RAM Even Bank (B0TCM)
                                                           - Uncorrectable error
                                                          */
#define ESM_G2ERR_B1TCM_UNCORR           (uint32)(8u)    /**< ESM Group 2 - TCM
                                                           RAM Odd Bank (B1TCM)
                                                           - Uncorrectable error
                                                          */
#define ESM_G2ERR_B0TCM_ADDPAR           (uint32)(10u)   /**< ESM Group 2 - TCM
                                                           RAM Even Bank (B0TCM)
                                                           - Address bus parity
                                                           error */
#define ESM_G2ERR_B1TCM_ADDPAR           (uint32)(12u)   /**< ESM Group 2 - TCM
                                                           RAM Odd Bank (B1TCM)
                                                           - Address bus parity
                                                           error */
#define ESM_G2ERR_FMC_UNCORR           (uint32)(4u)    /**< ESM Group 2 -
                                                         F021Flash Uncorrectable
                                                         error (address parity
                                                         on bus1 accesses) */
#define ESM_G2ERR_FLASH_LIVLCK           (uint32)(16u)   /**< ESM Group 2 -
                                                           Flash (ATCM - ECC
                                                           Live lock detect) */
#define ESM_G2ERR_RTI_WWD_NMI           (uint32)(24u)   /**< ESM Group 2 - RTI
                                                          Windowed WatchDog NMI
                                                         */

#define ESM_G1ERR_FMC_CORR               (uint32)(6u)    /**< ESM Group 1 - F021
                                                           Flash Correctable
                                                           error (Bus 1 & 2) */
#define ESM_G1ERR_PLL1_SLIP            (uint32)(10u)   /**< ESM Group 1 - PLL1
                                                         Slip error */
#define ESM_G1ERR_CLOCKMON_INTR        (uint32)(11u)   /**< ESM Group 1 - Clock
                                                         monitor interrupt */
#define ESM_G1ERR_VIMRAM_PARITY        (uint32)(15u)   /**< ESM Group 1 - VIM
                                                         RAM Parity error */
#define ESM_G1ERR_B0TCM_CORRERR        (uint32)(26u)   /**< ESM Group 1 - RAM
                                                         Even bank (B0TCM)
                                                         correctable error */
#define ESM_G1ERR_B1TCM_CORRERR        (uint32)(28u)   /**< ESM Group 1 - RAM
                                                         Odd bank (B1TCM)
                                                         correctable error */
#define ESM_G1ERR_DCC1_ERR             (uint32)(30u)   /**< ESM Group 1 - DCC1
                                                         error */
#define ESM_G1ERR_CCMR4_SELFTEST       (uint32)(31u)   /**< ESM Group 1 - CCMR4
                                                         Selftest error */
#define ESM_G1ERR_EEPROM_CORRERR       (uint32)(35u)   /**< ESM Group 1 - Flash
                                                         EEPROM Correctable
                                                         error */
#define ESM_G1ERR_EEPROM_UNCORRERR     (uint32)(36u)   /**< ESM Group 1 - Flash
                                                         EEPROM Uncorrectable
                                                         error */
#define ESM_G1ERR_PSCON_COMPARE_ERR    (uint32)(38u)   /**< ESM Group 1 - Power
                                                         Domain Controller -
                                                         Compare Error */
#define ESM_G1ERR_PSCON_SELTEST_ERR    (uint32)(39u)   /**< ESM Group 1 - Power
                                                         Domain Controller -
                                                         Self-Test Error */
#define ESM_G1ERR_PLL2_SLIP            (uint32)(42u)   /**< ESM Group 1 - PLL2
                                                         Slip error */
#define ESM_G1ERR_DCC2_ERR             (uint32)(62u)   /**< ESM Group 1 - DCC2
                                                         error */
#endif
#define ESM_G1ERR_EFUSE_ERR_OTHER      (uint32)(40u)   /**< ESM Group 1 - Power
                                                         Domain Controller -
                                                         Self-Test Error */
#define ESM_G1ERR_EFUSE_SELTEST_ERR    (uint32)(41u)   /**< ESM Group 1 - Power
                                                         Domain Controller -
                                                         Self-Test Error */

#define ESM_G1ERR_DMA_PARITY_CORRERR		(uint32)(3u)    /**< ESM Group 1 - DMA -
                                                         Parity correctable error (channel 3)
                                                        */

#define ESM_G1ERR_VIM_PARITY_CORRERR		(uint32)(15u)    /**< ESM Group 1 - VIM -
                                                         Parity correctable error (channel 15)
                                                        */

#define ESM_G1ERR_HET1_PARITY_CORRERR		(uint32)(7u)    /**< ESM Group 1 - HET1/HET2 -
                                                         Parity correctable error
                                                        */

#define ESM_G1ERR_HET2_PARITY_CORRERR		(uint32)(34u)    /**< ESM Group 1 - HET2 -
                                                         Parity correctable error
                                                        */

#define ESM_G1ERR_HTU_PARITY_CORRERR		(uint32)(8u)    /**< ESM Group 1 - HTU1/HTU2 -
                                                         Parity correctable error (channel 8)
                                                        */

#define ESM_G1ERR_MIBSPI1_PARITY_CORRERR	(uint32)(17u)    /**< ESM Group 1 - MISPI1 -
                                                         Parity correctable error (channel 17)
                                                        */

#define ESM_G1ERR_MIBSPI3_PARITY_CORRERR	(uint32)(18u)    /**< ESM Group 1 - MISPI1 -
                                                         Parity correctable error (channel 18)
                                                        */

#define ESM_G1ERR_MIBSPI5_PARITY_CORRERR	(uint32)(24u)    /**< ESM Group 1 - MISPI1 -
                                                         Parity correctable error (channel 24)
                                                        */

#define ESM_G1ERR_DCAN1_PARITY_CORRERR	(uint32)(21u)    /**< ESM Group 1 - MISPI1 -
                                                         Parity correctable error (channel 17)
                                                        */

#define ESM_G1ERR_DCAN2_PARITY_CORRERR	(uint32)(23u)    /**< ESM Group 1 - MISPI1 -
                                                         Parity correctable error (channel 18)
                                                        */

#define ESM_G1ERR_DCAN3_PARITY_CORRERR	(uint32)(22u)    /**< ESM Group 1 - MISPI1 -
                                                         Parity correctable error (channel 24)
                                                        */
#define ESM_G1ERR_ADC1_PARITY_CORRERR      (uint32)(19u)   /**< ESM Group 1 -
                                                         ADC1 - Parity
                                                         correctable error */
#define ESM_G1ERR_ADC2_PARITY_CORRERR       (uint32)(1u)   /**< ESM Group 1 -
                                                         ADC2 - Parity
                                                         correctable error */

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define ESM_G2ERR_CCMR5_CPUCOMP_ERR    (uint32)(2u)    /**< ESM Group 2 - CCMR5
                                                         Compare error */
#define ESM_G2ERR_CCMR5_VIMCOMP_ERR    (uint32)(25u)   /**< ESM Group 2 - CCMR5
                                                         Compare error */
#define ESM_G2ERR_CCMR5_INMCOMP_ERR    (uint32)(26u)   /**< ESM Group 2 - CCMR5
                                                         Compare error */
#define ESM_G2ERR_CCMR5_PDCOMP_ERR     (uint32)(28u)   /**< ESM Group 2 - CCMR5
                                                         Compare error */
#define ESM_G2ERR_CR5_BUSERR           (uint32)(3u)    /**< ESM Group 2 - R5 Bus
                                                         error */
#define ESM_G2ERR_L2RAMW_UNCORR_B       (uint32)(7u)       /**< ESM Group 2 -
                                                             L2RAM Uncorrectable
                                                             error (Type B) */
#define ESM_G1ERR_L2RAMW_CORRERR       (uint32)(26u)   /**< ESM Group 1 - L2RAM
                                                         correctable error */
#define ESM_G2ERR_EPC_UNCORR           (uint32)(21u)   /**< ESM Group 2 - EPC -
                                                         ECC uncorrectable error
                                                        */
#define ESM_G1ERR_EPC_SERR             (uint32)(4u)    /**< ESM Group 1 - EPC -
                                                         ECC single bit error */

#define ESM_G1ERR_CCMR5_SELFTEST       (uint32)(31u)   /**< ESM Group 1 - CCMR5
                                                         Selftest error */

#define ESM_G1ERR_CCMR5_OPERATING_STATUS    (uint32)(92u) /**< ESM Group 1 -
                                                            CCMR5 Selftests
                                                            running */

#define ESM_G1ERR_DMA_ECC_SBERR        (uint32)(82u)   /**< ESM Group 1 - DMA -
                                                         ECC single bit error
                                                         bit */
#define ESM_G1ERR_DMA_ECC_UNCORR       (uint32)(3u)    /**< ESM Group 1 - DMA -
                                                         ECC uncorrectable error
                                                        */

#define ESM_G1ERR_CAN1_ECC_SBERR       (uint32)(73u)    /**< ESM Group 1 - DCAN1
                                                         - ECC single bit error
                                                        */
#define ESM_G1ERR_CAN1_ECC_UNCORR      (uint32)(21u)   /**< ESM Group 1 - DCAN1
                                                         - ECC uncorrectable
                                                         error */
#define ESM_G1ERR_CAN2_ECC_SBERR       (uint32)(74u)   /**< ESM Group 1 - DCAN2
                                                         - ECC single bit error
                                                        */
#define ESM_G1ERR_CAN2_ECC_UNCORR      (uint32)(23u)   /**< ESM Group 1 - DCAN2
                                                         - ECC uncorrectable
                                                         error */
#define ESM_G1ERR_CAN3_ECC_SBERR       (uint32)(75u)   /**< ESM Group 1 - DCAN3
                                                         - ECC single bit error
                                                        */
#define ESM_G1ERR_CAN3_ECC_UNCORR      (uint32)(22u)   /**< ESM Group 1 - DCAN3
                                                         - ECC uncorrectable
                                                         error */
#define ESM_G1ERR_CAN4_ECC_SBERR       (uint32)(76u)   /**< ESM Group 1 - DCAN4
                                                         - ECC single bit error
                                                        */
#define ESM_G1ERR_CAN4_ECC_UNCORR      (uint32)(51u)   /**< ESM Group 1 - DCAN4
                                                         - ECC uncorrectable
                                                         error */

#define ESM_G1ERR_SPI1_ECC_SBERR       (uint32)(77u)   /**< ESM Group 1 -
                                                         MIBSPI1 - ECC single
                                                         bit error */
#define ESM_G1ERR_SPI2_ECC_SBERR       (uint32)(78u)   /**< ESM Group 1 -
                                                         MIBSPI2 - ECC single
                                                         bit error */
#define ESM_G1ERR_SPI3_ECC_SBERR       (uint32)(79u)   /**< ESM Group 1 -
                                                         MIBSPI3 - ECC single
                                                         bit error */
#define ESM_G1ERR_SPI4_ECC_SBERR       (uint32)(80u)   /**< ESM Group 1 -
                                                         MIBSPI4 - ECC single
                                                         bit error */
#define ESM_G1ERR_SPI5_ECC_SBERR       (uint32)(81u)   /**< ESM Group 1 -
                                                         MIBSPI5 - ECC single
                                                         bit error */

#define ESM_G1ERR_SPI1_ECC_UNCORR      (uint32)(17u)   /**< ESM Group 1 -
                                                         MIBSPI1 - ECC
                                                         uncorrectable error */
#define ESM_G1ERR_SPI2_ECC_UNCORR      (uint32)(49u)   /**< ESM Group 1 -
                                                         MIBSPI2 - ECC
                                                         uncorrectable error */
#define ESM_G1ERR_SPI3_ECC_UNCORR      (uint32)(18u)   /**< ESM Group 1 -
                                                         MIBSPI3 - ECC
                                                         uncorrectable error */
#define ESM_G1ERR_SPI4_ECC_UNCORR      (uint32)(50u)   /**< ESM Group 1 -
                                                         MIBSPI4 - ECC
                                                         uncorrectable error */
#define ESM_G1ERR_SPI5_ECC_UNCORR      (uint32)(24u)   /**< ESM Group 1 -
                                                         MIBSPI5 - ECC
                                                         uncorrectable error */
#define ESM_G1ERR_PSCON_COMPARE_ERR    (uint32)(38u)   /**< ESM Group 1 - Power
                                                         Domain Controller -
                                                         Compare Error */
#define ESM_G1ERR_PSCON_SELTEST_ERR    (uint32)(39u)   /**< ESM Group 1 - Power
                                                         Domain Controller -
                                                         Self-Test Error */
#define ESM_G3ERR_CPU_INTRCNT_ERR    (uint32)(12u)   /**< ESM Group 1 - Power
                                                         Domain Controller -
                                                         Self-Test Error */
#define ESM_G1ERR_PLL2_SLIP            (uint32)(42u)   /**< ESM Group 1 - PLL2
                                                         Slip error */
#define ESM_G1ERR_PLL1_SLIP            (uint32)(10u)   /**< ESM Group 1 - PLL1
                                                         Slip error */
#endif

/**@}*/



/** \typedef ESM_Application_Callback
  *
  * \brief Definition for ESM Application call back type.
  * \description The esm application call back happens in case of fault
  * injections and actual device faults. There will be no callback in case of failures in the selftests.\n
  * The application should use the provided status function to get the status
  * (pass/fail) of the last selftest which was triggered.
  * \param grpchannel - ESM Channel Number and group indicator to the application.Upper 16 bits indicate the group and lower 16 bits indicate the channel.\n
  * group indicate 1- for group1 and 2- for group2.
  *
  *
  * \param param   - Additional error information. Contents & type depends on channel# and are described below
  * 
  * **NOTE: No additional error information is defined for the ESMs in case of the
  * RM57Lx and the TMS570LC43x devices currently.  The application must read the
  * appropriate error information from the module signalling the ESM directly.**
  *
  |--------------------------|-----------------------------------------------------------------------------------------|
  | channel (ESM_xxxx)       |    Description                                                                          |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G3ERR_EFUSE_AUTOLOAD_ERR |   Not Applicable                                                                        |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G3ERR_B0TCM_ECC_UNCORR   |   Not Applicable                                                                        |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G3ERR_B1TCM_ECC_UNCORR   |   Not Applicable                                                                        |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G3ERR_FMC_UNCORR         |   Not Applicable                                                                        |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G2ERR_CCMR4_COMP         |   None                                                                                  |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G2ERR_FMC_UNCORR         | param1 - In case of Bus Parity Errors [31-0] indicates address location. In case of     |
  |                          |          ECC errors [31-23] 64bit aligned address and [0-2] are 0.                      |
  |                          | param2 & param3 - RESERVED                                                              |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G2ERR_B0TCM_UNCORR       | param1 - Address which failed the parity check as an offset from the base address of    |
  |                          |          TCM RAM (0x0800_0000 by default)                                               |
  |                          | param2 & param3 - RESERVED										   					   |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G2ERR_B1TCM_UNCORR       | param2 - Address which failed the parity check as an offset from the base address of    |
  |                          |          TCM RAM (0x0800_0000 by default)                                               |
  |                          | param1 & param3 - RESERVED										   					   |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G2ERR_B0TCM_ADDPAR       | param1 -
  |                          | param2 - param[8] - 1: Indicates a Read Address Parity failure                          |
  |                          |          param[9] - 1: Indicates a Write Address Parity failure                         |
  |                          | param2[31-10], param2[7-0] & param3 - RESERVED										   |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G2ERR_B1TCM_ADDPAR       |     None                                                                                |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G2ERR_FLASH_LIVLCK       |     None                                                                                |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G2ERR_RTI_WWD_NMI        |     None                                                                                |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_FMC_CORR           | param1 - 16bit counter contains the number of correctable error occurrences             |
  |                          | param2 - 32bit CPU address for which a correctable error is detected                    |
  |                          | param3 - RESERVED                                                                       |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_PLL1_SLIP          | param1[0] - 1: Indicates a under cycle slip error                                       |
  |                          | param1[1] - 1: Indicates a over cycle slip error                                        |
  |                          | param2 & param3 - RESERVED                                                              |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_CLOCKMON_INTR      | param1, param2, param3 - RESERVED                                                       |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_VIMRAM_PARITY      | param1 - 32bit address of the first parity error location                               |
  |                          | param2, param3 - RESERVED                                                               |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_B0TCM_CORRERR      | param1[17-3] - [17-3] of the address for which the Cortex-R4F CPU detects a single-bit  |
  |                          | error                                                                                   |
  |                          | param2 & param3 - RESERVED                                                              |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_B1TCM_CORRERR      | param1[17-3] - [17-3] of the address for which the Cortex-R4F CPU detects a single-bit  |
  |                          | error                                                                                   |
  |                          | param2 & param3 - RESERVED                                                              |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_DCC1_ERR           | param1[19-0] - Indicates the current value of Counter0                                  |
  |                          | param2[19-0] - Indicates the current value of Counter1                                  |
  |                          | param3 - RESERVED                                                                       |
  |                          | param1[19-0] - Indicates the current counter values might not be accurate               |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_EEPROM_CORRERR     | param1, param2 & param3 - RESERVED                                                      |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_EEPROM_UNCORRERR   | param1, param2 & param3 - RESERVED                                                      |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_PLL2_SLIP          | param1, param2 & param3 - RESERVED                                                      |
  |--------------------------|-----------------------------------------------------------------------------------------|
  | G1ERR_DCC2_ERR           | param1[19-0] - Indicates the current value of Counter0                                  |
  |                          | param2[19-0] - Indicates the current value of Counter1                                  |
  |                          | param3 - RESERVED                                                                       |
  |                          | param1[19-0] - Indicates the current counter values might not be accurate               |
  |--------------------------|-----------------------------------------------------------------------------------------|
  *
  */
  /*SAFETYMCUSW 298 S MR:- <APPROVED> Comment_1*/
typedef void (* ESM_Application_Callback) (uint32 grp_channel, uint32 param1, uint32 param2, uint32 param3);



/**
  * \enum       SL_ResetReason
  *
  * \brief      Enumerations for Reset Reason
  *
  */
typedef enum _SL_ResetReason {
    RESET_TYPE_UNKNOWN          = 0u,   /**< Reset reason is unknow */
    RESET_TYPE_POWERON          = 1u,   /**< nPORRST Reset or VMON Reset*/
    RESET_TYPE_OSC_FAILURE      = 2u,   /**< Reset due to Oscillator failure */
    RESET_TYPE_WATCHDOG         = 3u,   /**< Reset due to Watchdog expirty */
    RESET_TYPE_DEBUG            = 4u,   /**< Reset due to software(Debug) */
    RESET_TYPE_ICEPICK          = 5u,   /**< Reset by Ice Pick (JTAG) */
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    RESET_TYPE_CPU              = 6u,   /**< Reset due to CPU SelfTest Completion or toggling of SL_CPU_RESET bit */
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    RESET_TYPE_CPU0             = 6u,
#endif
    RESET_TYPE_SWRST            = 7u,   /**< Software Reset */
    RESET_TYPE_EXTRST           = 8u,   /**< nRST reset or Warm Reset */
    RESET_TYPE_EXTRST_NERROR    = 9u    /**< nRST due to nERROR */
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    ,
	RESET_TYPE_CPU1             = 10u,
	RESET_TYPE_ICSTRST			= 11u
#endif
}SL_ResetReason;

/**
  * \enum       SL_HET_Instance
  *
  * \brief      Enumerations for HET instances
  *
  */
typedef enum _SL_HET_Instance
{
    SL_HET1 = 1,
    SL_HET2
} SL_HET_Instance;

/**
  * \enum       SL_HTU_Instance
  *
  * \brief      Enumerations for HTU instances
  *
  */
typedef enum _SL_HTU_Instance
{
    SL_HTU1 = 1,
    SL_HTU2
} SL_HTU_Instance;


/**
  * \enum       SL_DCAN_Instance
  *
  * \brief      Enumerations for DCAN instances
  *
  */
typedef enum _SL_DCAN_Instance
{
    SL_DCAN1 = 1,
    SL_DCAN2,
    SL_DCAN3,
    SL_DCAN4
} SL_DCAN_Instance;

/**
  * \enum       SL_MIBSPI Interface
  *
  * \brief      Enumerations for MIBSPI instances
  *
  */
typedef enum _SL_MIBSPI_Instance
{
    SL_MIBSPI1 = 1,
    SL_MIBSPI2,
    SL_MIBSPI3,
    SL_MIBSPI4,
    SL_MIBSPI5
} SL_MIBSPI_Instance;

/**
  * \enum       SL_PeripheralSegment Interface
  *
  * \brief      Enumerations for PeripheralSegment instances
  *
  */
typedef enum _SL_PeripheralSegment_Instance
{
    SL_PERIPH_SEGMENT1 = 1,
	SL_PERIPH_SEGMENT2,
	SL_PERIPH_SEGMENT3

} SL_PeripheralSegment_Instance;


/**
  * \enum       SL_SPI Interface
  *
  * \brief      Enumerations for SPI instances
  *
  */
typedef enum _SL_SPI_Instance
{
    SL_SPI1 = 1,
    SL_SPI2,
    SL_SPI3,
    SL_SPI4,
    SL_SPI5
} SL_SPI_Instance;

/**
  * \enum       SL_SCI Interface
  *
  * \brief      Enumerations for SCI instances
  *
  */
typedef enum _SL_SCI_Instance
{
    SL_SCI1 = 1,
    SL_SCI2,
    SL_SCI3,
    SL_SCI4
} SL_SCI_Instance;

/**
  * \enum       SL_LIN Interface
  *
  * \brief      Enumerations for LIN instances
  *
  */
typedef enum _SL_LIN_Instance
{
    SL_LIN1 = 1,
    SL_LIN2
} SL_LIN_Instance;

/**
  * \enum       SL_gioPORT Instance
  *
  * \brief      Enumerations for GIO PORT instances
  *
  */
typedef enum _SL_gioPORT_Instance
{
    SL_GIOPORTA = 1,
    SL_GIOPORTB
} SL_gioPORT_Instance;

/**
  * \struct     SL_ResetReason_XInfo
  *
  * \brief      Provides extended information of the previous reset (using shadow registers)
  *
  */
typedef struct _SL_ResetReason_XInfo{
    boolean     nError;                 /**< TRUE when nError pin is active (low), else FALSE */
    uint32      esmGrp2Status;          /**< Each bit indicate the status of ESM Group2 channels */
    uint32      esmGrp3Status;          /**< Each bit indicate the status of ESM Group3 channels */
} SL_ResetReason_XInfo;



/**
  * \enum       SL_AutoInitRamId
  *
  * \brief      Enumerations for Auto Initializaiton memory API SL_Init_Memory
  *
  *
  */
typedef enum _SL_AutoInitRamId {
    RAMTYPE_RAM         = 0x00000001u,/**< Internal RAM*/

    RAMTYPE_DMA_RAM     = 0x00000002u,/**<DMA RAM*/

    RAMTYPE_VIM         = 0x00000004u,/**<VIM RAM*/

    RAMTYPE_NHET1       = 0x00000008u,/**<NHET1 RAM*/
    RAMTYPE_HET_TU1     = 0x00000010u/**<HTU RAM*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
    ,
RAMTYPE_NHET2       = 0x00008000u,/**<NHET2 RAM*/
RAMTYPE_HET_TU2     = 0x00010000u,/**<HTU2 RAM*/

RAMTYPE_MIBSPI1_RAM = 0x00000080u,/**<MibSPI1 RAM*/
RAMTYPE_MIBSPI3_RAM = 0x00000800u,/**<MibSPI3 RAM*/
RAMTYPE_MIBSPI5_RAM = 0x00001000u,/**<MibSPI5 RAM*/


RAMTYPE_DCAN1_RAM   = 0x00000020u,/**<DCAN1 RAM*/
RAMTYPE_DCAN2_RAM   = 0x00000040u,/**<DCAN2 RAM*/
RAMTYPE_DCAN3_RAM   = 0x00000400u/**<DCAN3 RAM*/
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
        ,
    RAMTYPE_NHET2       = 0x00008000u,/**<NHET2 RAM*/
    RAMTYPE_HET_TU2     = 0x00010000u,/**<HTU2 RAM*/

    RAMTYPE_DCAN1_RAM   = 0x00000020u,/**<DCAN1 RAM*/
    RAMTYPE_DCAN2_RAM   = 0x00000040u,/**<DCAN2 RAM*/
    RAMTYPE_DCAN3_RAM   = 0x00000400u,/**<DCAN3 RAM*/
    RAMTYPE_DCAN4_RAM   = 0x00100000u,/**<DCAN4 RAM*/

    RAMTYPE_MIBSPI1_RAM = 0x00000080u,/**<MibSPI1 RAM*/
    RAMTYPE_MIBSPI2_RAM = 0x00040000u,/**<MibSPI2 RAM*/
    RAMTYPE_MIBSPI3_RAM = 0x00000800u,/**<MibSPI3 RAM*/
    RAMTYPE_MIBSPI4_RAM = 0x00080000u,/**<MibSPI4 RAM*/
    RAMTYPE_MIBSPI5_RAM = 0x00001000u/**<MibSPI5 RAM*/
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
    	,
	RAMTYPE_MIBADC1_RAM = 0x00000100u,/**<MibADC1 RAM*/
    RAMTYPE_MIBADC2_RAM = 0x00004000u/**<MibADC2 RAM*/
#endif
#if defined(_TMS570LC43x_) 
        ,
    RAMTYPE_FTU_RAM     = 0x00002000u/**<MibADC2 RAM*/
#endif
} SL_AutoInitRamId;


#if defined(_TMS570LC43x_) 
#define   ALL_RAMTYPE_IDS       (uint32)(RAMTYPE_RAM    |   \
                                 RAMTYPE_DMA_RAM        |   \
                                 RAMTYPE_VIM            |   \
                                 RAMTYPE_NHET1          |   \
                                 RAMTYPE_NHET2          |   \
                                 RAMTYPE_HET_TU1        |   \
                                 RAMTYPE_HET_TU2        |   \
                                 RAMTYPE_DCAN1_RAM      |   \
                                 RAMTYPE_DCAN2_RAM      |   \
                                 RAMTYPE_DCAN3_RAM      |   \
                                 RAMTYPE_DCAN4_RAM      |   \
                                 RAMTYPE_MIBADC1_RAM    |   \
                                 RAMTYPE_MIBADC2_RAM    |   \
                                 RAMTYPE_MIBSPI1_RAM    |   \
                                 RAMTYPE_MIBSPI2_RAM    |   \
                                 RAMTYPE_MIBSPI3_RAM    |   \
                                 RAMTYPE_MIBSPI4_RAM    |   \
                                 RAMTYPE_MIBSPI5_RAM    |   \
                                 RAMTYPE_FTU_RAM) 
#endif
#if defined(_RM57Lx_)
#define   ALL_RAMTYPE_IDS       (uint32)(RAMTYPE_RAM    |   \
                                 RAMTYPE_DMA_RAM        |   \
                                 RAMTYPE_VIM            |   \
                                 RAMTYPE_NHET1          |   \
                                 RAMTYPE_NHET2          |   \
                                 RAMTYPE_HET_TU1        |   \
                                 RAMTYPE_HET_TU2        |   \
                                 RAMTYPE_DCAN1_RAM      |   \
                                 RAMTYPE_DCAN2_RAM      |   \
                                 RAMTYPE_DCAN3_RAM      |   \
                                 RAMTYPE_DCAN4_RAM      |   \
                                 RAMTYPE_MIBADC1_RAM    |   \
                                 RAMTYPE_MIBADC2_RAM    |   \
                                 RAMTYPE_MIBSPI1_RAM    |   \
                                 RAMTYPE_MIBSPI2_RAM    |   \
                                 RAMTYPE_MIBSPI3_RAM    |   \
                                 RAMTYPE_MIBSPI4_RAM    |   \
								 RAMTYPE_MIBSPI5_RAM	|	\
								 RAMTYPE_MIBADC1_RAM	|	\
                                 RAMTYPE_MIBADC2_RAM)
#endif

/**
  * \enum       SL_SelfTestType
  *
  * \brief      Self test types for use with SL_SelfTest_xxxx APIs
  *
  *
  */
typedef enum _SL_SelfTest_Type {
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
    SRAM_ECC_ERROR_FORCING_1BIT = 0xA0,            /**< BTCM SRAM 1-Bit ECC Error forcing tests(to be used with API @ref SL_SelfTest_SRAM)        */
    SRAM_ECC_1BIT_FAULT_INJECTION,                /**< BTCM SRAM 1-Bit ECC Fault injection(to be used with API @ref SL_SelfTest_SRAM)            */
    SRAM_ECC_ERROR_FORCING_2BIT,                /**< BTCM SRAM 2-Bit ECC Error forcing test(to be used with API @ref SL_SelfTest_SRAM)            */
    SRAM_ECC_2BIT_FAULT_INJECT,                    /**< BTCM SRAM 2-Bit ECC Error fault inject    (to be used with API @ref SL_SelfTest_SRAM)        */
    SRAM_PAR_ADDR_CTRL_SELF_TEST,                /**< BTCM SRAM Address & Control Parity self test(to be used with API @ref SL_SelfTest_SRAM)    */
    SRAM_ECC_ERROR_PROFILING,                    /**< SRAM Single bit Error Profiling test(to be used with API @ref SL_SelfTest_SRAM)            */
    SRAM_ECC_ERROR_PROFILING_FAULT_INJECT,        /**< SRAM Single bit Error Profiling fault injection(to be used with API @ref SL_SelfTest_SRAM)*/
    SRAM_RADECODE_DIAGNOSTICS,                    /**< SRAM Redundant Address Decode self test(to be used with API @ref SL_SelfTest_SRAM)        */
    SRAM_LIVELOCK_DIAGNOSTICS,					/**< SRAM Livelock test(to be used with API @ref SL_SelfTest_SRAM)        */
    ADC_SELFTEST_ALL,							/**< ADC self test(to be used with API @ref SL_SelfTest_ADC)        */
	ADC_SRAM_PARITY_TEST,						/**< ADC SRAM Parity test(to be used with API @ref SL_SelfTest_ADC)        */
    FEE_ECC_DATA_CORR_MODE,                        /**< ECC Data correction diagnostic mode.(to be used with API @ref SL_SelfTest_FEE)*/
    FEE_ECC_SYN_REPORT_MODE,                    /**< ECC Syndrome reporting diagnostic mode(to be used with API @ref SL_SelfTest_FEE) */
    FEE_ECC_MALFUNCTION_MODE1,                    /**< ECC Malfunction test mode 1 (same data mode)(to be used with API @ref SL_SelfTest_FEE)*/
    FEE_ECC_MALFUNCTION_MODE2,                    /**< ECC malfunction test mode 2 (inverted data mode)(to be used with API @ref SL_SelfTest_FEE) */
    FEE_ECC_ADDR_TAG_REG_MODE,                    /**< Address tag register diagnostic mode(to be used with API @ref SL_SelfTest_FEE) */
    FEE_ECC_TEST_MODE_1BIT,                        /**< 1Bit ECC Data correction diagnostic mode(to be used with API @ref SL_SelfTest_FEE) */
    FEE_ECC_TEST_MODE_1BIT_FAULT_INJECT,        /**< Address tag register diagnostic mode(to be used with API @ref SL_SelfTest_FEE) */
    FEE_ECC_TEST_MODE_2BIT,                        /**< 2Bit ECC Data correction diagnostic mode.Verifies whether the 2 bit ecc error is getting created in FEE properly*/
    FEE_ECC_TEST_MODE_2BIT_FAULT_INJECT,        /**< 2Bit ECC Fault inject creates a 2 bit ecc error and leaves the application to handle the scenario */

    FLASH_ECC_ADDR_TAG_REG_MODE,                /**< Address tag register diagnostic mode(to be used with API @ref SL_SelfTest_Flash) */
    /*FLASH_ECC_PIPELINE_BUF_PAR_MODE,            < Pipeline buffer parity diagnostic mode <NOT IMPLEMENTED IN THIS RELEASE> */
    FLASH_ADDRESS_ECC_SELF_TEST,                /**< Flash wrapper address ecc self test verifies whether the address ecc is working fine (to be used with API @ref SL_SelfTest_Flash)*/
    FLASH_ADDRESS_ECC_FAULT_INJECT,                /**< Flash wrapper address ecc self test fault inject creaters an address ecc error and leaves it for the application to handle it(to be used with API @ref SL_SelfTest_Flash)*/
    FLASH_ADDRESS_PARITY_SELF_TEST,             /**< Flash wrapper address parity self test.Verifies whether teh address parity feature is working fine*/
    FLASH_ADDRESS_PARITY_FAULT_INJECT,          /**< Flash wrapper address parity self test fault inject creates an address parity error and leaves it for the application to handle it(to be used with API @ref SL_SelfTest_Flash)*/
    FLASH_ECC_TEST_MODE_1BIT,                   /**< 1Bit ECC Data correction diagnostic mode(to be used with API @ref SL_SelfTest_Flash) */
    FLASH_ECC_TEST_MODE_2BIT,                    /**< 2Bit ECC Data correction diagnostic mode(to be used with API @ref SL_SelfTest_Flash) */
    FLASH_HARD_ERROR_LIVELOCK,
    FLASH_ECC_TEST_MODE_2BIT_FAULT_INJECT,         /**< 2Bit ECC fault injection mode. No response handled. NOTE: Application must read from 0x20000000 to generate error(to be used with API @ref SL_SelfTest_Flash) */

    PBIST_EXECUTE,                                 /**< PBIST self test mode .The RAM info register RINFOL and RINFOU are used to select the memories for test(to be used with API @ref SL_SelfTest_PBIST)*/
    PBIST_EXECUTE_OVERRIDE,                        /**< PBIST test mode with ROM override.The memory information available from ROM will override the RAM selection from RAM info Registers (to be used with API @ref SL_SelfTest_PBIST)*/

    STC_RUN,                                       /**< CCMR4 test using STC(to be used with API @ref SL_SelfTest_STC) */
    STC_COMPARE_SELFCHECK,                         /**< STC self check diagnostic(to be used with API @ref SL_SelfTest_STC) */

    PSCON_SELF_TEST,                               /**< PSCON self test using Compare match & mismatch tests (to be used with API @ref SL_SelfTest_PSCON)*/
    PSCON_ERROR_FORCING,                           /**< PSCON Error-Forcing self test mode (to be used with API @ref SL_SelfTest_PSCON)*/
    PSCON_ERROR_FORCING_FAULT_INJECT,               /**< PSCON Error-Forcing fault inject(to be used with API @ref SL_SelfTest_PSCON) */
    PSCON_SELF_TEST_ERROR_FORCING,                /**< PSCON Self-Test Error forcing mode (to be used with API @ref SL_SelfTest_PSCON)*/
    PSCON_SELF_TEST_ERROR_FORCING_FAULT_INJECT,    /**< PSCON Self-Test Error forcing fault inject(to be used with API @ref SL_SelfTest_PSCON) */
    PSCON_PMA_TEST,                                /**< PSCON Privilege Mode access test - MUST BE RUN IN USER MODE (to be used with API @ref SL_SelfTest_PSCON)*/

    EFUSE_SELF_TEST_AUTOLOAD,                      /**< EFuse Auto load self test (to be used with API @ref SL_SelfTest_EFuse)*/
    EFUSE_SELF_TEST_ECC,                           /**< EFuse ECC Self test (to be used with API @ref SL_SelfTest_EFuse)*/
    EFUSE_SELF_TEST_STUCK_AT_ZERO,                 /**< EFuse stuck-at-zero self test (to be used with API @ref SL_SelfTest_EFuse)*/
    CCMR4F_SELF_TEST,                              /**< CCMR4F Self-test (to be used with API @ref SL_SelfTest_CCMR4F)*/
    CCMR4F_ERROR_FORCING_TEST,                    /**< CCMR4F Error forcing test(to be used with API @ref SL_SelfTest_CCMR4F) */
    CCMR4F_ERROR_FORCING_TEST_FAULT_INJECT,        /**< CCMR4F Error focing test, but does not clear the nERROR & ESM Interrupt (Callee function must clear these) (to be used with API @ref SL_SelfTest_CCMR4F)*/
    CCMR4F_SELF_TEST_ERROR_FORCING, 				/**< CCMR4F Self-test error forcing (to be used with API @ref SL_SelfTest_CCMR4F)*/
    L2INTERCONNECT_RESERVED_ACCESS,                  /** L2 interconnect  reserved location access to create error trap (to be used with API @ref SL_SelfTestL2L3Interconnect)*/
    L2INTERCONNECT_UNPRIVELEGED_ACCESS,				/** L2 interconnect  unprivileged access to create error trap (to be used with API @ref SL_SelfTestL2L3Interconnect)*/
    L3INTERCONNECT_RESERVED_ACCESS,                  /** L3 interconnect reserved location access to create error trap(to be used with API @ref SL_SelfTestL2L3Interconnect)*/
    L3INTERCONNECT_UNPRIVELEGED_ACCESS,				/** L3 interconnect  unprivileged access to create error trap (to be used with API @ref SL_SelfTestL2L3Interconnect)*/
	VIM_SRAM_PARITY_TEST,							/**< VIM SRAM Parity test(to be used with API @ref SL_SelfTest_VIM)        */
    VIM_SOFTWARE_TEST,								/**< VIM Functionality test(to be used with API @ref SL_SelfTest_VIM)        */
#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
    DMA_SRAM_PARITY_TEST,					 	/**< DMA RAM SECDED Parity Errorforcing test(to be used with API@ref SL_SelfTest_DMA)*/
	DMA_SOFTWARE_TEST,							/**< DMA Functionality test(to be used with API @ref SL_SelfTest_DMA)        */
#endif
    HET_SRAM_PARITY_TEST,						/**< HET SRAM Parity test(to be used with API @ref SL_SelfTest_HET)        */
    HET_ANALOG_LOOPBACK_TEST,					/**< HET Analog Loopback test(to be used with API @ref SL_SelfTest_HET)        */
    HTU_SRAM_PARITY_TEST,						/**< HTU SRAM Parity test(to be used with API @ref SL_SelfTest_HTU)        */
    MIBSPI_SRAM_PARITY_TEST,					/**< MIBSPI SRAM Parity test(to be used with API @ref SL_SelfTest_MibSPI)        */
    MIBSPI_ANALOG_LOOPBACK_TEST,				/**< MIBSPI Analog Loopback test(to be used with API @ref SL_SelfTest_MibSPI)        */
    SPI_ANALOG_LOOPBACK_TEST,					/**< SPI Analog Loopback test(to be used with API @ref SL_SelfTest_SPI)        */
    CAN_SRAM_PARITY_TEST,						/**< CAN SRAM Parity test(to be used with API @ref SL_SelfTest_CAN)        */
    CAN_ANALOG_LOOPBACK_TEST,					/**< CAN Analog Loopback test(to be used with API @ref SL_SelfTest_CAN)        */
    GIO_ANALOG_LOOPBACK_TEST,					/**< GIO Analog Loopback test(to be used with API @ref SL_SelfTest_GIO)        */
    SCI_ANALOG_LOOPBACK_TEST,					/**< SCI Analog Loopback test(to be used with API @ref SL_SelfTest_SCI)        */
    LIN_ANALOG_LOOPBACK_TEST					/**< LIN Analog Loopback test(to be used with API @ref SL_SelfTest_LIN)        */

#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
    SRAM_ECC_ERROR_FORCING_1BIT = 0xA0A0,   /**< L2RAMW SECDED 1-Bit ECC Error
                                              forcing tests(to be used with API
                                              @ref SL_SelfTest_SRAM)
                                             */
    SRAM_ECC_ERROR_FORCING_2BIT,            /**< L2RAMW SECDED 2-Bit ECC Error
                                              forcing test(to be used with API
                                              @ref SL_SelfTest_SRAM)
                                             */
	SRAM_RADECODE_DIAGNOSTICS,				/**< L2RAMW Redundant address decode
												test(to be used with API
                                              @ref SL_SelfTest_SRAM)
                                             */

    FLASH_ECC_TEST_MODE_1BIT,               /**< Test of the CPU SECDED for 1
                                              bit ECC Error on read from L2FMC
                                              (to be used with API @ref
                                              SL_SelfTest_Flash) */
    FLASH_ECC_TEST_MODE_2BIT,               /**< Test of the CPU SECDED for 2
                                              bit ECC Error on read from L2FMC
                                              (to be used with API @ref
                                              SL_SelfTest_Flash) */

    PBIST_EXECUTE,                          /**< PBIST self test mode .The RAM
                                              info register RINFOL and RINFOU
                                              are used to select the memories
                                              for test(to be used with API @ref
                                              SL_SelfTest_PBIST)*/
    PBIST_EXECUTE_OVERRIDE,                 /**< PBIST test mode with ROM
                                              override.The memory information
                                              available from ROM will override
                                              the RAM selection from RAM info
                                              Registers (to be used with API
                                              @ref SL_SelfTest_PBIST)*/

    EFUSE_SELF_TEST_AUTOLOAD,               /**< EFuse Auto load self test (to
                                              be used with API @ref
                                              SL_SelfTest_EFuse)*/
    EFUSE_SELF_TEST_ECC,                    /**< EFuse ECC Self test (to be used
                                              with API @ref SL_SelfTest_EFuse)*/
    EFUSE_SELF_TEST_STUCK_AT_ZERO,          /**< EFuse stuck-at-zero self test
                                              (to be used with API @ref
                                              SL_SelfTest_EFuse)*/

    CCMR5F_CPUCOMP_SELF_TEST,                /**< CCMR5F CPU Comparator Self-test (to be used
                                              with API @ref
                                              SL_SelfTest_CCMR5F)*/
    CCMR5F_CPUCOMP_ERROR_FORCING_TEST,     /**< CCMR5F CPU Compare Error forcing test(to be
                                              used with API @ref
                                              SL_SelfTest_CCMR5F) */
    CCMR5F_CPUCOMP_ERROR_FORCING_TEST_FAULT_INJECT, /**< CCMR5F CPU Compare Error Fault injection , but
                                              does not clear the nERROR & ESM
                                              Interrupt (Callee function must
                                              clear these) (to be used with API
                                              @ref SL_SelfTest_CCMR5F)*/
    CCMR5F_CPUCOMP_SELF_TEST_ERROR_FORCING, /**< CCMR5F CPU Comparator Self-test error forcing
                                              (to be used with API @ref
                                              SL_SelfTest_CCMR5F)*/
    CCMR5F_VIMCOMP_SELF_TEST,               /**< CCMR5F VIM Comparator Self-test (to be used
                                              with API @ref
                                              SL_SelfTest_CCMR5F)*/
    CCMR5F_VIMCOMP_ERROR_FORCING_TEST,      /**< CCMR5F VIM Compare Error forcing test(to be
                                              used with API @ref
                                              SL_SelfTest_CCMR5F) */
    CCMR5F_VIMCOMP_ERROR_FORCING_TEST_FAULT_INJECT, /**< CCMR5F VIM Compare Error focing test, but
                                              does not clear the nERROR & ESM
                                              Interrupt (Callee function must
                                              clear these) (to be used with API
                                              @ref SL_SelfTest_CCMR5F)*/
    CCMR5F_VIMCOMP_SELF_TEST_ERROR_FORCING, /**< CCMR5F VIM Comparator Self-test error forcing
                                              (to be used with API @ref
                                              SL_SelfTest_CCMR5F)*/
    CCMR5F_PDCOMP_SELF_TEST,                /**< CCMR5F Power Domain Comparator Self-test (to be used
                                              with API @ref
                                              SL_SelfTest_CCMR5F)*/
    CCMR5F_PDCOMP_ERROR_FORCING_TEST,       /**< CCMR5F Power Domain Compare Error forcing test(to be
                                              used with API @ref
                                              SL_SelfTest_CCMR5F) */
    CCMR5F_PDCOMP_ERROR_FORCING_TEST_FAULT_INJECT, /**< CCMR5F Power Domain
                                                     Compare Error focing test, but
                                              does not clear the nERROR & ESM
                                              Interrupt (Callee function must
                                              clear these) (to be used with API
                                              @ref SL_SelfTest_CCMR5F)*/
    CCMR5F_PDCOMP_SELF_TEST_ERROR_FORCING,  /**< CCMR5F Power Domain Comparator Self-test error forcing
                                              (to be used with API @ref
                                              SL_SelfTest_CCMR5F)*/
    CCMR5F_INMCOMP_SELF_TEST,               /**< CCMR5F Inactive CPU Monitor
                                              Comparator Self-test (to be used
                                              with API @ref
                                              SL_SelfTest_CCMR5F)*/
    CCMR5F_INMCOMP_ERROR_FORCING_TEST,      /**< CCMR5F Inactive CPU Monitor
                                              Compare Error forcing test(to be
                                              used with API @ref
                                              SL_SelfTest_CCMR5F) */
    CCMR5F_INMCOMP_ERROR_FORCING_TEST_FAULT_INJECT, /**< CCMR5F Inactive CPU
                                                      Monitor Compare Error focing test, but
                                              does not clear the nERROR & ESM
                                              Interrupt (Callee function must
                                              clear these) (to be used with API
                                              @ref SL_SelfTest_CCMR5F)*/
    CCMR5F_INMCOMP_SELF_TEST_ERROR_FORCING, /**< CCMR5F Inactive CPU Monitor
                                              Comparator Self-test error forcing
                                              (to be used with API @ref
                                              SL_SelfTest_CCMR5F)*/

    STC1_RUN,                               /**< CPU test using STC1(to be used
                                              with API @ref SL_SelfTest_STC) */
    STC1_COMPARE_SELFCHECK,                 /**< STC1 self check diagnostic(to be
                                              used with API @ref
                                              SL_SelfTest_STC) */
    STC2_RUN,                               /**< */
    STC2_COMPARE_SELFCHECK,                 /**< */

    DMA_ECC_TEST_MODE_1BIT,                 /**< DMA RAM SECDED 1-Bit ECC Error
                                              forcing tests(to be used with API
                                              @ref SL_SelfTest_DMA)
                                             */

    DMA_ECC_TEST_MODE_2BIT,                 /**< DMA RAM SECDED 2-Bit ECC Error
                                              forcing tests(to be used with API
                                              @ref SL_SelfTest_DMA)
                                             */
    DMA_ECC_TEST_MODE_1BIT_FAULT_INJECT,    /**< DMA RAM SECDED 1-Bit ECC Error
                                              Fault injection(to be used with API
                                              @ref SL_SelfTest_DMA)
                                             */
    DMA_ECC_TEST_MODE_2BIT_FAULT_INJECT,     /**< DMA RAM SECDED 2-Bit ECC Error
                                              Fault injection(to be used with API
                                              @ref SL_SelfTest_DMA)
                                             */
	DMA_SOFTWARE_TEST,						/**< DMA Periodic software test
												(to be used with API
                                              @ref SL_SelfTest_DMA)
                                             */
    CAN_ECC_TEST_MODE_1BIT,                  /**< CAN RAM SECDED 1-Bit ECC Error
                                              forcing tests(to be used with API
                                              @ref SL_SelfTest_CAN)
                                             */
    CAN_ECC_TEST_MODE_2BIT,                  /**< CAN RAM SECDED 2-Bit ECC Error
                                              forcing tests(to be used with API
                                              @ref SL_SelfTest_CAN)
                                             */
    CAN_ECC_TEST_MODE_1BIT_FAULT_INJECT,     /**< CAN RAM SECDED 1-Bit ECC Error
                                              Fault injection (to be used with API
                                              @ref SL_SelfTest_CAN)
                                             */
    CAN_ECC_TEST_MODE_2BIT_FAULT_INJECT,     /**< CAN RAM SECDED 2-Bit ECC Error
                                              Fault injection (to be used with API
                                              @ref SL_SelfTest_CAN)
                                             */
    
    MIBSPI_ECC_TEST_MODE_1BIT,               /**< MibSPI RAM SECDED 1-Bit ECC Error
                                              forcing tests (to be used with API
                                              @ref SL_SelfTest_MibSPI)
                                             */
    MIBSPI_ECC_TEST_MODE_2BIT,               /**< MibSPI RAM SECDED 2-Bit ECC Error
                                              forcing tests (to be used with API
                                              @ref SL_SelfTest_MibSPI)
                                             */
    MIBSPI_ECC_TEST_MODE_1BIT_FAULT_INJECT,  /**< MibSPI RAM SECDED 1-Bit ECC Error
                                              Fault injection(to be used with API
                                              @ref SL_SelfTest_MibSPI)
                                             */
    MIBSPI_ECC_TEST_MODE_2BIT_FAULT_INJECT,   /**< MibSPI RAM SECDED 2-Bit ECC Error
                                              Fault injection (to be used with API
                                              @ref SL_SelfTest_MibSPI)
                                             */
	MIBSPI_ANALOG_LOOPBACK_TEST,			/**< MibSPI Analog loopback test
	 	 	 	 	 	 	 	 	 	 	 	 (to be used with API
                                              @ref SL_SelfTest_MibSPI)
                                             */

    PSCON_SELF_TEST,                           /**< PSCON self test using Compare match
    											& mismatch tests (to be used with API @ref
    											SL_SelfTest_PSCON)
    											*/
    PSCON_ERROR_FORCING,                           /**< PSCON Error-Forcing self test mode (to be used with API @ref SL_SelfTest_PSCON)*/
    PSCON_ERROR_FORCING_FAULT_INJECT,               /**< PSCON Error-Forcing fault inject(to be used with API @ref SL_SelfTest_PSCON) */
    PSCON_SELF_TEST_ERROR_FORCING,                /**< PSCON Self-Test Error forcing mode (to be used with API @ref SL_SelfTest_PSCON)*/
    PSCON_SELF_TEST_ERROR_FORCING_FAULT_INJECT,    /**< PSCON Self-Test Error forcing fault inject(to be used with API @ref SL_SelfTest_PSCON) */
    PSCON_PMA_TEST,

	MEMINTRCNT_RESERVED_ACCESS,					/**< Memory Interconnect test of accessing a
												reserved location on memory interconnect
												(to be used with API @ref
												SL_SelfTest_MemoryInterconnect)
												*/
	MEMINTRCNT_SELFTEST,						/**< Memory Interconnect selftest
												(to be used with API @ref
												SL_SelfTest_MemoryInterconnect)
												*/
	MAINPERIPHINTRCNT_RESERVED_ACCESS,			/**< Main Perpheral Interconnect test of accessing a
												reserved location on memory interconnect
												(to be used with API @ref
												SL_SelfTest_MemoryInterconnect)
												*/

	PERIPHSEGINTRCNT_RESERVED_ACCESS,			/**< Peripheral Segment Interconnect test of accessing a
												reserved location on memory interconnect
												(to be used with API @ref
												SL_SelfTest_MemoryInterconnect)
												*/

	PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS,		/**< Peripheral Segment Interconnect test of accessing a
												protected location on memory interconnect
												(to be used with API @ref
												SL_SelfTest_MemoryInterconnect)*/

	ADC_SELFTEST_ALL,

	ADC_SRAM_PARITY_TEST,						/**< Parity test on ADC SRAM
												(to be used with API @ref
												SL_SelfTest_ADC)*/

    VIM_SOFTWARE_TEST							/**< VIM Periodic software test
												(to be used with API
                                                @ref SL_SelfTest_VIM)
                                                */
#endif

} SL_SelfTestType;

/** \defgroup grp_testype_range SRAM Range defines
 *  Following macros are used to define the range of TCM RAM
 *
 */
/**@{*/
/*SAFETYMCUSW 79 S MR:19.4 <APPROVED> Comment_2*/
#define TESTTYPE_MIN          SRAM_ECC_ERROR_FORCING_1BIT     /**< starting testtype */
/*SAFETYMCUSW 79 S MR:19.4 <APPROVED> "Reason -  Reviewed and the value assigned is an enumeration"*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define TESTTYPE_MAX          LIN_ANALOG_LOOPBACK_TEST     /**< ending testtype */
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define TESTTYPE_MAX          VIM_SOFTWARE_TEST          /**< ending testtype */
#endif
/**@}*/

/**
  * \enum     SL_SelfTest_Result
  *
  * \brief    Enumberation to indicate the status of a self test
  */
typedef enum _SL_SelfTest_Result {
    ST_FAIL   = 0xAB,
    ST_PASS   = 0xCD
} SL_SelfTest_Result;


/** \defgroup PBIST_RAMGroup Macros for PBIST RAM groups
 *
 *  Enumerations for selecting the RAM Groups parameters for SL_SelfTest_PBIST API
 *  Naming Convention: PBIST_RAMGROUP_<RAM Group Select#>_<Peripheral Abbreviation>
 *
 *  Note : For details on RAM Group, Algorithm Mapping and supported types refer to device TRM
 */
/**@{*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define PBIST_RAMGROUP_01_PBIST_ROM  (uint32)0x00000001u  /**< PBIST ROM                  */
#define PBIST_RAMGROUP_02_STC_ROM    (uint32)0x00000002u  /**< STC ROM                    */
#define PBIST_RAMGROUP_03_DCAN1      (uint32)0x00000004u  /**< DCAN1 2 Port RAM           */
#define PBIST_RAMGROUP_04_DCAN2      (uint32)0x00000008u  /**< DCAN2 2 Port RAM           */
#define PBIST_RAMGROUP_05_DCAN3      (uint32)0x00000010u  /**< DCA3 2 Port RAM            */
#define PBIST_RAMGROUP_06_ESRAM1     (uint32)0x00000020u  /**< ESRAM1 Single Port RAM     */
#define PBIST_RAMGROUP_07_MIBSPI1    (uint32)0x00000040u  /**< MIBSPI1 2 Port RAM         */
#define PBIST_RAMGROUP_08_MIBSPI3    (uint32)0x00000080u  /**< MIBSPI3 2 Port RAM         */
#define PBIST_RAMGROUP_09_MIBSPI5    (uint32)0x00000100u  /**< MIBSPI5 2 Port RAM         */
#define PBIST_RAMGROUP_10_VIM        (uint32)0x00000200u  /**< VIM 2 Port RAM             */
#define PBIST_RAMGROUP_11_MIBADC1    (uint32)0x00000400u  /**< MIBADC1 Port RAM           */
#define PBIST_RAMGROUP_12_DMA        (uint32)0x00000800u  /**< DMA Port RAM               */
#define PBIST_RAMGROUP_13_N2HET1     (uint32)0x00001000u  /**< N2HET1 2 Port RAM          */
#define PBIST_RAMGROUP_14_HETTU1     (uint32)0x00002000u  /**< HETTU1 2 Port RAM          */
#define PBIST_RAMGROUP_15_RTP        (uint32)0x00004000u  /**< RTP 2 Port RAM             */
#define PBIST_RAMGROUP_16_FRAY       (uint32)0x00008000u  /**< FlexRay 2 Port RAM         */
#define PBIST_RAMGROUP_17_FRAY       (uint32)0x00010000u  /**< FlexRay 1 Port RAM         */
#define PBIST_RAMGROUP_18_MIBADC2    (uint32)0x00020000u  /**< MIBAD2 2 Port RAM          */
#define PBIST_RAMGROUP_19_N2HET2     (uint32)0x00040000u  /**< N2HET2 2 Port RAM          */
#define PBIST_RAMGROUP_20_HETTU2     (uint32)0x00080000u  /**< HETTU2 2 Port RAM          */
#define PBIST_RAMGROUP_21_ESRAM5     (uint32)0x00100000u  /**< ESRAM5 Single Port RAM     */
#define PBIST_RAMGROUP_22_ESRAM6     (uint32)0x00200000u  /**< ESRAM6 Single Port RAM     */
#define PBIST_RAMGROUP_23_ETHERNET1  (uint32)0x00400000u  /**< Ethernet 2 Port RAM        */
#define PBIST_RAMGROUP_24_ETHERNET2  (uint32)0x00800000u  /**< Ethernet 2 Port RAM        */
#define PBIST_RAMGROUP_25_ETHERNET3  (uint32)0x01000000u  /**< Ethernet 1 Port RAM        */
#define PBIST_RAMGROUP_26_USB1       (uint32)0x02000000u  /**< USB 2 Port RAM             */
#define PBIST_RAMGROUP_27_USB2       (uint32)0x04000000u  /**< USB 1 Port RAM             */
#define PBIST_RAMGROUP_28_ESRAM8     (uint32)0x08000000u  /**< ESRAM8 Single Port RAM     */
#define PBIST_RAMGROUP_29_RSVD       (uint32)0x10000000u  /**< --- Reserved               */
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define PBIST_RAMGROUP_01_PBIST_ROM                (uint64)0x0000000001u /**<  */
#define    PBIST_RAMGROUP_02_STC1_1_ROM_R5            (uint64)0x0000000002u /**<  */
#define    PBIST_RAMGROUP_03_STC1_2_ROM_R5            (uint64)0x0000000004u /**<  */
#define    PBIST_RAMGROUP_04_STC2_ROM_NHET            (uint64)0x0000000008u /**<  */
#define    PBIST_RAMGROUP_05_AWM1                    (uint64)0x0000000010u /**<  */
#define    PBIST_RAMGROUP_06_DCAN1                    (uint64)0x0000000020u /**<  */
#define    PBIST_RAMGROUP_07_DCAN2                    (uint64)0x0000000040u /**<  */
#define    PBIST_RAMGROUP_08_DMA                    (uint64)0x0000000080u /**<  */
#define    PBIST_RAMGROUP_09_HTU1                    (uint64)0x0000000100u /**<  */
#define    PBIST_RAMGROUP_10_MIBSPI1                (uint64)0x0000000200u /**<  */
#define    PBIST_RAMGROUP_11_MIBSPI2                (uint64)0x0000000400u /**<  */
#define    PBIST_RAMGROUP_12_MIBSPI3                (uint64)0x0000000800u /**<  */
#define    PBIST_RAMGROUP_13_NHET1                    (uint64)0x0000001000u /**<  */
#define    PBIST_RAMGROUP_14_VIM                    (uint64)0x0000002000u /**<  */
#define    PBIST_RAMGROUP_15_RSVD                    (uint64)0x0000004000u /**<  */
#define    PBIST_RAMGROUP_16_RTP                    (uint64)0x0000008000u /**<  */
#define    PBIST_RAMGROUP_17_ATB                    (uint64)0x0000010000u /**<  */
#define    PBIST_RAMGROUP_18_AWM2                    (uint64)0x0000020000u /**<  */
#define    PBIST_RAMGROUP_19_DCAN3                    (uint64)0x0000040000u /**<  */
#define    PBIST_RAMGROUP_20_DCAN4                    (uint64)0x0000080000u /**<  */
#define    PBIST_RAMGROUP_21_HTU2                    (uint64)0x0000100000u /**<  */
#define    PBIST_RAMGROUP_22_MIBSPI4                (uint64)0x0000200000u /**<  */
#define    PBIST_RAMGROUP_23_MIBSPI5                (uint64)0x0000400000u /**<  */
#define    PBIST_RAMGROUP_24_NHET2                    (uint64)0x0000800000u /**<  */
#define    PBIST_RAMGROUP_25_FTU                    (uint64)0x0001000000u /**<  */
#define    PBIST_RAMGROUP_26_FRAY_INBUF_OUTBUF        (uint64)0x0002000000u /**<  */
#define    PBIST_RAMGROUP_27_CPGMAC_STATE_RXADDR    (uint64)0x0004000000u /**<  */
#define    PBIST_RAMGROUP_28_CPGMAC_STAT_FIFO        (uint64)0x0008000000u /**<  */
#define    PBIST_RAMGROUP_29_L2RAMW                (uint64)0x0010000000u /**<  */
#define    PBIST_RAMGROUP_30_L2RAMW                (uint64)0x0020000000u /**<  */
#define    PBIST_RAMGROUP_31_R5_ICACHE                (uint64)0x0040000000u /**<  */
#define    PBIST_RAMGROUP_32_R5DCACHE                (uint64)0x0080000000u /**<  */
#define    PBIST_RAMGROUP_33_RSVD                    (uint64)0x0100000000u /**<  */
#define    PBIST_RAMGROUP_34_RSVD                    (uint64)0x0200000000u /**<  */
#define    PBIST_RAMGROUP_35_FRAY_TRBUF_MSGRAM        (uint64)0x0400000000u /**<  */
#define    PBIST_RAMGROUP_36_CPGMAC_CPPI            (uint64)0x0800000000u /**<  */
#define    PBIST_RAMGROUP_37_R5_DCACHE_DIRTY        (uint64)0x1000000000u /**<  */
#define    PBIST_RAMGROUP_38_RSVD                    (uint64)0x2000000000u /**<  */
#endif
/**@}*/



/** \defgroup PBIST_Algorithm Macros for PBIST Algorithms
 *
 *  Enumerations for selecting the PBIST algorith for SL_SelfTest_PBIST API
 *  Naming Convention: PBISTALGO_<Algo name>_<1/2>PORT
 *
 *  For details on Algorithms, Algorithm Mapping and supported types refer to device TRM
 *
 */
/**@{*/
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define PBISTALGO_TRIPLE_READ_SLOW_READ      (uint32)0x00000001u  /**<TRIPLE_READ_SLOW_READ  for PBIST and STC ROM*/
#define PBISTALGO_TRIPLE_READ_FAST_READ      (uint32)0x00000002u  /**<TRIPLE_READ_FAST_READ  for PBIST and STC ROM*/
#define PBISTALGO_MARCH13N_RED_2PORT         (uint32)0x00000004u  /**< March13 N Algo for 2 Port mem --- BASELINE TEST ALGO with HIGHEST OVERALL COVERAGE */
#define PBISTALGO_MARCH13N_RED_1PORT         (uint32)0x00000008u  /**< March13 N Algo for 1 Port mem --- BASELINE TEST ALGO with HIGHEST OVERALL COVERAGE */
#define PBISTALGO_DOWN1A_RED_2PORT           (uint32)0x00000010u  /**< Down1a Algor forces the switching fo all data bits & most addr bits on consecutive read cycles */
#define PBISTALGO_DOWN1A_RED_1PORT           (uint32)0x00000020u  /**< Down1a Algor forces the switching fo all data bits & most addr bits on consecutive read cycles */
#define PBISTALGO_MAPCOLUMN_2PORT            (uint32)0x00000040u  /**< Map Column algo (to identify bit line senstivities) for 2 Port memory */
#define PBISTALGO_MAPCOLUMN_1PORT            (uint32)0x00000080u  /**< Map Column algo (to identify bit line senstivities) for 1 Port memory */
#define PBISTALGO_PRECHARGE_2PORT            (uint32)0x00000100u  /**< Pre-Charge algo to exercise pre-charge capability for 2 port memory */
#define PBISTALGO_PRECHARGE_1PORT            (uint32)0x00000200u  /**< Pre-Charge algo to exercise pre-charge capability for 1 port memory */
#define PBISTALGO_DTXN2_2PORT                (uint32)0x00000400u  /**< Global column decode logic algo for 2 Port memories*/
#define PBISTALGO_DTXN2_1PORT                (uint32)0x00000800u  /**< Global column decode logic algo for 1 Port memories*/
#define PBISTALGO_PMOS_OPEN_2PORT            (uint32)0x00001000u  /**<pmos oper algo for 2 port memories*/
#define PBISTALGO_PMOS_OPEN_1PORT            (uint32)0x00002000u  /**<pmos oper algo for 1 port memories*/
#define PBISTALGO_PMOS_OP_SLICE1_2PORT       (uint32)0x00004000u  /**<pmos open slice1 for 2port memories*/
#define PBISTALGO_PMOS_OP_SLICE2_2PORT       (uint32)0x00008000u  /**<pmos open slice2 for 2port memories*/
#define PBISTALGO_FLIP10_2PORT               (uint32)0x00010000u  /**<flip10 algo for 2 port memories*/
#define PBISTALGO_FLIP10_1PORT               (uint32)0x00020000u  /**<flip10  algo for 1 port memories*/
#define PBISTALGO_IDDQ_2PORT                 (uint32)0x00040000u  /**<iddq  algo for 2 port memories*/
#define PBISTALGO_IDDQ_1PORT                 (uint32)0x00080000u  /**<iddq  algo for 1 port memories*/
#define PBISTALGO_RETENTION_2PORT            (uint32)0x00100000u  /**<retention  algo for 2 port memories*/
#define PBISTALGO_RETENTION_1PORT            (uint32)0x00200000u  /**<retention  algo for 1 port memories*/
#define PBISTALGO_IDDQ2_2PORT                (uint32)0x00400000u  /**<iddq2  algo for 2 port memories*/
#define PBISTALGO_IDDQ2_1PORT                (uint32)0x00800000u  /**<iddq2  algo for 1 port memories*/
#define PBISTALGO_RETENTION2_2PORT           (uint32)0x01000000u  /**<retention2  algo for 2 port memories*/
#define PBISTALGO_RETENTION2_1PORT           (uint32)0x02000000u  /**<retention2  algo for 1 port memories*/
#define PBISTALGO_IDDQROWSTRIPE_2PORT        (uint32)0x04000000u  /**<iddqwstripe  algo for 2 port memories*/
#define PBISTALGO_IDDQROWSTRIPE_1PORT        (uint32)0x08000000u  /**<iddqwstripe  algo for 1 port memories*/
#define PBISTALGO_IDDQROWSTRIPE2_2PORT       (uint32)0x10000000u  /**<iddqwstripe2  algo for 2 port memories*/
#define PBISTALGO_IDDQROWSTRIPE2_1PORT       (uint32)0x20000000u  /**<iddqwstripe2  algo for 1 port memories*/
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define PBISTALGO_TRIPLE_READ_SLOW_READ      (uint32)0x00000001u  /**<TRIPLE_READ_SLOW_READ  for PBIST and STC ROM*/
#define PBISTALGO_TRIPLE_READ_FAST_READ      (uint32)0x00000002u  /**<TRIPLE_READ_SLOW_READ  for PBIST and STC ROM*/
#define PBISTALGO_MARCH13N_2PORT             (uint32)0x00000004u  /**< March13 N Algo for 2 Port mem --- BASELINE TEST ALGO with HIGHEST OVERALL COVERAGE */
#define PBISTALGO_MARCH13N_1PORT             (uint32)0x00000008u  /**< March13 N Algo for 1 Port mem --- BASELINE TEST ALGO with HIGHEST OVERALL COVERAGE */
#define PBISTALGO_DOWN2_1PORT                (uint32)0x00000010u  /**<  */
#define PBISTALGO_DOWN2_2PORT                (uint32)0x00000020u  /**<  */
#define PBISTALGO_MARCH_DIS_INC_1PORT        (uint32)0x00000040u  /**<  */
#define PBISTALGO_MARCH_DIS_INC_2PORT        (uint32)0x00000080u  /**<  */
#define PBISTALGO_MARCH_DIS_DEC_1PORT        (uint32)0x00000100u  /**<  */
#define PBISTALGO_MARCH_DIS_DEC_2PORT        (uint32)0x00000200u  /**<  */
#define PBISTALGO_MARCH_DIS_INC_1PORT_1      (uint32)0x00000400u  /**<  */
#define PBISTALGO_MARCH_DIS_INC_2PORT_1      (uint32)0x00000800u  /**<  */
#define PBISTALGO_MARCH_DIS_DEC_1PORT_1      (uint32)0x00001000u  /**<  */
#define PBISTALGO_MARCH_DIS_DEC_2PORT_1      (uint32)0x00002000u  /**<  */
#define PBISTALGO_MAP_COLUMN_1PORT           (uint32)0x00004000u  /**< Map Column algo (to identify bit line senstivities) for 1 Port memory */
#define PBISTALGO_MAP_COLUMN_2PORT           (uint32)0x00008000u  /**< Map Column algo (to identify bit line senstivities) for 2 Port memory */
#define PBISTALGO_PRECHARGE_1PORT            (uint32)0x00010000u  /**< Pre-Charge algo to exercise pre-charge capability for 1 port memory */
#define PBISTALGO_PRECHARGE_2PORT            (uint32)0x00020000u  /**< Pre-Charge algo to exercise pre-charge capability for 2 port memory */
#define PBISTALGO_FLIP10_1PORT               (uint32)0x00040000u  /**<  */
#define PBISTALGO_FLIP10_2PORT               (uint32)0x00080000u  /**<  */
#define PBISTALGO_DTXN2_1PORT                (uint32)0x00100000u  /**< Global column decode logic algo for 1 Port memories*/
#define PBISTALGO_DTXN2_2PORT                (uint32)0x00200000u  /**< Global column decode logic algo for 2 Port memories*/
#define PBISTALGO_PMOS_OP_SLICE1_1PORT       (uint32)0x00400000u  /**<pmos open slice1 for 1port memories*/
#define PBISTALGO_PMOS_OP_SLICE1_2PORT       (uint32)0x00800000u  /**<pmos open slice1 for 2port memories*/
#define PBISTALGO_PMOS_OP_SLICE2_2PORT       (uint32)0x01000000u  /**<pmos open slice2 for 2port memories*/
#endif
/**@}*/

/**
  * \struct SL_PBIST_ExecStatus
  *
  * \brief Structure used to return the execution status of PBIST. This is used
  *         as a parmeter for the SL_SelfTest_PBIST_ExecStatus API.
  *
  */
typedef struct _SL_PBIST_FailInfo {
    SL_SelfTest_Result  stResult;           /**< Overall test status. TRUE if PBIST tests passed else FALSE */
    uint8               port0FailCount;     /**< Indicates number of failures on Port0 */
    uint8               port1FailCount;     /**< Indicates number for failures on Port1 */
    uint16              port0FailAddress;   /**< Contains address of first failure on Port0 */
    uint16              port1FailAddress;   /**< Contains address of first failure on Port1 */
    uint32              port0FailData;      /**< Failure data on Port0 */
    uint32              port1FailData;      /**< Failure data on Port1 */
} SL_PBIST_FailInfo;



/** \struct SL_STC_Config
  *
  * \brief STC Configuration information to be used with SL_SelfTest_STC API
  *
  */
typedef struct _SL_STC_Config {
    uint8        stcClockDiv;        /**< Supported values 0-7 */
    uint16         intervalCount;      /**< Max of 24 - Specifies the number of intervals to be ran from the value reflected in the current interval counter */
    boolean        restartInterval0;   /**< When TRUE to continue the run from next interval onwards or FALSE to restart from interval 0. */
    uint32         timeoutCounter;     /**<  Number of VBUS clock cycles to take before triggering timeout error (starting from intiation of run) */
} SL_STC_Config;



/** \struct SL_STC_FailInfo
  *
  * \brief STC failure information. Used in conjunction with SL_SelfTest_STC_Status API.
  *
  */
typedef struct _SL_STC_FailInfo {
    SL_SelfTest_Result    stResult;          /**< Result of self-test. */
    SL_SelfTest_Result     CPU1Failure;       /**< TRUE indicates Self-test run failure due to MISR mismatch for CPU1 */
    SL_SelfTest_Result     CPU2Failure;       /**< TRUE indicates Self-test run failure due to MISR mismatch for CPU2 */
    SL_SelfTest_Result     TimeOutFailure;    /**< TRUE indicates Self-test run failed due to a timeout error */
} SL_STC_FailInfo;


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
/** \struct SL_PSCON_FailInfo
  *
  * \brief PSCON failure information. Used in with SL_SelfTest_PSCON_FailInfo API.
  *
  */
typedef struct _SL_PSCON_FailInfo {
    SL_SelfTest_Result  stResult;         /**< TRUE indicates PSCON Self test failure on Power or Memory domains */
    uint32              pdFailure;         /**< 1 on bits[3-0] indicate self test failure Power domain 5/4/3/2.  bits[19-16]: For PD5/4/3/2, 0 indicates compare match test failure & 1 indicates compare mistach test failure*/
    uint32              mdFailure;         /**< 1 on bits[2-0] indicate self test failure Memory Power domain 3/2/1.  bits[18-16]: For Memory Power Domain 3/2/1, 0 indicates compare match test failure & 1 indicates compare mistach test failure*/
} SL_PSCON_FailInfo;
#endif

/**
  * \enum       SL_EFuse_Error
  *
  * \brief      Error codes returned by EFuse controller when an operation is completed.
  *
  *
  */
typedef enum _SL_EFuse_Error {
  EFUSE_ERROR_NONE              = 0u,  /**< Last operation completed successfully */
  EFUSE_ERROR_TIMEOUT           = 1u,  /**< Controller times out because there is no last row sent from the FuseROM */
  EFUSE_ERROR_AUTOLOAD_1        = 2u,  /**< Autoload machine did not find enough FuseROM data to fill the scan chain */
  EFUSE_ERROR_AUTOLOAD_2        = 3u,  /**< The autoload machine was able to filll the scan chain, but the wrong signature was returned*/
  EFUSE_ERROR_AUTOLOAD_INTRPTD  = 4u,  /**< Autoload machine was not able or not allowed to complete its operation */
  EFUSE_ERROR_AUTOLOAD_2B_ERR   = 5u,  /**< 2Bit error detected during Autoload */
  EFUSE_ERROR_PROG_W_PROTECT    = 6u,  /**< The write protect bit of the chip/row address stored in the controller's address register is set to 1 */
  EFUSE_ERROR_PROG_W_D_ITER     = 7u,  /**< The number of times that the controller tried and failed to write r exceeded the maximum number of iterations allowed (16) */
  EFUSE_ERROR_PROG_CTRL_ITER    = 8u,  /**< The number of times that the controller tried and failed to write the control bits r exceeded the maximum number of iterations allowed (16) */
  EFUSE_ERROR_UNPROG_BIT_SET    = 9u,  /**< A bit was found to be set by the test unprogrammed FuseROM instruction */
  EFUSE_ERROR_RED_REP_ROW       =10u   /**< Two rows where the repair bit is set point to the same repair row. */
 }SL_EFuse_Error;



 /** \struct SL_EFuse_FailInfo
   *
   * \brief EFuse Self-test failure information.
   *
   */
 typedef struct _SL_EFuse_FailInfo {
     SL_SelfTest_Result  stResult;
     SL_EFuse_Error      failInfo;
 } SL_EFuse_FailInfo;


/**
  * \enum       SL_EFuse_Config
  *
  * \brief      Configuration parameters for eFuse ECC Self test
  *
  */
typedef struct _SL_EFuse_Config {
    uint32              numPatterns;        /**< Number of patterns/clock-cycles to be generated. The coverage saturates around 600 patterns at 99.6% */
    uint32              seedSignature;      /**< 32bit sginature as input to the eFuse self-test module */
    SL_EFuse_FailInfo      failInfo;
} SL_EFuse_Config;

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
typedef enum _SL_CCMR4F_Error {
   CCMR4F_ST_ERR_COMPARE_MATCH     = 1u,
   CCMR4F_ST_ERR_COMPARE_MISMATCH  = 2u
 } SL_CCMR4F_Error;

typedef struct _SL_CCMR4F_FailInfo {
    SL_SelfTest_Result  stResult;
    SL_CCMR4F_Error     failInfo;
}SL_CCMR4F_FailInfo;
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
typedef enum _SL_CCMR5F_Error {
    CCMR5F_ST_ERR_COMPARE_MATCH     = 1u,
    CCMR5F_ST_ERR_COMPARE_MISMATCH  = 2u
} SL_CCMR5F_Error;

typedef struct _SL_CCMR5F_FailInfo {
    SL_SelfTest_Result  stResult;
    SL_CCMR5F_Error     failInfo;
}SL_CCMR5F_FailInfo;

#endif


/**
 * \enum       SL_ADC_Config
 *
 * \brief      Configuration parameters for ADC self test
 *
 */
typedef struct _SL_ADC_Config {
    adcBASE_t* adcbase;    /**<base address of the adc used*/
    uint8 adc_channel;     /**<channel on which the self test needs to be performed*/
} SL_ADC_Config;

/** @enum adcGroups
 *   @brief Alias name for ADC event group
 *
 *   @note These values should be used for API argument @a group
 */
typedef enum _SL_ADC_Pinstatus {
    ADC_PIN_GOOD,                     /**<The pinstate of adc is good and is usable*/
    ADC_PIN_SHORTED_TO_ADVREF_HIGH,    /**<The pin of the ADC is shorted to ADVREF High*/
    ADC_PIN_SHORTED_TO_ADVREF_LOW,    /**<The pin of the ADC is shorted to ADVREF Low*/
    ADC_PIN_OPEN,                    /**<The pin of the ADC pin is open*/
    ADC_PIN_UNDETERMINED            /**<The pin of the ADC pin state is undetermined*/
} SL_ADC_Pinstatus;



#endif /* __SL_TYPES_H__ */

/*Comment_1:
 * "Reason -  ESM callback changes at runtime"*/
 
/*Comment_2:
 * "Reason -  Reviewed and the value assigned is an enumeration"*/
