/**************************************************************************************************
*Filename:     module_tr.c
*Purpose:      公用模块tr方法定义
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
 * Identifier:   SCOD-AMC01-385 (Trace to: SLD-AMC01-385)
 * Function:     trFrameHandle
 * Description:  tr模块接收数据帧处理
 * Input:        slot      槽位号
 *               copyFlag  0：不进行复制且质量位无效（异常处理）；！0：执行复制操作
 *               pDst      目的地址
 *               pSrc      源地址
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
static void trFrameHandle(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc)
{
    uint8_t value = 0U;
    int ch = 0;
    uint32_t enable = 0U;

    trFrame_t *pDstFrame = (trFrame_t*)pDst;
    trFrame_t *pSrcFrame = (trFrame_t*)pSrc;

    LYNX_ASSERT(NULL != pDstFrame);
    LYNX_ASSERT(NULL != pSrcFrame);

    enable = cfgGetCardEnableCh(slot);

    if(copyFlag != 0)
    {
        infoSetHw(slot, pSrcFrame->info);

        for(ch = 0;ch < TC_CH_MAX;ch++)
        {
            if((enable & ((uint32_t)1<<ch)) != 0U)
            {
                pDstFrame->data[ch].value = pSrcFrame->data[ch].value;
                pDstFrame->data[ch].quality = (pSrcFrame->data[ch].quality) & 0x01U;
                value = pSrcFrame->data[ch].quality;
                infoSetIoCh(slot, ch, value);
            }
        }
    }
    else /* 通信异常时，所有物理点质量位无效 */
    {
        infoSetHw(slot, 0u); /* 脉冲采集模块自诊断状态反馈 */

        for(ch = 0;ch < TC_CH_MAX;ch++)
        {
            if((enable & ((uint32_t)1<<ch)) != 0U)
            {
                pDstFrame->data[ch].quality = QUALITY_STATE_INVALID;
                infoSetIoCh(slot, ch, QUALITY_STATE_INVALID);
            }
        }
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-386 (Trace to: SLD-AMC01-386)
* Function:     trRxHandle
* Description:  接收tr版卡的数据和状态
* Input:        slot  槽号
*               port  IO卡保留
*               pBuf  接收数据指针
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
int32_t trRxHandle(int32_t slot, int32_t port, void *pBuf)
{
    LYNX_ASSERT(0 == port);
    ioRxHandle(slot, pBuf, trFrameHandle);
    return 0;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-387 (Trace to: SLD-AMC01-387)
* Function:     trGetRxChAddr
* Description:  获取tr模块对应通道的缓冲区地址
* Input:        slot  槽号
*               port  参数保留，接口需要
*               ch    通道号
* Output:       none
* Return:       pSrc 通道地址
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
**************************************************************************************************/
uint8_t *trGetRxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pSrc = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, RX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < TR_CH_MAX)
    {
        pSrc = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)*((uint8_t)ch));
    }

    return pSrc;
}

