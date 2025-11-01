#ifndef __HAL_CCMR4F_H__
#define __HAL_CCMR4F_H__

typedef volatile struct _ccmr4fBASE_t
{
    uint32 _CCMSR;
    uint32 _CCMKEYR;
} ccmr4fBASE_t;

#define ccmr4fREG1   ((ccmr4fBASE_t *)0xFFFFF600u)

#define CCMR4F_CCMSR_CMPE	(uint32)(1u << 16u)
#define CCMR4F_CCMSR_STC	(uint32)(1u << 8u)
#define CCMR4F_CCMSR_STET	(uint32)(1u << 1u)
#define CCMR4F_CCMSR_STE 	(uint32)(1u << 0u)

#define CCMR4F_CCMKEYR_MODE                 (uint32)(0xFu << 0u)
#define CCMR4F_CCMKEYR_MODE_LOCKSTEP        (uint32)(0x0u)
#define CCMR4F_CCMKEYR_MODE_SELFTEST        (uint32)(0x6u)
#define CCMR4F_CCMKEYR_MODE_ERRFORCING      (uint32)(0x9u)
#define CCMR4F_CCMKEYR_MODE_STERRFORCG    (uint32)(0xFu)

#endif /* __HAL_CCMR4F_H__ */
