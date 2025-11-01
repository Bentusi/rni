/***************************************************************************************************
 * Filename: switch.c
 * Purpose:  模式开关驱动接口
 * Date:         Author      Modified 
 * 2021-11-24    hdq         Create  
***************************************************************************************************/
#include "bsp.h"
#include "info_class.h"
#include "switch.h"
#include "panel.h"
#include "error_class.h"
#include "tm.h"

/****************************************************************************************************
* Identifier:   SCOD-AMC01-039 (Trace to: SLD-AMC01-039)
* Function:     swUserStateCheck
* Description:  获取用户模式
* Input:        none
* Output:       none
* Return:       ret 模式开关状态
*               
* Others:
* Log:          Date          Author    Modified
*               2017/10/24        
*               2021/09/01    hdq       删除闭锁分支
****************************************************************************************************/
int32_t swUserStateCheck(void)
{
    int32_t ret = SYS_INIT;

    ret = (int32_t)drv2PanelSwitchStateGet();
    switch((uint32_t)ret)
    {
        case 0x01U:
            ret = SYS_MAIT; /* 维护 */
            break;

        case 0x02U:
            ret = SYS_NOR;  /* 正常 */
            break;

        default:
            ret = SYS_ERR; /* 错误 */
            break;
    }

    return ret;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-045 (Trace to: SLD-AMC01-045)
* Function:     swUserHandle
* Description:  运行模式实时检测
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
*               2021/09/01    hdq       删除闭锁分支
***********************************************************************/
void swUserHandle(void)
{
    int32_t state = 0;
    
    static uint32_t startTime2 = 0U; 
    static uint32_t endTime2   = 0U;

    /* 获取模式开关状态 */
    state = swUserStateCheck();
    if(state == SYS_NOR)
    {
        drv2ModeLedSet(PanelLedRun);
        infoSetSysState(state);
        infoClearPfError(KEY_MODE_ERR);
        startTime2 = 0U;
    }
    else if(state == SYS_MAIT)
    {
        drv2ModeLedSet(PanelLedMaint);
        infoSetSysState(state);
        infoClearPfError(KEY_MODE_ERR);
        startTime2 = 0U;
    }
    else
    {
        if(startTime2 == 0U)
        {
            startTime2 = tmGetCurTick();
        }
        else
        {
            endTime2  = tmGetCurTick();
            if((endTime2 -startTime2) > (2U * 1000U * 1000U))  /* 2秒后才报开关故障 */
            {
                infoSetPfError(KEY_MODE_ERR);
            }
        } 
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-051 (Trace to: SLD-AMC01-051)
* Function:     swUserModeInit
* Description:  获取系统上电时运行模式状态及指示灯显示
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
***********************************************************************/
void swUserModeInit(void)
{
    int32_t state = 0;

    /* 获取模式状态 并做对应的处理 */    
    state = swUserStateCheck();
    infoSetSysState(state);
    if(state == SYS_NOR)
    {
        drv2ModeLedSet(PanelLedRun);
    }
    else if(state == SYS_MAIT)
    {
        drv2ModeLedSet(PanelLedMaint);
    }
    else
    {
        errStopHandleInit(MODE_INIT_ERROR);/*说明需要用户重启系统*/
    }
}

