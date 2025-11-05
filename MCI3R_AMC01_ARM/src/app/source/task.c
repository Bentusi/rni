/**************************************************************************************************
 * Filename: task.c
 * Purpose:  创建所有运行任务
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
**************************************************************************************************/
#include "lynx_types.h"
#include "tm.h"
#include "task.h"
#include "error_class.h"
#include "maintain.h"
#include "standby.h"
#include "info_class.h"
#include "cfg_class.h"
#include "com_hw.h"
#include "switch.h"
#include "module_class.h"
#include "logic_class.h"
#include "wtd.h"
#include "monitor.h"
#include "algInterface.h"

static int32_t rxLocalTask(void);
static int32_t txLocalTask(void);

/*********************************************************************
* Identifier:   SCOD-AMC01-352 (Trace to: SLD-AMC01-352)
* Function:     rxLocalTask
* Description:  接收本地所有板卡的数据
* Input:        none
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
***********************************************************************/
static int32_t rxLocalTask(void)
{
    int32_t slot = 0, port = 0;
    uint32_t type = 0U;
    portAttr_t *pMsg = NULL;

    /* 状态收集 */
    com1UpdateCaseState(0);
    com1UpdateMpuState();

    for(slot = 0; slot < LYNX_SLOT_MAX; slot++)
    {
        type = cfgCardTypeOnSlot(slot);
        LYNX_ASSERT(type < MODULE_TYPE_MAX);

        if(NULL != g_moduleHandle[type].pFunRxHook)
        {
            (void)g_moduleHandle[type].pFunRxHook(slot, PRE_ACTION);
        }
    }

    for(slot = 0; slot < LYNX_SLOT_MAX; slot++)
    {
        type = cfgCardTypeOnSlot(slot);
        LYNX_ASSERT(type < MODULE_TYPE_MAX);

        for(port = 0; port < COM_PORT_MAX; port++)
        {
            pMsg = glGetPortAttr(slot, port, RX_PORT);
            if((NULL != g_moduleHandle[type].pFunRx) &&
                    (pMsg->pAddr != NULL))
            {
                (void)g_moduleHandle[type].pFunRx(slot, port, pMsg->pAddr);
            }
        }
    }

    for(slot = 0; slot < LYNX_SLOT_MAX; slot++)
    {
        type = cfgCardTypeOnSlot(slot);
        LYNX_ASSERT(type < MODULE_TYPE_MAX);

        if(NULL != g_moduleHandle[type].pFunRxHook)
        {
            (void)g_moduleHandle[type].pFunRxHook(slot, POST_ACTION);
        }
    }

    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-351 (Trace to: SLD-AMC01-351)
* Function:     txLocalTask
* Description:  send all output data to all output cards.
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
static int32_t txLocalTask(void)
{
    int32_t slot = 0, port = 0;
    uint32_t type = 0U;
    portAttr_t *pMsg = NULL;

    for(slot = 0; slot < LYNX_SLOT_MAX; slot++)
    {
        type = cfgCardTypeOnSlot(slot);
        LYNX_ASSERT(type < MODULE_TYPE_MAX);

        if(NULL != g_moduleHandle[type].pFunTxHook)
        {
            (void)g_moduleHandle[type].pFunTxHook(slot, PRE_ACTION);
        }
    }

    for(slot = 0; slot < LYNX_SLOT_MAX; slot++)
    {
        type = cfgCardTypeOnSlot(slot);
        LYNX_ASSERT(type < MODULE_TYPE_MAX);

        for(port = 0; port < COM_PORT_MAX; port++)
        {
            pMsg = glGetPortAttr(slot, port, TX_PORT);
            if((NULL != g_moduleHandle[type].pFunTx) &&
                    (pMsg->pAddr != NULL))
            {
                (void)g_moduleHandle[type].pFunTx(slot, 0, pMsg->pAddr);
            }
        }
    }

    for(slot = 0; slot < LYNX_SLOT_MAX; slot++)
    {
        type = cfgCardTypeOnSlot(slot);
        LYNX_ASSERT(type < MODULE_TYPE_MAX);

        if(NULL != g_moduleHandle[type].pFunTxHook)
        {
            (void)g_moduleHandle[type].pFunTxHook(slot, POST_ACTION);
        }
    }

    return 0;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-256 (Trace to: SLD-AMC01-256)
* Function:     tmIdleTaskHook
* Description:  空闲任务函数
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
***********************************************************************/
void tmIdleTaskHook(void)
{
    static int32_t allow = 0;
    int32_t errLevel = 0;

    /* set system mode that user setted. */
    mtUserModeProcess();
    sbCheckMode();
    mtMSMtDataProcess();

    /* ARM CPU 状态 */
    infoCurCpuError();
    /* ARM 内部硬件自诊断 */
    diagHardwareCycle();

    diagHardwareCyclePlus(0U);

    /* 更新lynx所有状态 */
    infoUpdateMpuStates();
    
    wdFeed(WD_1);

    com1ReportArmState(ARM_OK);

    /* 对平台中错误进行处理并显示 */
    if(allow > 10)
    {
        infoSetOutputState(CUR_STATE_ENABLE_OUTPUT);
        errLevel = errHandle();
        if((errLevel > 0)&&(errLevel < 4))
        {
            errStopHandleCycle();
        }
        sbSyncLedShow();
        /* 发送平台的维护数据 */
        mtTxHandle();
    }
    else
    {
        allow++;
        infoSetOutputState(CUR_STATE_DISABLE_OUTPUT);
        ifAgLogicTaskInit();
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-016 (Trace to: SLD-AMC01-016)
* Function:     createAllTask
* Description:  创建所有的运行任务
* Input:        none
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void createAllTask(void)
{
    int32_t ret = 0;
    /* recive data task */
    ret = tmCreateTask(LX_RX_TASK,
                       RECEIVE_TASK_TIME,
                       USED_WAIT,
                       NULL,
                       rxLocalTask);
    if(ret == TASK_CREAT_ERROR)
    {
        errStopHandleInit(TASK_CREAT_ERROR);
    }

    if(cfgLocalPfMode() == CFG_REDUNDANCY)
    {
        /* 热备冗余 */
        ret = tmCreateTask(PF_ASM_TASK,
                           MPU_ASM_MAX_TIME,
                           USED_WAIT,
                           sbTaskInit,
                           sbTask);

        if(ret == TASK_CREAT_ERROR)
        {
            errStopHandleInit(TASK_CREAT_ERROR);
        }
    }


    ret = tmCreateTask(LX_USR_TASK,
                       MPU_USR_LOGIC_MAX_TIME ,
                       USED_WAIT,
                       ifAgLogicTaskInit,
                       ifAgLogicTaskRun);

    if(ret == TASK_CREAT_ERROR)
    {
        errStopHandleInit(TASK_CREAT_ERROR);
    }

    /* send data task */
    ret = tmCreateTask(PF_TX_TASK,
                       MPU_SEND_LOCAL_MAX_TIME,
                       USED_WAIT,
                       NULL,
                       txLocalTask);

    if(ret == TASK_CREAT_ERROR)
    {
        errStopHandleInit(TASK_CREAT_ERROR);
    }

    /* maintan task */
    ret = tmCreateTask(PF_MAIT_TASK,
                       MAINTAIN_TASK_TIME,
                       USED_WAIT,
                       NULL,
                       mtModeTask);
    if(ret == TASK_CREAT_ERROR)
    {
        errStopHandleInit(TASK_CREAT_ERROR);
    }

    /* 当前运行模式不在维护模式下需 挂起维护任务 */
    if(infoCurSysMode() != (uint32_t)SYS_MAIT)
    {
        tmSuspendTask(PF_MAIT_TASK);
    }
}

