/****************************************************************************************************
*FILENAME:     tm.h
*PURPOSE:      完成任务管理
*DATE          AUTHOR          CHANGE
*2017.08.09    胡德全             创建文件
****************************************************************************************************/
#ifndef TM_H
#define TM_H

#include "tm_private.h"
#include "tm_timer.h"

#define TM_CFG_MAX_TASK_NUMBER              (0x05) /*TM支持最大任务数*/
#define TM_CFG_CYCLE_START_HOOK             (0x00) /*周期开始勾子函数使能*/
#define TM_CFG_PERIOD_EXPIRE_HOOK           (0x00) /*周期超时勾子函数使能*/
#define TM_CFG_TASK_ERR_HOOK                (0x00) /*任务运行错误勾子函数使能*/
#define TM_CFG_TASK_IDLE_HOOK               (0x01) /*周期空闲勾子函数使能*/
#define TM_CFG_DEBUG_TASK_HOOK              (0x00) /*TM调试函数使能*/

void tmSetCycleStart(uint32_t tick);
extern void tmInit(uint32_t maxUS);
extern void tmStartSchedule(void);
extern uint32_t tmCheckRemainTick(void);
extern void tmIdleTaskHook(void);

#endif

