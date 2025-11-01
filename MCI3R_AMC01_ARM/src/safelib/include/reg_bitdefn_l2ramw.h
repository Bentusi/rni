
/** @struct sl_tcramBase
*   @brief TCRAM Wrapper Register Frame Definition
*
*   This type is used to access the TCRAM Wrapper Registers.
*/
#ifndef __HAL_TCMRAM_H__
#define __HAL_TCMRAM_H__

#include <HL_reg_l2ramw.h>

#define L2RAM_RAMCTRL_ADDR_PARITY_OVER 				(uint32)(0xDu << 24u)
#define L2RAM_RAMCTRL_ADDR_PARITY_OVRRIDE_MASK 		(uint32)(0xFu << 24u)
/*macro names changed so as to adhere to misra rule MISRA-C:2004 5.1 */
#define L2RAM_RAMCTRL_ADDR_PAR_DIS_START 			(uint32)16u
/*macro names changed so as to adhere to misra rule MISRA-C:2004 5.1 */
#define L2RAM_RAMCTRL_ADDR_PAR_DIS_LENGTH 			(uint32)4u
#define L2RAM_RAMCTRL_ECC_DETECT_EN_START 			(uint32)0u
#define L2RAM_RAMCTRL_ECC_DETECT_EN_LENGTH 			(uint32)4u
#define L2RAM_RAMCTRL_ECCWREN 						(uint32)(1u << 8u)

#define L2RAM_RAMERRSTATUS_DRDE 					(uint32)(22u)
#define L2RAM_RAMERRSTATUS_DRSE 					(uint32)(21u)
#define L2RAM_RAMERRSTATUS_DWDE 					(uint32)(20u)
#define L2RAM_RAMERRSTATUS_DWSE 					(uint32)(19u)
#define L2RAM_RAMERRSTATUS_MMDE						(uint32)(1u << 12u)
#define L2RAM_RAMERRSTATUS_WEMDE					(uint32)(1u << 11u)
#define L2RAM_RAMERRSTATUS_REMDE					(uint32)(1u << 10u)
#define L2RAM_RAMERRSTATUS_RMWDE					(uint32)(1u << 7u)
#define L2RAM_RAMERRSTATUS_CPUDE					(uint32)(1u << 5u)
#define L2RAM_RAMERRSTATUS_ADDE						(uint32)(1u << 4u)
#define TCRAM_RAMERRSTAT_ADDCMPLG_FAIL				(uint32)(1u << 4u)
#define L2RAM_RAMERRSTATUS_ADE						(uint32)(1u << 2u)
#define TCRAM_RAMERRSTAT_ADDR_DEC_FAIL				(uint32)(1u << 2u)
#define L2RAM_RAMERRSTATUS_DIAG_DOUBLEBIT_ERR		(uint32)(0x500000)
#define L2RAM_RAMERRSTATUS_CLRALL					(uint32)(0x781C10)

#define TCRAM_RAMERRSTATUS_ADDR_SERR 				(uint32)(1u << 0u)
#define TCRAM_RAMSERRADD_SINGLE_ERRADD

#define TCRAM_RAMUERRADDR_UNC_ERRADD 			(uint32)0x3FFFFU

#define SRAM_ADDR_PARITY_DISABLED 					(uint32)0xAU
#define SRAM_ECC_DISABLED 							(uint32)0x5U

#define L2RAM_SYN_1BIT_DATA_ECC    		       		((uint64)0xCECECECECECECECEU)/*this corrupts the ecc for data bit 0*/
#define L2RAM_SYN_2BIT_DATA_ECC  			 	 	((uint64)0x0303030303030303U)/* this corrupts the ecc for 2 bit error*/

#define L2RAM_RAMTEST_TST_TRIGGER 					(uint32)((uint32)1u << 8u)
#define TCRAM_RAMTEST_TST_TRIGGER 					(uint32)((uint32)1u << 8u)

#endif /* __HAL_TCMRAM_H__ */
