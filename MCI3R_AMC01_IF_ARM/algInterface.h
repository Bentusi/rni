/**************************************************************************************************
*FILENAME:     algInterface.h
*PURPOSE:      基础固件与系统应用接口
*DATE          AUTHOR          CHANGE
*2023.05.12    wxb             创建文件
**************************************************************************************************/
#ifndef _ALGINTERFACE_H_
#define _ALGINTERFACE_H_

#include "lynx_types.h"

#define ALG_VERSION    (GEN_VERSION('3','0','0','0'))

/* 算法部分接口地址 */
#define USER_IMAGE_LOAD_INIT_ADDR        (0x80000U)
#define USER_IMAGE_INIT_ADDR             (0x80004U)
#define USER_IMAGE_CYCLE_ADDR            (0x80008U)
#define USER_IMAGE_GET_FORCE_ADDR        (0x8000CU)
#define USER_IMAGE_GET_PARA_ADDR         (0x80010U)

/* 供算法调用函数接口 */
typedef struct
{
    uint64_t (*pFunDrv2CrcCal)(const void *pSrc, uint32_t len);
    uint8_t *(*pFunLxGetOutChAddr)(int32_t slot, int32_t ch);
    uint8_t *(*pFunLxGetInChAddr)(int32_t slot, int32_t ch);
    int32_t (*pFunInfoGetSysVar)(int32_t slot, int32_t para);
    void (*pFunLxSetParaHead)(void *pAddr);
    void (*pFunLxSetCfgHead)(void *pHead);
    void (*pFunLxSetImageHead)(void *pAddr);
    void (*pFunLxSetUsrInAddr)(void *pAddr);
    void (*pFunLxSetUsrOutAddr)(void *pAddr);
    void (*pFunLxSetNetPortAddr)(int32_t slot, int32_t port, void *pAddr, int32_t para);
    void (*pFunLxSetForceHead)(void *pAddr);
    void (*pFunLxGetStation)(uint8_t* st);
} algMethod_t;

/* 外部函数声明 */
extern int32_t ifAgLoadInit(void);
extern void ifAgLogicTaskInit(void);
extern int32_t ifAgLogicTaskRun(void);
extern const uint32_t *ifAgGetOneForceAddr(uint32_t index);
extern const uint32_t *ifAgGetOneParaAddr(uint32_t index);

#endif /* _ALGINTERFACE_H_ */

