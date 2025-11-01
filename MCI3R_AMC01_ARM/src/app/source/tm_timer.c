/****************************************************************************************************
*FILENAME:     tm_timer.c
*PURPOSE:      系统时间管理
*DATE          AUTHOR          CHANGE
*2017.08.09    胡德全          创建文件
*2019.01.22    张庆林          删除tmDelayus函数
****************************************************************************************************/
#include "tm_timer.h"
#include "tm_private.h"

/****************************************************************************************************
* Identifier:   SCOD-AMC01-275 (Trace to: SLD-AMC01-275)
* Function:     tmDelayTick
* Description:  延时tick微秒
* Input:        tick: 延时时间，单位为us
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmDelayTick(uint32_t tick)
{
    uint32_t start = tmGetCurTick();
    uint32_t end = 0U;
    uint32_t cnt = 0u;

    while (1)
    {
        end = tmGetCurTick();
        if((end - start) < tick)
        {
            for (cnt = 0u; cnt < 10u; cnt++)
            {
                ; /* wait */
            }
        }
        else
        {
            break;
        }        
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-276 (Trace to: SLD-AMC01-276)
* Function:     tmDelayUS
* Description:  微秒延时
* Input:        us:    延时时间
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmDelayus(uint32_t us)
{
    tmDelayTick(TM_CFG_US_TO_TICK(us));
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-277 (Trace to: SLD-AMC01-277)
* Function:     tmDelayms
* Description:  毫秒延时
* Input:        ms:    延时时间
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void tmDelayms(uint32_t ms)
{
    tmDelayTick(TM_CFG_US_TO_TICK(ms*1000U));
}

