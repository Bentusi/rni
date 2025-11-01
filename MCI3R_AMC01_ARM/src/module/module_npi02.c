/**************************************************************************************************
*Filename:     module_npi02.c
*Purpose:      公用模块npi02方法定义
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
 * Identifier:   SCOD-AMC01-369 (Trace to: SLD-AMC01-369)
 * Function:     npi02FrameHandle            
 * Description:  npi02模块接收数据帧处理                     
 * Input:        slot      槽位号                 
 *               copyFlag  0 不进行复制且质量位无效（异常处理） !0 执行复制操作
 *               pDst      目的地址
 *               pSrc      源地址 
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
static void npi02FrameHandle(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc)
{
    npi02FrameRx_t *pDstFrame = (npi02FrameRx_t*)pDst;
    npi02FrameRx_t *pSrcFrame = (npi02FrameRx_t*)pSrc;

    LYNX_ASSERT(NULL != pDstFrame);
    LYNX_ASSERT(NULL != pSrcFrame);

    if(copyFlag != 0)
    {
        infoSetHw(slot, pSrcFrame->info);

        /* 测试通道切换状态 */
        pDstFrame->channelSwitchRec.value = pSrcFrame->channelSwitchRec.value;
        pDstFrame->channelSwitchRec.quality = (pSrcFrame->channelSwitchRec.quality)&0x01U;
        infoSetIoCh(slot, 0, pSrcFrame->channelSwitchRec.quality);
        /* 甄别阈坪曲线绘制状态 */
        pDstFrame->thresholdFlatCurveDraw.value = pSrcFrame->thresholdFlatCurveDraw.value;
        pDstFrame->thresholdFlatCurveDraw.quality = (pSrcFrame->thresholdFlatCurveDraw.quality)&0x01U;
        infoSetIoCh(slot, 1, pSrcFrame->thresholdFlatCurveDraw.quality);
        /* 脉冲计数率 */
        pDstFrame->countRate.value = pSrcFrame->countRate.value;
        pDstFrame->countRate.quality = (pSrcFrame->countRate.quality)&0x01U;
        infoSetIoCh(slot, 2, pSrcFrame->countRate.quality);
        /* MSV通道方差1倍 */
        pDstFrame->MSVChVariance1.value = pSrcFrame->MSVChVariance1.value;
        pDstFrame->MSVChVariance1.quality = (pSrcFrame->MSVChVariance1.quality)&0x01U;
        infoSetIoCh(slot, 3, pSrcFrame->MSVChVariance1.quality);
        /* MSV通道方差10倍 */
        pDstFrame->MSVChVariance10.value = pSrcFrame->MSVChVariance10.value;
        pDstFrame->MSVChVariance10.quality = (pSrcFrame->MSVChVariance10.quality)&0x01U;
        infoSetIoCh(slot, 4, pSrcFrame->MSVChVariance10.quality);
        /* 甄别阈输出回采值 */
        pDstFrame->thresholdOutputRec.value = pSrcFrame->thresholdOutputRec.value;
        pDstFrame->thresholdOutputRec.quality = (pSrcFrame->thresholdOutputRec.quality)&0x01U;
        infoSetIoCh(slot, 5, pSrcFrame->thresholdOutputRec.quality);
        /* 计数率滤波参数返馈 */
        pDstFrame->countRateFilterParaRec.value = pSrcFrame->countRateFilterParaRec.value;
        pDstFrame->countRateFilterParaRec.quality = (pSrcFrame->countRateFilterParaRec.quality)&0x01U;
        infoSetIoCh(slot, 6, pSrcFrame->countRateFilterParaRec.quality);
        /* 低计数率更新时间t1 */
        pDstFrame->lowCountRateUpdate_T1.value = pSrcFrame->lowCountRateUpdate_T1.value;
        pDstFrame->lowCountRateUpdate_T1.quality = (pSrcFrame->lowCountRateUpdate_T1.quality)&0x01U;
        infoSetIoCh(slot, 7, pSrcFrame->lowCountRateUpdate_T1.quality);
    }
    else /* 通信异常时，所有物理点质量位无效 */
    {
        infoSetHw(slot, 0u); /* 脉冲采集模块自诊断状态反馈 */
        /* 测试通道切换状态 */
        pDstFrame->channelSwitchRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 0, QUALITY_STATE_INVALID);
        /* 甄别阈坪曲线绘制状态 */
        pDstFrame->thresholdFlatCurveDraw.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 1, QUALITY_STATE_INVALID);
        /* 脉冲计数率 */
        pDstFrame->countRate.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 2, QUALITY_STATE_INVALID);
        /* MSV通道方差1倍 */
        pDstFrame->MSVChVariance1.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 3, QUALITY_STATE_INVALID);
        /* MSV通道方差10倍 */
        pDstFrame->MSVChVariance10.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 4, QUALITY_STATE_INVALID);
        /* 甄别阈输出回采值 */
        pDstFrame->thresholdOutputRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 5, QUALITY_STATE_INVALID);
        /* 计数率滤波参数返馈 */
        pDstFrame->countRateFilterParaRec.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 6, QUALITY_STATE_INVALID);
        /* 低计数率更新时间t1 */
        pDstFrame->lowCountRateUpdate_T1.quality = QUALITY_STATE_INVALID;
        infoSetIoCh(slot, 7, QUALITY_STATE_INVALID);
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-370 (Trace to: SLD-AMC01-370)
* Function:     npi02RxHandle
* Description:  接收NPI02版卡的数据和状态
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
int32_t npi02RxHandle(int32_t slot, int32_t port, void *pBuf)
{
    LYNX_ASSERT(0 == port);
    ioRxHandle(slot, pBuf, npi02FrameHandle);
    return 0;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-371 (Trace to: SLD-AMC01-371)
* Function:     npi02GetRxChAddr
* Description:  获取npi02模块对应通道的缓冲区地址
* Input:        slot  槽号
*               port  端口号
*               ch    通道号
* Output:       none
* Return:       pSrc 通道地址
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
**************************************************************************************************/
uint8_t *npi02GetRxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pSrc = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, RX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < NPI02_RX_CH_MAX)
    {
        pSrc = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)*((uint8_t)ch));
    }

    return pSrc;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-372 (Trace to: SLD-AMC01-372)
 * Function:     npi02GetTxChAddr
 * Description:  获取npi02模块通道地址
 * Input:        slot  槽位号
 *               port  参数保留，接口需要
 *               ch    通道号
 * Output:       None
 * Return:       pDst  对应通道的地址指针
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
uint8_t *npi02GetTxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pDst = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, TX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);
    if(ch < NPI02_TX_CH_MAX)
    {
        pDst = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary6B_t)* (uint32_t)ch);
    }

    return pDst;
}

