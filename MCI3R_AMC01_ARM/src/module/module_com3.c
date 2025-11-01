/**************************************************************************************************
*Filename:     module_com3.c
*Purpose:      公用模块com3方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#include "global.h"
#include "module_class.h"
#include "cfg_class.h"
#include "info_class.h"
#include "com_hw.h"
#include "net_class.h"
#include "project.h"

static void disableAllRxQuality(int32_t slot, int32_t port);
static int32_t com3RecvData(int32_t slot, void *pBuf);

/**************************************************************************************************
* Identifier:   SCOD-AMC01-258 (Trace to: SLD-AMC01-258)
* Function:     disableAllRxQuality
* Description:  站间通信接收数据无效，质量位置无效
* Input:        slot 槽号
*               port 端口号
* Output:       none
* Return:       none
*
* Others:       圈复杂度过高由ASSERT断言引起，函数本身不复杂
* Log:          Date          Author    Modified
*               2017/08/14
**************************************************************************************************/
static void disableAllRxQuality(int32_t slot, int32_t port)
{
    uint32_t type = 0U;
    uint8_t *baseAddr = NULL;
    uint32_t byte1SCnt = 0U;    /* 1bytes single 变量个数 */
    uint32_t byte4SCnt = 0U;    /* 4bytes single 变量个数 */
    uint32_t i = 0u;
    portAttr_t *com3PortInfo = glGetPortAttr(slot, port, RX_PORT);

    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    type = cfgCardTypeOnSlot(slot);

    switch(type)
    {
        case COM3_TYPE:
            LYNX_ASSERT(port < COM3_PORT_MAX);
            LYNX_ASSERT(NULL != com3PortInfo);
            byte1SCnt = cfgNetPortRxCfgAddr(slot, port)->byte1SignalCnt;
            byte4SCnt = cfgNetPortRxCfgAddr(slot, port)->byte4SignalCnt;
            baseAddr = com3PortInfo->pAddr;
            break;
        default:
            /* do nothing */
            break;
    }
    LYNX_ASSERT(NULL != baseAddr);
    for(i=0u; i < byte4SCnt; i++)
    {
        ((analog6BF_t*)baseAddr)->quality = QUALITY_STATE_INVALID;
        baseAddr += sizeof(analog6BF_t);
    }
    for(i = 0u; i < byte1SCnt; i++ )
    {
        ((binary2B_t*)baseAddr)->quality = QUALITY_STATE_INVALID;
        baseAddr += sizeof(binary2B_t);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-259 (Trace to: SLD-AMC01-259)
* Function:     com3RecvData
* Description:  接收通信3数据
* Input:        slot 槽位号
*               pBuf  数据存放缓存
* Output:       none
* Return:       0    无错误 EMIF_CRC_ERROR crc错误 其它 通信状态错误
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
static int32_t com3RecvData(int32_t slot, void *pBuf)
{
    int32_t ret = 0;
    uint32_t baseAddr = 0u;
    uint32_t targetAddr = 0U;
    comFrame_t  segFrame;
    uint8_t  i = 0U;

    LYNX_ASSERT(NULL != pBuf);

    targetAddr = (uint32_t)pBuf;
    baseAddr = com1GetAddr(slot, COM_RX1_ADDR);

    for(i = 0U ; i < 4U ; i++)
    {
        ret = com1ReadData((void*)&segFrame, baseAddr, sizeof(comFrame_t));
        if(0 != ret)
        {
            break;
        }
        else
        {
            if(segFrame.comState == 0U)
            {
                (void)memcpy((void *)targetAddr, (const void *)&segFrame, (uint32_t)COM_SEG_DATA_SIZE_MAX);
                targetAddr += (uint32_t)COM_SEG_DATA_SIZE_MAX;
                baseAddr += (uint32_t)ASYNC_EMIF_COM_STEP_ADDR;
            }
            else
            {
                ret = (int32_t)segFrame.comState; /* 通信异常，放弃所有 */
                break;
            }
        }
    }

    return ret;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-260 (Trace to: SLD-AMC01-260)
* Function:     com3RxHandle
* Description:  通信III接收函数
* Input:        slot 槽位号
*               port 端口号
*               pBuf 数据指针
* Output:       none
* Return:       0
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
**************************************************************************************************/
int32_t com3RxHandle(int32_t slot, int32_t port, void *pBuf)
{
    int32_t offset = 0;
    uint32_t  lockState = 0U;
    uint32_t enable  = 0u;
    uint32_t comState = 0U;
    portState_t *pPortState = NULL;
    com3Frame_t *pFrame = (com3Frame_t *)g_fastRxBuffer;

    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(port < COM3_PORT_MAX);

    pPortState = infoGetPortState(slot, port, RX_PORT);
    LYNX_ASSERT(NULL != pPortState);

    enable = cfgGetCardEnableCh(slot) & 0x01u;
    if(0 != enable)
    {
        com3RecvData(slot, (void *)pFrame);

        /* 获取通信状态 */
        comState = infoGetCom(slot);
        offset = 12 + g_localSlot;
        comState &= (1u << offset); 

        if(comState == 0x0U)
        {
            lockState = pFrame->ctrl;
            if((lockState == CUR_STATE_ENABLE_OUTPUT) && \
                    (pFrame->magic == lxGetCfgHead()->magic))
            {
                (void)memcpy(pBuf,
                             (const void*)pFrame->data,
                             (size_t)cfgNetPortRxCfgAddr(slot, port)->totalSize);
            }
            else if(lockState == CUR_STATE_DISABLE_OUTPUT)
            {
                disableAllRxQuality(slot, port);
            }
            else
            {
                /* Do nothing */
            }

            /* 设置当前接收数据接收状态 */
            pPortState->avail = 0;
        }
        else /* 接收通信 错误时 */
        {
            disableAllRxQuality(slot, port);
            /* 保存 本次数据 接收状态 */
            pPortState->avail = 1;
        }
    }

    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-261 (Trace to: SLD-AMC01-261)
* Function:     com3SendData
* Description:  通信模块III发送数据驱动接口
* Input:        slot 槽位号 
*               size 发送数据长度
*               pBuf 发送数据缓存指针
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
static void com3SendData(int32_t slot, uint32_t size, void *pBuf)
{
    comFrame_t segFrame;
    uint8_t *pSrc = pBuf;
    uint32_t addr = 0U;
    uint32_t i = 0U;

    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(size == sizeof(com3Frame_t));

    addr = com1GetAddr(slot, COM_TX1_ADDR);
    for(i = 0U ; i < 4U ;i++)
    {
        (void)memcpy((void *)&segFrame, (const void *)pSrc, (uint32_t)COM_SEG_DATA_SIZE_MAX);  /* 耗时50~60us */
        com1WriteData(addr, (void*)&segFrame, sizeof(comFrame_t));

        pSrc += COM_SEG_DATA_SIZE_MAX;
        addr += (uint32_t)ASYNC_EMIF_COM_STEP_ADDR;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-262 (Trace to: SLD-AMC01-262)
* Function:     com3TxHandle
* Description:  向通信模块III发送数据
* Input:        slot 槽位号
*               port 参数保留，接口需要
*               pBuf 发送数据指针
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
int32_t com3TxHandle(int32_t slot, int32_t port, void *pBuf)
{
    uint32_t size   = 0u;
    uint32_t enable = 0u;
    uint32_t lockState = 0U;
    LYNX_ASSERT(NULL != pBuf);
    
    com3Frame_t *tFrame   =  (com3Frame_t *)g_fastTxBuffer;
    portAttr_t *portInfo  = (portAttr_t *)pBuf;

    enable = cfgGetCardEnableCh(slot);
    enable = (enable>>4u)&0x01u;

    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    LYNX_ASSERT(NULL != portInfo);
    LYNX_ASSERT(NULL != tFrame);

    if(0u != enable)
    {
        /* pack frame size指示的是实际发送的长度 */
        size = cfgNetPortTxCfgAddr(slot, port)->totalSize;

        LYNX_ASSERT(size <= COM3_DATA_MAX_SIZE); /* 致命错误 */

        lockState = infoCurOutputState();
        tFrame->ctrl = lockState;
        tFrame->magic = lxGetCfgHead()->magic;
        netCpyFromTxPort(tFrame->data, slot, port, 0u, size);
        com3SendData(slot, sizeof(com3Frame_t), (void*)tFrame);
    }

    return 0;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-263 (Trace to: SLD-AMC01-263)
 * Function:     com3RxHook
 * Description:  通信3模块去冗余操作
 * Input:        slot 槽位号
 *               para 数据处理方式 0 接收数据前处理 1 接收数据后处理
 * Output:       None
 * Return:       0：正常
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
int32_t com3RxHook(int32_t slot, int32_t para)
{
    int32_t  slotSlave = 0;       /* 冗余端口所在槽位号 */
    uint32_t enable  = 0u;
    portState_t *pPortStateMaster = NULL;
    portState_t *pPortStateSlave = NULL;
    portAttr_t  *pAttrMaster = NULL;
    portAttr_t  *pAttrSlave = NULL;

    int32_t port = 0;
    uint32_t copySize = 0U;

    LYNX_ASSERT(slot < CASE_SLOT_MAX);

    if(para == POST_ACTION)
    {
        pAttrMaster = glGetPortAttr(slot, port, RX_PORT);
        LYNX_ASSERT(NULL != pAttrMaster);

        enable = cfgGetCardEnableCh(slot) & 0x01u;
        if(0 != enable)
        {
            /* 单配时 无冗余端口 */
            if(cfgNetPortRxCfgAddr(slot, 0)->portType == (uint8_t)COM3_MASTER_PORT)
            {
                slotSlave = (int32_t)(cfgNetPortRxCfgAddr(slot, 0)->backPortId.detail.slot);

                pAttrSlave = glGetPortAttr(slotSlave, port, RX_PORT);
                LYNX_ASSERT(NULL != pAttrSlave);

                pPortStateMaster = infoGetPortState(slot, port, 0);
                pPortStateSlave = infoGetPortState(slotSlave, port, 0);

                copySize = cfgNetPortRxCfgAddr(slot, 0)->totalSize;

                if((pPortStateMaster->avail != 0) &&
                        (pPortStateSlave->avail == 0))
                {
                    (void)memcpy(pAttrMaster->pAddr, (const void*)pAttrSlave->pAddr, (size_t)copySize);
                }
            }
        }
    }

    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-264 (Trace to: SLD-AMC01-264)
* Function:     com3TxHook
* Description:  模块III发送数据冗余处理
* Input:        slot 槽位号
*               para 数据处理方式 0 发送数据前处理 1 发送数据后处理
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
int32_t com3TxHook(int32_t slot, int32_t para)
{
    uint32_t enable = 0u;
    portAttr_t *pPortAttr = glGetPortAttr(slot, 0, TX_PORT);

    enable = cfgGetCardEnableCh(slot);
    enable = (enable>>4u)&0x01u;

    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    LYNX_ASSERT(NULL != pPortAttr);

    if(PRE_ACTION == para)
    {
        if((enable != 0u) &&
                (cfgNetPortTxCfgAddr(slot, 0)->portType == COM3_SLAVE_PORT))
        {
            netCpyFromTxPort(pPortAttr->pAddr,
                             (int32_t)(cfgNetPortTxCfgAddr(slot, 0)->backPortId.detail.slot),
                             0,
                             0u,
                             pPortAttr->length);
        }
    }

    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-265 (Trace to: SLD-AMC01-265)
* Function:     com3Init
* Description:  模块III初始化
* Input:        slot 槽位号
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
int32_t com3Init(int32_t slot)
{
    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    /* 已根据组态生成用静态内存 */
    return 0;
}

