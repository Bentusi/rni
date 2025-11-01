
/** @struct sl_stcBase
*   @brief STC Base Register Definition
*
*   This structure is used to access the STC module egisters.
*/
#ifndef __HAL_STC_H__
#define __HAL_STC_H__

#include <HL_reg_stc.h>

#define STC_STCGCR0_RS_CNT		(uint32)((uint32)1u << 0u)

#define STC_STCGSTAT_TEST_DONE  ((uint32)((uint32)1u << 0u))
#define STC_STCGSTAT_TEST_FAIL  ((uint32)((uint32)1u << 1u))

#define STC_STCFSTAT_CPU1_FAIL  ((uint32)((uint32)1u << 0u))
#define STC_STCFSTAT_CPU2_FAIL  ((uint32)((uint32)1u << 1u))
#define STC_STCFSTAT_TO_ERR     ((uint32)((uint32)1u << 2u))

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define STC_MAX_INTERVALS   		(uint16)24U
#endif

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define STC1_MAX_INTERVALS   			(uint16)125U
#define STC2_MAX_INTERVALS   			(uint16)57U
#define STC_GCR1_SEG0_CORE_PARLL	 	(uint32)0x0U
#define STC_GCR1_SEG0_CORE_SEL_START	(uint32)8U
#define STC_GCR1_SEG0_CORE_SEL_LENGTH	(uint32)4U
#endif

#define STC_MAX_CLOCK_DIV			((uint8)7U)
#define STC_STCGCR0_INTCOUNT_START	(uint32)16U
#define STC_STCGCR1_STC_ENA			(uint32)0xAU
#define STC_STCSCSCR_FAULT_INS		((uint32)((uint32)1u << 4u))
#define STC_STCSCSCR_SELF_CHECK_KEY	((uint32)0xAU)

#endif /* __HAL_STC_H__ */
