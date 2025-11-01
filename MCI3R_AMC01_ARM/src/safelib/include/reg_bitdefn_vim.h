
/* Vim Register Frame Definition */
/** @struct sl_vimBase
*   @brief Vim Register Frame Definition
*
*   This type is used to access the Vim Registers.
*/

#ifndef __HAL_VIM_H__
#define __HAL_VIM_H__

#include <HL_sys_vim.h>

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define VIM_SRAM_PARITY_DISABLED					(uint32)0x5u
#define VIM_SRAM_PARITY_ENABLED						(uint32)0xAu
#define VIM_PARCTRL_PARITY_EN_START					(uint32)0u
#define VIM_PARCTRL_PARITY_EN_LENGTH				(uint32)4u
#define VIM_TEST_MODE								(uint32)0x100u
#define VIM_PAR_ERR_FLG								(uint32)0x1u
#endif

#define VIM_SSI_REQUEST								(uint32)21u

/* VIM General Configuration */

typedef volatile struct sl_vimRam
{
    t_isrFuncPTR ISR[VIM_CHANNELS + 1U];
} sl_vimRAM_t;

#define sl_vimRAM ((sl_vimRAM_t *)0xFFF82000U)

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define VIM_CHANNEL_EPC_FIFO_CAM_FULL		(1u << 28u)
#endif

#endif /* __HAL_VIM_H__ */
