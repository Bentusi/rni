/****************************************************************************************************
*FILENAME:     tm_private.h
*PURPOSE:      任务管理私有数据
*DATE          AUTHOR          CHANGE
*2017.08.09    胡德全             创建文件
****************************************************************************************************/
#ifndef TM_PRIVATE_H
#define TM_PRIVATE_H

#include <stdint.h>

typedef void  (*fnInit_t) (void);
typedef int32_t  (*fnCycle_t)(void);

typedef enum
{
    USED_WAIT = 0,  /*  waiting */
    USED_SKIP   /*  discard */
}IdleMethod_t;

/*task state*/
typedef enum
{
    TASK_STATE_INIT    = 0,
    TASK_STATE_READY,
    TASK_STATE_SUSPEND
}TaskState_t;

typedef enum
{
    TM_ALL_OK            = 0,
    TM_ERROR_TOTAL_TICKS,
    /*only for task*/
    TM_ERROR_PRIORITY,
    TM_ERROR_TASK_ORDER
}TmErrorInfo_t;

typedef struct
{
    uint32_t     maxTick;   /*任务可用允许的最大时间*/
    fnInit_t     initTask;  /*任务初始化函数指针 */
    fnCycle_t    cycleTask; /*任务周期运行函数指针*/
    int32_t      priority;  /*任务的优先级，值越小优先级越高*/
    TaskState_t  state;    /*任务的状态*/
    int8_t       method;   /*任务空闲时间处理方式 */
} Tcb_t;

typedef struct 
{
    uint32_t    usedTick;       /*实际使用时间*/
    uint32_t    timeoutTick;    /*超时时间*/
    int32_t     taskRet;        /*任务返回值*/
}TcbInfo_t;

typedef struct
{
    uint32_t idleUsedTick;
    int32_t  tmError;
    uint32_t cycleCnt;
    uint32_t curCycleTick;
    uint32_t timeoutTick;
    uint8_t  stopTM;
    int32_t  curTask;
}tmState_t;

extern tmState_t g_tmState;
#define tmCurrentTask()              (g_tmState.curTask)
#define tmGetCycleCnt()              (g_tmState.cycleCnt)
#define tmGetError()                 (g_tmState.tmError)


#define tmGetCycleTimeoutTick()      (g_tmState.timeoutTick)
#define tmGetCurCycleTick()          (g_tmState.curCycleTick)

extern uint32_t g_cycleStartTick;
#define tmGetCycleStartTick()        (g_cycleStartTick)


/* get the max cycle ticks. */
extern uint32_t g_maxCycleTick;
#define tmGetMaxCycleTick()          (g_maxCycleTick)

/*外部函数接口*/
extern uint32_t tmGetCurTick(void);
extern void tmTimerStart(uint32_t flag);

extern void tmStopHook(void);

extern int32_t tmCreateTask(int32_t   priority,
                     uint32_t  maxUS,
                     int8_t    method,
                     fnInit_t  init,
                     fnCycle_t cycle);

extern uint32_t tmGetTaskUsedTick(int32_t i);
extern uint32_t tmGetTaskTimeoutTick(int32_t i);

extern void tmSuspendTask(int32_t i);
extern void tmResumeTask(int32_t i);

#endif

