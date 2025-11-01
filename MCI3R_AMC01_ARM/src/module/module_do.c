/**************************************************************************************************
*Filename:     module_do.c
*Purpose:      公用模块do方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#include "global.h"
#include "module_class.h"
#include "info_class.h"
#include "cfg_class.h"
#include "io_class.h"
#include "com_hw.h"
#include "project.h"

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-362 (Trace to: SLD-AMC01-362)
 * Function:     doFrameHandle
 * Description:  通用do模块接收数据帧处理
 * Input:        slot      槽位号
 *               copyFlag  0 不进行复制且质量位无效（异常处理） !0 执行复制操作
 *               pDst      目的地址
 *               pSrc      源地址
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
static void doFrameHandle(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc)
{
    uint8_t value = 0u;
    int32_t ch = 0;
    uint32_t enable = 0U;

    doFrame_t *pDstFrame = (doFrame_t*)pDst;
    doFrame_t *pSrcFrame = (doFrame_t*)pSrc;

    LYNX_ASSERT(NULL != pDstFrame);
    LYNX_ASSERT(NULL != pSrcFrame);

    enable = cfgGetCardEnableCh(slot);

    if(copyFlag)
    {
        infoSetHw(slot, pSrcFrame->info);/* 硬件信息 */
        for(ch = 0; ch < DO_CH_MAX; ch++)
        {
            if((enable & ((uint32_t)1<<ch)) != 0u)
            {
            	pDstFrame->data[ch].quality = QUALITY_STATE_INVALID;
                value = pSrcFrame->data[ch].quality; /* 原始值保存到 */
                infoSetIoCh(slot, ch, value);
            }
        }
    }
    else
    {
        infoSetHw(slot, 0U);/* 硬件信息 */
        /* invalid all quality that channel is enable. */
        for(ch = 0; ch < DO_CH_MAX; ch++)
        {   /* 通信异常时，所有物理点质量位无效 */
            if((enable & ((uint32_t)1<<ch)) != 0u)
            {   /* 值不变，质量位无效 */
                pDstFrame->data[ch].quality = QUALITY_STATE_INVALID;
                infoSetIoCh(slot, ch, QUALITY_STATE_INVALID);
            }
        }
    }
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-363 (Trace to: SLD-AMC01-363)
* Function:     doRxHandle
* Description:  接收的是DO的状态数据，包括诊断，通道状态
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
int32_t doRxHandle(int32_t slot, int32_t para, void *pBuf)
{
    LYNX_ASSERT(0 == para);
    ioRxHandle(slot, pBuf, doFrameHandle);
    return 0;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-364 (Trace to: SLD-AMC01-364)
 * Function:     doGetTxChAddr
 * Description:  获取通用DO模块通道地址
 * Input:        slot 槽位号
 *               port 参数保留，接口需要
 *               ch   通道号
 * Output:       None
 * Return:       pDst 对应通道的地址指针
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
uint8_t *doGetTxChAddr(int32_t slot, int32_t port, int32_t ch)
{
    uint8_t *pDst = NULL;
    portAttr_t *pMsg = glGetPortAttr(slot, port, TX_PORT);
    uint8_t *pBuf = pMsg->pAddr;

    LYNX_ASSERT(NULL != pBuf);

    if(ch < DO_CH_MAX)
    {
        pDst = pBuf + IO_FRAME_HEAD_SIZE + (sizeof(binary2B_t)* (uint32_t)ch);
    }

    return pDst;
}

