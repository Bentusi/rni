/****************************************************************************************************
*FILENAME:     standby.c
*PURPOSE:      热备冗余模块
*DATE          AUTHOR          CHANGE
*2017.08.14    胡德全          创建文件
****************************************************************************************************/
#include "lynx_types.h"
#include "standby.h"
#include "task.h"
#include "tm.h"
#include "global.h"
#include "cfg_class.h"
#include "panel.h"
#include "error_class.h"
#include "info_class.h"
#include "com_hw.h"
#include "force_class.h"
#include "para_class.h"
#include "flash.h"
#include "wtd.h"
#include "maintain.h"
#include "logic_class.h"
#include "module_class.h"
#include "switch.h"
#include "algInterface.h"

static int32_t s_cycleDataType =  TYPE_CPU_STATE; /* 周期同步数据净荷组成 20 本CPU状态和时间相关变量 21 维护数据帧和时间相关变量 */

static syncFrame_t *s_pRxFrame = NULL;
static syncFrame_t *s_pTxFrame = NULL;

/* 函数声明 */
static int32_t initSyncData(int8_t cmd,const uint8_t *pBuf,uint32_t size);
static void slaveTxProcess(void);

static void memCpyToGlobal(const void *src);
static void memCpyFromGlobal(void *pDst);

static int32_t sbSendData(syncFrame_t *pBuf);
static int32_t sbRecvData(syncFrame_t *pBuf);

