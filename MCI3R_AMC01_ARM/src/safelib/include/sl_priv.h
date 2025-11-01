/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
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
 * @file        sl_priv.h
 * @version     2.3.1
 * @brief       Private header file for SafeTI&trade; Hercules&trade; Diagnostic Library
 *
 * @details Contains interfaces/definitions that are private to SafeTI&trade; Hercules&trade; Diagnostic library & are not\n
 *  exposed to application.
 */
#ifndef __SL_PRIV_H__
#define __SL_PRIV_H__

#include <sl_types.h>
#include <sl_config.h>
#include <sl_api.h>

#include <HL_reg_system.h>

extern boolean vimTestFlag; /* global variable used for VIM software test */

/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BIT(n)                  ((uint32)((uint32)1u <<(n)))

/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BIT_SET(y, mask)        ((y) |= (mask))

/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BIT_CLEAR(y, mask)      ((y) &= ~(mask))

/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BIT_FLIP(y, mask)       ((y) ^= (mask))

/* Create a bitmask of length \a len.*/
/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BIT_MASK(len)           (uint32)(BIT(len)-(uint32)1u)

/* Create a bitfield mask of length \a starting at bit \a start.*/
/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BF_MASK(start, len)     (BIT_MASK(len)<<(start))

/* Prepare a bitmask for insertion or combining.*/
/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BF_PREP(x, start, len)  (((x)&BIT_MASK(len)) << (start))

/*SAFETYMCUSW 79 S MR:19.4 <APPROVED> "Use of ternary operator is allowed" */
/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define GET_ESM_BIT_NUM(x)		((x)<(uint32)32U? (BIT((x))): ((x)<(uint32)64u?(BIT(((x)-((uint32)32U)))):(BIT(((x)-((uint32)64u))))))
/* Extract a bitfield of length \a len starting at bit \a start from \a y.
 * y is the register
 * start is the starting bit of the field
 * len is the length of the field*/
/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BF_GET(y, start, len)   ((uint32)(((uint32)((y)>>(start))) & BIT_MASK(len)))

/*Insert a new bitfield value \a x into \a y.
 * y is the register
 * x is the value of the field
 * start is the starting bit of the field
 * len is the length of the field*/
/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define BF_SET(y, x, start, len)    \
    ( (y) = ((y) & (~(BF_MASK((start),(len))))) | BF_PREP((x),(start),(len)))


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define _SELFTEST_SRAMECC_MIN       ((SL_SelfTestType)SRAM_ECC_ERROR_FORCING_1BIT)
#define _SELFTEST_SRAMECC_MAX       ((SL_SelfTestType)SRAM_LIVELOCK_DIAGNOSTICS)

#define _SELFTEST_FLASHECC_MIN      ((SL_SelfTestType)FLASH_ECC_ADDR_TAG_REG_MODE)
#define _SELFTEST_FLASHECC_MAX      ((SL_SelfTestType)FLASH_ECC_TEST_MODE_2BIT_FAULT_INJECT)

#define _SELFTEST_FEEECC_MIN		((SL_SelfTestType)FEE_ECC_DATA_CORR_MODE)
#define _SELFTEST_FEEECC_MAX		((SL_SelfTestType)FEE_ECC_TEST_MODE_2BIT_FAULT_INJECT)

#define _SELFTEST_PBIST_MIN         ((SL_SelfTestType)PBIST_EXECUTE)
#define _SELFTEST_PBIST_MAX         ((SL_SelfTestType)PBIST_EXECUTE_OVERRIDE)

#define _SELFTEST_STC_MIN           ((SL_SelfTestType)STC_RUN)
#define _SELFTEST_STC_MAX           ((SL_SelfTestType)STC_COMPARE_SELFCHECK)

#define _SELFTEST_PSCON_MIN         (SL_SelfTestType)(PSCON_SELF_TEST)
#define _SELFTEST_PSCON_MAX         ((SL_SelfTestType)PSCON_PMA_TEST)

#define _SELFTEST_EFUSE_MIN         ((SL_SelfTestType)EFUSE_SELF_TEST_AUTOLOAD)
#define _SELFTEST_EFUSE_MAX         ((SL_SelfTestType)EFUSE_SELF_TEST_STUCK_AT_ZERO)

#define _SELFTEST_CCMR4F_MIN        ((SL_SelfTestType)CCMR4F_SELF_TEST)
#define _SELFTEST_CCMR4F_MAX        ((SL_SelfTestType)CCMR4F_SELF_TEST_ERROR_FORCING)

#define _SELFTEST_L2L3INTERCONNECT_MIN        ((SL_SelfTestType)L2INTERCONNECT_RESERVED_ACCESS)
#define _SELFTEST_L2L3INTERCONNECT_MAX        ((SL_SelfTestType)L3INTERCONNECT_UNPRIVELEGED_ACCESS)

#define _SELFTEST_DMA_MIN        ((SL_SelfTestType)DMA_SRAM_PARITY_TEST)
#define _SELFTEST_DMA_MAX        ((SL_SelfTestType)DMA_SOFTWARE_TEST)

#define _SELFTEST_VIM_MIN        ((SL_SelfTestType)VIM_SRAM_PARITY_TEST)
#define _SELFTEST_VIM_MAX        ((SL_SelfTestType)VIM_SOFTWARE_TEST)

#define _SELFTEST_HET_MIN        ((SL_SelfTestType)HET_SRAM_PARITY_TEST)
#define _SELFTEST_HET_MAX        ((SL_SelfTestType)HET_ANALOG_LOOPBACK_TEST)

#define HET_MIN                   ((SL_HET_Instance)SL_HET1)
#define HET_MAX                   ((SL_HET_Instance)SL_HET2)


#define HET1_PIN_MIN                   ((uint32)0U)
#define HET1_PIN_MAX                   ((uint32)31U)

#define HET2_PIN_MIN                   ((uint32)0U)
#define HET2_PIN_MAX                   ((uint32)18U)


