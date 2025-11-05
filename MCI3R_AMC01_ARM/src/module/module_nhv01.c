/**************************************************************************************************
*Filename:     module_nhv01.c
*Purpose:      公用模块nhv01方法定义
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
 * Identifier:   SCOD-AMC01-377 (Trace to: SLD-AMC01-377)
 * Function:     nhv01FrameHandle
 * Description:  nhv01模块接收数据帧处理
 * Input:        slot      槽位号
 *               copyFlag  0：不进行复制且质量位无效（异常处理）；！0：执行复制操作
 *               pDst      目的地址
 *               pSrc      源地址
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
static void nhv01FrameHandle(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc)
{
    nhv01FrameRx_t *pDstFrame = (nhv01FrameRx_t*)pDst;
    nhv01FrameRx_t *pSrcFrame = (nhv01FrameRx_t*)pSrc;

    LYNX_ASSERT(NULL != pDstFrame);
    LYNX_ASSERT(NULL != pSrcFrame);

    if(copyFlag != 0)
    {
        infoSetHw(slot, pSrcFrame->info);

        /* 高压坪曲线绘制状态反馈 */
        pDstFrame->hiPreflatCurveDrawRec.value = pSrcFrame->hiPreflatCurveDrawRec.value;
        pDstFrame->hiPreflatCurveDrawRec.quality = (pSrcFrame->hiPreflatCurveDrawRec.quality)&0x01U;
        infoSetIoCh(slot, 0, 0U);
        /* 正高压输出回采 */
        pDstFrame->posPreOutputRec.value = pSrcFrame->posPreOutputRec.value;
        pDstFrame->posPreOutputRec.quality = (pSrcFrame->posPreOutputRec.quality)&0x01U;
        infoSetIoCh(slot, 1, pSrcFrame->posPreOutputRec.quality);
        /* 电流值 */
        pDstFrame->outputCurrent.value = pSrcFrame->outputCurrent.value;
        pDstFrame->outputCurrent.quality = (pSrcFrame->outputCurrent.quality)&0x01U;
        infoSetIoCh(slot, 2, pSrcFrame->outputCurrent.quality);
    }
    else /* 通信异常时，所有物理点质量位无效 */
    {
        infoSetHw(slot, 0u); /* 脉冲采集模块自诊断状态反馈 */

        /* 高压坪曲线绘制状态反馈 */
        pDstFrame->hiPreflatCurveDrawRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 0, QUALITY_STATE_INVALID);
        /* 正高压输出回采 */
        pDstFrame->posPreOutputRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 1, QUALITY_STATE_INVALID);
        /* 电流值 */
        pDstFrame->outputCurrent.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 2, QUALITY_STATE_INVALID);
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-378 (Trace to: SLD-AMC01-378)
* Function:     nhv01RxHandle
* Description:  接收NHV01版卡的数据和状态
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
int32_t nhv01RxHandle(int32_t slot, int32_t port, void *pBuf)
{
    LYNX_ASSERT(0 == port);
    ioRxHandle(slot, pBuf, nhv01FrameHandle);
    return 0;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-379 (Trace to: SLD-AMC01-379)
* Function:     nhv01GetRxChAddr
* Description:  获取nhv01模块对应通道的缓冲区地址
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
uint8_t *nhv01GetRxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pSrc = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, RX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < NHV01_RX_CH_MAX)
    {
        pSrc = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)*((uint8_t)ch));
    }

    return pSrc;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-380 (Trace to: SLD-AMC01-380)
 * Function:     nhv01GetTxChAddr
 * Description:  获取nhv01模块通道地址
 * Input:        slot  槽位号
 *               port  参数保留，接口需要
 *               ch    通道号
 * Output:       None
 * Return:       pDst  对应通道的地址指针
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
uint8_t *nhv01GetTxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pDst = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, TX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < NHV01_TX_CH_MAX)
    {
        pDst = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)* (uint32_t)ch);
    }

    return pDst;
}

