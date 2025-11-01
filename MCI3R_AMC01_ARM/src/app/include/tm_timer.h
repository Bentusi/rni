/****************************************************************************************************
*FILENAME:     tm_timer.h
*PURPOSE:      系统时间管理
*DATE          AUTHOR          CHANGE
*2017.08.09    胡德全          创建文件
****************************************************************************************************/
#ifndef TM_TIMER_H
#define TM_TIMER_H

#include <stdint.h>

#define TM_CFG_TICK_TO_US(tick)    ((tick))
#define TM_CFG_US_TO_TICK(us)      ((us))

#define ENABLE_CNT1                ((uint32_t)1U << 1)
#define ENABLE_CNT0                ((uint32_t)1U << 0)
#define TICK_TIMER_STOP            (1U)
#define TICK_TIMER_START           (2U)

void tmDelayTick(uint32_t tick);
void tmDelayus(uint32_t us);
void tmDelayms(uint32_t ms);

#endif