#define _SELFTEST_HTU_MIN        ((SL_SelfTestType)HTU_SRAM_PARITY_TEST)
#define _SELFTEST_HTU_MAX        ((SL_SelfTestType)HTU_SRAM_PARITY_TEST)

#define HTU_MIN                   ((SL_HTU_Instance)SL_HTU1)
#define HTU_MAX                   ((SL_HTU_Instance)SL_HTU2)


#define _SELFTEST_MIBSPI_MIN        ((SL_SelfTestType)MIBSPI_SRAM_PARITY_TEST)
#define _SELFTEST_MIBSPI_MAX        ((SL_SelfTestType)MIBSPI_ANALOG_LOOPBACK_TEST)

#define MIBSPI_MIN                   ((SL_MIBSPI_Instance)SL_MIBSPI1)
#define MIBSPI_MAX                   ((SL_MIBSPI_Instance)SL_MIBSPI5)

#define SPI_MIN                   ((SL_SPI_Instance)SL_SPI1)
#define SPI_MAX                   ((SL_SPI_Instance)SL_SPI5)

#define SCI_MIN                   ((SL_SCI_Instance)SL_SCI1)
#define SCI_MAX                   ((SL_SCI_Instance)SL_SCI4)

#define _SELFTEST_SPI_MIN        ((SL_SelfTestType)SPI_ANALOG_LOOPBACK_TEST)
#define _SELFTEST_SPI_MAX        ((SL_SelfTestType)SPI_ANALOG_LOOPBACK_TEST)

#define _SELFTEST_CAN_MIN        ((SL_SelfTestType)CAN_SRAM_PARITY_TEST)
#define _SELFTEST_CAN_MAX        ((SL_SelfTestType)CAN_SRAM_PARITY_TEST)

#define DCAN_MIN                    1
#define DCAN_MAX                    3

#define _SELFTEST_GIO_MIN        ((SL_SelfTestType)GIO_ANALOG_LOOPBACK_TEST)
#define _SELFTEST_GIO_MAX        ((SL_SelfTestType)GIO_ANALOG_LOOPBACK_TEST)

#define GIO_PORT_MIN                    1
#define GIO_PORT_MAX                    2

#define _SELFTEST_SCI_MIN        ((SL_SelfTestType)SCI_ANALOG_LOOPBACK_TEST)
#define _SELFTEST_SCI_MAX        ((SL_SelfTestType)SCI_ANALOG_LOOPBACK_TEST)

#define _SELFTEST_LIN_MIN        ((SL_SelfTestType)LIN_ANALOG_LOOPBACK_TEST)
#define _SELFTEST_LIN_MAX        ((SL_SelfTestType)LIN_ANALOG_LOOPBACK_TEST)

#define _SELFTEST_ADC_MIN        ((SL_SelfTestType)ADC_SELFTEST_ALL)
#define _SELFTEST_ADC_MAX        ((SL_SelfTestType)ADC_SRAM_PARITY_TEST)

#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define _SELFTEST_SRAMECC_MIN       ((SL_SelfTestType)SRAM_ECC_ERROR_FORCING_1BIT)
#define _SELFTEST_SRAMECC_MAX       ((SL_SelfTestType)SRAM_RADECODE_DIAGNOSTICS)

#define _SELFTEST_FLASHECC_MIN      ((SL_SelfTestType)FLASH_ECC_TEST_MODE_1BIT)
#define _SELFTEST_FLASHECC_MAX      ((SL_SelfTestType)FLASH_ECC_TEST_MODE_2BIT)

#define _SELFTEST_PBIST_MIN         ((SL_SelfTestType)PBIST_EXECUTE)
#define _SELFTEST_PBIST_MAX         ((SL_SelfTestType)PBIST_EXECUTE_OVERRIDE)

#define _SELFTEST_PSCON_MIN         ((SL_SelfTestType)PSCON_SELF_TEST)
#define _SELFTEST_PSCON_MAX         ((SL_SelfTestType)PSCON_PMA_TEST)

#define _SELFTEST_EFUSE_MIN         ((SL_SelfTestType)EFUSE_SELF_TEST_AUTOLOAD)
#define _SELFTEST_EFUSE_MAX         ((SL_SelfTestType)EFUSE_SELF_TEST_STUCK_AT_ZERO)

#define _SELFTEST_CCMR5F_MIN        ((SL_SelfTestType)CCMR5F_CPUCOMP_SELF_TEST)
#define _SELFTEST_CCMR5F_MAX        ((SL_SelfTestType)CCMR5F_INMCOMP_SELF_TEST_ERROR_FORCING)

#define _SELFTEST_DMA_MIN        ((SL_SelfTestType)DMA_ECC_TEST_MODE_1BIT)
#define _SELFTEST_DMA_MAX        ((SL_SelfTestType)DMA_SOFTWARE_TEST)

#define _SELFTEST_CAN_MIN        ((SL_SelfTestType)CAN_ECC_TEST_MODE_1BIT)
#define _SELFTEST_CAN_MAX        ((SL_SelfTestType)CAN_ECC_TEST_MODE_2BIT_FAULT_INJECT)

#define _SELFTEST_SCI_MIN        ((SL_SelfTestType)SCI_ANALOG_LOOPBACK_TEST)
#define _SELFTEST_SCI_MAX        ((SL_SelfTestType)SCI_ANALOG_LOOPBACK_TEST)

#define _SELFTEST_LIN_MIN        ((SL_SelfTestType)LIN_ANALOG_LOOPBACK_TEST)
#define _SELFTEST_LIN_MAX        ((SL_SelfTestType)LIN_ANALOG_LOOPBACK_TEST)

#define DCAN_MIN                    1
#define DCAN_MAX                    4

#define _SELFTEST_STC_MIN           ((SL_SelfTestType)STC1_RUN)
#define _SELFTEST_STC_MAX           ((SL_SelfTestType)STC2_COMPARE_SELFCHECK)

#define _SELFTEST_MIBSPI_MIN        ((SL_SelfTestType)MIBSPI_ECC_TEST_MODE_1BIT)
#define _SELFTEST_MIBSPI_MAX        ((SL_SelfTestType)MIBSPI_ANALOG_LOOPBACK_TEST)

