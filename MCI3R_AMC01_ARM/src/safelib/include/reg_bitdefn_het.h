/* (c) Texas Instruments 2009-2012, All rights reserved. */


#ifndef __SL_HET_BITDEFN_H__
#define __SL_HET_BITDEFN_H__

#include <sl_reg_het.h>

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define HET_SRAM_PARITY_DISABLED					(uint32)0x5u
#define HET_PARCTRL_PARITY_EN_START					(uint32)0u
#define HET_PARCTRL_PARITY_EN_LENGTH				(uint32)4u
#define HET_TEST_MODE								(uint32)0x100u
#define HET_PAR_ADDR_START							(uint32)2u
#define HET_PAR_ADDR_LENGTH							(uint32)11u
#define HET_INST7_SET								(uint32)0x7F
#define HET_INST7_SET_START							(uint32)0u
#define HET_INST7_SET_LENGTH						(uint32)7u
#define HET_LRP8									(uint32)0x3u
#define HET_LRP_START								(uint32)8u
#define HET_LRP_LENGTH								(uint32)3u
#define HET_INTR0_BIT								(uint32)0x1u
#endif

#define HET_TO_BIT									(uint32)0x1u

#endif
