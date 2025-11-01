#ifndef __HL_SCM_BITDEFN_H__
#define __HL_SCM_BITDEFN_H__


#include <HL_reg_scm.h>
#define SCM_DTC_SOFTRESET_EN_START 						(uint32)8u
#define SCM_DTC_SOFTRESET_EN_LENGTH 					(uint32)4u
#define SCM_DTC_SOFTRESET_EN							(uint32)0xAu
#define SCM_DTC_SOFTRESET_DIS							(uint32)0x500u
#define SCM_TO_CLEAR_START 								(uint32)0u
#define SCM_TO_CLEAR_LENGTH 							(uint32)4u
#define SCM_TO_CLEAR_EN									(uint32)0xAu
#define SCM_REQ2ACC_START 								(uint32)0u
#define SCM_REQ2ACC_LENGTH 								(uint32)16u
#define SCM_REQ2ACC_VAL									(uint32)0x400u
#define SCM_REQ2RES_START 								(uint32)16u
#define SCM_REQ2RES_LENGTH 								(uint32)16u
#define SCM_REQ2RES_VAL									(uint32)0x400u
#define SCM_R2An_MASK									(uint32)0xFFu
#define SCM_R2An_MASK_START								(uint32)0x0u
#define SCM_R2An_MASK_LENGTH							(uint32)0x8u
#define SCM_R2Rn_MASK									(uint32)0xFFu
#define SCM_R2Rn_MASK_START								(uint32)0x0u
#define SCM_R2Rn_MASK_LENGTH							(uint32)0x8u

#endif
