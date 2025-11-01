/* (c) Texas Instruments 2009-2012, All rights reserved. */


#ifndef __HL_CAN_BITDEFN_H__
#define __HL_CAN_BITDEFN_H__

#include <HL_reg_can.h>

#define CAN_CTRL_ECC_DETECT_EN_START 				(uint32)0u
#define CAN_CTRL_ECC_DETECT_EN_LENGTH 				(uint32)4u
#define CAN_CTRL_SECDED_EN							(uint32)0xAu
#define CAN_CTRL_SECDED_DIS							(uint32)0x5u
#define CAN_CTRL_SECDED_START						(uint32)10u
#define CAN_CTRL_SECDED_LENGTH						(uint32)4u
#define CAN_CTRL_TEST_EN							(uint32)7u
#define CAN_CTRL_INIT								(uint32)0u

#define CAN_SRAM_ECC_DISABLED 						(uint32)0x5u
#define CAN_TEST_RDA_EN								(uint32)9u
#define CAN_ECCDIAG_SECDED_EN						(uint32)0x5u
#define CAN_ECCDIAG_SECDED_DIS						(uint32)0xAu

#define CAN_ECC_CS_SBE_EVT_EN						(uint32)0xAu
#define CAN_ECC_CS_SBE_EVT_START					(uint32)24u
#define CAN_ECC_CS_SBE_EVT_LENGTH					(uint32)4u
#define CAN_ECC_CS_ECC_MODE_DIS						(uint32)0x5u
#define CAN_ECC_CS_ECC_MODE_START					(uint32)16u
#define CAN_ECC_CS_ECC_MODE_LENGTH					(uint32)4u

#define CAN_ECC_SBERR 								(uint32)(0u)
#define CAN_ECC_UNCORR_ERR 							(uint32)(8u)
#define CAN_ECC_ES_PER	 							(uint32)(8u)

#define CAN_ECC_SERR_MSG_NO							(uint32)0xFFu

#define CAN_FLIP_DATA_1BIT							(uint32)0x1u
#define CAN_FLIP_DATA_2BIT							(uint32)0x03u
/*
#define CAN_CTRL_SECDED_DIS							(uint32)0x5u
#define CAN_CTRL_TEST_MODE_EN						(uint32)0x80u
#define CAN_CTRL_INIT_SET							(uint32)0x1u
*/

#define CAN_INIT_BIT								(uint32)0x1u
#define CAN_CTL_PMD_DIS							    (uint32)0x5u
#define CAN_CTL_PMD_EN							    (uint32)0xAu
#define CAN_CTL_PMD_START							(uint32)10u
#define CAN_CTL_PMD_LENGTH							(uint32)4u
#define CAN_TEST_BIT								(uint32)0x80u
#define CAN_RDA_BIT									(uint32)0x200u
#define CAN_PAR_ERR_FLG								(uint32)0x100u

#define CAN_CTL_IE1_BIT								(uint32)0x20000u
#define CAN_CTL_IE0_BIT								(uint32)0x2u

#endif /* __CAN_BITDEFN_H__ */
