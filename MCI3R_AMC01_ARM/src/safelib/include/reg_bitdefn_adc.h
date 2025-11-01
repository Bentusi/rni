#ifndef __HAL_ADC_H__
#define __HAL_ADC_H__

#include <HL_reg_adc.h>

#define ADC_12BIT_RESOLUTION 	   (uint32)(0x80000000U)
#define ADC_ENABLE_CALIBRATION 	   (uint32)(0x00000001U)
#define ADC_CALIB_BRIDGE_ENABLE    (uint32)(0x00000200U)
#define ADC_CALIB_HILO_ENABLE	   (uint32)(0x00000100U)
#define ADC_START_CALIBRATION      (uint32)(0x00010000U)
#define ADC_ENABLE_SELFTEST		   (uint32)(0x01000000U)
#define ADC_VALUE_MASK   		   (uint32)(0x00000FFFU)
#define ADC_GROUP_THR_INT_FLG	   (uint32)(0x00000001U)
#define ADC_GROUP_CONV_END         (uint32)(0x00000008U)
#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_) || defined(_TMS570LC43x_) || defined(_RM57Lx_)
/*SAFETYMCUSW 340 S MR:19.7 <APPROVED> "Reason -  This is an advisory by MISRA.We accept this as a coding convention*/
#define ADC_GET_CHANNEL_ID(fifobuffer) (uint32)(((fifobuffer) & 0x001F0000U)>>16)
#endif


#define ADC_RSTCR_RESET	   (uint32)(0x1U)
#define ADC_PAR_DIS	   				(uint32)(0x5U)
#define ADC_PAR_ENA	   				(uint32)(0xAU)
#define ADC_PARCR_PAR_START	   		(uint32)(0U)
#define ADC_PARCR_PAR_LENGTH	    (uint32)(4U)
#define ADC_TEST_MODE				(uint32)(0x100U)


#endif /* __HAL_ADC_H__ */
