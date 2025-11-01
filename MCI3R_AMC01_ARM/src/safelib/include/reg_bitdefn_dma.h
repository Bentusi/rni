/** @file rti.h
*   @brief RTI Driver Header File
*   @date 15.Mar.2012
*   @version 03.01.00
*
*   This file contains:
*   - Definitions
*   - Types
*   - Interface Prototypes
*   .
*   which are relevant for the RTI driver.
*/

/* (c) Texas Instruments 2009-2012, All rights reserved. */


#ifndef __HL_DMA_BITDEFN_H__
#define __HL_DMA_BITDEFN_H__

#include <HL_reg_dma.h>

#if !(defined(_RM42x_) || defined(_TMS570LS04x_))
#define DMA_PARCTRL_ECC_DETECT_EN_START 			(uint32)0u
#define DMA_PARCTRL_ECC_DETECT_EN_LENGTH 			(uint32)4u
#define DMA_PARCTRL_ECC_EN							(uint32)0xAu
#define DMA_PARCTRL_ECC_DIS							(uint32)0x5u
#define DMA_PARCTRL_ECC_EN_START					(uint32)0u
#define DMA_PARCTRL_ECC_EN_LENGTH					(uint32)4u
#define DMA_SBERRADDR								((uint32)0xFFFu << 0)
#define DMA_SRAM_ECC_DISABLED 						(uint32)0x5u

#define DMA_RES_BIT							(uint32)0x1u
#define DMA_GCTRL_BUS_BUSY					(uint32)0x4000u
#define DMA_GCTRL_DMA_EN					(uint32)0x10000u
#define DMAMPST1_REG0FT						(uint32)0x1u
#define DMAPCTRL1_REG0AP_START				(uint32)1u
#define DMAPCTRL1_REG0AP_LENGTH				(uint32)2u

#define DMAPCTRL1_REG0AP_EN					(uint32)0x1u
#define DMAPCTRL1_INT0_EN					(uint32)0x8u

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_)
#define DMA_SRAM_PARITY_DISABLED					(uint32)0x5u
#define DMA_PARCTRL_PARITY_EN_START					(uint32)0u
#define DMA_PARCTRL_PARITY_EN_LENGTH				(uint32)4u
#define DMA_TEST_MODE								(uint32)0x100u
#define DMA_PARITY_EDFLAG							(uint32)0x1000000u
#define DMA_PAR_ADDR_START							(uint32)0u
#define DMA_PAR_ADDR_LENGTH							(uint32)12u
#endif

#define DMA_ECC_TST_EN			 					(uint32)((uint32)1u << 8u)
#define DMA_SBE_EVT_EN								(uint32)0xAu
#define DMA_SBE_EVT_DIS							    (uint32)0x5u
#define DMA_SBE_EVT_START							(uint32)8u
#define DMA_SBE_EVT_LENGTH							(uint32)4u

#define DMA_ECC_SBERR 								(uint32)16u
#define DMA_ECC_ERR 							(uint32)24u
#define DMA_ECC_UNCORERR_ADDR_START					(uint32)0u
#define DMA_ECC_UNCORERR_ADDR_LEN					(uint32)12u

#define DMA_FLIP_DATA_1BIT							(uint32)0x1u
#define DMA_FLIP_DATA_2BIT							(uint32)0x03u
#define DMA_SYN_1BIT_DATA_ECC						(uint32)0x1Cu	/* Introduce a one bit error in bit 23 */
#define DMA_SYN_2BIT_DATA_ECC						(uint32)0x1Du	/* Introduce a two bit error for 0 -> 5 */


typedef enum sl_dmaChannel
{
	SL_DMA_CH0 = 0U,	SL_DMA_CH1,		SL_DMA_CH2,		SL_DMA_CH3,
	SL_DMA_CH4,	    	SL_DMA_CH5,		SL_DMA_CH6,		SL_DMA_CH7,
	SL_DMA_CH8,	    	SL_DMA_CH9,		SL_DMA_CH10,	SL_DMA_CH11,
	SL_DMA_CH12,	    SL_DMA_CH13,	SL_DMA_CH14,	SL_DMA_CH15,
	SL_DMA_CH16,	    SL_DMA_CH17,	SL_DMA_CH18,	SL_DMA_CH19,
	SL_DMA_CH20,	    SL_DMA_CH21,	SL_DMA_CH22,	SL_DMA_CH23,
	SL_DMA_CH24,	    SL_DMA_CH25,	SL_DMA_CH26,	SL_DMA_CH27,
	SL_DMA_CH28,	    SL_DMA_CH29,	SL_DMA_CH30,	SL_DMA_CH31
}sl_dmaChannel_t;

typedef enum sl_dmaTriggerType
{
	SL_DMA_HW,
	SL_DMA_SW
}sl_dmaTriggerType_t;



typedef struct  sl_dmaCTRLPKT
{
    uint32 SADD;       /* Initial source address           */
    uint32 DADD;       /* Initial destination address      */
    uint32 CHCTRL;     /* Next channel to be triggered + 1 */
    uint32 FRCNT;      /* Frame   count                    */
    uint32 ELCNT;      /* Element count                    */
    uint32 ELDOFFSET;  /* Element destination offset       */
    uint32 ELSOFFSET;  /* Element source offset            */
    uint32 FRDOFFSET;  /* Frame destination offset         */
    uint32 FRSOFFSET;  /* Frame source offset              */
    uint32 PORTASGN;   /* DMA port                         */
    uint32 RDSIZE;     /* Read element size                */
    uint32 WRSIZE;     /* Write element size               */
    uint32 TTYPE;      /* Trigger type - frame/block       */
    uint32 ADDMODERD;  /* Addressing mode for source       */
    uint32 ADDMODEWR;  /* Addressing mode for destination  */
    uint32 AUTOINIT;   /* Auto-init mode                   */
} sl_g_dmaCTRL;

#endif

#endif
