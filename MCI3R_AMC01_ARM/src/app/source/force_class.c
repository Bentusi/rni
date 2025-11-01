/****************************************************************************************************
*FILENAME:     force_class.c
*PURPOSE:      强制数据驱动接口文件
*DATE          AUTHOR          CHANGE
*2017.08.14    胡德全          创建文件
****************************************************************************************************/
#include "force_class.h"
#include "com_hw.h"
#include "para_class.h"
#include "global.h"
#include "flash.h"
#include "env.h"
#include "algInterface.h"

/* 算法使用的强制缓冲区 */
static uint8_t *s_pForceHead = NULL;

/**************************************************************************************************
* Identifier:   SCOD-AMC01-326 (Trace to: SLD-AMC01-326)
* Function:     forceCopyTo
* Description:  向强制变量区写入数据
* Input:        idx  强制表中的索引号
                pSrc    源地址
                size   长度
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2020/08/28    hdq
**************************************************************************************************/
void forceCopyTo(uint32_t idx, const void *pSrc, uint32_t size)
{
    const uint32_t *pBuf = ifAgGetOneForceAddr(idx);

    if((NULL != pBuf) && (size <= pBuf[1]))
    {
        (void)memcpy((void *)pBuf[0], pSrc, (size_t)pBuf[1]);
    }
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-317 (Trace to: SLD-AMC01-317)
* Function:     lxSetForceAddr
* Description:  设置强制区地址
* Input:        pAddr  强制区地址
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void lxSetForceHead(void *pAddr)
{
    s_pForceHead = (uint8_t*)pAddr;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-318 (Trace to: SLD-AMC01-318)
* Function:     lxGetForceHead
* Description:  获取强制区地址
* Input:        none
* Output:       none
* Return:       强制区地址
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
fileHead_t *lxGetForceHead(void)
{
    return (fileHead_t *)s_pForceHead;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-319 (Trace to: SLD-AMC01-319)
* Function:     lxGetForce
* Description:  获取强制区地址
* Input:        none
* Output:       none
* Return:       强制区地址
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void *lxGetForce(void)
{
    return (void*)s_pForceHead + sizeof(fileHead_t);
}

