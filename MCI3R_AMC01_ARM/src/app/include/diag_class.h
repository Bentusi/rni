/***************************************************************************************************
 * Filename: diag_class.h
 * Purpose:  应用层的诊断接口
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/
#ifndef _DIAG_CLASS_H_
#define _DIAG_CLASS_H_

#include <stdint.h>
#include "error_class.h"

#define GET_BIT_NUM(x)      ((uint32)((uint32)1U <<(uint32)(x)))

#define ERR_NO                              (300)
#define ERR_EFUSE_SELF_TEST                 (301)
#define ERR_EFUSE_ECC                       (302)
#define ERR_SRAM_SELF_TEST                  (303)
#define ERR_PSON_SELF_TEST                  (304)
#define ERR_PSCON_ERR_FORCE                 (305)
#define ERR_PSCON_SL_ERR_FORCE              (307)
#define ERR_PSON_PMA_TEST                   (309)
#define ERR_PERIPHT_UNPRI_ACC_S1            (313)
#define ERR_PERIPH_UNPRI_ACC_S2             (315)
#define ERR_PERIPH_UNPRI_ACC_S3             (317)
#define ERR_ADC1_SRAM_PARITY_TEST           (318)
#define ERR_FLASH_ECC_TEST_MODE_1BIT        (319)
#define ERR_PBIST_MARCH13N_2PORT_MEMORY     (323)
#define ERR_PBIST_MARCH13N_ROM_MEMORY       (324)
#define ERR_CCMR5F_CPUCOMP_SL               (325)
#define ERR_CCMR5F_CPUCOMP_SL_ERR_FORCE     (326)
#define ERR_CCMR5F_PDCOMP_SL                (328)
#define ERR_CCMR5F_PDCOMP_SL_ERR_FORCE      (329)
#define ERR_CCMR5F_INMCOMP_SL               (331)
#define ERR_CCMR5F_INMCOMP_SL_ERR_FORCE     (332)
#define ERR_DCC                             (337)
#define ERR_ST_MAX                          (339)

extern void diagHardwareInit(void);
extern void diagHardwareCycle(void);
extern void diagHardwareCyclePlus(uint8_t flag);

#endif

