/***************************************************************************************************
 * Filename: module_mpu.c
 * Purpose:  主控模块功能定义
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/
#include "lynx_types.h"
#include "global.h"
#include "bsp.h"
#include "tm.h"
#include "com_hw.h"
#include "error_class.h"
#include "module_class.h"
#include "cfg_class.h"
#include "info_class.h"
#include "monitor.h"
#include "standby.h"
#include "env.h"
#include "project.h"

/**************************************************************************************************
* Identifier:   SCOD-AMC01-350 (Trace to: SLD-AMC01-350)
* Function:     mpuMemInit
* Description:  主控模块内存分配
* Input:        slot 槽号
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
**************************************************************************************************/
static void mpuMemInit(int32_t slot)
{
    portAttr_t *pAttr = NULL;

    pAttr = glGetPortAttr(slot, 0, RX_PORT); /* rx port */
    LYNX_ASSERT(NULL != pAttr);
    pAttr->length = COM_SEG_DATA_SIZE_MAX * 4U;
    pAttr->pAddr = glGetMem(pAttr->length);

    pAttr = glGetPortAttr(slot, 0, TX_PORT); /* tx port */
    LYNX_ASSERT(NULL != pAttr);
    pAttr->length = COM_SEG_DATA_SIZE_MAX * 4U;
    pAttr->pAddr = glGetMem(pAttr->length);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-225 (Trace to: SLD-AMC01-225)
* Function:     mpuRxHandle
* Description:  接收同步数据，每次读取4K数据
* Input:        slot  槽位号
*               port  端口号
*               pBuf  数据存放缓存
* Output:       none
* Return:       0:接收数据有效；其它：数据无效
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
int32_t mpuRxHandle(int32_t slot, int32_t port, void *pBuf)
{
    int32_t err = 0;
    int32_t i = 0;

    uint8_t *pDst = (uint8_t *)pBuf;
    syncFrame_t *pSyncFrame = (syncFrame_t *)pBuf;
    uint32_t src = SYNC_READ_BASE_ADDR;
    comFrame_t *pFrame = (comFrame_t *)g_fastRxBuffer;
    portState_t *pPortState = NULL;
    uint32_t comState = 0U;

    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(slot < 2);
    LYNX_ASSERT(0 == port);

    pPortState = infoGetPortState(g_localSlot, port, RX_PORT);
    comState = infoGetCom(g_localSlot);
    comState &= 0x01U;
    LYNX_ASSERT(NULL != pPortState);
    
    if((slot == g_localSlot))
    {
        if(pPortState->full == 0)  /* 有数据 */
        {
            for(i = 0; i < 4; i++)
            {
                pPortState->avail = 0;
                err = com1ReadData((void*)pFrame, src, sizeof(comFrame_t));
                if((err == 0) &&
                        (comState == 0U))
                {
                    (void)memcpy((void*)pDst, (const void*)pFrame->data, (uint32_t)COM_SEG_DATA_SIZE_MAX);
                    pDst += (uint32_t)(COM_SEG_DATA_SIZE_MAX);
                    src += (uint32_t)sizeof(comFrame_t);
                }
                else
                {
                    pPortState->avail = 1;
                    break;
                }
            }
        }
        else
        {
            pPortState->avail = 1;
        }

        /* 校验数据 */
        if(pPortState->avail == 0)
        {
            /* 判断 是否为有效数据帧 */
            if(drv2CrcCal((const void*)pSyncFrame, sizeof(syncFrame_t)- sizeof(uint64_t)) != pSyncFrame->crc64)
            {
                pPortState->avail = 1;
            }
        }
    }
    return (int32_t)(pPortState->avail);
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-226 (Trace to: SLD-AMC01-226)
 * Function:     mpuRxHook
 * Description:  冗余同步数据接收处理
 * Input:        slot 槽位号
 *               para 数据处理方式 0 接收前处理 1 接收后处理
 * Output:       None
 * Return:       ret 0
 * Date:         Author      Modified
 * 2021-12-20    hdq         Create
 *************************************************************************************************/
int32_t mpuRxHook(int32_t slot, int32_t para)
{
    int32_t ret = 0;
    portState_t *pPortState = NULL;
    uint32_t mode = infoGetMpuMS();

    LYNX_ASSERT(slot < 2);

    pPortState = infoGetPortState(g_localSlot, 0, RX_PORT);
    
    LYNX_ASSERT(NULL != pPortState);

    if(para == POST_ACTION)
    {
        if((mode == MPU_SLAVE) &&
            (slot == g_localSlot))
        {
            if(pPortState->avail == 0)
            {
                /* 从模块解析命令 并处理 */
                sbSlaveRxProcess();
            }
        }
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-227 (Trace to: SLD-AMC01-227)
* Function:     mpuTxHandle
* Description:  同步数据发送
* Input:        slot 槽位号
*               port 端口号
*               pBuf 发送数据缓存指针
* Output:       none
* Return:       返回0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
int32_t mpuTxHandle(int32_t slot, int32_t port, void *pBuf)
{
    int32_t i = 0;
    uint8_t *pSrc = (uint8_t *)pBuf;
    comFrame_t *pFrame = (comFrame_t *)g_fastTxBuffer;
    uint32_t dst = SYNC_WRITE_BASE_ADDR;

    LYNX_ASSERT(slot < 2);
    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(0 == port);
    LYNX_ASSERT(NULL != pFrame);
    
    if(slot == g_localSlot)
    {
        for(i = 0; i < 4; i++)
        {   /* 通信 占用了 4个 字节 */
            (void)memcpy((void*)pFrame->data, (const void*)pSrc, (uint32_t)COM_SEG_DATA_SIZE_MAX);

            com1WriteData(dst, (void*)pFrame, sizeof(comFrame_t));

            dst += sizeof(comFrame_t);
            pSrc += COM_SEG_DATA_SIZE_MAX;
        }
    }

    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-015 (Trace to: SLD-AMC01-015)
* Function:     mpuInit
* Description:  下发组态配置信息
* Input:        slot 槽位号
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
int32_t mpuInit(int32_t slot)
{
    int32_t idx = 0;

    uint8_t mode = 0U;
    int32_t ret = 0;
    uint8_t stationNum = 0U;

    uint32_t okCnt = 0U;
    uint32_t failCnt = 0U;
    int32_t slotMateCnt = 0;
    static int32_t allow = 0;

    fpgaReadReg_t *pReadReg = (fpgaReadReg_t *)g_fastTxBuffer;
    lynxID_t cfgId;     /* 配置ID*/
    /* 取出对应槽号的 配置ID */
    cfgId.value = cfgSlotToId(slot);

    LYNX_ASSERT(slot < 2);
    LYNX_ASSERT(NULL != pReadReg);

    mpuMemInit(slot);

    /* 发送主控配置信息 */
    com1CalReadData(slot);
    com1CalWriteData(slot);
    com1CalConfigData(slot);

    (void)memcpy((void*)g_fastTxBuffer, (const void*)&lxGetCfg()->cardCfg[slot], sizeof(moduleCfg_t));
    com1WriteData(com1GetAddr(slot, COM_CFG_ADDR), (void*)g_fastTxBuffer, SLOT_CFG_MAX_SIZE);

    if(allow == 0)
    {
        volDiganostic(1U);     /* 上电 电压自监视 */
        /* 等待FPGA初始化完成  */
        while(1)
        {
            /* 读取数据 */
            ret = com1ReadData((void*)pReadReg, MPU_HADR_INFO_ADDR, sizeof(fpgaReadReg_t));
            if(ret == 0)
            {
                if( ((pReadReg->msg1 & 0x40U) == 0x40U) &&
                    ((pReadReg->slotAndcaseNum >> 5U) == cfgId.detail.caseNum)&&
                    ((pReadReg->stationNum & 0x3FU) == cfgId.detail.stationNum) &&
                    ((pReadReg->slotAndcaseNum & 0x1FU) < 2U)) /* 槽位号在 0 或 1 槽 */
                 {
                     okCnt++;
                     failCnt = 0U;
                     if(okCnt > 10U)
                     {
                         cfgId.detail.slot = pReadReg->slotAndcaseNum & 0x1FU;
                         cfgId.detail.aOrb = pReadReg->slotAndcaseNum & 0x1FU;
                         cfgId.detail.stationNum = pReadReg->stationNum;
                         g_localSlot = cfgId.detail.aOrb;
                         initMpuSates(); /* 初始化 主控模块 平台信息 */
                         infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.id = cfgId; /* 设置当前主控ID */
                         infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.fpgaVersion = pReadReg->fpgaVersion;
                         allow = 1;
                         if(pReadReg->isMaster == 0xFFU)
                         {/* 主模式 */
                             mode = MPU_MASTER;
                         }
                         else
                         {/* 从模式 */
                             mode = MPU_SLAVE;
                             if(cfgLocalPfMode()== CFG_SINGLE)
                             {
                                 errStopHandleInit(PF_MODE_ERR);
                             }
                         }
                         break;
                     }
                 }
                 else /* 槽号故障 */
                 {
                     failCnt++;
                     okCnt = 0U;
                     if(failCnt > 1000U) /* 超过10秒 则认为 FPGA 初始化失败 */
                     {
                         errStopHandleInit(FPGA_INIT_ERROR);
                     }
                     slotMateCnt++;
                     if((cfgId.detail.slot != (pReadReg->slotAndcaseNum & 0x1FU)) \
                             && (slotMateCnt >= 2) \
                             && ((pReadReg->slotAndcaseNum & 0x1FU) < 2))
                     {
                        ret = 1;
                        break;
                     }
                 }
            }

            tmDelayms(10U);
        }
        if(0 == ret)
        {
            infoSetMpuMS(mode);
            com1ReportArmState(ARM_OK);

            /* 发送非主控模块组态配置信息 */
            for(idx = 2; idx < LYNX_SLOT_MAX; idx++)
            {
                com1CalReadData(idx);
                com1CalWriteData(idx);
                com1CalConfigData(idx);

                (void)memcpy((void*)g_fastTxBuffer, (const void*)&lxGetCfg()->cardCfg[idx], sizeof(moduleCfg_t));
                if(idx == ((moduleCfg_t*)g_fastTxBuffer)->baseCfg.id.detail.slot)
                {
                    ((moduleCfg_t*)g_fastTxBuffer)->baseCfg.id.detail.stationNum = cfgId.detail.stationNum;
                }
                if(COM3_TYPE == ((moduleCfg_t*)g_fastTxBuffer)->baseCfg.id.detail.type)
                {
                    stationNum = (g_psStation[(cfgId.detail.stationNum & 0xC0U) >> 6U]) | (cfgId.detail.stationNum & 0xC0U);

                    ((moduleCfg_t*)g_fastTxBuffer)->difCfg.comCardCfg.txCfg[0].targetId.detail.stationNum = stationNum;
                    ((moduleCfg_t*)g_fastTxBuffer)->difCfg.comCardCfg.txCfg[0].backPortId.detail.stationNum = stationNum;

                    ((moduleCfg_t*)g_fastTxBuffer)->difCfg.comCardCfg.rxCfg[0].targetId.detail.stationNum = stationNum;
                    ((moduleCfg_t*)g_fastTxBuffer)->difCfg.comCardCfg.rxCfg[0].backPortId.detail.stationNum = stationNum;
                }
                com1WriteData(com1GetAddr(idx, COM_CFG_ADDR), (void*)g_fastTxBuffer, SLOT_CFG_MAX_SIZE);
            }
        }
    }
    return 0;
}

