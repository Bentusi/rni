/**************************************************************************************************
*Filename:     info_class.c
*Purpose:      平台状态管理模块
*Log:          Date          Author    Modified
*              2021/9/20     hdq       create
**************************************************************************************************/
#include "error_class.h"
#include "task.h"
#include "info_class.h"
#include "tm.h"
#include "cfg_class.h"
#include "HL_esm.h"
#include "switch.h"
#include "monitor.h"
#include "standby.h"
#include "com_hw.h"
#include "module_class.h"
#include "global.h"

static lynxInfo_t s_InfoAll;

static void infoUpdateSlot(int32_t slot);

/****************************************************************************************************
* Identifier:   SCOD-AMC01-041 (Trace to: SLD-AMC01-041)
* Function:     infoSetSysState
* Description:  设置当前真实的模式
* Input:        s  当前模式
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoSetSysState(int32_t s)
{
    if(s < SYS_ERR)
    {
        infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.curTrueMode = (uint32_t)s;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-013 (Trace to: SLD-AMC01-013)
* Function:     infoSetOutputState
* Description:  设置闭锁状态
* Input:        s  当前闭锁开关状态
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void infoSetOutputState(uint8_t s)
{
    if(s < (uint8_t)CUR_STATE_ERROR)
    {
        infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.outputState = s;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-068 (Trace to: SLD-AMC01-068)
* Function:     infoUpdateSlot
* Description:   更新主控模块状态信息
* Input:        slot  槽位号
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
static void infoUpdateSlot(int32_t slot)
{
    LYNX_ASSERT(slot < 2);
    mpuPrivInfo_t *pMpuInfo = &infoGetAddr()->moduleInfo[slot].difInfo.mpuInfo;
    LYNX_ASSERT(NULL != pMpuInfo);
    
    if((pMpuInfo->cpuErr[0] != 0U) ||
            (pMpuInfo->cpuErr[1] != 0U) ||
            (pMpuInfo->stErr[0] != 0U) ||
            (pMpuInfo->stErr[1] != 0U))
    {
        infoSetHw(slot,1U);
    }
    
    if((pMpuInfo->pfErr[0] != 0U) ||
            (pMpuInfo->pfErr[1] != 0U))
    {
        infoSetsoft(slot,1U);
    }

    /* comInfo 已在接收任务中更新 */
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-224 (Trace to: SLD-AMC01-224)
 * Function:     infoClearSlot
 * Description:  设置冗余主控模块指定状态信息
 * Input:        slot  slot number of the module.
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-12-28    hdq         Create
 *************************************************************************************************/
