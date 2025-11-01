/* (c) Texas Instruments 2009-2012, All rights reserved. */


#ifndef __SL_HTU_BITDEFN_H__
#define __SL_HTU_BITDEFN_H__

#include <sl_reg_htu.h>

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define HTU_SRAM_PARITY_DISABLED					(uint32)0x5u
#define HTU_PARCTRL_PARITY_EN_START					(uint32)0u
#define HTU_PARCTRL_PARITY_EN_LENGTH				(uint32)4u
#define HTU_PAR_ADDR_START							(uint32)0u
#define HTU_PAR_ADDR_LENGTH							(uint32)9u
#define HTU_TEST_MODE								(uint32)0x100u
#define HTU_PAR_ERR_FLG								(uint32)0x10000u
#endif

#define HTU_EN_BIT									(uint32)0x10000u



#endif