/****************************************************************************************************
* Identifier:   SCOD-AMC01-228 (Trace to: SLD-AMC01-228)
* Function:     sbSendData
* Description:  热备冗余发送数据接口
* Input:        pBuf 发送数据缓存
* Output:       none
* Return:       ret 0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
static int32_t sbSendData(syncFrame_t *pBuf)
{
    int32_t ret = 0;
    LYNX_ASSERT(NULL != pBuf);
    
    void *pCur = (void *)pBuf;

    pBuf->crc64 = drv2CrcCal((const void*)pBuf, sizeof(syncFrame_t) - sizeof(uint64_t));

    if(NULL != g_moduleHandle[MPU_TYPE].pFunTx)
    {
        ret = g_moduleHandle[MPU_TYPE].pFunTx(g_localSlot, 0, pCur);
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-229 (Trace to: SLD-AMC01-229)
* Function:     sbRecvData
* Description:  热备冗余接收数据接口
* Input:        pBuf 接收数据缓存
* Output:       none
* Return:       ret 0 数据有效 1 数据无效
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
static int32_t sbRecvData(syncFrame_t *pBuf)
{
    int32_t ret = 1;    /* 默认数据无效 */
    LYNX_ASSERT(NULL != pBuf);
    
    void *pCur = (void *)pBuf;

    if(NULL != g_moduleHandle[MPU_TYPE].pFunRx)
    {
        ret = g_moduleHandle[MPU_TYPE].pFunRx(g_localSlot, 0, pCur);
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-230 (Trace to: SLD-AMC01-230)
* Function:     slaveTxProcess
* Description:  从模块请求周期同步数据
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2021/11/25    hdq
****************************************************************************************************/
static void slaveTxProcess(void)
{
    syncFrame_t *pTxFrame = s_pTxFrame;
    LYNX_ASSERT(NULL != pTxFrame);
    
    pTxFrame->head.cmd = SYNC_REQ_DATA_CYCLE;
    pTxFrame->head.idx = 0U;
    pTxFrame->head.type = TYPE_CPU_STATE;
    pTxFrame->head.size = (uint16_t)sizeof(moduleInfo_t);

    (void)memcpy((void*)&pTxFrame->data[0],
           (const void*)&infoGetAddr()->moduleInfo[g_localSlot],
           sizeof(moduleInfo_t));

    pTxFrame->crc64 = drv2CrcCal((const void *)pTxFrame, sizeof(syncFrame_t) - sizeof(uint64_t));
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-231 (Trace to: SLD-AMC01-231)
* Function:     initSyncData
* Description:  同步数据处理
* Input:        cmd 请求命令
*               pBuf 接收数据缓存地址
*               size 数据长度
* Output:       none
* Return:       ret: 错误码 0: 成功 1 不成功 2 同步过程中由从升为主
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14       
****************************************************************************************************/
static int32_t initSyncData(int8_t cmd, const uint8_t *pBuf, uint32_t size)
{
    uint16_t idx = 0U;
    uint32_t maxIndex = 0U;
    int32_t ret = 0;
    uint32_t reCnt = 0U;    /* 接收到命令码或帧序号出错时重新请求次数 */
    const uint8_t *baseAddr = NULL;
    uint32_t offset = 0U;
    uint16_t i = 0U;
    uint32_t outFlag = 0U;
    uint32_t  mode = 0U;
    syncFrame_t *pTxFrame = s_pTxFrame;
    LYNX_ASSERT(NULL != pTxFrame);
    syncFrame_t *pRxFrame = s_pRxFrame;
    LYNX_ASSERT(NULL != pRxFrame);

    LYNX_ASSERT(NULL != pBuf);

    maxIndex = size / SYNC_FRAME_DATA_MAX;
    if((size % SYNC_FRAME_DATA_MAX ) != 0U)
    {
        maxIndex++;
    }

    for(;;)
    {
        pTxFrame->head.cmd = cmd;
        pTxFrame->head.idx = idx;
        pTxFrame->head.size = 0U;

        (void)sbSendData(pTxFrame);
        wdFeed(WD_1);

        for(i = 0U; i< (cfgMpuPeriod() / 5U) ;i++)
        {/* 延时 确保1个周期 */
            com1ReportArmState(ARM_OK);
            tmDelayms(5U);
        }

        com1UpdateMpuState();
        /* 解析数据  */
        if((sbRecvData(pRxFrame) == 0) &&
                (pRxFrame->head.cmd == (cmd + 1)))
        {
            offset = (uint32_t)idx * SYNC_FRAME_DATA_MAX;
            baseAddr = pBuf + offset;
            size = (uint32_t)pRxFrame->head.size;
            (void)memcpy((void*)baseAddr, (const void*)pRxFrame->data, (size_t)size);
            idx++;
            reCnt = 0U;
            if(idx >= maxIndex)
            {
                outFlag = 1U;
            }
        }
        else
        {
            reCnt++;  /* 连续100次数据 接收都不正确 则退出数据请求 */
            com1ReportArmState(ARM_OK);
            if(reCnt > 100U)
            {
                ret = 1;
                outFlag = 1U;
            }
        }

        /* 同步过程中 检查是否发生主从变化  */
        mode = infoGetMpuMS();
        if(mode == MPU_MASTER)
        {
            ret = 2;
            outFlag = 1U;
        }

        if(outFlag != 0u)
        {
            break;
        }
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-232 (Trace to: SLD-AMC01-232)
* Function:     cfgFileVerCmp
* Description:  主从模块工程文件版本信息比较
* Input:        none
* Output:       none
* Return:       0:完全一致；1：参数不一致; -2：固件不一致; -1：接收数据失败
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
*               2019/07/05              函数名更改 原函数名:sbVersionCmp
****************************************************************************************************/
static int32_t cfgFileVerCmp(void)
{
    syncVerInfo_t verInfo = {0U,0UL,0U,0UL,0U,0UL,0U,0UL};
    uint32_t timeCnt = 0U;    /* 延时计数器 */
    uint32_t delayMaxCnt = 0U;
    int32_t ret = -1;
    syncFrame_t *pTxFrame = s_pTxFrame;
    LYNX_ASSERT(NULL != pTxFrame);
    syncFrame_t *pRxFrame = s_pRxFrame;
    LYNX_ASSERT(NULL != pRxFrame);

    syncVerInfo_t *pVerInfo = (syncVerInfo_t *)pRxFrame->data;
    LYNX_ASSERT(NULL != pVerInfo);
    
    verInfo.logicVer = lxGetImageHead()->version;     /* 用户算法版本与CRC */
    verInfo.logicCrc = lxGetImageHead()->crc;
    verInfo.cfgVer   = lxGetCfgHead()->version;       /* 平台配置版本与CRC */
    verInfo.cfgCrc   = lxGetCfgHead()->crc;

    delayMaxCnt = ((uint32_t)2000U / 10U);

    pTxFrame->head.cmd = SYNC_REQ_VERSION;
    pTxFrame->head.idx = 0U;
    pTxFrame->head.size = (uint16_t)sizeof(verInfo);
    (void)memcpy((void*)pTxFrame->data,(const void*)&verInfo,sizeof(verInfo));

    for(;;)
    {
        (void)sbSendData(pTxFrame);
        tmDelayms(10U);
        wdFeed(WD_1);
        com1ReportArmState(ARM_OK);

        timeCnt++;
        com1UpdateMpuState();
        if(sbRecvData(pRxFrame) == 0)
        {
            if((pRxFrame->head.cmd == SYNC_REPLAY_VERSION) ||
                    (pRxFrame->head.cmd == SYNC_REQ_VERSION))
            {
                verInfo.paraVer  = pVerInfo->paraVer;     /* 算法参数版本与CRC */
                verInfo.paraCrc  = pVerInfo->paraCrc;
                if(memcmp((const void*)&verInfo, (const void*)pRxFrame->data, sizeof(syncVerInfo_t)) != 0)
                {
                    /* 组态配置不一致 */
                    ret = -2;
                }
                else
                {
                    verInfo.paraVer  = lxGetParaHead()->version;     /* 算法参数版本与CRC */
                    verInfo.paraCrc  = lxGetParaHead()->crc;
                    if((verInfo.paraVer != pVerInfo->paraVer) ||
                            (verInfo.paraCrc != pVerInfo->paraCrc))
                    {
                        /* 组态参数不一致 */
                        ret = 1;
                    }
                    else
                    {
                        /* 组态配置完全一致 */
                        ret = 0;
                    }
                    break;
                }
            }
        }

        if(timeCnt > delayMaxCnt)
        {   /* 超出2秒都没有接收到同步数据 则认为另外一个槽位没有插卡,系统正常运行 */
            break;
        }
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-233 (Trace to: SLD-AMC01-233)
* Function:     sbTaskInit
* Description:  热备冗余初始化
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void sbTaskInit(void)
{
    uint32_t mode = infoGetMpuMS();
    uint32_t size = 0U;
    int32_t ret = 0;
    uint8_t *pAddr = NULL;

    portAttr_t *pAttr = NULL;
    pAttr = glGetPortAttr(g_localSlot, 0, RX_PORT);
    s_pRxFrame = (syncFrame_t *)pAttr->pAddr;
    pAttr = glGetPortAttr(g_localSlot, 0, TX_PORT);
    s_pTxFrame = (syncFrame_t *)pAttr->pAddr;

    LYNX_ASSERT(NULL != s_pRxFrame);
    LYNX_ASSERT(NULL != s_pTxFrame);

    /* 初始化算法 */
    ifAgLogicTaskInit();

    if(mode == MPU_SLAVE)
    {
        ret = cfgFileVerCmp();            /* 主从 工程文件一致性检测 */
        if(ret == -1)
        {
            errStopHandleInit(MPUS_COM_SYNC_INIT_ERROR);
        }
        else if(ret == -2)
        {
            errStopHandleInit(VERSION_COMPARE_ERROR);
        }
        else
        {
            /* do nothing */
        }

        /* 从模块 上电请求同步数据 */
        drv2LcdStrDisp((const int8_t *)"SYNC");

        if(ret == 1)
        {
            /* 请求参数变量 */
            size = cfgParaRamSize();
            pAddr = (uint8_t*)lxGetPara();
            ret = initSyncData(SYNC_REQ_PARA_DATA, pAddr, size);
            if(0 == ret)
            {               
                (void)paraLockFile(1); 
                /* save parameter to flash */
                do 
                {
                    com1ReportArmState(ARM_OK);
                    wdFeed(WD_1);
                    ret = paraSave();
                } while(ret != 0);
                (void)paraLockFile(0);
            }
        }

        /* 请求强制数据 */
        if(0 == ret)
        {
            size = cfgForceRamSize();
            ret = initSyncData(SYNC_REQ_FORCE_DATA, (uint8_t *)lxGetForce(), size);
        }

        /* 请求时间相关变量 */
        if(0 == ret)
        {
            size = cfgLogicGlobalSize();
            pAddr = (uint8_t *)logicGetUsrOutAddr();
            pAddr += cfgLogicGlobalStart();
            ret = initSyncData(SYNC_REQ_TIME_DATA, pAddr, size);
        }

        if(ret != 0)
        {
            errStopHandleInit(MPUS_COM_SYNC_INIT_ERROR);
        }

        slaveTxProcess();
    }

    drv2LcdStrDisp((const int8_t *)"CYCL");
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-234 (Trace to: SLD-AMC01-234)
* Function:     sbSyncForceData
* Description:  拷贝维护数据帧到同步数据发送缓存
* Input:        pBuf            需要同步的维护数据指针
* Output:       s_pTxFrame      发送数据缓存
*               s_cycleDataType 同步数据帧的数据内容
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void sbSyncForceData(const uint8_t *pBuf)
{
    uint32_t msMode = 0U;

    msMode = infoGetMpuMS();
    LYNX_ASSERT(NULL != pBuf);
    
    if((cfgLocalPfMode() == CFG_REDUNDANCY) &&
            (msMode == MPU_MASTER))
    {
        (void)memcpy((void*)&s_pTxFrame->data[0],(const void*)pBuf, (uint32_t)COM_SEG_SIZE_MAX);
        s_cycleDataType = TYPE_MATAIN_DATA;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-235 (Trace to: SLD-AMC01-235)
* Function:     errSyncState
* Description:  主控模块故障停机状态下同步CPU状态信息
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void errSyncState(void)
{
    syncFrame_t *pRxFrame = s_pRxFrame;
    LYNX_ASSERT(NULL != pRxFrame);
    /* 发送 本地主控状态 到对端 */
    slaveTxProcess();

    /* 接收 对端模块的状态 */
    if(sbRecvData(pRxFrame) == 0)
    {
        if(((pRxFrame->head.cmd ==  SYNC_REPLY_DATA_CYCLE) || \
             (pRxFrame->head.cmd ==  SYNC_REQ_DATA_CYCLE)) && \
           (pRxFrame->head.type == TYPE_CPU_STATE))
        {
            (void)memcpy((void*)&infoGetAddr()->moduleInfo[1 - g_localSlot],
                   (const void*)&pRxFrame->data,
                   sizeof(moduleInfo_t));
        }
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-327 (Trace to: SLD-AMC01-327)
* Function:     memCpyToGlobal
* Description:  复制数据到逻辑算法输出区的指定地址区域内
* Input:        src 源地址，长度由平台配置文件获取
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
static void memCpyToGlobal(const void *src)
{
    uint32_t offset = cfgLogicGlobalStart();
    uint32_t size = cfgLogicGlobalSize();
    LYNX_ASSERT(NULL != src);

    (void)memcpy(logicGetUsrOutAddr() + offset, src, (size_t)size);
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-328 (Trace to: SLD-AMC01-328)
* Function:     memCpyFromGlobal
* Description:  得到算法的DATA段数据
* Input:        pDst 目的地址，长度由平台配置文件获取
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
*               2021/08/31    hdq       使用获取输出buffer地址函数代替全局变量
**************************************************************************************************/
static void memCpyFromGlobal(void *pDst)
{
    uint32_t offset = cfgLogicGlobalStart();
    uint32_t size = cfgLogicGlobalSize();
    LYNX_ASSERT(NULL != pDst);
    
    (void)memcpy(pDst, (const void*)(logicGetUsrOutAddr() + offset), (size_t)size);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-236 (Trace to: SLD-AMC01-236)
* Function:     cmdReqDataCycle
* Description:  主模块发送周期同步数据
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2021/11/25    hdq
****************************************************************************************************/
static void cmdReqDataCycle(void)
{
    uint8_t *baseAddr = (uint8_t *)NULL;
    int32_t type = 0;
    syncFrame_t *pRxFrame = s_pRxFrame;
    LYNX_ASSERT(NULL != pRxFrame);
    syncFrame_t *pTxFrame = s_pTxFrame;
    LYNX_ASSERT(NULL != pTxFrame);

    type = pRxFrame->head.type;

    /* 解析从模块的cpu状态 */
    if(type == TYPE_CPU_STATE)
    {
        /* 解析对端主控CPU状态 */
        (void)memcpy((void*)&infoGetAddr()->moduleInfo[1 - g_localSlot],
               (const void*)&pRxFrame->data[0],
               sizeof(moduleInfo_t));
    }

    /* 周期同步 帧头填充 */
    pTxFrame->head.cmd = SYNC_REPLY_DATA_CYCLE;
    pTxFrame->head.idx = 0U;

    if(s_cycleDataType == TYPE_MATAIN_DATA)
    {
        pTxFrame->head.type = TYPE_MATAIN_DATA;
        s_cycleDataType = TYPE_CPU_STATE;           /* 维护数据帧发送后将继续发送主控状态 */
    }
    else
    {
        /* 将本主控的 cpu状态 填充到发送数据帧中 */
        (void)memcpy((void*)&pTxFrame->data[0],
               (const void*)&infoGetAddr()->moduleInfo[g_localSlot],
               sizeof(moduleInfo_t));
        pTxFrame->head.type = TYPE_CPU_STATE;
    }

    /* 发送时间相关变量 */
    pTxFrame->head.size = (uint16_t)cfgLogicGlobalSize();
    if(pTxFrame->head.size <= (SYNC_FRAME_DATA_MAX - SYNC_SEG_SIZE))
    {
        baseAddr = pTxFrame->data + SYNC_SEG_SIZE;
        memCpyFromGlobal((void*)baseAddr);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-237 (Trace to: SLD-AMC01-237)
* Function:     masterRxProcess
* Description:  主模块处理同步数据
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/11/25    hdq
****************************************************************************************************/
static void masterRxProcess(void)
{
    uint16_t idx = 0U;
    uint32_t size = 0U;
    uint16_t maxIndex = 0U;
    uint8_t *baseAddr = NULL;
    syncVerInfo_t verInfo = {0U};
    uint8_t *tag = NULL;
    uint8_t *src = NULL;
    syncFrame_t *pRxFrame = s_pRxFrame;
    LYNX_ASSERT(NULL != pRxFrame);
    syncFrame_t *pTxFrame = s_pTxFrame;
    LYNX_ASSERT(NULL != pTxFrame);

    switch(pRxFrame->head.cmd)
    {
        case SYNC_REQ_VERSION:
            idx = pRxFrame->head.idx;
            pTxFrame->head.cmd = SYNC_REPLAY_VERSION;
            pTxFrame->head.idx = idx;
            verInfo.logicVer = lxGetImageHead()->version;     /* 用户算法版本与CRC */
            verInfo.logicCrc = lxGetImageHead()->crc;
            verInfo.cfgVer   = lxGetCfgHead()->version;       /* 平台配置版本与CRC */
            verInfo.cfgCrc   = lxGetCfgHead()->crc;
            verInfo.paraVer  = lxGetParaHead()->version;      /* 算法参数版本与CRC */
            verInfo.paraCrc  = lxGetParaHead()->crc;
            tag = pTxFrame->data;
            src = (uint8_t *)&verInfo;
            LYNX_ASSERT(NULL != tag);
            (void)memcpy((void*)tag, (const void*)src, sizeof(verInfo));
            break;

        case SYNC_REQ_FORCE_DATA:
            idx = pRxFrame->head.idx;
            pTxFrame->head.cmd = SYNC_REPLAY_FORCE_DATA;
            pTxFrame->head.idx = idx;
            size = cfgForceRamSize();
            maxIndex = (uint16_t)(size / SYNC_FRAME_DATA_MAX);
            if((size % SYNC_FRAME_DATA_MAX) != 0u)
            {
                maxIndex++;
            }
            if((idx + 1U ) == maxIndex)
            {   /* 最后 一帧 */
                pTxFrame->head.size = (uint16_t)(size - ((uint32_t)idx * (uint32_t)SYNC_FRAME_DATA_MAX));
            }
            else
            {
                pTxFrame->head.size = SYNC_FRAME_DATA_MAX;
            }
            baseAddr = (uint8_t *)lxGetForce() + ((uint32_t)idx * SYNC_FRAME_DATA_MAX);
            LYNX_ASSERT(NULL != baseAddr);
            tag = pTxFrame->data;
            size = (uint32_t)pTxFrame->head.size;
            LYNX_ASSERT(NULL != tag);
            (void)memcpy((void*)tag, (const void*)baseAddr, (size_t)size);
            break;

        case SYNC_REQ_PARA_DATA:
            idx = pRxFrame->head.idx;
            baseAddr = lxGetPara();
            pTxFrame->head.cmd = SYNC_REPLAY_PARA_DATA;
            pTxFrame->head.idx = idx;

            size = cfgParaRamSize();
            maxIndex = (uint16_t)(size / SYNC_FRAME_DATA_MAX);
            if((size % SYNC_FRAME_DATA_MAX) != 0u)
            {
                maxIndex++;
            }
            if((idx + 1U ) == maxIndex)
            {
                pTxFrame->head.size = (uint16_t)(size - ((uint32_t)idx * (uint32_t)SYNC_FRAME_DATA_MAX));
            }
            else
            {
                pTxFrame->head.size = SYNC_FRAME_DATA_MAX;
            }
            baseAddr += (idx * SYNC_FRAME_DATA_MAX);
            tag = pTxFrame->data;
            size = (uint32_t)pTxFrame->head.size;
            (void)memcpy((void*)tag, (const void*)baseAddr, (size_t)size);
            break;

        case SYNC_REQ_TIME_DATA:
            /* 时间相关变量 只有1帧数据 */
            pTxFrame->head.cmd = SYNC_REPLAY_TIME_DATA;
            pTxFrame->head.idx = 0U;
            pTxFrame->head.size = (uint16_t)cfgLogicGlobalSize();
            memCpyFromGlobal((void*)pTxFrame->data);
            break;

        case SYNC_REQ_DATA_CYCLE:
            cmdReqDataCycle();      /* 周期 同步时间相关数据 */
            break;

        default:
            break;
    }

    pRxFrame->head.cmd = SYNC_CMD_INIT;
    pTxFrame->crc64 = drv2CrcCal((const void*)pTxFrame, sizeof(syncFrame_t) - sizeof(uint64_t));
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-238 (Trace to: SLD-AMC01-238)
* Function:     sbSlaveRxProcess
* Description:  从模块解析周期同步数据
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2021/11/25    hdq
****************************************************************************************************/
void sbSlaveRxProcess(void)
{
    uint8_t *baseAddr = (uint8_t *)NULL;
    int32_t type = 0;
    syncFrame_t *pRxFrame = s_pRxFrame;
    LYNX_ASSERT(NULL != pRxFrame);

    if(pRxFrame->head.cmd == SYNC_REPLY_DATA_CYCLE)
    {
        type = pRxFrame->head.type;
        if(type == TYPE_MATAIN_DATA)
        {   /* 解析 维护数据帧 */
            mtSaveFile(pRxFrame->data);
            /* 拷贝时间相关变量到算法输出区 */
            baseAddr = pRxFrame->data + SYNC_SEG_SIZE;
            memCpyToGlobal((const void*)baseAddr);
        }
        else if(type == TYPE_CPU_STATE)
        {   /* 解析对端CPU状态 */
            (void)memcpy((void*)&infoGetAddr()->moduleInfo[1 - g_localSlot],
                   (const void*)&pRxFrame->data,
                   sizeof(moduleInfo_t));

            /* 拷贝时间相关变量到算法输出区 */
            baseAddr = pRxFrame->data + SYNC_SEG_SIZE;
            memCpyToGlobal((const void*)baseAddr);
        }
        else
        {
            /* do nothing */
        }
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-239 (Trace to: SLD-AMC01-239)
* Function:     sbTask
* Description:  主机向从机发送同步数据，从机接收同步数据
* Input:        none
* Output:       none
* Return:       0
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
int32_t sbTask(void)
{
    /* 获取本身的主从模式 */
    uint32_t mode = infoGetMpuMS();
    portState_t *pPortState0 = infoGetPortState(g_localSlot, 0, RX_PORT);
    static uint8_t syncFlag = 1U;

    /* 判断 是否为有效数据帧 */
    if(pPortState0->avail == 0)
    {
        syncFlag = 1U;
        /* 接收成功 */
        infoClearPfError(MPUS_COM_RX_FIAL);
        if(mode == MPU_MASTER)
        {
            /* 主模块解析命令 并处理 */
            masterRxProcess();
        }
    }
    else
    {
        if(syncFlag > 1)
        {
            /* CRC 数据校验失败时 且对端主控无故障 则表明 接收数据失败 */
            infoSetPfError(MPUS_COM_RX_FIAL);
            infoSetCom(1 - g_localSlot,0x01U);   /* 当从卡被拨出时报通信故障 */
            if(infoGetPfErrorState(BACK_MPU_ERROR) != 0)
            {
                infoClearSlot(1 - g_localSlot);
            }
        }
        else
        {
            syncFlag++;
        }
    }
    if(mode == MPU_SLAVE)
    {
        /* 发起新的请求 */
        slaveTxProcess();
    }
    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-097 (Trace to: SLD-AMC01-097)
* Function:     sbSyncLedShow
* Description:  同步数据指示灯控制
* Input:        none
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void sbSyncLedShow(void)
{
    if((infoGetPfErrorState(MPUS_COM_RX_FIAL) == 0U) && \
        (cfgLocalPfMode() == CFG_REDUNDANCY))
    {
        drv2LedTurnOn(PanelLedSync, GREEN);
    }
    else
    {
        drv2LedTurnOff(PanelLedSync, GREEN);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-240 (Trace to: SLD-AMC01-240)
* Function:     sbCheckMode
* Description:  校验冗余配置主控模块运行模式
* Input:        none
* Output:       none
* Return:       ret 0 模式一致 1 模式不一致
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
int32_t sbCheckMode(void)
{
    int32_t ret = 0; /* same */
    uint32_t localTrueMode = infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.curTrueMode;
    uint32_t standbyTrueMode = infoGetAddr()->moduleInfo[1 - g_localSlot].difInfo.mpuInfo.curTrueMode;
    
    if(cfgLocalPfMode() == CFG_REDUNDANCY)
    {
        /* 模式不一致  禁止执行维护命令 */
        if((localTrueMode != standbyTrueMode) &&
                (standbyTrueMode != SYS_INIT))
        {
            ret = 1;
            infoSetPfError(MODE_NOT_MATCH);
        }
        else
        {
            infoClearPfError(MODE_NOT_MATCH);
        }
    }

    return ret;
}

