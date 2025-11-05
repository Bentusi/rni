/**************************************************************************************************
*Filename:     module_npi01.c
*Purpose:      公用模块npi01方法定义
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
* Identifier:   SCOD-AMC01-365 (Trace to: SLD-AMC01-365)
* Function:     npi01FrameHandle
* Description:  npi01模块接收数据帧处理
* Input:        slot      槽位号
*               copyFlag  0 不进行复制且质量位无效（异常处理） !0 执行复制操作
*               pDst      目的地址
*               pSrc      源地址
* Output:       None
* Return:       None
* Date:         Author      Modified
* 2021-11-09    hdq         Create
**************************************************************************************************/
static void npi01FrameHandle(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc)
{
    npi01FrameRx_t *pDstFrame = (npi01FrameRx_t*)pDst;
    npi01FrameRx_t *pSrcFrame = (npi01FrameRx_t*)pSrc;

    LYNX_ASSERT(NULL != pDstFrame);
    LYNX_ASSERT(NULL != pSrcFrame);

    if(copyFlag != 0)
    {
        infoSetHw(slot, pSrcFrame->info);

        /* 高压切除反馈信号 */
        pDstFrame->hiPreLoseFeedback.value = pSrcFrame->hiPreLoseFeedback.value;
        pDstFrame->hiPreLoseFeedback.quality = (pSrcFrame->hiPreLoseFeedback.quality)&0x01U;
        infoSetIoCh(slot, 0, pSrcFrame->hiPreLoseFeedback.quality);
        /* 测试通道切换状态反馈 */
        pDstFrame->channelSwitchRec.value = pSrcFrame->channelSwitchRec.value;
        pDstFrame->channelSwitchRec.quality = (pSrcFrame->channelSwitchRec.quality)&0x01U;
        infoSetIoCh(slot, 1, pSrcFrame->channelSwitchRec.quality);
        /* 坪曲线绘制使能 */
        pDstFrame->flatCurveDrawEn.value = pSrcFrame->flatCurveDrawEn.value;
        pDstFrame->flatCurveDrawEn.quality = (pSrcFrame->flatCurveDrawEn.quality)&0x01U;
        infoSetIoCh(slot, 2, pSrcFrame->flatCurveDrawEn.quality);
        /* 低计数率更新时间t1 */
        pDstFrame->lowCountRateUpdate_T1.value = pSrcFrame->lowCountRateUpdate_T1.value;
        pDstFrame->lowCountRateUpdate_T1.quality = (pSrcFrame->lowCountRateUpdate_T1.quality)&0x01U;
        infoSetIoCh(slot, 3, pSrcFrame->lowCountRateUpdate_T1.quality);
        /* 脉冲计数率 */
        pDstFrame->countRate.value = pSrcFrame->countRate.value;
        pDstFrame->countRate.quality = (pSrcFrame->countRate.quality)&0x01U;
        infoSetIoCh(slot, 4, pSrcFrame->countRate.quality);
        /* 高压输出回采 */
        pDstFrame->hiPreOutputRec.value = pSrcFrame->hiPreOutputRec.value;
        pDstFrame->hiPreOutputRec.quality = (pSrcFrame->hiPreOutputRec.quality)&0x01U;
        infoSetIoCh(slot, 5, pSrcFrame->hiPreOutputRec.quality);
        /* 甄别阀输出回采 */
        pDstFrame->thresholdOutputRec.value = pSrcFrame->thresholdOutputRec.value;
        pDstFrame->thresholdOutputRec.quality = (pSrcFrame->thresholdOutputRec.quality)&0x01U;
        infoSetIoCh(slot, 6, pSrcFrame->thresholdOutputRec.quality);
        /* TTL信号闭锁反馈 */
        pDstFrame->ttlSignalLookRec.value = pSrcFrame->ttlSignalLookRec.value;
        pDstFrame->ttlSignalLookRec.quality = (pSrcFrame->ttlSignalLookRec.quality)&0x01U;
        infoSetIoCh(slot, 7, pSrcFrame->ttlSignalLookRec.quality);
        /* 计数率滤波参数反馈值   */
        pDstFrame->countRateRec.value = pSrcFrame->countRateRec.value;
        pDstFrame->countRateRec.quality = (pSrcFrame->countRateRec.quality)&0x01U;
        infoSetIoCh(slot, 8, pSrcFrame->countRateRec.quality);
    }
    else /* 通信异常时，所有物理点质量位无效 */
    {
        infoSetHw(slot, 0u); /* 脉冲采集模块自诊断状态反馈 */
        /* 高压切除反馈信号 */
        pDstFrame->hiPreLoseFeedback.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 0, QUALITY_STATE_INVALID);
        /* 测试通道切换状态反馈 */
        pDstFrame->channelSwitchRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 1, QUALITY_STATE_INVALID);
        /* 坪曲线绘制使能 */
        pDstFrame->flatCurveDrawEn.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 2, QUALITY_STATE_INVALID);
        /* 低计数率更新时间t1 */
        pDstFrame->lowCountRateUpdate_T1.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 3, QUALITY_STATE_INVALID);
        /* 脉冲计数率 */
        pDstFrame->countRate.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 4, QUALITY_STATE_INVALID);
        /* 高压输出回采 */
        pDstFrame->hiPreOutputRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 5, QUALITY_STATE_INVALID);
        /* 甄别阀输出回采 */
        pDstFrame->thresholdOutputRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 6, QUALITY_STATE_INVALID);
        /* TTL信号闭锁反馈 */
        pDstFrame->ttlSignalLookRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 7, QUALITY_STATE_INVALID);
        /* 计数率滤波参数反馈值   */
        pDstFrame->countRateRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 8, QUALITY_STATE_INVALID);
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-366 (Trace to: SLD-AMC01-366)
* Function:     npi01RxHandle
* Description:  接收NPI01版卡的数据和状态
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
int32_t npi01RxHandle(int32_t slot, int32_t port, void *pBuf)
{
    LYNX_ASSERT(0 == port);
    ioRxHandle(slot, pBuf, npi01FrameHandle);
    return 0;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-367 (Trace to: SLD-AMC01-367)
* Function:     npi01GetRxChAddr
* Description:  获取npi01模块对应通道的缓冲区地址
* Input:        slot  槽号
*               port  端口号
*               ch:   通道号
* Output:       none
* Return:       pSrc 通道地址
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
**************************************************************************************************/
uint8_t *npi01GetRxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pSrc = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, RX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < NPI01_RX_CH_MAX)
    {
        pSrc = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)*((uint8_t)ch));
    }

    return pSrc;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-368 (Trace to: SLD-AMC01-368)
 * Function:     npi01GetTxChAddr
 * Description:  获取npi01模块通道地址
 * Input:        slot  槽位号
 *               port  参数保留，接口需要
 *               ch    通道号
 * Output:       None
 * Return:       pDst 对应通道的地址指针
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
uint8_t *npi01GetTxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pDst = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, TX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < NPI01_TX_CH_MAX)
    {
        pDst = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)* (uint32_t)ch);
    }

    return pDst;
}

