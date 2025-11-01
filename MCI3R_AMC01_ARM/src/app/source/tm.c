/****************************************************************************************************
*FILENAME:     tm.c
*PURPOSE:      完成任务管理
*DATE          AUTHOR          CHANGE
*2017.08.09    胡德全          创建文件
****************************************************************************************************/
#include <stddef.h>
#include "tm_private.h"
#include "tm_timer.h"
#include "tm.h"
#include "error_class.h"

uint32_t g_cycleStartTick = 0U;
uint32_t g_maxCycleTick   = 0U;

static Tcb_t     s_taskCB[TM_CFG_MAX_TASK_NUMBER];
static TcbInfo_t s_taskInfo[TM_CFG_MAX_TASK_NUMBER];
tmState_t g_tmState;

/*函数声明*/
static int32_t taskInit(void);
static void idleTask(void);
static void setMaxCycleTick(uint32_t us);

/****************************************************************************************************
* Identifier:   SCOD-AMC01-017 (Trace to: SLD-AMC01-017)
* Function:     tmInit
* Description:  init the global variable of the NPTM.
* Input:        maxUS  : 最大周期 ，单位: us
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmInit(uint32_t maxUS)
{
    int32_t i = 0;

    for(i = 0; i < TM_CFG_MAX_TASK_NUMBER; i++)
    {
        s_taskCB[i].maxTick = 0u;
        s_taskCB[i].state = TASK_STATE_INIT;
        s_taskCB[i].method = 0U;
        s_taskCB[i].priority = 0;
        s_taskCB[i].initTask = NULL;
        s_taskCB[i].cycleTask = NULL;
    }
    setMaxCycleTick(maxUS);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-018 (Trace to: SLD-AMC01-018)
* Function:     taskInit
* Description:  initialize all task and check the cycle ticks.
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
static int32_t taskInit(void)
{
    fnInit_t  init = NULL;
    int32_t i = 0;
    uint32_t total = 0u;

    int32_t ret = 0;

    for(i = 0; i < TM_CFG_MAX_TASK_NUMBER; i++)
    {
        if(s_taskCB[i].state == TASK_STATE_READY)
        {
            init = s_taskCB[i].initTask;
            if(NULL != init)
            {
                init();
            }
            total += s_taskCB[i].maxTick;
        }
    }

    if(total >= tmGetMaxCycleTick())
    {
        /*must stop*/
        ret = TM_ERROR_TOTAL_TICKS;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-257 (Trace to: SLD-AMC01-257)
