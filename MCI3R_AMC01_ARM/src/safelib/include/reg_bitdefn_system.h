
/* System Register Frame 1 Definition */
/** @struct sl_systemBase1
*   @brief System Register Frame 1 Definition
*
*   This type is used to access the System 1 Registers.
*/
#ifndef __HAL_SYSTEM_H__
#define __HAL_SYSTEM_H__

#define SYSESR_PORRST   (uint32)0x00008000u
#define SYSESR_OSCRST   (uint32)0x00004000u
#define SYSESR_WDRST    (uint32)0x00002000u

#if defined(_TMS570LS31x_) || defined(_TMS570LS12x_) || defined(_RM48x_) || defined(_RM46x_) || defined(_RM42x_) || defined(_TMS570LS04x_)
#define SYSESR_CPURST   (uint32)0x00000020u
#endif
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define SYSESR_CPURST0   (uint32)0x00000020u
#define SYSESR_CPURST1   (uint32)0x00000040u
#define SYSESR_ICSTRST   (uint32)0x00000080u
#endif

#define SYSESR_SWRST    (uint32)0x00000010u
#define SYSESR_EXTRST   (uint32)0x00000008u

#define SYS1_SYSECR_RST1 (uint32)(0x1u << 15u)
#define SYS1_SYSECR_RST0 (uint32)(0x1u << 14u)

#define SYS1_ISR1_KEY	(uint32)0x7500u

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define CSDIS_CLK0_EN	(uint32)0x0u
#define CDDIS_GCLK_EN	(uint32)0x0u
#endif

#define SYSREG1_MSINENA_MSIENA (uint32)0x00000001u


#endif /* __HAL_SYSTEM_H__ */
