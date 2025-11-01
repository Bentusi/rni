/**************************************************************************************************
*Filename:     module_di.c
*Purpose:      公用模块di方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#include "lynx_types.h"
#include "global.h"
#include "module_class.h"
#include "info_class.h"
#include "io_class.h"
#include "cfg_class.h"
#include "com_hw.h"
#include "project.h"

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-359 (Trace to: SLD-AMC01-359)
 * Function:     diFrameHandle
 * Description:  通用di模块接收数据帧处理
 * Input:        slot      槽位号                 
 *               copyFlag  0 不进行复制且质量位无效（异常处理） !0 执行复制操作
 *               pDst      目的地址
 *               pSrc      源地址
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
static void diFrameHandle(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc)
{
    uint8_t value = 0U;
    int32_t ch = 0;
    uint32_t enable = 0U;

    diFrame_t *pDstFrame = (diFrame_t*)pDst;
    diFrame_t *pSrcFrame = (diFrame_t*)pSrc;

    LYNX_ASSERT(NULL != pDstFrame);
    LYNX_ASSERT(NULL != pSrcFrame);

    enable = cfgGetCardEnableCh(slot);

    if(copyFlag != 0)
    {
        infoSetHw(slot, pSrcFrame->info);/* 硬件信息 */
        for(ch = 0; ch < DI_CH_MAX; ch++)
        {
            if((enable & (1UL<<ch)) != 0U)
            {   /* 原始值保存到 */
                pDstFrame->data[ch].value = pSrcFrame->data[ch].value;
                pDstFrame->data[ch].quality = (pSrcFrame->data[ch].quality) & 0x01U;
                value = pSrcFrame->data[ch].quality;
                infoSetIoCh(slot, ch, value);
            }
        }
    }
    else
    {
        infoSetHw(slot, 0U);/* 硬件信息 */
        /*invalid all quality that channel is enable. */
        for(ch = 0; ch < DI_CH_MAX; ch++)
        {   /* 通信异常时，所有物理点质量位无效 */
            if((enable & ((uint32_t)1<<ch)) != 0U)
            {   /* 值不变，质量位无效 */
                pDstFrame->data[ch].quality = QUALITY_STATE_INVALID;
                infoSetIoCh(slot, ch, QUALITY_STATE_INVALID);
            }
        }
    }
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-360 (Trace to: SLD-AMC01-360)
* Function:     diRxHandle
* Description:  接收DI数据
* Input:        slot 槽号
*               para IO卡保留
*               pBuf 接收数据指针
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
**************************************************************************************************/
int32_t diRxHandle(int32_t slot, int32_t para, void *pBuf)
{
    LYNX_ASSERT(0 == para);
    ioRxHandle(slot, pBuf, diFrameHandle);
    return 0;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-361 (Trace to: SLD-AMC01-361)
* Function:     diGetRxChAddr
* Description:  获取di模块对应通道的缓冲区地址
* Input:        slot 槽号
*               port 参数保留，接口需要
*               ch   通道号
* Output:       none
* Return:       pSrc 通道地址
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
*               2023/03/09    wxb       diGetInChAddr修改为diGetRxChAddr
**************************************************************************************************/
uint8_t *diGetRxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pSrc = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, RX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);

    if(ch < DI_CH_MAX)
    {
        pSrc = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary2B_t)* (uint32_t)ch);
    }

    return pSrc;
}