* Function:     idleTask
* Description:  the idle task of NPTM.
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
static void idleTask(void)
{
    uint32_t startTick = tmGetCurTick();
    uint32_t cycleMaxTick = tmGetMaxCycleTick();
    uint32_t endTick = 0u;
    uint32_t usedTick = 0u;
    uint32_t remainTick = 0u;

    /*increse the total cycle counter.*/
    g_tmState.cycleCnt++;

    tmIdleTaskHook();

    endTick = tmGetCurTick();
    usedTick = endTick - startTick;
    g_tmState.idleUsedTick = usedTick;
    usedTick = endTick - g_cycleStartTick;
    g_tmState.curCycleTick = usedTick;
    if (g_tmState.curCycleTick <= cycleMaxTick)
    {
        remainTick = cycleMaxTick - usedTick;
        while(1)
        {
            if(remainTick > (8U * 1000U))
            {     
                tmDelayTick(8000U);
                remainTick = remainTick - 8000U;
            }
            else
            {
                tmDelayTick(remainTick);
                break;
            }
        }
        g_tmState.timeoutTick = 0u;
        
    } /*timeout.*/
    else
    {
        g_tmState.timeoutTick = g_tmState.curCycleTick - cycleMaxTick;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-266 (Trace to: SLD-AMC01-266)
* Function:     tmCreateTask
* Description:  the api of NPTM that create the user task.
* Input:        priority-the priority of the task.
                maxUS-max time that the task allowe to run.
                init-the initialization function pointer of the task.
                cycle-the cycle function pointer of the task.
* Output:       none
* Return:       TM_ERROR_PRIORITY-the priority is exceed the max.
                TM_ERROR_NO_TASK-the cycle function pointer is null.
* Others:    
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
int32_t tmCreateTask(int32_t   priority,
                     uint32_t  maxUS,
                      int8_t   method,
                     fnInit_t  init,
                     fnCycle_t cycle
                    )
{
    int32_t ret = 0;

    if((priority >= TM_CFG_MAX_TASK_NUMBER) || (NULL == cycle))
    {
        ret = TM_ERROR_PRIORITY + ERROR_TOTAL_TICKS - 1;
    }
    else
    {
        s_taskCB[priority].initTask = init;
        s_taskCB[priority].cycleTask = cycle;
        s_taskCB[priority].method = method;
        s_taskCB[priority].maxTick = TM_CFG_US_TO_TICK(maxUS);
        s_taskCB[priority].state = TASK_STATE_READY;
        s_taskCB[priority].priority = priority;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-267 (Trace to: SLD-AMC01-267)
* Function:     tmStartSchedule
* Description:  the api of NPTM that schedule all tasks.
* Input:        none
* Output:       none
* Return:       none
* Others:    
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmStartSchedule(void)
{
    int32_t i = 0;
    uint32_t startTick = 0U;
    uint32_t endTick = 0U;
    uint32_t usedTick = 0U;
    uint32_t maxTick = 0U;
    uint32_t remainTick = 0U;
    fnCycle_t cycle = NULL;
    int32_t ret = 0;

    /* 初始化所有任务 */
    ret = taskInit();    
    if(ret != 0)
    {
        /* 创建，不能运行 */
        g_tmState.tmError = ret;
        tmStopHook();
    }

    /* 周期运行，永不退出 */
    while(1)
    {
        g_cycleStartTick = tmGetCurTick();
        for(i = 0; i < TM_CFG_MAX_TASK_NUMBER; i++)
        {
            if(s_taskCB[i].state == TASK_STATE_READY)
            {
                startTick = tmGetCurTick();

                s_taskInfo[i].taskRet = 0;
                g_tmState.curTask = i;
                maxTick = s_taskCB[i].maxTick;

                if(s_taskCB[i].priority != i)
                {
                    /* 任务执行顺序异常，停止 */
                    g_tmState.tmError = TM_ERROR_TASK_ORDER;
                    tmStopHook();
                }

                cycle = s_taskCB[i].cycleTask;
                if(NULL != cycle)
                {
                    s_taskInfo[i].taskRet = cycle();
                }

                endTick = tmGetCurTick();

                usedTick = endTick - startTick; /* get the used time.unit: us */
                s_taskInfo[i].usedTick = usedTick;
                if ((uint8_t)USED_WAIT == s_taskCB[i].method) /* is wait method */
                {
                    if (usedTick <= maxTick)
                    {
                        s_taskInfo[i].timeoutTick = 0U;
                        remainTick = maxTick - usedTick;
                        tmDelayTick(remainTick);
                    }
                    else/* timeout. */
                    {
                        s_taskInfo[i].timeoutTick = usedTick - maxTick;
                    }
                }
                else/* ignore method. */
                {
                }             
            }
        }

        idleTask();
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-268 (Trace to: SLD-AMC01-268)
* Function:     tmGetTaskTimeoutTick
* Description:  获取任务超时时间
* Input:        i : 任务优先级
* Output:       none
* Return:       ret 任务超时时间值,单位us
* Others:    
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
uint32_t tmGetTaskTimeoutTick(int32_t i)
{
    uint32_t ret = 0U;

    if(i<TM_CFG_MAX_TASK_NUMBER)
    {
        ret = s_taskInfo[i].timeoutTick;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-269 (Trace to: SLD-AMC01-269)
* Function:     tmGetTaskUsedTick
* Description:  获取任务已使用时间
* Input:        i : 任务优先级
* Output:       none
* Return:       时间值,单位us
* Others:       
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
uint32_t tmGetTaskUsedTick(int32_t i)
{
    uint32_t ret = 0U;

    if(i<TM_CFG_MAX_TASK_NUMBER)
    {
        ret = s_taskInfo[i].usedTick;
    }
    else if(i==TM_CFG_MAX_TASK_NUMBER)
    {
        ret = g_tmState.idleUsedTick;
    }
    else
    {
        ret = 0U;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-270 (Trace to: SLD-AMC01-270)
* Function:     tmCheckRemainTick
* Description:  获取当前周期剩余时间
* Input:        none
* Output:       none
* Return:       时间值,单位us
* Others:    
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
uint32_t tmCheckRemainTick(void)
{
    uint32_t cur = 0U;
    uint32_t value = 0U;

    cur = tmGetCurTick();
    value = cur - g_cycleStartTick;

    if(g_maxCycleTick >= value)
    {
        value = g_maxCycleTick  - value;
    }
    else
    {
        value = 0U;
    }

    return value;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-271 (Trace to: SLD-AMC01-271)
* Function:     tmSetCycleStart
* Description:  设置周期超始时间
* Input:        tick  设置的时间，单位us
* Output:       none
* Return:       none
* Others:    
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmSetCycleStart(uint32_t tick)
{
    g_cycleStartTick = tick;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-043 (Trace to: SLD-AMC01-043)
* Function:     tmSuspendTask
* Description:  挂起指定任务
* Input:        i : 任务优先级
* Output:       none
* Return:       none
* Others:    
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmSuspendTask(int32_t i)
{
    if(i<TM_CFG_MAX_TASK_NUMBER)
    {
        s_taskCB[i].state = TASK_STATE_SUSPEND;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-044 (Trace to: SLD-AMC01-044)
* Function:     tmResumeTask
* Description:  唤醒指定任务
* Input:        i : 任务优先级
* Output:       none
* Return:       none
* Others:    
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmResumeTask(int32_t i)
{
    if(i < TM_CFG_MAX_TASK_NUMBER)
    {
        s_taskCB[i].state = TASK_STATE_READY;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-272 (Trace to: SLD-AMC01-272)
* Function:     setMaxCycleTick
* Description:  set the max cycle tick of the NPTM.
* Input:        周期时间，单位: 微秒
* Output:       none
* Return:       none
* Others:    
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
static void setMaxCycleTick(uint32_t us)
{
    g_maxCycleTick = TM_CFG_US_TO_TICK(us);
}

