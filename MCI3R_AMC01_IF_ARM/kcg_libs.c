/**************************************************************************************************
*Filename:     kcg_libs.c
*Purpose:      变量读写接口
*Log:          Date          Author    Modified
*              2021/08/31    hdq       create
**************************************************************************************************/
#include "kcg_types.h"
#include "kcg_libs.h"
#include "ag_run.h"

/**************************************************************************************************
* Identifier:   SCOD-AMC01-338 (Trace to: SLD-AMC01-338)
* Function:     lxCpyToIo
* Description:  拷贝算法输出变量到IO
* Input:        slot    模块的槽位号
*               ch      通道号
*               pSrc    真实值指针
*               size    数据大小
*               pForce  强制值指针
*               pFlag   强制标志指针
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void lxCpyToIo(int32_t slot, int32_t ch, void *pSrc, size_t size, void *pForce, uint8_t *pFlag)
{
    uint8_t flag = 0U;
    uint8_t *pDst = g_pMethod->pFunLxGetOutChAddr(slot, ch);

    LYNX_ASSERT(NULL != pDst);
    LYNX_ASSERT(NULL != pSrc);

    if((NULL == pFlag) || (NULL == pForce))
    {
        flag = FORCE_N;
    }
    else
    {
        flag = *pFlag;
    }

    switch(flag)
    {
        case FORCE_N:/* 不强制 */
            break;

        case FORCE_V:/* 强制值 */
            if(2U == size)
            {
                (void)memcpy(pSrc, pForce, 1u);
            }
            else if(8U == size)
            {
                (void)memcpy(pSrc, pForce, 4u);
            }
            else
            {
                /* Do Nothing */
            }
            break;

        case FORCE_Q:/* 强制质量位 */
            if(2U == size)
            {
                (void)memcpy((uint8_t*)pSrc + 1u, (uint8_t *)pForce + 1u, 1u);
            }
            else if(8U == size)
            {
                (void)memcpy((uint8_t*)pSrc + 4u, (uint8_t *)pForce + 4u, 1u);
            }
            else
            {
                /* Do Nothing */
            }
            break;

        case FORCE_A:/* 强制全部 */
            (void)memcpy(pSrc, pForce, size);
            break;

        default: /* 不强制 */
            break;
    }
    (void)memcpy(pDst, pSrc, size);
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-339 (Trace to: SLD-AMC01-339)
* Function:     lxCpyFromIo
* Description:  拷贝IO数据到输入区
* Input:        pDst    目标地址
*               slot    模块的槽位号
*               ch      通道号
*               size    数据大小
*               pForce  强制值指针
*               pFlag   强制标志指针
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void lxCpyFromIo(void *pDst, int32_t slot, int32_t ch, size_t size, void *pForce, uint8_t *pFlag)
{
    bool_t flag = 0U;
    const uint8_t *pSrc =g_pMethod->pFunLxGetInChAddr(slot, ch);

    LYNX_ASSERT(NULL != pDst);
    LYNX_ASSERT(NULL != pSrc);

    if((NULL == pFlag) || (NULL == pForce))
    {
        flag = FORCE_N;
    }
    else
    {
        flag = *pFlag;
    }

    switch(flag)
    {
        case FORCE_N:/* 不强制 */
            (void)memcpy(pDst, pSrc, size);
            break;

        case FORCE_V:/* 强制值 */
            if(2U == size)
            {
                (void)memcpy(pDst, pForce, 1u);
                (void)memcpy((uint8_t*)pDst + 1u, pSrc + 1u, 1u);
            }
            else if(8U == size)
            {
                (void)memcpy(pDst, pForce, 4u);
                (void)memcpy((uint8_t*)pDst + 4u, pSrc + 4u, 1u);
            }
            else
            {
                /* Do Nothing */
            }
            break;
        case FORCE_Q:/* 强制质量位 */
            if(2U == size)
            {
                (void)memcpy(pDst, pSrc, 1u);
                (void)memcpy((uint8_t*)pDst + 1u, (uint8_t *)pForce + 1u, 1u);
            }
            else if(8U == size)
            {
                (void)memcpy(pDst, pSrc, 4u);
                (void)memcpy((uint8_t*)pDst + 4u, (uint8_t *)pForce + 4u, 1u);
            }
            else
            {
                /* Do Nothing */
            }
            break;

        case FORCE_A:/* 强制全部 */
            (void)memcpy(pDst, pForce, size);
            break;

        default: /* do nothing */
            break;
    }
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-340 (Trace to: SLD-AMC01-340)
* Function:     lxCpyOutputToNet
* Description:  拷贝算法输出数据到网络
* Input:        pDst    目的地址
*               pSrc    源地址
*               size    数据大小
*               pForce  强制值的地址
*               pFlag   强制标志地址
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2021/08/29    hdq       create
**************************************************************************************************/
void lxCpyOutputToNet(void *pDst, void *pSrc, size_t size, void *pForce, uint8_t *pFlag)
{
    bool_t flag = 0U;

    LYNX_ASSERT(NULL != pDst);
    LYNX_ASSERT(NULL != pSrc);

    if((NULL == pFlag) || (NULL == pForce))
    {
        flag = FORCE_N;
    }
    else
    {
        flag = *pFlag;
    }

    switch(flag)
    {
        case FORCE_N:/* 不强制 */
            break;

        case FORCE_V:/* 强制值 */
            if(2U == size)
            {
                (void)memcpy(pSrc, pForce, 1u);
            }
            else if(8U == size)
            {
                (void)memcpy(pSrc, pForce, 4u);
            }
            else
            {
                (void)memcpy(pSrc, pForce, size);
            }
            break;

        case FORCE_Q:/* 强制质量位 */
            if(2U == size)
            {
                (void)memcpy((uint8_t*)pSrc + 1u, (uint8_t *)pForce + 1u, 1u);
            }
            else if(8U == size)
            {
                (void)memcpy((uint8_t*)pSrc + 4u, (uint8_t *)pForce + 4u, 1u);
            }
            else
            {
                /* Do Nothing */
            }
            break;

        case FORCE_A:/* 强制全部 */
            (void)memcpy(pSrc, pForce, size);
            break;

        default: /* 不强制 */
            break;
    }
    (void)memcpy(pDst, pSrc, size);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-341 (Trace to: SLD-AMC01-341)
* Function:     lxCpyParaToInput
* Description:  拷贝参数输入数据到输入区
* Input:        pDst    目的地址
*               pSrc    源地址
*               size    数据大小
*               pForce  强制值的地址
*               pFlag   强制标志地址
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2021/08/29    hdq       create
****************************************************************************************************/
void lxCpyParaToInput(void *pDst, void *pSrc, size_t size, void *pForce, uint8_t *pFlag)
{
    bool_t flag = 0U;

    LYNX_ASSERT(NULL != pDst);
    LYNX_ASSERT(NULL != pSrc);

    if((NULL == pFlag) || (NULL == pForce))
    {
        flag = FORCE_N;
    }
    else
    {
        flag = *pFlag;
    }

    switch(flag)
    {
        case FORCE_N:/* 不强制 */
            break;

        case FORCE_V:/* 强制值 */
        case FORCE_A:/* 强制全部 */
            (void)memcpy(pSrc, pForce, size);
            break;

        default: /* do nothing */
            break;
    }
    (void)memcpy(pDst, pSrc, size);
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-342 (Trace to: SLD-AMC01-342)
* Function:     lxCpyFromInfo
* Description:  带强制功能的系统变量读取
* Input:        pDst    目标地址
*               slot    模块的槽位号
*               para    该模块系统变量索引号
*               size    数据大小
*               pForce  强制值指针
*               pFlag   强制标志指针
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2020/09/01    hdq       create
**************************************************************************************************/
void lxCpyFromInfo(void *pDst, int32_t slot, int32_t para, size_t size, const void *pForce, const uint8_t *pFlag)
{
    bool_t flag = 0U;
    int32_t src = g_pMethod->pFunInfoGetSysVar(slot, para);


    LYNX_ASSERT(NULL != pDst);

    if((NULL == pFlag) || (NULL == pForce))
    {
        flag = FORCE_N;
    }
    else
    {
        flag = *pFlag;
    }

    switch(flag)
    {
        case FORCE_N:/* 不强制 */
            (void)memcpy(pDst, &src, size);
            break;

        case FORCE_V:/* 强制值 */
        case FORCE_A:/* 强制全部 */
            (void)memcpy(pDst, pForce, size);
            break;

        case FORCE_Q:/* 强制质量位 */
            break;

        default: /* 不强制 */
            break;
    }
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-343 (Trace to: SLD-AMC01-343)
* Function:     lxCpyNetToInput
* Description:  拷贝网络数据到输入区
* Input:        pDst    目的地址
*               pSrc    源地址
*               size    数据大小
*               pForce  强制值的地址
*               pFlag   强制标志地址
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2021/08/29    hdq       create
**************************************************************************************************/
void lxCpyNetToInput(void *pDst, const void *pSrc, size_t size, void *pForce, uint8_t *pFlag)
{
    uint8_t flag = 0U;

    LYNX_ASSERT(NULL != pDst);
    LYNX_ASSERT(NULL != pSrc);

    if((NULL == pFlag) || (NULL == pForce))
    {
        flag = FORCE_N;
    }
    else
    {
        flag = *pFlag;
    }

    switch(flag)
    {
        case FORCE_N:/* 不强制 */
            (void)memcpy(pDst, pSrc, size);
            break;

        case FORCE_V:/* 强制值 */
            if(2U == size)
            {
                (void)memcpy(pDst, pForce, 1u);
                (void)memcpy((uint8_t*)pDst + 1u, (uint8_t*)pSrc + 1u, 1u);
            }
            else if(8U == size)
            {
                (void)memcpy(pDst, pForce, 4u);
                (void)memcpy((uint8_t*)pDst + 4u, (uint8_t*)pSrc + 4u, 1u);
            }
            else
            {
                (void)memcpy(pDst, pForce, size);
            }
            break;
        case FORCE_Q:/* 强制质量位 */
            if(2U == size)
            {
                (void)memcpy(pDst, pSrc, 1u);
                (void)memcpy((uint8_t*)pDst + 1u, (uint8_t *)pForce + 1u, 1u);
            }
            else if(8U == size)
            {
                (void)memcpy(pDst, pSrc, 4u);
                (void)memcpy((uint8_t*)pDst + 4u, (uint8_t *)pForce + 4u, 1u);
            }
            else
            {
                /* Do Nothing */
            }
            break;
        case FORCE_A:/* 强制全部 */
            (void)memcpy(pDst, pForce, size);
            break;

        default: /* Do Nothing */
            break;
    }
}
