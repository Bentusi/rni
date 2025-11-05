/***************************************************************************************************
 * Filename: maintain.c
 * Purpose:  维护模式接口模块
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/
#include "lynx_types.h"
#include "standby.h"
#include "com_hw.h"
#include "global.h"
#include "panel.h"
#include "info_class.h"
#include "module_class.h"
#include "task.h"
#include "force_class.h"
#include "tm.h"
#include "wtd.h"
#include "switch.h"
#include "para_class.h"
#include "logic_class.h"
#include "cfg_class.h"
#include "monitor.h"
#include "maintain.h"
#include "error_class.h"

static int32_t s_mtSlot = 0;
static int32_t s_mtPort = 0;

/*********************************************************************
* Identifier:   SCOD-AMC01-242 (Trace to: SLD-AMC01-242)
* Function:     mtRxData
* Description:  接收维护数据接口
* Input:        pBuf 数据缓存指针
* Output:       none
* Return:       0
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
***********************************************************************/
static int32_t mtRxData(void *pBuf)
{
    int32_t ret = -99;

    LYNX_ASSERT(NULL != pBuf);
    if(NULL != g_moduleHandle[COM4_TYPE].pFunRx)
    {
        ret = g_moduleHandle[COM4_TYPE].pFunRx(s_mtSlot, s_mtPort, pBuf);
    }

    return ret;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-243 (Trace to: SLD-AMC01-243)
 * Function:     mtTxData
 * Description:  发送维护数据接口
 * Input:        pBuf: send buffer address
 * Output:       None
 * Return:       0: success; other:fail
 * Date:         Author      Modified
 * 2021-12-23    hdq         Create
 *************************************************************************************************/
static int32_t mtTxData(void *pBuf)
{
    int32_t ret = -99;

    LYNX_ASSERT(NULL != pBuf);

    if(NULL != g_moduleHandle[COM4_TYPE].pFunTx)
    {
        ret = g_moduleHandle[COM4_TYPE].pFunTx(s_mtSlot, s_mtPort, pBuf);
    }

    tmDelayus(200U);

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-244 (Trace to: SLD-AMC01-244)
* Function:     checkFrame
* Description:  数据帧检测
* Input:        frame  数据帧指针
* Output:       none
* Return:       ret     检测结果， 0 正常 其它 无效
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
static int32_t checkFrame(const com4Frame_t *frame)
{
    int32_t ret = FRAME_OK;

    LYNX_ASSERT(NULL != frame);

    if(0 != infoGetPfErrorState(MODE_NOT_MATCH))
    {
        /* 模式不一致  禁止执行维护命令 */
        ret = FRAME_ERR_MODE;
    }

    if((ret == FRAME_OK)&&(frame->size > (uint16_t)COM4_DATA_SIZE_MAX))
    {
        ret = FRAME_ERR_CNT;
    }

    if((ret == FRAME_OK) && (frame->cmd != 0x100))
    {
        ret = FRAME_ERR_CMD;
    }

    if((ret == FRAME_OK) && (frame->idx >= frame->idxMax))
    {
        ret = FRAME_ERR_IDX;
    }

    if((ret == FRAME_OK) && (frame->id != infoCurMpuId().value))
    {
        ret = FRAME_ERR_ID;
    }

    if(ret == FRAME_OK)
    {
        if((frame->type < (int16_t)FILE_MODIFY_PARA) ||
                (frame->type > (int16_t)FILE_MODIFY_T2)) 
        {
            ret = FRAME_ERR_TYPE;
        }
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-245 (Trace to: SLD-AMC01-245)
* Function:     ackMtsData
* Description:  通信接收数据响应帧
* Input:        cmd 响应命令
*              para 响应参数
*             fType 响应文件类型
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
static void ackMtsData(int32_t cmd, int32_t para, int16_t fType)
{
    com4Frame_t *pTxFrame = (com4Frame_t *)g_fastTxBuffer;

    /* 本帧回复 */
    pTxFrame->id = infoCurMpuId().value;
    pTxFrame->type = fType;    /* 文件类型 */
    pTxFrame->idxMax = 1U;     /* 分片数据帧总数据量 */
    pTxFrame->idx = 0U;        /* 帧序号 */
    pTxFrame->size = 4U;       /* 当前帧长度 */
    pTxFrame->cmd = cmd;

    (void)memcpy((void*)pTxFrame->data, (const void*)&para, sizeof(int32_t));

    (void)mtTxData((void*)pTxFrame);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-246 (Trace to: SLD-AMC01-246)
* Function:     ackMtsData
* Description:  checkMagic
* Input:        magic 校验值
*                type 文件类型
* Output:       none
* Return:       ret   校验结果
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
static int32_t checkMagic(uint32_t magic, uint32_t type)
{
    int32_t ret = 0;

    switch(type)
    {
        case FILE_MODIFY_FORCE:
        case FILE_MODIFY_T2:
            if(magic != lxGetForceHead()->magic)
            {
                ret = -1; /* 工程不匹配 */
            }
            break;

        case FILE_MODIFY_PARA:
            if(magic != lxGetParaHead()->magic)
            {
                ret = -2; /* 工程不匹配 */
            }
            break;

        default:
            ret = -3;
            break;
    }

    return ret;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-247 (Trace to: SLD-AMC01-247)
* Function:     mtFileProcess
* Description:  解析强制文件
* Input:        pBuf 强制文件输入指针
* Output:       none
* Return:       0    成功 1 文件校验失败 2 不支持文件类型 3 工程不匹配
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
static int32_t mtFileProcess(void *pBuf)
{
    int32_t ret = 0;
    uint32_t i = 0U;
    uint32_t size = 0U;
    uint32_t type = 0U;
    uint32_t idx = 0u;
    uint64_t crc = 0ULL;
    uint8_t *curAddr = (uint8_t *)pBuf + sizeof(fileHead_t);

    fileHead_t *pInfo = (fileHead_t *)pBuf;

    LYNX_ASSERT(NULL != pBuf);

    /* 所有数据进行校验 */
    crc = drv2CrcCal((const void*)pBuf + sizeof(fileHead_t), pInfo->length);
    if( crc == pInfo->crc)
    {
        /* 数据OK */
        type = pInfo->type;
        switch(type)
        {
            case FILE_MODIFY_FORCE:
            case FILE_MODIFY_T2:
                if(checkMagic(pInfo->magic, FILE_MODIFY_FORCE) != 0)
                {
                    ret = 3; /* 工程不匹配 */
                }
                else
                {
                    for(i = 0u; i < pInfo->cnt ; i++)
                    {
                        size = ((modifyHead_t*)curAddr)->valueSize;
                        idx = ((modifyHead_t*)curAddr)->index;
                        curAddr += sizeof(modifyHead_t);
                        forceCopyTo(idx, (const void*)curAddr, size); /* 拷贝强制文件 */
                        curAddr += size;
                    }
                }
                break;

            case FILE_MODIFY_PARA:
                if(checkMagic(pInfo->magic, FILE_MODIFY_PARA) != 0)
                {
                    ret = 3; /* 工程不匹配 */
                }
                else
                {
                    for(i = 0u; i < pInfo->cnt ; i++)
                    {
                        size = ((modifyHead_t*)curAddr)->valueSize;
                        idx = ((modifyHead_t*)curAddr)->index;
                        curAddr += sizeof(modifyHead_t);
                        paraMemCpyTo(idx, (const void*)curAddr, size);
                        curAddr += size;
                    }
                    (void)paraLockFile(1);
                }
                break;

            default:
                /* Do nothing */
                ret = 2;
                break;
        }
    }
    else
    {
        /* 文件校验失败 */
        ret = 1;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-248 (Trace to: SLD-AMC01-248)
* Function:     mtRxFile
* Description:  接通信IV数据，并发出响应帧
* Input:        pFile 接收文件缓存
* Output:       none
* Return:       0 接收数据完成 1 帧校验失败 2 通信出错 4 文件正在接收中
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
static int32_t mtRxFile(void *pFile)
{
    int32_t result = -99;
    int32_t ret = 0;
    uint16_t idx = 0u;
    uint8_t  *curAddr = NULL;
    uint16_t cpySize = 0u;

    const com4Frame_t *pRxFrame = (com4Frame_t *)g_fastRxBuffer;

    /* 接收维护数据帧 */
    result = mtRxData((void*)pRxFrame);
    if(result == 0)
    {
        /* 数据帧检测 */
        result = checkFrame(pRxFrame);
        if(result == 0)
        {
            /* 根据索引保存数据 */
            cpySize = pRxFrame->size;
            idx = pRxFrame->idx;
            curAddr = (uint8_t *)pFile + (idx * COM4_DATA_SIZE_MAX);
            (void)memcpy((void*)curAddr, (const void*)pRxFrame->data, (size_t)cpySize);

            /* 将接收到的维护命令帧复制到同步发送缓存 下一周期发送 */
            sbSyncForceData((const uint8_t *)pRxFrame);

            ret = 4; /* 文件正在接收中 */
        }
        else
        {
            ret = 1; /* 帧校验失败 */
        }

        /* 本帧回复 */
        ackMtsData(0x200, result, pRxFrame->type);

        /* 帧接收完成 */
        if((idx >= (pRxFrame->idxMax - (uint16_t)1U)) &&
                (ret == 4)) /* 最后一帧 数据 且数据帧正常 */
        {
            result = mtFileProcess(pFile);
            result += FILE_OK;
            ackMtsData(0x200, result, pRxFrame->type);
            ret = 0;
        }
    }
    else if(result == -1)
    {
        ret = 2; /* 通信接收失败 */
        ackMtsData(0x200, FRAME_COM_ERR, pRxFrame->type);
    }
    else
    {
        /* do nothing */
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-249 (Trace to: SLD-AMC01-249)
* Function:     mtTxFile
* Description:  向工程师站发送维护数据
* Input:        pBuf 发送数据指针
* Output:       none
* Return:       ret  当前文件是否发送完成 1 未发完 0 发送完成
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
static int32_t mtTxFile(void *pBuf)
{
    static int8_t locker = 0;
    /* 当前剩余数据量 */
    static uint32_t remainSize = 0U;
    /* 当前要发送缓冲区地址 */
    static uint32_t curPosition = 0U;
    static uint16_t curIndex = 0U;
    static uint16_t maxIndex = 0U;

    uint32_t cpySize = 0U;
    int32_t ret = 0;

    fileHead_t *pFileHead = (fileHead_t *)pBuf;
    com4Frame_t *pFrame = (com4Frame_t *)(g_fastTxBuffer);

    LYNX_ASSERT(NULL != pFileHead);

    if(0 == locker) /* 第一次*/
    {
        remainSize = pFileHead->length + sizeof(fileHead_t);
        maxIndex = (uint16_t)(remainSize/COM4_DATA_SIZE_MAX);
        if((remainSize % COM4_DATA_SIZE_MAX) != 0u)
        {
            maxIndex++;
        }
        curIndex = 0u;
        curPosition = 0u;
        locker = 0x55;
    }

    /* 最后一帧 */
    if(remainSize > COM4_DATA_SIZE_MAX)
    {
        cpySize = COM4_DATA_SIZE_MAX;
        ret = 1;
    }
    else /* yes */
    {
        cpySize = remainSize;
        locker = 0;
        ret = 0;
    }

    /* ready the data */
    if(curIndex == 0) /* first frame */
    {
        (void)memcpy((void*)pFrame->data, (const void*)pFileHead, sizeof(fileHead_t));
        (void)memcpy((void*)pFrame->data + sizeof(fileHead_t),
            (const void*)(pFileHead->para + curPosition), (size_t)cpySize - sizeof(fileHead_t));
        curPosition += (cpySize - sizeof(fileHead_t));
    }
    else
    {
        (void)memcpy((void*)pFrame->data, (const void*)(pFileHead->para + curPosition), (size_t)cpySize);
        curPosition += cpySize;
    }
    remainSize -= cpySize;

    /* 没有添加CRC计算，原因为：节约时间且这些数据非安全 */
    pFrame->id = infoCurMpuId().value;
    pFrame->type = (int16_t)pFileHead->type;   /* 文件类型 */
    pFrame->idxMax = maxIndex;                 /* 分片数据帧总数据量 */
    pFrame->idx = curIndex;                    /* 帧序号 */
    pFrame->size = (uint16_t)cpySize;          /* 当前帧长度 */
    pFrame->cmd = 0x1000;

    /* send data */
    (void)mtTxData((void*)pFrame);
    curIndex++;

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-250 (Trace to: SLD-AMC01-250)
* Function:     mtTxHandle
* Description:  向SU服务单元发送维护数据
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void mtTxHandle(void)
{
    static uint8_t cnt = 0U;
    fileHead_t head = {0U};
    uint32_t mtSize = 0;

    if(infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.mtSize == 0U)
    {
        /* 计算维护数据 总长度 */
        mtSize += sizeof(lynxInfo_t);
        mtSize += cfgInputRamSize();
        mtSize += cfgOutputRamSize();
        mtSize += cfgForceRamSize();
        mtSize += cfgParaRamSize();
        infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.mtSize = mtSize;
    }

    while(tmCheckRemainTick() > ((uint32_t)1400U))
    {
        switch(cnt)
        {
            case 0U:  /* platform state information */
                head.magic = lxGetImageHead()->magic;
                head.length = sizeof(lynxInfo_t);
                head.version = LYNX_VERSION;
                head.type = FILE_PF_INFO;
                head.cnt = 0u;
                head.para = (uint32_t)infoGetAddr();
                break;

            case 1U:/* logic input data */
                head.magic = lxGetImageHead()->magic;
                head.length = cfgInputRamSize();
                head.version = lxGetImageHead()->version;
                head.type = FILE_LOGIC_INPUT;
                head.cnt = 0U;
                head.para = (uint32_t)logicGetUsrInAddr();
                break;

            case 2U: /* logic output data */
                head.magic = lxGetImageHead()->magic;
                head.length = cfgOutputRamSize();
                head.version = lxGetImageHead()->version;
                head.type = FILE_LOGIC_OUTPUT;
                head.cnt = 0U;
                head.para = (uint32_t)logicGetUsrOutAddr();
                break;

            case 3U: /* force value */
                head.magic = lxGetForceHead()->magic;
                head.length = cfgForceRamSize();
                head.version = lxGetForceHead()->version;
                head.type = FILE_LOGIC_FORCE;
                head.cnt = 0U;
                head.para = (uint32_t)lxGetForce();
                break;

            /* 参数区上传接口 */
            case 4U:
                head.magic = lxGetParaHead()->magic;
                head.length = cfgParaRamSize();
                head.version = lxGetParaHead()->version;
                head.type = FILE_LOGIC_PARA;
                head.cnt = 0U;
                head.para = (uint32_t)lxGetPara();
                break;

            default:
                /* Do nothing */
                break;
        }

        if(0 == mtTxFile((void*)&head))
        {
            cnt++;
        }

        if(cnt > 4U)
        {
            cnt = 0U;
            break;
        }
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-251 (Trace to: SLD-AMC01-251)
* Function:     mtModeTask
* Description:  维护任务函数
* Input:        none
* Output:       none
* Return:       0
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
int32_t mtModeTask(void)
{
    int32_t ret = 0;

    if(paraLockFile(3) == 0)
    {
        /* get data from com. */
        if(infoGetMpuMS() == MPU_MASTER)
        {
            (void)mtRxFile((void*)g_flieBuf);
        }
    }

    /* save to flash */
    if(paraLockFile(3) == 1)
    {
        ret = paraSave();
        /* save successfully */
        if(ret == 0)
        {
            (void)paraLockFile(0);
        }
    }

    return 0;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-014 (Trace to: SLD-AMC01-014)
* Function:     mtSetSlot
* Description:  设置通信4槽位号
* Input:        slot 槽位号
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
void mtSetSlot(int32_t slot)
{
    s_mtSlot = slot;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-042 (Trace to: SLD-AMC01-042)
* Function:     mtUserModeProcess
* Description:  模式切换时数据处理
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
void mtUserModeProcess(void)
{
    /* 对用户的模式输入进行处理 */
    swUserHandle();

    switch(infoCurSysMode())
    {
        case SYS_MAIT:
            tmResumeTask(PF_MAIT_TASK);
            /* 说明当前为维护模式 */
            break;

        case SYS_NOR:
            /* 当从其它模式进入正常运行模式时，要对强制区进行清除
            * 该清除时间有点长，通信会出现超时警告，但只运行一次 */
            tmSuspendTask(PF_MAIT_TASK);
            break;

        default:
            /* Do nothing */
            break;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-252 (Trace to: SLD-AMC01-252)
* Function:     mtSaveFile
* Description:  解析维护数据同步帧
* Input:        pBuf 数据指针
* Output:
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void mtSaveFile(uint8_t *pBuf)
{
    uint8_t *pDst = g_flieBuf;
    fileHead_t *pHead = (fileHead_t*)g_flieBuf;
    /* 读取数据到文件 */
    com4Frame_t *pFrame = (com4Frame_t *)pBuf;

    LYNX_ASSERT(NULL != pBuf);
    LYNX_ASSERT(NULL != pHead);

    pDst += pFrame->idx * COM4_DATA_SIZE_MAX;
    (void)memcpy((void*)pDst, (const void*)pFrame->data, (size_t)pFrame->size);

    if(pFrame->idxMax == (pFrame->idx +1U))    /* 最后一帧数据 */
    {
        /* 文件完整性校验 */
        if(drv2CrcCal((const void*)(g_flieBuf + sizeof(fileHead_t)), pHead->length) == pHead->crc)
        {
            (void)mtFileProcess((void*)g_flieBuf);
        }
        else
        {
            /* 数据不完整 则丢弃数据 */
        }
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-048 (Trace to: SLD-AMC01-048)
* Function:     mtMSMtDataProcess
* Description:  冗余模式下主控模块模式切换强制数据处理
* Input:        none
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void mtMSMtDataProcess(void)
{
    static int8_t lock = 0;
    uint32_t localTrueMode = infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.curTrueMode;
    uint32_t standbyTrueMode = infoGetAddr()->moduleInfo[1 - g_localSlot].difInfo.mpuInfo.curTrueMode;

    if(infoGetMpuMS() == MPU_SLAVE)
    {
        if((SYS_MAIT == localTrueMode) && (SYS_NOR == standbyTrueMode) && (0 == lock))
        {   /* 当从模块在维护模式，主模块在运行模式时，清除 */
            (void)memset(lxGetForce(), 0, (size_t)lxGetForceHead()->length);
            lock = 1;
        }
        if(SYS_MAIT == standbyTrueMode)
        {
            lock = 0;
        }
    }
    else
    {
        if((SYS_NOR == localTrueMode) && (0 == lock))
        {
            (void)memset(lxGetForce(), 0, (size_t)lxGetForceHead()->length);
            lock = 1;
        }
        if(SYS_MAIT == localTrueMode)
        {
            lock = 0;
        }
    }
}