void infoClearSlot(int32_t slot)
{
    moduleInfo_t *pInfo = NULL;
    LYNX_ASSERT(slot < 2);
    pInfo = &infoGetAddr()->moduleInfo[slot];
    pInfo->difInfo.mpuInfo.curMSState = MPU_SLAVE;
    pInfo->difInfo.mpuInfo.curTrueMode = SYS_INIT;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-069 (Trace to: SLD-AMC01-069)
* Function:     infoUpdateMpuStates
* Description:   更新系统状态
* Input:        none
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoUpdateMpuStates(void)
{
    uint32_t temp = 0U;
    int32_t i = 0;
    mpuPrivInfo_t *pMpuInfo = &infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo;
    LYNX_ASSERT(NULL != pMpuInfo);

    /*  计算CPU使用率 */
    temp = tmGetTaskUsedTick(LX_USR_TASK);
    temp = ((temp * 100U) / MPU_USR_LOGIC_MAX_TIME);
    if(temp == 0U )
    {
        temp = 1U;
    }
    pMpuInfo->cpuRate = temp;

    /* Caculate the ram used rate. */
    if(pMpuInfo->ramRate == 0U)
    {
        pMpuInfo->ramRate = glMemUsedRatio();
    }

    temp = tmGetTaskUsedTick(LX_RX_TASK);
    pMpuInfo->recTime = temp;
    
    temp = tmGetTaskTimeoutTick(LX_RX_TASK);
    pMpuInfo->recTimeT = temp;

    temp = tmGetTaskUsedTick(LX_USR_TASK);
    pMpuInfo->logicTime = temp;
    temp = tmGetTaskTimeoutTick(LX_USR_TASK);
    pMpuInfo->logicTimeT = temp;

    temp = tmGetTaskUsedTick(PF_TX_TASK);
    pMpuInfo->txTime = temp;
    temp = tmGetTaskTimeoutTick(PF_TX_TASK);
    pMpuInfo->txTimeT = temp;

    for(i = 0; i < TM_CFG_MAX_TASK_NUMBER; i++)
    {
        temp = tmGetTaskTimeoutTick(i);
        if(temp != 0U)
        {
            infoSetPfError(TASK0_TIMEOUT_WARN + i);
        }
        else
        {
            infoClearPfError(TASK0_TIMEOUT_WARN + i);
        }
    }
    /* 获取周期使用时间 */
    temp = tmGetCurCycleTick();
    pMpuInfo->cycTime = temp;

    /* 获取周期超时时间 */
    temp = tmGetCycleTimeoutTick();
    pMpuInfo->cycTimeT = temp;
    if(temp != 0U)
    {
        infoSetPfError(CYCLE_TIMEOUT_WARN);
    }
    else
    {
        infoClearPfError(CYCLE_TIMEOUT_WARN);
    }

    pMpuInfo->cycleCnt = tmGetCycleCnt();

    /* 更新主控模块状态 */
    infoUpdateSlot(g_localSlot);
    infoUpdateSlot(1 - g_localSlot);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-070 (Trace to: SLD-AMC01-070)
* Function:     infoSetPfError
* Description:  记录平台错误信息
* Input:        err  错误信息
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void infoSetPfError(int32_t err)
{
    uint8_t offset = 0U;
    uint64_t pfState = 0U;

    (void)memcpy((void*)&pfState, (const void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.pfErr, sizeof(uint64_t));

    if((err >= CHECK_PF_CFG_FAIL)&&(err < PF_ERR_MAX))
    {
        err = err - CHECK_PF_CFG_FAIL;
        offset = (uint8_t)(err);
        pfState |= ((uint64_t)1U << offset);
        (void)memcpy((void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.pfErr, (const void*)&pfState, sizeof(uint64_t));
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-071 (Trace to: SLD-AMC01-071)
* Function:     infoClearPfError
* Description:  clear the platform information.
* Input:        err  err code.
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoClearPfError(int32_t err)
{
    uint8_t offset = 0U;

    uint64_t pfState = 0U;

    (void)memcpy((void*)&pfState, (const void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.pfErr, sizeof(uint64_t));

    if((err >= CHECK_PF_CFG_FAIL)&&(err < PF_ERR_MAX))
    {
        err = err - CHECK_PF_CFG_FAIL;
        offset = (uint8_t)(err);
        pfState &= (~((uint64_t)1U << offset));
        (void)memcpy((void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.pfErr, (const void*)&pfState, sizeof(uint64_t));
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-066 (Trace to: SLD-AMC01-066)
* Function:     infoGetPfErrorState
* Description:  根据错误码返回当前状态.
* Input:        err - err code.
* Output:       none
* Return:       ret 0 无故障 1 有故障
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
int32_t infoGetPfErrorState(int32_t err)
{
    int32_t ret = 0;
    uint64_t pfState = 0U;
    uint8_t offset = 0U;
    
    (void)memcpy((void*)&pfState, (const void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.pfErr, sizeof(uint64_t));
    if((err >= CHECK_PF_CFG_FAIL)&&(err < PF_ERR_MAX))
    {
        err = err - CHECK_PF_CFG_FAIL;
        offset = (uint8_t)(err);
        pfState &= 1ULL << offset;
        if(pfState != 0U)
        {
            ret = 1;
        }
    }
    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-072 (Trace to: SLD-AMC01-072)
* Function:     infoSetStError
* Description:  设置ARM自诊断故障信息
* Input:        err  错误信息
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoSetStError(int32_t err)
{
    uint8_t offset = 0U;
    uint64_t stState = 0U;

    (void)memcpy((void*)&stState, (const void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.stErr, sizeof(uint64_t));
    if((err > ERR_NO) && (err < ERR_ST_MAX))
    {
        err = (err - ERR_NO);
        offset = (uint8_t)err;
        stState |= (1ULL << offset);
        (void)memcpy((void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.stErr, (const void*)&stState, sizeof(uint64_t));
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-073 (Trace to: SLD-AMC01-073)
* Function:     infoClearStError
* Description:  清除ARM自诊断故障信息
* Input:        err  故障序号
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoClearStError(int32_t err)
{
    uint8_t offset = 0U;
    uint64_t stState = 0U;

    (void)memcpy((void*)&stState, (const void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.stErr, sizeof(uint64_t));

    if((err > ERR_NO) && (err < ERR_ST_MAX))
    {
        err = (err - ERR_NO);
        offset = (uint8_t)err;
        stState &= ~((uint64_t)1U << offset);
        (void)memcpy((void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.stErr, (const void*)&stState, sizeof(uint64_t));
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-074 (Trace to: SLD-AMC01-074)
* Function:     infoSetHw
* Description:  设置模块硬件状态信息
* Input:        slot  槽位号 value 硬件状态
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoSetHw(int32_t slot, uint32_t value)
{
    if(slot < LYNX_SLOT_MAX)
    {
        infoGetAddr()->moduleInfo[slot].hwInfo = value;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-075 (Trace to: SLD-AMC01-075)
* Function:     infoSetIoCh
* Description:  模块通道状态信息
* Input:        slot  槽位号
*               ch    通道号
*               value 状态
* Output:       none
* Return:       none
* 
* Others:       none
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoSetIoCh(int32_t slot, int32_t ch, uint8_t value)
{
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(ch < IO_CH_MAX);
    infoGetAddr()->moduleInfo[slot].difInfo.ioInfo.chInfo[ch] = value;
    if(value != 0U)
    {
        infoGetAddr()->moduleInfo[slot].chInfo |= ((uint32_t)QUALITY_STATE_INVALID << ch);
    }
    else
    {
        infoGetAddr()->moduleInfo[slot].chInfo &= ~((uint32_t)QUALITY_STATE_INVALID << ch);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-076 (Trace to: SLD-AMC01-076)
* Function:     infoGetIoCh
* Description:  获取模块通道状态信息
* Input:        slot  槽位号
*               ch    通道号
* Output:       none
* Return:       value 模块通道状态信息
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
uint32_t infoGetIoCh(int32_t slot, int32_t ch)
{
    uint32_t value = 0U;

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(ch < IO_CH_MAX);

    value = infoGetAddr()->moduleInfo[slot].difInfo.ioInfo.chInfo[ch];

    return value;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-077 (Trace to: SLD-AMC01-077)
* Function:     infoSetCom
* Description:  更新模块通信状态
* Input:        slot  槽位号
*               value 通信状态
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoSetCom(int32_t slot, uint32_t value)
{
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    infoGetAddr()->moduleInfo[slot].comInfo = value;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-078 (Trace to: SLD-AMC01-078)
* Function:     infoGetCom
* Description:  获取模块通信状态信息
* Input:        slot  槽位号
* Output:       none
* Return:       通信状态
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
uint32_t infoGetCom(int32_t slot)
{
    return infoGetAddr()->moduleInfo[slot].comInfo;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-079 (Trace to: SLD-AMC01-079)
 * Function:     infoSetsoft
 * Description:  设置模块的运行状态
 * Input:        slot  槽位号
 *               value 模块运行状态
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-11-29    hdq         Create
 *************************************************************************************************/
void infoSetsoft(int32_t slot, uint32_t value)
{
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    infoGetAddr()->moduleInfo[slot].softInfo = value;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-080 (Trace to: SLD-AMC01-080)
 * Function:     infoGetsoft
 * Description:  获取模块的运行状态
 * Input:        slot 槽位号
 * Output:       None
 * Return:       模块的运行状态
 * Date:         Author      Modified
 * 2021-11-29    hdq         Create
 *************************************************************************************************/
uint32_t infoGetsoft(int32_t slot)
{
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    return infoGetAddr()->moduleInfo[slot].softInfo;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-046 (Trace to: SLD-AMC01-046)
* Function:     infoSetMpuMS
* Description:  设置主从状态
* Input:        value  当前状态
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoSetMpuMS(uint8_t value)
{
    static uint8_t old = 0x55U; /* ugly interface */
    mpuPrivInfo_t *pMpuInfo = &infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo;
    LYNX_ASSERT(NULL != pMpuInfo);
    
    pMpuInfo->curMSState = value;

    if((old != value) &&
            (old != 0x55U))
    {
        pMpuInfo->switched++;
    }
    old = value;

    com1SetArmMS(value);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-047 (Trace to: SLD-AMC01-047)
* Function:     infoGetMpuMS
* Description:  获取主从状态
* Input:        none
* Output:       none
* Return:       返回当前主从状态
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
uint32_t infoGetMpuMS(void)
{
    return infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.curMSState;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-314 (Trace to: SLD-AMC01-314)
* Function:     infoCurCpuError
* Description:  获取CPU状态
* Input:        none
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void infoCurCpuError(void)
{
    uint64_t state = 0U;
    state = esmGetStatus(0U, 0x3FFFFFFFFFFFFFULL);
    (void)memcpy((void*)infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.cpuErr, (const void*)&state, sizeof(uint64_t));
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-329 (Trace to: SLD-AMC01-329)
* Function:     infoGetSysVar
* Description:  系统变量接口函数，可以设置强制值
* Input:        slot  槽位号
                para  系统变量索引号
* Output:       none
* Return:       sysInfo 系统变量值（与Int兼容）
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
**************************************************************************************************/
int32_t infoGetSysVar(int32_t slot, int32_t para)
{
    int32_t sysInfo = 0;
    int32_t curSlot = slot;
    uint32_t type = cfgCardTypeOnSlot(curSlot);

    LYNX_ASSERT(type < MODULE_TYPE_MAX);
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    if(g_moduleHandle[type].pFunGetSysInfo)
    {
        /* 系统变量工作模式只获取当前主控的工作模式（运行或维护） */
        if(149u == para)
        {
            curSlot = g_localSlot;
        }
        sysInfo = g_moduleHandle[type].pFunGetSysInfo(curSlot, 0, para);
    }

    return sysInfo;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-081 (Trace to: SLD-AMC01-081)
* Function:     infoUpdateCardError
* Description:  更新非主控模块故障信息
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void infoUpdateCardError(void)
{
    int32_t slot = 0;
    uint32_t value = 0U;
    const baseCfg_t *cfg = NULL;
    lynxID_t id;

    moduleInfo_t *pInfo = NULL;
    uint32_t error = 0U;

    for(slot = 2; slot < LYNX_SLOT_MAX; slot++)
    {
        /*slot 转化为机箱 号 槽号*/
        cfg = cfgGetBaseAddr(slot);
        LYNX_ASSERT(NULL != cfg);
        id.value = cfg->id.value;
		
		pInfo = &infoGetAddr()->moduleInfo[slot];
        if(((pInfo->hwInfo != 0U) ||
                (pInfo->softInfo != 0U) ||
                (pInfo->comInfo != 0U) ||
                (pInfo->chInfo != 0U)))
        {
            error = 0U;
            value = id.detail.caseNum + 1U;
            error |= (value << 24U);
            
            value = id.detail.slot + 1U;
            error |= ((value / 10U) << 16U);
            error |= ((value % 10U) << 8U);
            
            if(pInfo->hwInfo != 0U)
            {
                error |= (1UL << 0U);
            }
            if(pInfo->softInfo != 0U)
            {
                error |= (1UL << 1U);
            }
            if(pInfo->comInfo != 0U)
            {
                error |= (1UL << 2U);
            }
            if(pInfo->chInfo != 0U)
            {
                error |= (1UL << 3);
            }        

            break; /* 发现板卡出错 就跳出查找 */
        }
    }
    infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo.error = error;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-038 (Trace to: SLD-AMC01-038)
 * Function:     infoGetAddr
 * Description:  获取全局变量平台信息地址
 * Input:        None
 * Output:       None
 * Return:       全局变量平台信息指针
 * Date:         Author      Modified
 * 2021-11-08    hdq         Create
 *************************************************************************************************/
lynxInfo_t *infoGetAddr(void)
{
	return &s_InfoAll;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-082 (Trace to: SLD-AMC01-082)
* Function:     infoGetPortState
* Description:  获取通信模块端口接收缓存当前数据状态
* Input:        slot 槽位号
*               port 端口号
*               para 0：接收端口；其它：发送端口
* Output:       none
* Return:       数据状态
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
**************************************************************************************************/
portState_t *infoGetPortState(int32_t slot, int32_t port, int32_t para)
{
    portState_t *pRet = NULL;

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(port < COM_PORT_MAX);
    moduleInfo_t *pInfo = &infoGetAddr()->moduleInfo[slot];
    LYNX_ASSERT(NULL != pInfo);
    
    if(RX_PORT == para)
    {
        pRet = &pInfo->portDataInfo.rxPort[port];
    }
    else
    {
        pRet = &pInfo->portDataInfo.txPort[port];
    }

    return pRet;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-104 (Trace to: SLD-AMC01-104)
* Function:     infoModuleIoToSys
* Description:  把模块状态转换成系统变量
* Input:        slot  槽位号
* Output:       none
* Return:       32位系统变量
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
uint32_t infoModuleIoToSys(int32_t slot)
{
    uint32_t hwInfo = 0U;
    uint32_t ret = 0U;

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);

    hwInfo = infoGetAddr()->moduleInfo[slot].hwInfo;
    if(hwInfo != 0U)
    {
        ret = 0x01U;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-111 (Trace to: SLD-AMC01-111)
* Function:     infoModuleComToSysPatch
* Description:  把通信模块运行状态转换成系统变量
* Input:        slot  槽位号
* Output:       none
* Return:       模块系统变量
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
uint32_t infoModuleComToSysPatch(int32_t slot)
{
    uint32_t value = 0U;
    uint32_t ret = 0U;
    uint32_t soft = infoGetsoft(slot);
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);

    /*获取通信卡电源故障和看门狗诊断故障*/
    value = ((soft >> 8U) & 0x01U)
        ||((soft >> 10U) & 0x01U);
    if(value != 0U)
    {
        ret = 0x01U;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-105 (Trace to: SLD-AMC01-105)
* Function:     infoIoChToSys
* Description:  把通道状态转换成系统变量
* Input:        slot  槽位号
* Output:       none
* Return:       32位系统变量
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
static uint32_t infoIoChToSys(int32_t slot)
{
    uint32_t value = 0U;
    int32_t i = 0;

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    for(i = 0; i < 32; i++)
    {
        if(infoGetIoCh(slot,i) != 0U)
        {
            value |= (1UL << i);
        }
    }

    return value;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-106 (Trace to: SLD-AMC01-106)
* Function:     infoHwToSys
* Description:  系统变量获取硬件状态
* Input:        slot  槽位号
* Output:       none
* Return:       32位系统变量
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
uint32_t infoHwToSys(int32_t slot)
{
    uint32_t value = 0U;

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    value = infoGetAddr()->moduleInfo[slot].hwInfo;

    return value;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-107 (Trace to: SLD-AMC01-107)
* Function:     infoSoftToSys
* Description:  系统变量获取运行状态
* Input:        slot  槽位号
* Output:       none
* Return:       32位系统变量
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
uint32_t infoSoftToSys(int32_t slot)
{
    uint32_t value = 0U;

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    value = infoGetAddr()->moduleInfo[slot].softInfo;

    return value;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-108 (Trace to: SLD-AMC01-108)
 * Function:     infoComToSys
 * Description:  功能模块的通信状态转化为系统变量
 * Input:        slot  槽位号
 * Output:       None
 * Return:       模块的系统变量
 * Date:         Author      Modified
 * 2021-11-08    hdq         Create
 *************************************************************************************************/
uint32_t infoComToSys(int32_t slot)
{
    uint32_t ret = 0U;

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);

    ret = infoGetAddr()->moduleInfo[slot].comInfo;

    return ret;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-109 (Trace to: SLD-AMC01-109)
 * Function:     infoGetIoSys
 * Description:  获取指定槽位的模块系统变量
 * Input:        slot  槽位号
 *               para  IO模块保留
 *               index 系统变量的索引号
 * Output:       None
 * Return:       用户期望的系统变量
 * Date:         Author      Modified
 * 2021-11-08    hdq         Create
 *************************************************************************************************/
int32_t infoGetIoSys(int32_t slot, int32_t para, int32_t idx)
{
    uint32_t sysVar = 0U;

    LYNX_ASSERT(0 == para);

    switch((uint32_t)idx)
    {
    case 0U: /* 模块状态  0*/
        sysVar = infoModuleIoToSys(slot);
        break;

    case 1U: /* 通信状态 1*/
        sysVar = infoComToSys(slot);
        break;

    case 2U: /* 通道状态 1*/
        sysVar = infoIoChToSys(slot);
        break;

    case 32U: /* 硬件状态 1*/
        sysVar = infoHwToSys(slot);
        break;

    case 48U: /* 运行状态 1*/
        sysVar = infoSoftToSys(slot);
        break;

    case 96U ... 127U: /* 通道状态详情 */
        sysVar = infoGetIoCh(slot, idx-96);
        break;

    default:
        break;
    }

    return (int32_t)sysVar;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-110 (Trace to: SLD-AMC01-110)
 * Function:     infoGetIoSys
 * Description:  获取通信模块系统变量
 * Input:        slot  槽位号
 *               para  IO模块保留
 *               index 系统变量的索引号
 * Output:       None
 * Return:       用户期望的系统变量
 * Date:         Author      Modified
 * 2021-11-08    hdq         Create
 *************************************************************************************************/
int32_t infoGetComSys(int32_t slot, int32_t para, int32_t idx)
{
    uint32_t sysVar = 0U;

    LYNX_ASSERT(0 == para);

    switch((uint32_t)idx)
    {
    case 0U: /* 模块状态 0*/
        sysVar = infoModuleComToSysPatch(slot);
        break;

    case 1U: /* 通信状态 1*/
        sysVar = infoComToSys(slot);
        break;

    case 48U: /* 运行状态 1*/
        sysVar = infoSoftToSys(slot);
        break;

    default:
        break;
    }

    return (int32_t)sysVar;
}


/****************************************************************************************************
* Identifier:   SCOD-AMC01-330 (Trace to: SLD-AMC01-330)
* Function:     infoGetMpuSysVar
* Description:  获取主控模块系统变量
* Input:        slot  槽位号
*               para  保留
*               idx   系统变量索引
* Output:       none
* Return:       sysVar  系统变量值
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
int32_t infoGetMpuSysVar(int32_t slot, int32_t para, int32_t idx)
{
    uint32_t sysVar = 0U;
    lynxInfo_t *pInfo = infoGetAddr();
    uint32_t *pMpuInfo = pInfo->moduleInfo[slot].difInfo.mpuInfo.cpuErr;

    LYNX_ASSERT(slot < 2);
    LYNX_ASSERT(0 == para);
    LYNX_ASSERT(NULL != pInfo);

    switch((uint32_t)idx)
    {
    case 0U: /* 模块状态0 */
        sysVar = pMpuInfo[5] & 0xFFFF0000U;
        break; 

    case 1U: /* 模块状态1 */
        sysVar = pMpuInfo[5] & 0x0000FFFFU;
        break;

    case 32U ... 35U: /*hardware information*/
        sysVar = pMpuInfo[idx - 32];
        break;

    case 48U ... 49U: /*software information*/
        sysVar = pMpuInfo[idx - 44];
        break;

    case 128U ... 152U: /*other information*/
        sysVar = pMpuInfo[idx - 122];
        break;

    default:
        break;
    }

    return (int32_t)sysVar;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-067 (Trace to: SLD-AMC01-067)
 * Function:     infoIOComPatch
 * Description:  分发IO通信发送状态，将主控模块通信状态反填到IO模块通信状态。
                 主控模块:bit[11:2]主控slot2-slot11点对点通信接收异常
                 IO模块:bit[14]上行数据通信错误
                        bit[13]slot2主控接收IO模块通信错误
                        bit[12]slot1主控接收IO模块通信错误
 * Input:        None
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2022-5-28     hdq         Create
 *************************************************************************************************/
void infoIOComPatch(void)
{
    uint32_t comInfo0 = infoGetCom(0);
    uint32_t comInfo1 = infoGetCom(1);

    uint32_t ioComInfo = 0UL;
    int32_t idx = 0;

    for(idx = 2; idx < LYNX_SLOT_MAX; idx++)
    {
        ioComInfo = infoGetCom(idx);
        ioComInfo &= ~((uint32_t)7U << 12);

        if(comInfo0 & ((uint32_t)1U << idx))
        {/* 设置主控0与IO模块点对点通信状态 */
            ioComInfo |= ((uint32_t)1U << 12);
            if(MPU_MASTER == infoGetAddr()->moduleInfo[0].difInfo.mpuInfo.curMSState)
            {/* 去除从模块状态 */
                ioComInfo |= ((uint32_t)1U << 14);
            }
        }

        if(comInfo1 & ((uint32_t)1U << idx))
        {/* 设置主控1与IO模块点对点通信状态 */
            ioComInfo |= ((uint32_t)1U << 13);
            if(MPU_MASTER == infoGetAddr()->moduleInfo[1].difInfo.mpuInfo.curMSState)
            {/* 去除从模块状态 */
                ioComInfo |= ((uint32_t)1U << 14);
            }
        }
        infoSetCom(idx,ioComInfo);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-381 (Trace to: SLD-AMC01-381)
* Function:     infoGetStation
* Description:  函数实现功能为获取系统站号
* Input:        st 站号
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
void infoGetStation(uint8_t* st)
{
    g_psStation = st;
}
