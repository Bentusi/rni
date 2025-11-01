/**************************************************************************************************
*Filename:     cfg_class.c
*Purpose:      板卡配置信息或硬件信息相关的接口函数
*Log:          Date          Author    Modified
*              2021/08/31    hdq       create
**************************************************************************************************/
#include "cfg_class.h"

/* 平台配置定义 */
static uint8_t *s_pDevCfgHead = NULL;

/**************************************************************************************************
* Identifier:   SCOD-AMC01-029 (Trace to: SLD-AMC01-029)
* Function:     cfgCardTypeOnSlot
* Description:  获取对应槽位配置的板卡类型
* Input:        slot  槽位号
* Output:       none
* Return:       value 板卡类型
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14    hdq       create
*               2021/08/29    hdq       修改返回数据类型
**************************************************************************************************/
uint32_t cfgCardTypeOnSlot(int32_t slot)
{
    uint32_t value = 0U;
    lynxCfg_t *pDevCfg = (lynxCfg_t *)(s_pDevCfgHead + sizeof(fileHead_t));
    LYNX_ASSERT(NULL != pDevCfg);

    if(slot < LYNX_SLOT_MAX)
    {
        value = pDevCfg->cardCfg[slot].baseCfg.id.detail.type;
    }

    return value;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-030 (Trace to: SLD-AMC01-030)
* Function:     cfgNetPortRxCfgAddr
* Description:  获取通信卡II或III 接收端口信息
* Input:        slot  槽位号
*               port  端口号
* Output:       none
* Return:       端口信息地址
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
const comPortCfg_t *cfgNetPortRxCfgAddr(int32_t slot, int32_t port)
{
    const comPrivCfg_t *pCfg = (const comPrivCfg_t *)NULL;
    lynxCfg_t *pDevCfg = (lynxCfg_t *)(s_pDevCfgHead + sizeof(fileHead_t));

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(port < COM_PORT_MAX);
    LYNX_ASSERT(NULL != pDevCfg);

    pCfg = &pDevCfg->cardCfg[slot].difCfg.comCardCfg;

    return &pCfg->rxCfg[port];
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-031 (Trace to: SLD-AMC01-031)
* Function:     cfgNetPortTxCfgAddr
* Description:  获取通信卡II或III 发送端口信息
* Input:        slot  槽位号 
*               port  端口号
* Output:       none
* Return:       端口信息地址
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
const comPortCfg_t *cfgNetPortTxCfgAddr(int32_t slot,int32_t port)
{
    const comPrivCfg_t *pCfg = (const comPrivCfg_t *)NULL;
    lynxCfg_t *pDevCfg = (lynxCfg_t *)(s_pDevCfgHead + sizeof(fileHead_t));

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(port < COM_PORT_MAX);
    LYNX_ASSERT(NULL != pDevCfg);

    pCfg = &pDevCfg->cardCfg[slot].difCfg.comCardCfg;

    return &pCfg->txCfg[port];
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-032 (Trace to: SLD-AMC01-032)
* Function:     cfgSlotToId
* Description:  根据槽位号获取板卡ID
* Input:        slot  槽位号
* Output:       none
* Return:       id值
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
uint32_t cfgSlotToId(int32_t slot)
{
    const baseCfg_t *cfg = (baseCfg_t *)NULL;
    uint32_t value = 0u;
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);

    /* slot 转化为机箱 号 槽号 */
    cfg = cfgGetBaseAddr(slot);
    if(NULL != cfg)
    {
        value = cfg->id.value;
    }

    return value;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-033 (Trace to: SLD-AMC01-033)
* Function:     cfgGetCardEnableCh
* Description:  获取板卡通道使能信息
* Input:        slot  槽位号
* Output:       none
* Return:       ret  通道使能信息
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
uint32_t cfgGetCardEnableCh(int32_t slot)
{
    uint32_t ret = 0U;
    lynxCfg_t *pDevCfg = (lynxCfg_t *)(s_pDevCfgHead + sizeof(fileHead_t));
    LYNX_ASSERT(NULL != pDevCfg);

    if(slot < LYNX_SLOT_MAX)
    {
        ret = pDevCfg->cardCfg[slot].baseCfg.chEn;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-034 (Trace to: SLD-AMC01-034)
* Function:     cfgGetBaseAddr
* Description:  获取基础配置信息
* Input:        slot  槽位号
* Output:       none
* Return:       基本配置指针
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
const baseCfg_t *cfgGetBaseAddr(int32_t slot)
{
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);

    lynxCfg_t *pDevCfg = (lynxCfg_t *)(s_pDevCfgHead + sizeof(fileHead_t));
    LYNX_ASSERT(NULL != pDevCfg);
    return &pDevCfg->cardCfg[slot].baseCfg;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-035 (Trace to: SLD-AMC01-035)
* Function:     lxGetCfgHead
* Description:  获取设备配置信息地址
* Input:        noen
* Output:       none
* Return:       设备配置信息头指针
*
* Others:
* Log:          Date          Author    Modified
*               2021/08/31    hdq       create
**************************************************************************************************/
fileHead_t *lxGetCfgHead(void)
{
    return (fileHead_t *)s_pDevCfgHead;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-036 (Trace to: SLD-AMC01-036)
 * Function:     lxSetCfgHead
 * Description:  设置配置信息头指针
 * Input:        pHead: the pointer of the device configure file.
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-12-28    hdq         Create
 *************************************************************************************************/
void lxSetCfgHead(void *pHead)
{
    s_pDevCfgHead = pHead;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-037 (Trace to: SLD-AMC01-037)
* Function:     lxGetCfg
* Description:  获取设备配置地址
* Input:        none
* Output:       none
* Return:       设备配置地址
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
const lynxCfg_t *lxGetCfg(void)
{
    lynxCfg_t *pDevCfg = (lynxCfg_t *)(s_pDevCfgHead + sizeof(fileHead_t));
    return pDevCfg;
}

