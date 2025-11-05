/**************************************************************************************************
*Filename:     module_com4.c
*Purpose:      公用模块com4方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#include <string.h>
#include "bsp.h"
#include "global.h"
#include "module_class.h"
#include "info_class.h"
#include "cfg_class.h"
#include "com_hw.h"
#include "maintain.h"
#include "force_class.h"
#include "standby.h"
#include "project.h"

/****************************************************************************************************
* Identifier:   SCOD-AMC01-253 (Trace to: SLD-AMC01-253)
* Function:     com4RxHandle
* Description:  接收通信IV数据
* Input:        slot 槽位号
*               port 端口号
*               pBuf 数据缓存
* Output:       none
* Return:       ret 0 接收数据成功 -1 通信失败；-2 无数据
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
int32_t com4RxHandle(int32_t slot, int32_t port, void *pBuf)
{
    int32_t ret = 0;
    int32_t err = 0;

    int32_t offset = 0;
    uint32_t src = 0U;
    portState_t *pPortState = NULL;
    comFrame_t *pFrame = (comFrame_t *)pBuf; /* 可以污染 */
    uint32_t comState = 0U;

    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(port < COM_PORT_MAX);

    src = com1GetAddr(slot, COM_RX1_ADDR);
    src += ((uint32_t)port * ASYNC_EMIF_COM_STEP_ADDR);

    /* 接收维护数据帧 */
    pPortState = infoGetPortState(slot, port, RX_PORT);
    LYNX_ASSERT(NULL != pPortState);

    comState = infoGetCom(slot);
    offset = 12 + g_localSlot;
    comState &= (1U << offset); 

    if(pPortState->full == 0) /* 有数据 */
    {
        err = com1ReadData((void*)pFrame, src, sizeof(comFrame_t));
        if(( err == 0 ) &&
                (comState == 0U))
        {
            ret = 0;
        }
        else
        {
            ret = -1; /* 通信接收失败 */
        }
    }
    else
    {   /* 端口数据为空，没有数据数据 */
        ret = -2;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-254 (Trace to: SLD-AMC01-254)
* Function:     com4TxHandle
* Description:  向工程师站发送维护数据
* Input:        slot 槽位号
*               port 端口号
*               pBuf 发送数据指针，可以污染
* Output:       none
* Return:       ret 0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
int32_t com4TxHandle(int32_t slot, int32_t port, void *pBuf)
{
    int32_t ret = 0;
    uint32_t dst = com1GetAddr(slot, COM_TX1_ADDR);

    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    LYNX_ASSERT(port < COM_PORT_MAX);
    LYNX_ASSERT(NULL != pBuf);

    dst = dst + (ASYNC_EMIF_COM_STEP_ADDR * (uint32_t)port);
    /* 写入数据 */
    com1WriteData(dst, pBuf, sizeof(comFrame_t));

    return ret;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-255 (Trace to: SLD-AMC01-255)
* Function:     com4Init
* Description:  com4 初始化
* Input:        slot 槽号
* Output:       none
* Return:       0
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
int32_t com4Init(int32_t slot)
{
    mtSetSlot(slot);
    return 0;
}