#define MIBSPI_MIN                   ((SL_MIBSPI_Instance)SL_MIBSPI1)
#define MIBSPI_MAX                   ((SL_MIBSPI_Instance)SL_MIBSPI5)

#define _SELFTEST_MEMINTRCNT_MIN     ((SL_SelfTestType)MEMINTRCNT_RESERVED_ACCESS)
#define _SELFTEST_MEMINTRCNT_MAX     ((SL_SelfTestType)MEMINTRCNT_SELFTEST)

#define _SELFTEST_MAINPERIPHINTRCNT_MIN     ((SL_SelfTestType)MAINPERIPHINTRCNT_RESERVED_ACCESS)
#define _SELFTEST_MAINPERIPHINTRCNT_MAX     ((SL_SelfTestType)MAINPERIPHINTRCNT_RESERVED_ACCESS)

#define _SELFTEST_PERIPHSEGINTRCNT_MIN     ((SL_SelfTestType)PERIPHSEGINTRCNT_RESERVED_ACCESS)
#define _SELFTEST_PERIPHSEGINTRCNT_MAX     ((SL_SelfTestType)PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS)

#define PERIPHSEGINTRCNT_MIN                   ((SL_PeripheralSegment_Instance)SL_PERIPH_SEGMENT1)
#define PERIPHSEGINTRCNT_MAX                   ((SL_PeripheralSegment_Instance)SL_PERIPH_SEGMENT3)

#define _SELFTEST_ADC_MIN        ((SL_SelfTestType)ADC_SRAM_PARITY_TEST)
#define _SELFTEST_ADC_MAX        ((SL_SelfTestType)ADC_SRAM_PARITY_TEST)

#define MIBADC_MIN                   ((SL_MIBADC_Instance)SL_MIBADC1)
#define MIBADC_MAX                   ((SL_MIBADC_Instance)SL_MIBADC2)

#define _SELFTEST_VIM_MIN        ((SL_SelfTestType)VIM_SOFTWARE_TEST)
#define _SELFTEST_VIM_MAX        ((SL_SelfTestType)VIM_SOFTWARE_TEST)

#endif


#define ARM_MODE_USR    (0x10u)
#define ARM_MODE_FIQ    (0x11u)
#define ARM_MODE_IRQ    (0x12u)
#define ARM_MODE_SVC    (0x13u)
#define ARM_MODE_ABT    (0x17u)
#define ARM_MODE_UND    (0x1Bu)
#define ARM_MODE_SYS    (0x1Fu)

#define CCMR4F_EFT_EXEC 0u
#define CCMR4F_STEFT_EXEC 1u
#define CCMR4F_PSCON_PMA 2u

#define ADC_APPROXIMATION_THRESHOLD 5u
#define ADC_FIFO_SIZE 1u
#define ADC_POWERDOWN_MODE         (0x00000100U)
#define ADC_ENABLE                 (0x00000001U)

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define ALL_PBIST_RAM_GROUPS (PBIST_RAMGROUP_01_PBIST_ROM |\
                             PBIST_RAMGROUP_02_STC_ROM   |\
                             PBIST_RAMGROUP_03_DCAN1     |\
                             PBIST_RAMGROUP_04_DCAN2     |\
                             PBIST_RAMGROUP_05_DCAN3     |\
                             PBIST_RAMGROUP_06_ESRAM1    |\
                             PBIST_RAMGROUP_07_MIBSPI1   |\
                             PBIST_RAMGROUP_08_MIBSPI3   |\
                             PBIST_RAMGROUP_09_MIBSPI5   |\
                             PBIST_RAMGROUP_10_VIM       |\
                             PBIST_RAMGROUP_11_MIBADC1   |\
                             PBIST_RAMGROUP_12_DMA       |\
                             PBIST_RAMGROUP_13_N2HET1    |\
                             PBIST_RAMGROUP_14_HETTU1    |\
                             PBIST_RAMGROUP_15_RTP       |\
                             PBIST_RAMGROUP_16_FRAY      |\
                             PBIST_RAMGROUP_17_FRAY      |\
                             PBIST_RAMGROUP_18_MIBADC2   |\
                             PBIST_RAMGROUP_19_N2HET2    |\
                             PBIST_RAMGROUP_20_HETTU2    |\
                             PBIST_RAMGROUP_21_ESRAM5    |\
                             PBIST_RAMGROUP_22_ESRAM6    |\
                             PBIST_RAMGROUP_23_ETHERNET1 |\
                             PBIST_RAMGROUP_24_ETHERNET2 |\
                             PBIST_RAMGROUP_25_ETHERNET3 |\
                             PBIST_RAMGROUP_26_USB1      |\
                             PBIST_RAMGROUP_27_USB2      |\
                             PBIST_RAMGROUP_28_ESRAM8    |\
                             PBIST_RAMGROUP_29_RSVD)

