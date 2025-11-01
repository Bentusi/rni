/* (c) Texas Instruments 2009-2012, All rights reserved. */


#ifndef __SL_LIN_BITDEFN_H__
#define __SL_LIN_BITDEFN_H__

#include <sl_reg_lin.h>

#define LIN_GCR0_RESET_BIT									(uint32)0x1u
#define LIN_GCR1_SWRST_BIT									(uint32)0x80u
#define LIN_FLR_TX_READY 									(uint32)0x100u
#define LIN_FLR_BUSY_BIT 									(uint32)0x8u
#define LIN_FLR_RX_INT 										(uint32)0x200u
#define LIN_GCR1_TXENA_BIT									(uint32)0x2000000u
#define LIN_GCR1_RXENA_BIT									(uint32)0x1000000u


#endif
