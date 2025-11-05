/**************************************************************************************************
*Filename:     logic_class.c
*Purpose:      算法相关接口
*Log:          Date          Author    Modified
*              2021/08/31    hdq       create
**************************************************************************************************/
#include "global.h"
#include "logic_class.h"

/* user logiec variable */
static void *s_pImageHead = NULL;

/* 算法使用的输出缓冲区 */
static void *s_pImageOutput = NULL;
/* 算法使用的输入缓冲区 */
static void *s_pImageInput = NULL;

/**************************************************************************************************
* Identifier:   SCOD-AMC01-320 (Trace to: SLD-AMC01-320)
* Function:     logicGetUsrInAddr
* Description:  获取算法输入区地址
* Input:        none
* Output:       none
* Return:       算法输入区地址
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void *logicGetUsrInAddr(void)
{
    return s_pImageInput;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-321 (Trace to: SLD-AMC01-321)
* Function:     logicGetUsrOutAddr
* Description:  获取算法输出区地址
* Input:        none
* Output:       none
* Return:       算法输出区地址
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void *logicGetUsrOutAddr(void)
{
    return s_pImageOutput;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-322 (Trace to: SLD-AMC01-322)
* Function:     lxSetUsrInAddr
* Description:  设置算法输入区地址
* Input:        pAddr 算法输入区地址
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void lxSetUsrInAddr(void *pAddr)
{
    s_pImageInput = pAddr;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-323 (Trace to: SLD-AMC01-323)
* Function:     lxSetUsrOutAddr
* Description:  设置算法输出区地址
* Input:        pAddr 算法输出区地址
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void lxSetUsrOutAddr(void *pAddr)
{
    s_pImageOutput = pAddr;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-324 (Trace to: SLD-AMC01-324)
 * Function:     lxSetImageHead
 * Description:  设置算法属性信息指针
 * Input:        pAddr pointer of the image file.
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-12-28    hdq         Create
 *************************************************************************************************/
void lxSetImageHead(void *pAddr)
{
    s_pImageHead = pAddr;
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-325 (Trace to: SLD-AMC01-325)
* Function:     lxGetImageHead
* Description:  获取算法属性信息地址
* Input:        none
* Output:       none
* Return:       算法属性信息地址
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
fileHead_t *lxGetImageHead(void)
{
    return (fileHead_t *)s_pImageHead;
}

