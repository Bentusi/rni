/**************************************************************************************************
*Filename:     io_class.c
*Purpose:      IO模块处理相关方法
*Log:          Date          Author    Modified
*              2021/08/31    hdq       create
**************************************************************************************************/
#include "module_class.h"
#include "cfg_class.h"
#include "com_hw.h"
#include "io_class.h"
#include "info_class.h"
#include "panel.h"

/**************************************************************************************************
* Identifier:   SCOD-AMC01-049 (Trace to: SLD-AMC01-049)
* Function:     lxGetInChAddr
* Description:  解析输入类模块通道的缓冲区地址
* Input:        slot 槽号
                ch:  通道号
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
**************************************************************************************************/
uint8_t *lxGetInChAddr(int32_t slot, int32_t ch)
{
    void *pSrc = NULL;
    uint8_t type = (uint8_t)cfgCardTypeOnSlot(slot);

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(type < MODULE_TYPE_MAX);
    if(NULL != g_moduleHandle[type].pFunGetRxChAddr)
    {
        pSrc = g_moduleHandle[type].pFunGetRxChAddr(slot, 0, ch);
    }

    return pSrc;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-050 (Trace to: SLD-AMC01-050)
* Function:     lxGetOutChAddr
* Description:  解析输出类模块通道的缓冲区地址
* Input:        slot   槽号
                ch     通道号
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
**************************************************************************************************/
uint8_t *lxGetOutChAddr(int32_t slot, int32_t ch)
{
    uint8_t *pDst = NULL;
    uint8_t type = (uint8_t)cfgCardTypeOnSlot(slot);

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(type < MODULE_TYPE_MAX);
    if(NULL != g_moduleHandle[type].pFunGetTxChAddr)
    {
        pDst = g_moduleHandle[type].pFunGetTxChAddr(slot, 0, ch);
    }

    return pDst;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-346 (Trace to: SLD-AMC01-346)
* Function:     ioComRecvData
* Description:  IO接收数据驱动
* Input:        slot  槽位号
*               size  接收数据长度
*               pBuf  接收数据缓存指针
* Output:       none
* Return:       0 : 接收数据有效 0x10000: 接收数据时CRC出错
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
*               2019/01/24    张庆林    拆分函数功能，将接收通信II数据独立出去
****************************************************************************************************/
int32_t ioComRecvData(int32_t slot, uint32_t size, void *pBuf)
{
    int32_t err = 0;
    ioFrame_t *ioFrame = (ioFrame_t *)NULL;
    uint16_t state = 0U;
    uint32_t baseAddr = 0u;

    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);

    baseAddr = com1GetAddr(slot, COM_RX1_ADDR);

    err = com1ReadData(pBuf, baseAddr, size);
    if(0 == err)
    {
        ioFrame = (ioFrame_t *)pBuf;
        state = ioFrame->comState;
        err = (int32_t)state;
    }

    return err;
}


/****************************************************************************************************
* Identifier:   SCOD-AMC01-347 (Trace to: SLD-AMC01-347)
* Function:     ioComSendData
* Description:  IO数据发送驱动
* Input:        slot 槽位号
*               size 数据长度 
*               pBuf 发送数据缓存指针
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
void ioComSendData(int32_t slot, uint32_t size, const void *pBuf)
{
    uint32_t addr = 0U;

    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(slot < CASE_SLOT_MAX);

    addr = com1GetAddr(slot, COM_TX1_ADDR);

    com1WriteData(addr, (void *)pBuf, size);
}

/*********************************************************************
* Identifier:   SCOD-AMC01-348 (Trace to: SLD-AMC01-348)
* Function:     ioRxHandle
* Description:  接收IO版卡的数据和状态
* Input:        slot  槽号
*               pBuf  IO模块数据指针
*       pFrameHandle  接收数据处理回调函数
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
void ioRxHandle(int32_t slot, void *pBuf, pFunRxFrameHandle_t pFrameHandle)
{
    uint32_t ioComErr  = 0U;
    int32_t offset = 0;
    void *pFastRxBuffer = (void *)g_fastRxBuffer;
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(NULL != pFrameHandle);

    /* 获取模块通信状态 */
    ioComErr = infoGetCom(slot);
    /* bit12为槽位0主控与IO卡通信状态 */
    offset = 12 + g_localSlot;
    ioComErr &= (1u << (uint32_t)offset); 

    /* 接收IO模块数据 */
    (void)ioComRecvData(slot, IO_FRAME_SIZE_MAX, pFastRxBuffer);

    if(ioComErr == 0x0U)
    {
        pFrameHandle(slot, 1, pBuf, (const void*)pFastRxBuffer);
    }
    else
    {
        pFrameHandle(slot, 0, pBuf, (const void*)pFastRxBuffer);
    }

}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-349 (Trace to: SLD-AMC01-349)
* Function:     ioTxHandle
* Description:  向IO模块发送数据
* Input:        slot  槽位号
*               para  IO模块保留
*               pBuf  发送数据缓存指针
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
int32_t ioTxHandle(int32_t slot, int32_t para, void *pBuf)
{
    uint32_t state = 0U;

    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(0 == para);

    state = infoCurOutputState();
    ((ioFrame_t*)pBuf)->info = state;

    /* 维护通信功能 */
    ioComSendData(slot, sizeof(ioFrame_t), (const void*)pBuf);

    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-011 (Trace to: SLD-AMC01-011)
* Function:     ioMemInit
* Description:  初始化IO模块内存
* Input:        slot 槽位号
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
int32_t ioMemInit(int32_t slot)
{
    portAttr_t *pAttr = NULL;

    pAttr = glGetPortAttr(slot, 0, RX_PORT); /* rx port */
    LYNX_ASSERT(NULL != pAttr);
    pAttr->length = IO_FRAME_SIZE_MAX;
    pAttr->pAddr = glGetMem(IO_FRAME_SIZE_MAX);

    pAttr = glGetPortAttr(slot, 0, TX_PORT); /* tx port */
    LYNX_ASSERT(NULL != pAttr);
    pAttr->length = IO_FRAME_SIZE_MAX;
    pAttr->pAddr = glGetMem(IO_FRAME_SIZE_MAX);

    return 0;
}

