#ifndef __HAL_PMM_H__
#define __HAL_PMM_H__

#include <HL_reg_pmm.h>

#define PMM_PRCKEYREG_MKEY_LKSTEP_MODE		 (uint32)0x00000000u
#define PMM_PRCKEYREG_MKEY_ST_MODE           (uint32)0x00000006u
#define PMM_PRCKEYREG_MKEY_ERRFCG_MODE       (uint32)0x00000009u
#define PMM_PRCKEYREG_MKEY_STERRF_MODE       (uint32)0x0000000Fu

#if defined(_RM46x_) || defined(_TMS570LS12x_)
#define PMM_PSCON_LPDDCSTAT1_LSTC                   ((uint32)((uint32)0xBu << 0u))
#endif
#if defined(_RM48x_) || defined(_TMS570LS31x_)
#define PMM_PSCON_LPDDCSTAT1_LSTC                   ((uint32)((uint32)0xFu << 0u))
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define PMM_PSCON_LPDDCSTAT1_LSTC                   ((uint32)((uint32)0x1Fu << 0u))
#endif

#define PMM_PSCON_LPDDCSTAT1_LCMPE                  ((uint32)((uint32)0xFu << 16u))

#if defined(_RM46x_) || defined(_TMS570LS12x_)
#define PMM_PSCON_LPDDCSTAT2_LSTE                   (uint32)(0xBu << 0u)
#endif
#if defined(_RM48x_) || defined(_TMS570LS31x_)
#define PMM_PSCON_LPDDCSTAT2_LSTE                   (uint32)(0xFu << 0u)
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define PMM_PSCON_LPDDCSTAT2_LSTE                   ((uint32)((uint32)0x1Fu << 0u))
#endif

#define PMM_PSCON_LPDDCSTAT2_LSTET                  (uint32)(0xFu << 16u)

#if defined(_RM46x_) || defined(_TMS570LS12x_)
#define PMM_PSCON_MPDDCSTAT1_MSTC                   (uint32)((uint32)0x3u << 0u)
#endif
#if defined(_RM48x_) || defined(_TMS570LS31x_)
#define PMM_PSCON_MPDDCSTAT1_MSTC                   (uint32)((uint32)0x7u << 0u)
#endif

#define PMM_PSCON_MPDDCSTAT1_MCMPE                  (uint32)(0x7u << 16u)

#if defined(_RM46x_) || defined(_TMS570LS12x_)
#define PMM_PSCON_MPDDCSTAT2_MSTE                   (uint32)(0x3u << 0u)
#endif
#if defined(_RM48x_) || defined(_TMS570LS31x_)
#define PMM_PSCON_MPDDCSTAT2_MSTE                   (uint32)(0x7u << 0u)
#endif

#define PMM_PSCON_MPDDCSTAT2_MSTET                  (uint32)(0x7u << 16u)

#endif /* __HAL_PMM_H__ */
