/**************************************************************************************************
 * Filename: task.h
 * Purpose:  创建所有运行任务
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
**************************************************************************************************/
#ifndef _TASK_H_
#define _TASK_H_

#include "lynx_types.h"

/* MAX time */
#define RECEIVE_TASK_TIME           ((uint32_t)3U * (uint32_t)1000U)                /* 接收任务最大时间 */
#define MPU_ASM_MAX_TIME            ((uint32_t)1U * (uint32_t)1000U)                /* 热备同步任务最大时间 */
#define MPU_USR_LOGIC_MAX_TIME      ((cfgMpuPeriod() / 5U) *(uint32_t)1000U)        /* 用户算法 最大时间 */
#define MPU_SEND_LOCAL_MAX_TIME     ((uint32_t)3U * (uint32_t)1000U)                /* 发送任务最大时间 */
#define MAINTAIN_TASK_TIME          ((uint32_t)1U * (uint32_t)1000U)                /* 维护任务最大时间 */

/* TASK define */
#define LX_RX_TASK        (0)                /* 接收任务 */
#define LX_USR_TASK       (1)                /* 用户算法任务 */
#define PF_ASM_TASK       (2)                /* 热备冗余任务 */
#define PF_TX_TASK        (3)                /* 发送任务 */
#define PF_MAIT_TASK      (4)                /* 维护任务 */

/* 外部函数声明 */
extern void createAllTask(void);

#endif

