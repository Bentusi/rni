#ifndef __HAL_CCMR5F_H__
#define __HAL_CCMR5F_H__

typedef volatile struct _ccmr5fBASE_t
{
    uint32 _CCMSR1;
    uint32 _CCMKEYR1;
    uint32 _CCMSR2;
    uint32 _CCMKEYR2;
    uint32 _CCMSR3;
    uint32 _CCMKEYR3;
    uint32 _CCMPOLCNTRL;
    uint32 _CCMSR4;
    uint32 _CCMKEYR4;
    uint32 _CCMPDSTAT0;
} ccmr5fBASE_t;

#define ccmr5fREG1   ((ccmr5fBASE_t *)0xFFFFF600u)

#define CCMR5F_CCMSR_CMPE	(uint32)(1u << 16u)
#define CCMR5F_CCMSR_STC	(uint32)(1u << 8u)
#define CCMR5F_CCMSR_STET	(uint32)(1u << 1u)
#define CCMR5F_CCMSR_STE 	(uint32)(1u << 0u)

#define CCMR5F_CCMKEYR_MODE                 (uint32)(0xFu << 0u)
#define CCMR5F_CCMKEYR_MODE_LOCKSTEP        (uint32)(0x0u)
#define CCMR5F_CCMKEYR_MODE_SELFTEST        (uint32)(0x6u)
#define CCMR5F_CCMKEYR_MODE_ERRFORCING      (uint32)(0x9u)
#define CCMR5F_CCMKEYR_MODE_STERRFORCING    (uint32)(0xFu)

#endif /* __HAL_CCMR5F_H__ */
