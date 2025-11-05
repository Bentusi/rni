/**************************************************************************************************
*Filename:     module_nai01.c
*Purpose:      公用模块nai01方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#include "lynx_types.h"
#include "global.h"
#include "module_class.h"
#include "info_class.h"
#include "io_class.h"
#include "com_hw.h"
#include "project.h"

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-373 (Trace to: SLD-AMC01-373)
 * Function:     nai01FrameHandle
 * Description:  nai01模块接收数据帧处理
 * Input:        slot      槽位号
 *               copyFlag  0 不进行复制且质量位无效（异常处理） !0 执行复制操作
 *               pDst      目的地址
 *               pSrc      源地址
 * Output:       None                
 * Return:       None                 
 * Date:         Author      Modified 
 * 2021-11-09    hdq         Create  
 *************************************************************************************************/
static void nai01FrameHandle(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc)
{
    nai01RxFrame_t *pDstFrame = (nai01RxFrame_t*)pDst;
    nai01RxFrame_t *pSrcFrame = (nai01RxFrame_t*)pSrc;

    LYNX_ASSERT(NULL != pDstFrame);
    LYNX_ASSERT(NULL != pSrcFrame);

    if(copyFlag != 0)
    {
        infoSetHw(slot, pSrcFrame->info);

        /* 测试通道切换状态反馈 */
        pDstFrame->channelSwitchRec.value = pSrcFrame->channelSwitchRec.value;
        pDstFrame->channelSwitchRec.quality = (pSrcFrame->channelSwitchRec.quality)&0x01U;
        infoSetIoCh(slot, 0, pSrcFrame->channelSwitchRec.quality);
        /* 测试通道选择反馈 */
        pDstFrame->channelChoiceRec.value = pSrcFrame->channelChoiceRec.value;
        pDstFrame->channelChoiceRec.quality = (pSrcFrame->channelChoiceRec.quality)&0x01U;
        infoSetIoCh(slot, 1, pSrcFrame->channelChoiceRec.quality);
        /* 档位信号反馈 */
        pDstFrame->gearSignalRec.value = pSrcFrame->gearSignalRec.value;
        pDstFrame->gearSignalRec.quality = (pSrcFrame->gearSignalRec.quality)&0x01U;
        infoSetIoCh(slot, 2, pSrcFrame->gearSignalRec.quality);
        /* 1段电流 */
        pDstFrame->upCurrent_1.value = pSrcFrame->upCurrent_1.value;
        pDstFrame->upCurrent_1.quality = (pSrcFrame->upCurrent_1.quality)&0x01U;
        infoSetIoCh(slot, 4, pSrcFrame->upCurrent_1.quality);
        /* 2段电流 */
        pDstFrame->upCurrent_2.value = pSrcFrame->upCurrent_2.value;
        pDstFrame->upCurrent_2.quality = (pSrcFrame->upCurrent_2.quality)&0x01U;
        infoSetIoCh(slot, 5, pSrcFrame->upCurrent_2.quality);
        /* 3段电流 */
        pDstFrame->upCurrent_3.value = pSrcFrame->upCurrent_3.value;
        pDstFrame->upCurrent_3.quality = (pSrcFrame->upCurrent_3.quality)&0x01U;
        infoSetIoCh(slot, 6, pSrcFrame->upCurrent_3.quality);
        /* 4段电流 */
        pDstFrame->upCurrent_4.value = pSrcFrame->upCurrent_4.value;
        pDstFrame->upCurrent_4.quality = (pSrcFrame->upCurrent_4.quality)&0x01U;
        infoSetIoCh(slot, 7, pSrcFrame->upCurrent_4.quality);
    }
    else /* 通信异常时，所有物理点质量位无效 */
    {
        infoSetHw(slot, 0u); /* 脉冲采集模块自诊断状态反馈 */

        /* 测试通道切换状态反馈 */
        pDstFrame->channelSwitchRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 0, QUALITY_STATE_INVALID);
        /* 测试通道选择反馈 */
        pDstFrame->channelChoiceRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 1, QUALITY_STATE_INVALID);
        /* 档位信号反馈 */
        pDstFrame->gearSignalRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 2, QUALITY_STATE_INVALID);
        /* 1段电流 */
        pDstFrame->upCurrent_1.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 4, QUALITY_STATE_INVALID);
        /* 2段电流 */
        pDstFrame->upCurrent_2.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 5, QUALITY_STATE_INVALID);
        /* 3段电流 */
        pDstFrame->upCurrent_3.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 6, QUALITY_STATE_INVALID);
        /* 4段电流 */
        pDstFrame->upCurrent_4.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 7, QUALITY_STATE_INVALID);
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-374 (Trace to: SLD-AMC01-374)
* Function:     aiRxHandle
* Description:  接收AI版卡的数据和状态
* Input:        slot 槽号
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
int32_t nai01RxHandle(int32_t slot, int32_t port, void *pBuf)
{
    LYNX_ASSERT(0 == port);
    ioRxHandle(slot, pBuf, nai01FrameHandle);
    return 0;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-375 (Trace to: SLD-AMC01-375)
* Function:     nai01GetRxChAddr
* Description:  获取nai01模块对应通道的缓冲区地址
* Input:        slot 槽号
*               port IO卡保留
*               ch   通道号
* Output:       none
* Return:       pSrc 通道地址
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
*               2023/01/30    wxb       nai01GetInChAddr修改为nai01GetRxChAddr
**************************************************************************************************/
uint8_t *nai01GetRxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pSrc = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, RX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < NAI01_RX_CH_MAX)
    {
        pSrc = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)*((uint8_t)ch));
    }

    return pSrc;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-376 (Trace to: SLD-AMC01-376)
 * Function:     nai01GetTxChAddr
 * Description:  获取nai01模块通道地址
 * Input:        slot  槽位号
 *               port  参数保留，接口需要
 *               ch    通道号
 * Output:       None
 * Return:       pDst  对应通道的地址指针
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
uint8_t *nai01GetTxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pDst = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, TX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < NAI01_TX_CH_MAX)
    {
        pDst = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)* (uint32_t)ch);
    }

    return pDst;
}