#define ALL_PBISALGO (PBISTALGO_TRIPLE_READ_SLOW_READ |\
                     PBISTALGO_TRIPLE_READ_FAST_READ  |\
                     PBISTALGO_MARCH13N_RED_2PORT     |\
                     PBISTALGO_MARCH13N_RED_1PORT     |\
                     PBISTALGO_DOWN1A_RED_2PORT       |\
                     PBISTALGO_DOWN1A_RED_1PORT       |\
                     PBISTALGO_MAPCOLUMN_2PORT        |\
                     PBISTALGO_MAPCOLUMN_1PORT        |\
                     PBISTALGO_PRECHARGE_2PORT        |\
                     PBISTALGO_PRECHARGE_1PORT        |\
                     PBISTALGO_DTXN2_2PORT            |\
                     PBISTALGO_DTXN2_1PORT            |\
                     PBISTALGO_PMOS_OPEN_2PORT        |\
                     PBISTALGO_PMOS_OPEN_1PORT        |\
                     PBISTALGO_PMOS_OP_SLICE1_2PORT |\
                     PBISTALGO_PMOS_OP_SLICE2_2PORT |\
                     PBISTALGO_FLIP10_2PORT           |\
                     PBISTALGO_FLIP10_1PORT           |\
                     PBISTALGO_IDDQ_2PORT             |\
                     PBISTALGO_IDDQ_1PORT             |\
                     PBISTALGO_RETENTION_2PORT        |\
                     PBISTALGO_RETENTION_1PORT        |\
                     PBISTALGO_IDDQ2_2PORT            |\
                     PBISTALGO_IDDQ2_1PORT            |\
                     PBISTALGO_RETENTION2_2PORT       |\
                     PBISTALGO_RETENTION2_1PORT       |\
                     PBISTALGO_IDDQROWSTRIPE_2PORT    |\
                     PBISTALGO_IDDQROWSTRIPE_1PORT    |\
                     PBISTALGO_IDDQROWSTRIPE2_2PORT   |\
                     PBISTALGO_IDDQROWSTRIPE2_1PORT)
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define ALL_PBIST_RAM_GROUPS (PBIST_RAMGROUP_01_PBIST_ROM |\
					PBIST_RAMGROUP_02_STC1_1_ROM_R5		|\
					PBIST_RAMGROUP_03_STC1_2_ROM_R5		|\
					PBIST_RAMGROUP_04_STC2_ROM_NHET		|\
					PBIST_RAMGROUP_05_AWM1				|\
					PBIST_RAMGROUP_06_DCAN1				|\
					PBIST_RAMGROUP_07_DCAN2				|\
					PBIST_RAMGROUP_08_DMA				|\
					PBIST_RAMGROUP_09_HTU1				|\
					PBIST_RAMGROUP_10_MIBSPI1			|\
					PBIST_RAMGROUP_11_MIBSPI2			|\
					PBIST_RAMGROUP_12_MIBSPI3			|\
					PBIST_RAMGROUP_13_NHET1				|\
					PBIST_RAMGROUP_14_VIM				|\
					PBIST_RAMGROUP_15_RSVD				|\
					PBIST_RAMGROUP_16_RTP				|\
					PBIST_RAMGROUP_17_ATB				|\
					PBIST_RAMGROUP_18_AWM2				|\
					PBIST_RAMGROUP_19_DCAN3				|\
					PBIST_RAMGROUP_20_DCAN4				|\
					PBIST_RAMGROUP_21_HTU2				|\
					PBIST_RAMGROUP_22_MIBSPI4			|\
					PBIST_RAMGROUP_23_MIBSPI5			|\
					PBIST_RAMGROUP_24_NHET2				|\
					PBIST_RAMGROUP_25_FTU				|\
					PBIST_RAMGROUP_26_FRAY_INBUF_OUTBUF	|\
					PBIST_RAMGROUP_27_CPGMAC_STATE_RXADDR |\
					PBIST_RAMGROUP_28_CPGMAC_STAT_FIFO	|\
					PBIST_RAMGROUP_29_L2RAMW			|\
					PBIST_RAMGROUP_30_L2RAMW			|\
					PBIST_RAMGROUP_31_R5_ICACHE			|\
					PBIST_RAMGROUP_32_R5DCACHE			|\
					PBIST_RAMGROUP_33_RSVD				|\
					PBIST_RAMGROUP_34_RSVD				|\
					PBIST_RAMGROUP_35_FRAY_TRBUF_MSGRAM	|\
					PBIST_RAMGROUP_36_CPGMAC_CPPI		|\
					PBIST_RAMGROUP_37_R5_DCACHE_DIRTY	|\
					PBIST_RAMGROUP_38_RSVD)

#define ALL_PBISALGO (PBISTALGO_TRIPLE_READ_SLOW_READ    |\
					PBISTALGO_TRIPLE_READ_FAST_READ      |\
					PBISTALGO_MARCH13N_2PORT             |\
					PBISTALGO_MARCH13N_1PORT             |\
					PBISTALGO_DOWN2_1PORT                |\
					PBISTALGO_DOWN2_2PORT                |\
					PBISTALGO_MARCH_DIS_INC_1PORT    |\
					PBISTALGO_MARCH_DIS_INC_2PORT    |\
					PBISTALGO_MARCH_DIS_DEC_1PORT    |\
					PBISTALGO_MARCH_DIS_DEC_2PORT    |\
					PBISTALGO_MARCH_DIS_INC_1PORT_1  |\
					PBISTALGO_MARCH_DIS_INC_2PORT_1  |\
					PBISTALGO_MARCH_DIS_DEC_1PORT_1  |\
					PBISTALGO_MARCH_DIS_DEC_2PORT_1  |\
					PBISTALGO_MAP_COLUMN_1PORT    	     |\
					PBISTALGO_MAP_COLUMN_2PORT    	     |\
					PBISTALGO_PRECHARGE_1PORT            |\
					PBISTALGO_PRECHARGE_2PORT            |\
					PBISTALGO_FLIP10_1PORT               |\
					PBISTALGO_FLIP10_2PORT               |\
					PBISTALGO_DTXN2_1PORT                |\
					PBISTALGO_DTXN2_2PORT                |\
					PBISTALGO_PMOS_OP_SLICE1_1PORT     |\
					PBISTALGO_PMOS_OP_SLICE1_2PORT     |\
					PBISTALGO_PMOS_OP_SLICE2_2PORT)
#endif

#define ST_BOUNDARY_VALUE ((uint32)(EFC_BOUNDARY_SELF_TEST_ERR | EFC_BOUNDARY_SINGLE_BIT_ERR | \
		EFC_BOUNDARY_INSTR_ERR | EFC_BOUNDARY_AUTOLOAD_ERR))
#define ST_PIN_VALUE      ((uint32)(EFC_PINS_AUTOLOAD_ERR | EFC_PINS_INSTR_ERR | \
		EFC_PINS_SINGLE_BIT_ERR | EFC_PINS_SELF_TEST_ERROR))


#define ROM_CLK_SRC_HCKL_DIV_2  1u
#define ENABLE_OVERRIDE         1u
#define DISABLE_OVERRIDE        0u
#define ALG_RAM_FROM_ROM        3u
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define VBUS_CLK_CYCLES         64u
#endif
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define VBUS_CLK_CYCLES         32u
#endif

