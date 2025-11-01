/**************************************************************************************************
*Filename:     cfg_class.h
*Purpose:      板卡配置信息或硬件信息相关的接口函数
*Log:          Date          Author    Modified
*              2021/08/31    hdq       create
**************************************************************************************************/
#ifndef _CFG_CLASS_H_
#define _CFG_CLASS_H_

#include "global.h"
#include "lynx_types.h"

#define cfgInputRamSize()        (lxGetCfg()->cardCfg[g_localSlot].difCfg.cpuCardCfg.inputRamSize)
#define cfgOutputRamSize()       (lxGetCfg()->cardCfg[g_localSlot].difCfg.cpuCardCfg.outputRamSize)
#define cfgForceRamSize()        (lxGetCfg()->cardCfg[g_localSlot].difCfg.cpuCardCfg.forceRamSize)
#define cfgParaRamSize()         (lxGetCfg()->cardCfg[g_localSlot].difCfg.cpuCardCfg.paraRamSize)
#define cfgLogicGlobalSize()     (lxGetCfg()->cardCfg[g_localSlot].difCfg.cpuCardCfg.logicGlobalSize)
#define cfgLogicGlobalStart()    (lxGetCfg()->cardCfg[g_localSlot].difCfg.cpuCardCfg.logicGlobalStart)

#define cfgMpuPeriod()           (lxGetCfg()->cardCfg[g_localSlot].netCfg.mpuPeriod)
#define cfgLocalMpuId()          (lxGetCfg()->cardCfg[g_localSlot].baseCfg.id.value)
#define cfgLocalPfMode()         (lxGetCfg()->cardCfg[g_localSlot].netCfg.pfCfg)

extern uint32_t cfgCardTypeOnSlot(int32_t slot);
extern const comPortCfg_t *cfgNetPortRxCfgAddr(int32_t slot, int32_t port);
extern const comPortCfg_t *cfgNetPortTxCfgAddr(int32_t slot,int32_t port);
extern const baseCfg_t *cfgGetBaseAddr(int32_t slot);
extern uint32_t cfgGetCardEnableCh(int32_t slot);
extern uint32_t cfgSlotToId(int32_t slot);
extern const lynxCfg_t *lxGetCfg(void);
extern fileHead_t *lxGetCfgHead(void);
extern void lxSetCfgHead(void *pHead);

#endif

