/**************************************************************************************************
*Filename:     net_class.c
*Purpose:      为算法提供网络相关功能接口
*Log:          Date          Author    Modified
*Log:          2021/08/31    hdq       create
**************************************************************************************************/
#include "lynx_types.h"
#include "cfg_class.h"
#include "net_class.h"

/****************************************************************************************************
* Identifier:   SCOD-AMC01-331 (Trace to: SLD-AMC01-331)
* Function:     netCpyFromTxPort
* Description:  从网络数据发送区拷贝数据到发送端口临时缓存区
* Input:        dst     目的地址
*               slot    槽位号
*               port    通道号
*               offset  偏移
*               size:    长度
* Output:       none 
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
void netCpyFromTxPort(void *dst, int32_t slot, int32_t port, uint32_t offset, uint32_t size)
{   
    portAttr_t   *pPortInfo  = NULL;
    uint8_t *baseAddr = NULL;
    uint32_t maxSize = 0u;
    
    LYNX_ASSERT(NULL != dst);
    LYNX_ASSERT(port < COM_PORT_MAX);
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);

    pPortInfo = glGetPortAttr(slot, port, TX_PORT);
    if(NULL != pPortInfo)
    {
        baseAddr = pPortInfo->pAddr;
        maxSize = cfgNetPortTxCfgAddr(slot, port)->totalSize;
    }

    if(NULL != baseAddr)
    {
        if((offset + size) <= maxSize)
        {
            (void)memcpy(dst, (const void*)(baseAddr+offset), (size_t)size);
        }
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-332 (Trace to: SLD-AMC01-332)
* Function:     lxSetNetPortAddr
* Description:  初始化网络发送或接收端口的缓冲区地址
* Input:        slot    槽位号
*               port    端口号，范围为0~3
*               pAddr   缓冲区地址
*               para    1 设置发送缓冲区地址，0 设置接收缓冲地址
* Output:       none 
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2020/08/28    hdq       create        
****************************************************************************************************/
void lxSetNetPortAddr(int32_t slot, int32_t port, void *pAddr, int32_t para)
{
    portAttr_t* pPortAttr = NULL;

    LYNX_ASSERT(slot < CASE_SLOT_MAX);
    LYNX_ASSERT(port < COM_PORT_MAX);

    pPortAttr = glGetPortAttr(slot, port, para);
    LYNX_ASSERT(NULL != pPortAttr);
    pPortAttr->pAddr = pAddr;
}