/*Bit Fields*/
#define BF_ROM_DIV_START        8u
#define BF_ROM_DIV_LENGTH       2u

/*Bit Fields*/
#define BF_MSTGENA_START              0u
#define BF_MSTGENA_LENGTH             4u
#define ENABLE_MEM_STEST_CTRL         0xau
#define DISABLE_MEM_STEST_CTRL        0x5u

/*Bit Fields*/
#define BF_MINITGENA_START            0u
#define BF_MINITGENA_LENGTH           4u
#define ENABLE_GLOB_MEM_HW_INIT       0xau
#define DISABLE_GLOB_MEM_HW_INIT      0x5u

/*Bit Fields*/
#define BF_MINIDONE_START            (uint32)8u
#define BF_MINIDONE_LENGTH           (uint32)1u
#define MEM_HW_INIT_COMPLETE         (uint32)1u
#define MEM_HW_INIT_INCOMPLETE       0u

/*Bit Fields*/
#define BF_EPSF_START   (uint32)0u
#define BF_EPSF_LENGTH  (uint32)1u

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/*Bit Fields*/
#define BF_CCMR4_CMP_ERROR          2u
#define BF_CCMR4_CMP_ERROR_LENGTH   1u
#define BF_CCMR4_STEST_ERROR        31u
#define BF_CCMR4_STEST_ERROR_LENGTH 1u
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define BF_CCMR5_CPUCOMP_ERROR      2u
#define BF_CCMR5_VIMCOMP_ERROR      25u
#define BF_CCMR5_PDCOMP_ERROR       28u
#define BF_CCMR5_INMCOMP_ERROR      26u
#define BF_CCMR5_CMP_ERROR_LENGTH   1u
#define BF_CCMR5_STEST_ERROR        31u
#define BF_CCMR5_STEST_ERROR_LENGTH 1u
#endif

/*Bit Fields*/
#define BF_SERR_EN_START               0u
#define BF_SERR_EN_LEN                 1u
#define SERR_ENABLE                    1u
#define SERR_DISABLE                   0u

/*Bit Fields*/
#define BF_ECC_DETECT_EN_START         0u
#define BF_ECC_DETECT_EN_LEN           4u
#define ECC_DETECT_ENABLE              5u
#define ECC_DETECT_DISABLE             0u

/*Bit Fields*/
#define BF_ECC_WR_EN_START             8u
#define BF_ECC_WR_EN_LEN               1u
#define ECC_WR_ENABLE                  1u
#define ECC_WR_DISABLE                 0u

/*Bit Fields*/
#define BF_EDACEN_START             0u
#define BF_EDACEN_LEN               4u
#define EDAC_ENABLE_DEF             0xau
#define EDAC_DISABLE                5u

#define BF_EDACMODE_START             16u
#define BF_EDACMODE_LEN               4u
#define EDACMODE_DEF                  0xau

#define ESM_GRP1_MASK (uint32)(1U<<16U)
#define ESM_GRP2_MASK (uint32)(2U<<16U)
#define ESM_GRP3_MASK (uint32)(3U<<16U)

/** @enum _SL_Adc_Groups
 *   @brief Alias name for ADC event group
 *
 *   @note These values should be used for API argument @a group
 */
typedef enum _SL_ASC_Groups {
	sl_adcEVTGROUP, sl_adcGROUP1, sl_adcGROUP2
} SL_ADC_Groups;

/** @struct SL_adcData
 *   @brief ADC Conversion data structure
 *
 *   This type is used to pass adc conversion data.
 */
/** @typedef SL_adcData
 *   @brief ADC Data Type SL_adcData
 */
typedef struct _SL_ADC_Data {
	uint32 id; /**< Channel/Pin Id        */
	uint16 value; /**< Conversion data value */
} SL_ADC_Data;



/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> "This function is in asm file" */
void	_SL_Restore_IRQ(uint32 val);
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> "This function is in asm file" */
uint32	_SL_Disable_IRQ(void);
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> "This function is in asm file" */
uint32  _SL_Get_ARM_Mode(void);


/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> "This function is in asm file" */
void	_SL_Kickoff_STC_execution(void);
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> "This function is in asm file" */
void	_SL_Barrier_Data_Access(void);

/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> "This function is in asm file" */
void _SL_Init_EnableEventExport(void);
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> "This function is in asm file" */
void _SL_Init_ECCFlash(void);
/*SAFETYMCUSW 61 D MR: 8.10,8.11 <APPROVED> "This function is in asm file" */
void _SL_Init_ECCTCMRAM(void);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
boolean _SL_SelfTest_adcStartConversion_selChn(adcBASE_t * adc, uint8 channel, uint32 fifo_size, uint32 group);
boolean _SL_SelfTest_adcGetSingleData(adcBASE_t * adc, uint32 group, SL_ADC_Data * adcData);
boolean _SL_SelfTest_adcIsConversionComplete(const adcBASE_t * adc, uint32 group);
uint16  _SL_Approximate_value(uint16 value, uint16 reference, uint8 difference);
#endif

uint64 _SL_CRC_Calculate(uint64 * start_addr, uint32 count_dw);
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
void _SL_SelfTest_SRAM_RAD (sl_tcramBASE_t * ramBase, SL_SelfTest_Result * rad_stResult);
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
void _SL_SelfTest_SRAM_RAD (l2ramwBASE_t * ramBase, SL_SelfTest_Result * rad_stResult);
#endif
boolean SL_FLAG_GET(sint32 flag_id);
void SL_FLAG_CLEAR(sint32 flag_id);
boolean SL_FLAG_SET(sint32 flag_id);
void _SL_HoldNClear_nError(void);


#ifdef LOOP_BACK_ENABLE
#define	CAN_LOOP_BACK_MSG_SIZE 9
#define	CAN_LOOP_BACK_MSG_BOX 34U
void _SL_loadDataPattern(uint16 * pptr, uint16 seed_value, uint32 psize);
#endif

