/**************************************************************************************************
*Filename:     info_class.h
*Purpose:      平台状态管理模块
*Log:          Date          Author    Modified
*              2021/9/20     hdq       create
**************************************************************************************************/
#ifndef _INFO_CLASS_H_
#define _INFO_CLASS_H_

#include "lynx_types.h"

/* 状态说明
 * 0 板卡状态
 * 1 通信状态
 * 2 通道状态
 */
#define MPU_MASTER              (0xFFU)
#define MPU_SLAVE               (0x00U)

#define infoCurOutputState()    (infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.outputState)
#define infoCurSysMode()        (infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.curTrueMode)
#define infoCurMpuId()          (infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.id)

extern void infoSetSysState(int32_t s);
extern void infoClearPfError(int32_t err);
extern void infoSetPfError(int32_t err);
extern void infoSetStError(int32_t err);
extern void infoClearStError(int32_t err);
extern void infoSetOutputState(uint8_t s);
extern void infoUpdateCardError(void);
extern void infoSetHw(int32_t slot,uint32_t value);
extern uint32_t infoGetCom(int32_t slot);
extern void infoSetCom(int32_t slot, uint32_t value);
extern void infoSetsoft(int32_t slot, uint32_t value);
extern uint32_t infoGetsoft(int32_t slot);
extern void infoSetIoCh(int32_t slot, int32_t ch, uint8_t value);
extern uint32_t infoGetIoCh(int32_t slot, int32_t ch);
extern void infoUpdateMpuStates(void);
extern void infoSetMpuMS(uint8_t value);
extern uint32_t infoGetMpuMS(void);
extern void infoCurCpuError(void);
extern lynxInfo_t *infoGetAddr(void);
extern portState_t *infoGetPortState(int32_t slot, int32_t port, int32_t para);
extern int32_t infoGetIoSys(int32_t slot, int32_t para, int32_t index);
extern uint32_t infoModuleIoToSys(int32_t slot);
extern uint32_t infoHwToSys(int32_t slot);
extern uint32_t infoSoftToSys(int32_t slot);
extern uint32_t infoComToSys(int32_t slot);
extern void infoClearSlot(int32_t slot);
extern int32_t infoGetSysVar(int32_t slot, int32_t para);
extern int32_t infoGetComSys(int32_t slot, int32_t para, int32_t idx);
extern int32_t infoGetPfErrorState(int32_t err);
extern int32_t infoGetMpuSysVar(int32_t slot, int32_t para, int32_t idx);
extern void infoIOComPatch(void);
extern void infoGetStation(uint8_t* st);
#endif

