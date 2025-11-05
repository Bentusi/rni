/***************************************************************************************************
 * Filename: para_class.c
 * Purpose:  为算法提供 参数变量 读写/保存接口
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/
#include "global.h"
#include "bsp.h"
#include "flash.h"
#include "com_hw.h"
#include "para_class.h"
#include "algInterface.h"

/* for parameter */
static uint8_t *s_pParaHead = NULL;                      /* 参数区 */
static int32_t s_paraLocker = 0;

/****************************************************************************************************
* Identifier:   SCOD-AMC01-023 (Trace to: SLD-AMC01-023)
* Function:     paraLockFile
* Description:  参数锁定功能
* Input:        req    0 未锁定 1 锁定
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
int32_t paraLockFile(int32_t req)
{
    int32_t ret = -1;
    fileHead_t *pHead = (fileHead_t *)s_pParaHead;
    LYNX_ASSERT(NULL != pHead);
    
    switch((uint32_t)req)
    {
    case 0U:
        s_paraLocker = 0;
        break;
    case 1U:
        s_paraLocker = 1;
        pHead->crc = drv2CrcCal((const void*)(s_pParaHead + sizeof(fileHead_t)), pHead->length);
        break;
    default:
        ret = s_paraLocker;
        break;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-024 (Trace to: SLD-AMC01-024)
* Function:     paraMemCpyTo
* Description:  参数变量区数据修改
* Input:        idx     参数变量在参数表中的下标
                pSrc    源地址
                size    长度 
* Output:       none 
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14    hdq       create
*               2021/08/31    hdq       使用索引表进行参数修改
****************************************************************************************************/
void paraMemCpyTo(uint32_t idx, const void *pSrc, uint32_t size)
{
    const uint32_t *pBuf = ifAgGetOneParaAddr(idx);

    if((NULL != pBuf) && (size <= pBuf[1]))
    {
        (void)memcpy((void *)pBuf[0], pSrc, (size_t)pBuf[1]);
    }
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-025 (Trace to: SLD-AMC01-025)
 * Function:     lxSetParaAddr
 * Description:  设置参数区地址
 * Input:        pAddr         参数区地址
 * Output:       s_pUserPara   参数区地址全局变量
 * Return:       None
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
void lxSetParaHead(void *pAddr)
{
    s_pParaHead = (uint8_t*)pAddr;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-026 (Trace to: SLD-AMC01-026)
 * Function:     lxGetPara
 * Description:  获取参数区头地址
 * Input:        None
 * Output:       None
 * Return:       参数区地址指针
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
fileHead_t *lxGetParaHead(void)
{
    return (fileHead_t *)s_pParaHead;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-027 (Trace to: SLD-AMC01-027)
 * Function:     lxGetPara
 * Description:  获取参数区地址
 * Input:        None
 * Output:       None
 * Return:       参数区地址指针
 * Date:         Author      Modified
 * 2021-11-09    hdq         Create
 *************************************************************************************************/
void *lxGetPara(void)
{
    return (void*)(s_pParaHead + sizeof(fileHead_t));
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-028 (Trace to: SLD-AMC01-028)
* Function:     paraSave
* Description:  保存参数文件
* Input:        none
* Output:       none
* Return:       0 正常 1 失败
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
int32_t paraSave(void)
{
    int32_t ret = 0;

    fileHead_t *pHead = lxGetParaHead();
    LYNX_ASSERT(NULL != pHead);
    
    if(pHead->length != 0) /* 为0 也合法 */
    {
        ret = flashWriteNoblock(USER_PARA_BASE_ADDRESS, (const void *)pHead, pHead->length + sizeof(fileHead_t));
    }

    return ret;
}