/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define CHECK_RANGE_RAM_PTR(x)      ((((uint32)(x)) >= SRAM_START) && ((((uint32)(x)) <= ((SRAM_END-(uint32)sizeof((x)))))))

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief      Perform loopback setings and enable loopback for HET
 *
 *
 *   @param[in] hetRAM Pointer to HET RAM:
 *              - hetRAM1: HET1 RAM pointer
 *              - hetRAM2: HET2 RAM pointer
 *
 *   @param[in] pin to be used for loopback
 *
 *
 * @return      void
 *
 * Example Usage:
 * @code
 * 				SL_hetEnableLoopback(sl_hetREG, 5u);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_hetEnableLoopback(sl_hetBASE_t * hetREG, uint32 pin);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief      Gets current signal of the given capture signal.
 *
 *
 *   @param[in] hetRAM Pointer to HET RAM:
 *              - hetRAM1: HET1 RAM pointer
 *              - hetRAM2: HET2 RAM pointer
 *
 *   @param[in] signal Container signal variable
 *              - duty cycle in %.
 *              - period period in us.
 *
 *
 * @return      void
 *
 * Example Usage:
 * @code
 * 				sl_hetSIGNAL_t received_signal;
 * 				SL_capGetSignal(sl_hetRAM, &received_signal);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_capGetSignal(sl_hetRAMBASE_t * hetRAM, sl_hetSIGNAL_t * signal);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
