/**************************************************************************************************
*Filename:     module_ai.c
*Purpose:      公用模块ai方法定义
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
 * Identifier:   SCOD-AMC01-353 (Trace to: SLD-AMC01-353)
 * Function:     aiFrameHandle
 * Description:  ai模块接收帧处理
 * Input:        slot     槽位号
 *               copyFlag 0 不拷贝，质量位无效操作 !0 正常拷贝
 *               pDst     目的地址
 *               pSrc     源地址
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
static void aiFrameHandle(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc)
{
    uint8_t value = 0U;
    int32_t ch = 0;
    uint32_t enable = 0U;

    aiFrame_t *pDstFrame = (aiFrame_t*)pDst;
    aiFrame_t *pSrcFrame = (aiFrame_t*)pSrc;

    LYNX_ASSERT(NULL != pDstFrame);
    LYNX_ASSERT(NULL != pSrcFrame);

    enable = cfgGetCardEnableCh(slot);

    if(copyFlag != 0)
    {
        infoSetHw(slot, pSrcFrame->info);    /* 硬件信息 */
        for(ch = 0; ch < AI_CH_MAX; ch++)
        {
            if((enable & ((uint32_t)1<<ch)) != 0U)
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
        infoSetHw(slot, 0U);    /* 硬件信息 */
        /* invalid all quality that channel is enable. */
        for(ch = 0; ch < AI_CH_MAX; ch++)
        {   /* 通信异常时，所有物理点质量位无效 */
            if((enable & ((uint32_t)1<<ch)) != 0U)
            {   /* 值不变，质量位无效 */
                pDstFrame->data[ch].quality = QUALITY_STATE_INVALID;
                infoSetIoCh(slot, ch, QUALITY_STATE_INVALID);
            }
        }
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-354 (Trace to: SLD-AMC01-354)
* Function:     aiRxHandle
* Description:  接收AI版卡的数据和状态
* Input:        slot 槽号
*               para 参数保留，接口需要
*               pBuf 接收数据指针
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
int32_t aiRxHandle(int32_t slot, int32_t para, void *pBuf)
{
    LYNX_ASSERT(0 == para);

    ioRxHandle(slot, pBuf, aiFrameHandle);
    return 0;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-355 (Trace to: SLD-AMC01-355)
* Function:     aiGetRxChAddr
* Description:  获取ai模块对应通道的缓冲区地址
* Input:        slot 槽号
*               port 端口号
*                 ch 通道号
* Output:       none
* Return:       pSrc : 通道数据地址
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
**************************************************************************************************/
uint8_t *aiGetRxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pSrc = NULL;
    portAttr_t *pAttr = glGetPortAttr(slot, port, RX_PORT);
    uint8_t *pBuf = pAttr->pAddr;

    LYNX_ASSERT(NULL != pBuf);

    if(ch < AI_CH_MAX)
    {
        pSrc = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(analog6BF_t)* (uint8_t)ch);
    }

    return pSrc;
}

