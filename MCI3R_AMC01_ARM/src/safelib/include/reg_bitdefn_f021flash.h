#ifndef __HAL_F021FLASH_H__
#define __HAL_F021FLASH_H__

#include <HL_reg_flash.h>

#define F021F_FEDACCTRL1_E0FEN  (uint32)(1U << 10)
#define F021F_FEDACCTRL1_EZFEN  (uint32)(1U << 9)
#define F021F_FEDACCTRL1_EPEN   (uint32)(1U << 8)
#define F021F_FEDACCTRL1_EOCV   (uint32)(1U << 5)
#define F021F_FEDACCTRL1_EZCV   (uint32)(1U << 4)
#define F021F_CTRLFLAG_DEFAULT  (uint32)(F021F_FEDACCTRL1_E0FEN | F021F_FEDACCTRL1_EZFEN)

#define F021F_FDIAGCTRL_DIAG_EN_KEY (uint32)0x00050000U
#define F021F_FDIAGCTRL_DIAG_EN_MASK (uint32)(0xFU<<16)

/* F021F_FDIAGCTRL_DIAGMODE_DATA_CORR */
#define F021F_FDCTRL_DMODE_DATA_CORR        (uint32)0x00000001U
#define F021F_FDIAGCTRL_DMODE_SYN_RPT       (uint32)0x00000002U
/*macro names changed so as to adhere to misra rule MISRA-C:2004 5.1 */
#define F021F_FDIAGCTRL_DMODE_MALFUNC1  	(uint32)0x00000003U
/*macro names changed so as to adhere to misra rule MISRA-C:2004 5.1 */
#define F021F_FDIAGCTRL_DMODE_MALFUNC2   	(uint32)0x00000004U
#define F021F_FDIAGCTRL_DMODE_ADDR_TAG      (uint32)0x00000005U
#define F021F_FDCTRL_DMODE_P_BUF_PAR        (uint32)0x00000006U
#define F021F_FDCTRL_DMODE_TEST_MODE        (uint32)0x00000007U
#define F021F_FDIAGCTRL_DIAG_TRIG		    (uint32)(((uint32)1U)<<24U)

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define F021F_FDIAGCTRL_DIAG_BUF_SEL_A		(uint32)(0U<<8U)
#define F021F_FDIAGCTRL_DIAG_BUF_SEL_B		(uint32)(4U<<8U)
#define F021F_FDIAGCTRL_DIAG_EN_START		(uint32)(8U)
#define F021F_FDIAGCTRL_DIAG_EN_LENGTH		(uint32)(3U)
#define F021F_FEMU_ECC_SYN_1BIT_DATA_ECC     ((uint32)0xCEU)/*this corrupts the ecc for data bit 0*/
#define F021F_FEMU_ECC_SYN_2BIT_DATA_ECC     ((uint32)0x03U)/* this corrupts the ecc for 2 bit error*/
#endif

#define F021F_FEDACSTATUS_ERR_ZERO_FLG  	(uint32)(1U<<1U)
#define F021F_EE_STATUS_ERR_ZERO_FLG    	(uint32)(1U<<1U)
#define F021F_EE_STATUS_ERR_ONE_FLG    	    (uint32)(1U<<2U)
#define F021F_EE_STATUS_D_UNC_ERR_FLG  	    (uint32)(1U<<12U)
#define F021F_EE_STATUS_D_CORR_ERR_FLG      (uint32)(1U<<3U)
#define F021F_EE_STATUS_UNC_ERR_FLG       	(uint32)(1U<<8U)
#define F021F_EE_STATUS_CME			       	(uint32)(1U<<4U)
#define F021F_FEDACSTATUS_ADD_TAG_ERR    	(uint32)(1U<<11U)
#define F021F_FPAROVR_PAR_OVR_ENABLE		(uint32)(0x5U << 9U)
#define F021F_FPAROVR_BUS_PAR_DISABLE		(uint32)(0xAU << 12U)
#define F021F_FPAROVR_BUS_PAR_ENABLE		(uint32)(0x5U << 12U)
/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define F021F_FPAROVR_DAT_INV_PAR_FOR(x)	(uint32)(1U<<(x)) /* X from 0 to 7 allowed */
#define F021F_FPAROVR_SYN_1BIT_DATA_ECC     ((uint32)0xCEU)/*this corrupts the ecc for data bit 0*/
#define F021F_FPAROVR_SYN_2BIT_DATA_ECC     ((uint32)0x03U)/* this corrupts the ecc for 2 bit error*/
#define F021F_FPAROVR_SYN_ADDRESS_ECC       ((uint32)0x9EU)/*this corrupts the ecc for address bit 3(a03)*/
#define F021F_FPAROVR_ADD_INV_PAR			(uint32)(((uint32)1U)<<8U) /* X from 0 to 7 allowed */
#define F021F_FEDACSTATUS_B1_UNC_ERR		(uint32)(1U<<8U)
#define F021F_FEDACSTATUS_B2_UNC_ERR		(uint32)(1U<<17U)
#define F021F_FEDACSTATUS_ADD_PAR_ERR		(uint32)(1U<<10U)

#define F021F_FDIAGCTRL_DIAGECCSEL_FEE		(uint32)(((uint32)0x5U)<<12U)
#define F021F_EECORRERRPOS_MASK				(uint32)(0xFFU<<0U)
#define F021F_EEUNCERRADD_MASK				(uint32)((uint32)(((uint32)1U<<22U)-1U)<<3U)
/*Bit Fields*/
#define BF_DIAG_BUF_SEL_START            (uint32)8u
#define BF_DIAG_BUF_SEL_LEN              (uint32)2u
#endif /* __HAL_F021FLASH_H__ */