/**
 * @brief       Sets up the DMA Control Packet
 *
 * \if imageenabled
 * @image html
 * @image rtf
 * @image latex
 * \endif
 *
 * @param [in]	channel - Specifies the DMA channel to be used
 *
 * @param [in]  g_dmaCTRLPKT - The control packet to be programed to DMA RAM
 *
 * @return      void
 *
 * Example Usage:
 * @code
 * 				sl_g_dmaCTRL dmaCTRLPKT;
 *
 * 				dmaCTRLPKT.SADD = (uint32)0x08000000u;       //Initial source address
 *       		dmaCTRLPKT.DADD = (uint32)0x08000020u;       //Initial destination address
 *      		dmaCTRLPKT.CHCTRL = (uint32)0u;     		 // Next channel to be triggered + 1
 *      		dmaCTRLPKT.FRCNT = (uint32)1u;     			 // Frame count
 *       		dmaCTRLPKT.ELCNT = (uint32)1u;     			 // Element count
 *      		dmaCTRLPKT.ELDOFFSET = (uint32)0u; 			 // Element destination offset
 *       		dmaCTRLPKT.ELSOFFSET = (uint32)0u;  		 // Element source offset
 *      		dmaCTRLPKT.FRDOFFSET = (uint32)0u; 			 // Frame destination offset
 *       		dmaCTRLPKT.FRSOFFSET = (uint32)0u;		     // Frame source offset
 *      		dmaCTRLPKT.PORTASGN = (uint32)0x2u;   		 // DMA port
 *      		dmaCTRLPKT.RDSIZE = (uint32)0x2u;     		 // Read element size
 *       		dmaCTRLPKT.WRSIZE = (uint32)0x2u;     		 // Write element size
 *      		dmaCTRLPKT.TTYPE = (uint32)0u;      		 // Trigger type - frame/block
 *       		dmaCTRLPKT.ADDMODERD = (uint32)0u;  		 // Addressing mode for source
 *      		dmaCTRLPKT.ADDMODEWR = (uint32)0u;  		 // Addressing mode for destination
 *       		dmaCTRLPKT.AUTOINIT = (uint32)0u;  			 // Auto-init mode
 *
 *       		sl_dmaSetCtrlPacket(DMA_CH0, dmaCTRLPKT);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void sl_dmaSetCtrlPacket(sl_dmaChannel_t channel, sl_g_dmaCTRL g_dmaCTRLPKT);



/**
 * @brief       Sets up the DMA channel to be Hardware or Software triggered
 *
 * \if imageenabled
 * @image html
 * @image rtf
 * @image latex
 * \endif
 *
 * @param [in]	channel - Specifies the DMA channel to be set up
 *
 * @param [in]  type - Trigger type (hardware or software)
 *
 * @return      void
 *
 * Example Usage:
 * @code
 * 				sl_dmaSetChEnable(DMA_CH0, DMA_SW);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void sl_dmaSetChEnable(sl_dmaChannel_t channel, sl_dmaTriggerType_t type);



/**
 * @brief       Sets up the DMA access permissions, programs control packet to DMA RAM and initiates transfer
 *
 * \if imageenabled
 * @image html
 * @image rtf
 * @image latex
 * \endif
 *
 * @param [in]	srcAddr - source address for dma transfer
 *
 * @param [in]  destAddr - destination address for dma transfer
 *
 * @param [in]  permission - access permission for dma
 *
 * @param [in]  dmaCTRLPKT - the transfer control packet
 *
 * @return      TRUE if transfer successful
 *
 * Example Usage:
 * @code
 * 				boolean retVal;
 * 				retVal = sl_dmaSoftwrTestConfig(&dma_test_varA, &dma_test_varB, DMA_PERMISSION_READ_ACCESS, &dmaCTRLPKT)
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
boolean sl_dmaSoftwrTestConfig(uint32* srcAddr, uint32* destAddr, uint32 permission, sl_g_dmaCTRL* dmaCTRLPKT);



/**
 * @brief       Configures the control packet to be used for DMA transfer
 *
 * \if imageenabled
 * @image html
 * @image rtf
 * @image latex
 * \endif
 *
 * @param [in]  dmaCTRLPKT - the transfer control packet
 *
 * @param [in]	srcAddr - source address for dma transfer
 *
 * @param [in]  destAddr - destination address for dma transfer
 *
 *
 * @return      void
 *
 * Example Usage:
 * @code
 * 				    sl_dmaConfigCtrlPacket(dmaCTRLPKT, &dma_test_varA, &dma_test_varB);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void sl_dmaConfigCtrlPacket(sl_g_dmaCTRL* dmaCTRLPKT, uint32* srcAddr, uint32* destAddr);



/**
 * @brief       Define and set up the region to be protected by DMA MPU
 *
 * \if imageenabled
 * @image html
 * @image rtf
 * @image latex
 * \endif
 *
 * @param [in]	srcAddr - source address for dma transfer
 *
 *
 * @return      void
 *
 * Example Usage:
 * @code
 * 				    sl_dmaSetProtectedRegion(&dma_test_varA);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void sl_dmaSetProtectedRegion(uint32* srcAddr);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief     Set the direction of GIO pins at runtime.
 *
 *
*   @param[in] port pointer to GIO port:
*              - gioPORTA: PortA pointer
*              - gioPORTB: PortB pointer
*   @param[in] dir value to write to DIR register
*
*   @return void
 *
 * Example Usage:
 * @code
 *				SL_gioSetDirection(sl_gioPORTA, 0xFFu); //all ouput
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_gioSetDirection(sl_gioPORT_t * port, uint32 dir);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief     Writes a value to all pin of a given GIO port
 *
 *
*   @param[in] port pointer to GIO port:
*              - gioPORTA: PortA pointer
*              - gioPORTB: PortB pointer
*   @param[in] value value to write to port
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				uint8 txdata = 0xAAu;
 *				SL_gioSetPort(sl_gioPORTA, txdata);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_gioSetPort(sl_gioPORT_t * port, uint32 value);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief    Reads a the current value of a given GIO port
 *
 *
*   @param[in] port pointer to GIO port:
*              - gioPORTA: PortA pointer
*              - gioPORTB: PortB pointer
*
*   @return The value read
 *
 * Example Usage:
 * @code
 * 				uint8 rxdata = 0u;
 *				rxdata = SL_gioGetPort(sl_gioPORTA);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
uint32 SL_gioGetPort(sl_gioPORT_t * port);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief    Writes to a bit of a given GIO port
 *
 *
*   @param[in] port pointer to GIO port:
*              - gioPORTA: PortA pointer
*              - gioPORTB: PortB pointer
*
*   @param[in] the bit to be written to
*
*   @param[in] the value to be written (1 or 0)
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				uint32 txdata = 0x1u;
 *   			SL_gioSetBit(sl_gioPORT, pin, txdata);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_gioSetBit(sl_gioPORT_t* port, uint32 bit, uint32 value);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief    Returns the value of a bit of a given GIO port
 *
 *
*   @param[in] port pointer to GIO port:
*              - gioPORTA: PortA pointer
*              - gioPORTB: PortB pointer
*
*   @param[in] the bit to be read
*
*
*   @return The value read
 *
 * Example Usage:
 * @code
 *
 * 				uint32 rxdata = 0x0u;
 *   			rxdata = SL_gioGetBit(sl_gioPORT, pin);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
uint32 SL_gioGetBit(sl_gioPORT_t* port, uint32 bit);
#endif


#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief   This function enables the Loopback mode for self test of LIN.
 *
*   @param[in] lin        - lin module base address
*   @param[in] Loopbacktype  - Digital or Analog
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				SL_linEnableLoopback(sl_linREG, SL_Analog_Lbk);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_linEnableLoopback(sl_linBASE_t * lin, loopBackType_t Loopbacktype);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief   Send data response length in bytes.
 *
*   @param[in] lin    - lin module base address
*   @param[in] length - number of data words in bytes. Range: 1-8.
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				SL_linSetLength(sl_linREG, 8u);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_linSetLength(sl_linBASE_t * lin, uint32 length);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief   Send lin header including sync break field, sync field and identifier.
 *
*   @param[in] lin  - lin module base address
*   @param[in] identifier - lin header id
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				SL_linSendHeader(sl_linREG, 0x28);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_linSendHeader(sl_linBASE_t * lin, uint8 identifier);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief  Send a block of data pointed to by 'data'.
*   The number of data to transmit must be set with 'linSetLength' before.
 *
*   @param[in] lin    - lin module base address
*   @param[in] data   - pointer to data to send
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				uint8  TEXT1[TSIZE]= {'H','E','R','C','U','L','E','S'};
 * 				SL_linSend(sl_linREG, TEXT1);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_linSend(sl_linBASE_t * lin, uint8 * data);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief Read a block of bytes and place it into the data buffer pointed to by 'data'.
 *
*   @param[in] lin    - lin module base address
*   @param[in] data   - pointer to data buffer
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				uint8 TEXT2[TSIZE]= {0};
 * 				SL_linGetData(sl_linREG, TEXT2);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_linGetData(sl_linBASE_t * lin, uint8 * const data);
#endif

/**
 * @brief      Enable Loopback mode in MIBSPI for self test
 *
 *
 *   @param[in] mibspi        - Mibspi module base address
 *   @param[in] Loopbacktype  - Digital or Analog
 *
 *
 * @return      void
 *
 * Example Usage:
 * @code
 * 				SL_mibspiEnableLoopback(mibspiREG, SL_Analog_Lbk);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_mibspiEnableLoopback(mibspiBASE_t * mibspi, loopBackType_t Loopbacktype);

/**
 * @brief      This function updates the data for the specified transfer group,
 * the length of the data must match the length of the transfer group.
 *
 *
 *   @param[in] mibspi   - Spi module base address
 *   @param[in] group - Transfer group (0..7)
 *   @param[in] data  - new data for transfer group
 *
 *
 * @return      void
 *
 * Example Usage:
 * @code
 * 				uint16 txdata = 0xAAAAU;
 *				uint32 tGroup = 0U;
 * 				SL_mibspiSetData(mibspiREG, tGroup, &txdata);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_mibspiSetData(mibspiBASE_t * mibspi, mibspiRAM_t * ram, uint32 group, uint16 * data);

/**
 * @brief      Initiates a transfer for the specified transfer group.
 *
 *
 *   @param[in] mibspi   - Spi module base address
 *   @param[in] group - Transfer group (0..7)
 *
 *
 * @return      void
 *
 * Example Usage:
 * @code
 *				uint32 tGroup = 0U;
 * 				SL_mibspiTransfer(mibspiREG, tGroup);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_mibspiTransfer(mibspiBASE_t * mibspi, uint32 group);

/**
 * @brief     Checks to see if the transfer for the specified transfer group has finished.
 *
 *
 *   @param[in] mibspi   - Spi module base address
 *   @param[in] group - Transfer group (0..7)
 *
 *
 * @return     TRUE is transfer complete, otherwise FALSE.
 *
 * Example Usage:
 * @code
 * 						uint32 tGroup = 0U;
 *				    	while(FALSE == SL_mibspiIsTransferComplete(mibspiREG, tGroup))
 *   					{
 *
 *   					}//wait
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
boolean SL_mibspiIsTransferComplete(mibspiBASE_t * mibspi, uint32 group);

/**
 * @brief     This function transfers the data from the specified transfer group receive
 * buffers to the data array,  the length of the data must match the length of the transfer group.
 *
 *   @param[in]  mibspi   - Spi module base address
 *   @param[in]  group - Transfer group (0..7)
 *   @param[out] data  - pointer to data array
 *
 *
 * @return     error flags from data buffer, if there was a receive error on
 *             one of the buffers this will be reflected in the return value.
 *
 * Example Usage:
 * @code
 *					boolean retVal;
 * 					if(0U != SL_mibspiGetData(mibspiREG, tGroup, &rxdata))
 *   				{
 *   	    			retVal = FALSE;
 *   				}
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
uint32 SL_mibspiGetData(mibspiBASE_t * mibspi, mibspiRAM_t * ram, uint32 group, uint16 * data);

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief    Sends a single byte in polling mode, will wait in the
*   routine until the transmit buffer is empty before sending
*   the byte.  Use sciIsTxReady to check for Tx buffer empty
*   before calling sciSendByte to avoid waiting.
 *
*   @param[in] sci  - sci module base address
*   @param[in] byte - byte to transfer
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				uint8 txdata = 0xAAu;
 *				SL_sciSendByte(sl_sciREG, txdata);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_sciSendByte(sl_sciBASE_t * sci, uint8 byte);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief      Receives a single byte in polling mode.  If there is
*    not a byte in the receive buffer the routine will wait
*    until one is received.   Use sciIsRxReady to check to
*    see if the buffer is full to avoid waiting.
 *
*   @param[in] sci - sci module base address
*
*   @return Received byte
 *
 * Example Usage:
 * @code
 * 				uint8 rxdata = 0u;
 *				rxdata = SL_sciReceiveByte(sl_sciREG);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
uint32 SL_sciReceiveByte(sl_sciBASE_t * sci);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief   This function enables the Loopback mode for self test of SCI.
 *
*   @param[in] sci        - sci module base address
*   @param[in] Loopbacktype  - Digital or Analog
*
*   @return void
 *
 * Example Usage:
 * @code
 * 				 SL_sciEnableLoopback(sl_sciREG, SL_Analog_Lbk);
 *
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_sciEnableLoopback(sl_sciBASE_t * sci, loopBackType_t Loopbacktype);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief     This function enables the Loopback mode for self test of SPI.
 *
 *   @param[in] spi        - spi module base address
 *   @param[in] Loopbacktype  - Digital or Analog
 *
 *
 * @return     void
 *
 * Example Usage:
 * @code
 *					SL_spiEnableLoopback(sl_spiREG, SL_Analog_Lbk);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
void SL_spiEnableLoopback(sl_spiBASE_t * spi, loopBackType_t Loopbacktype);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief     Get the status of the SPI Transmit data block.
 *
 *   @param[in] spi - Spi module base address
 *
 *
 * @return     Spi Transmit block data status.
 *
 * Example Usage:
 * @code
 *   		while(SL_SPI_PENDING == SL_SpiTxStatus(sl_spiREG))
 *   		{
 *
 *   		}//wait
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
sl_SpiDataStatus_t SL_SpiTxStatus(sl_spiBASE_t * spi);
#endif

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
/**
 * @brief     This function transmits and receives blocksize number of data from source
 * buffer using polling method.
 *
 *   @param[in] spi           - Spi module base address
 *   @param[in] dataconfig_t    - Spi DAT1 register configuration
 *   @param[in] blocksize    - number of data
 *   @param[in] srcbuff        - Pointer to the source data ( 16 bit).
 *   @param[in] destbuff        - Pointer to the destination data ( 16 bit).
 *
 *
 * @return     flag register value.
 *
 * Example Usage:
 * @code
 * 			uint16 rxdata = 0u, txdata = 0xAAU;
 * 			uint32 blocksize = 1U;
 * 			sl_spiDAT1_t dataconfig_t;
 * 			dataconfig_t.CSNR = SL_SPI_CS_0;
 *   		dataconfig_t.CS_HOLD = FALSE;
 *   		dataconfig_t.WDEL = TRUE;
 *   		dataconfig_t.DFSEL = SL_SPI_FMT_0;
 *   		SL_spiTransmitAndReceiveData(sl_spiREG, &dataconfig_t, blocksize, &txdata, &rxdata);
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */
uint32 SL_spiTransmitAndReceiveData(sl_spiBASE_t * spi, sl_spiDAT1_t * dataconfig_t, uint32 blocksize, uint16 * srcbuff, uint16 * destbuff);
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)

/**
 * @brief       Checks the status of Memory Interconnect Selftest status after the test is executed.
 *
 * \if imageenabled
 *
 * \endif
 *
 * @param [in]    void
 *
 *
 * @return      TRUE if selftest executed successfully else FALSE.
 *
 * Example Usage:
 * @code
 *                if(RESET_TYPE_ICSTRST == resetReason)
 *               {
 *                   if(SL_SelfTest_Status_MemIntrcntSelftest())
 *                       afterSTC();
 *                   else
 *                       while(1); //Memory Interconnect Error
 *               }
 * @endcode
 *
 * @entrymode   Any privilege mode
 * @exitmode    Same as entry
 *
 */

boolean SL_SelfTest_Status_MemIntrcntSelftest(void);

#endif

#endif /* __SL_PRIV_H__ */
