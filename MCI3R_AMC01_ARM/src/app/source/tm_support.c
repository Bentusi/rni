/****************************************************************************************************
*FILENAME:     tm_support.c
*PURPOSE:      任务管理
*DATE          AUTHOR          CHANGE
*2017.08.09    胡德全          创建文件
*2019.01.22    张庆林          删除tmTimerInit函数
****************************************************************************************************/
#include "tm.h"
#include "info_class.h"
#include "error_class.h"
#include "wtd.h"
#include "bsp.h"

#include "maintain.h"

/****************************************************************************************************
* Identifier:   SCOD-AMC01-273 (Trace to: SLD-AMC01-273)
* Function:     tmGetCurTick
* Description:  used by NPTM,return the current tick.
* Input:        none
* Output:       none
* Return:       current tick
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
uint32_t tmGetCurTick(void)
{
    /*最大只能到0xffffffff*/
    /**
     * rtiGetCounter - get the current value of specified free running counter
     * @param counter: counter number, 0/1, or macro RTI_COUNTER0/1
     * @return: current value of specified free running counter
     */
    return (rtiREG1->CNT[rtiCOMPARE0].FRCx);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-274 (Trace to: SLD-AMC01-274)
* Function:     tmTimerStart
* Description:  使能定时器或关闭定时器1
* Input:        flag ：1 停止 2 使能
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2019/12/11              
****************************************************************************************************/
void tmTimerStart(uint32_t flag)
{
    if(flag == TICK_TIMER_STOP)
    {
        rtiREG1->GCTRL = (rtiREG1->GCTRL & (~ ENABLE_CNT1)); 
        rtiREG1->GCTRL = (rtiREG1->GCTRL & (~ ENABLE_CNT0)); 
    }
    else
    {
        rtiREG1->GCTRL = (rtiREG1->GCTRL | ( ENABLE_CNT1));
        rtiREG1->GCTRL = (rtiREG1->GCTRL | ( ENABLE_CNT0)); 
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-087 (Trace to: SLD-AMC01-087)
* Function:     tmStopHook
* Description:  任务管理模块故障停机处理
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmStopHook(void)
{
    /*显示错误码*/
    int32_t err = tmGetError();

    infoSetPfError(ERROR_TOTAL_TICKS + err - 1);

    drv2LedTurnOff(PanelLedRun, GREEN);
    drv2LedTurnOff(PanelLedMaint, GREEN);
    drv2LedTurnOn(PanelLedErr, RED);

    /* 显示 错误码  */
    (void)errHandle();
    /*程序不允许到这里，来了就是有问题了。*/
    while(1)
    {
        /*喂狗 */
        wdFeed(WD_1);
        tmDelayms(15U);
        /* 发送平台的维护数据 */
        mtTxHandle();
    }
}

