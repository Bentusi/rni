/**************************************************************************************************
*Filename:     global.c
*Purpose:      与模块无关全局变量定义
*Log:          Date          Author    Modified
*              2021/9/22     hdq       create
**************************************************************************************************/

#include "global.h"

/* ram使用地址 */
extern uint32_t _end;
static uint32_t s_usrMemEnd = 0U;

/* 发送临时缓冲区与接收临时缓冲全局用 */
uint8_t g_fastRxBuffer[4096] __attribute__((aligned(8)));
uint8_t g_fastTxBuffer[4096] __attribute__((aligned(8)));

/* 强制命令文件临时接收缓存 */
uint8_t g_flieBuf[FILE_BUF_SIZE_MAX] __attribute__((aligned(8)));
int32_t g_localSlot = 0;

uint8_t* g_psStation = NULL;

static slotPortAttr_t s_slotPortMsg[LYNX_SLOT_MAX];

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-009 (Trace to: SLD-AMC01-009)
 * Function:     glGetPortAttr
 * Description:  获取通信端口管理信息
 * Input:        slot 槽位号
 *               port 端口号
 *               para 0:接收端口；否则为发送端口
 * Output:       None
 * Return:       管理信息地址
 * Date:         Author      Modified
 * 2021-11-26    hdq         Create
 *************************************************************************************************/
portAttr_t *glGetPortAttr(int32_t slot, int32_t port, int32_t para)
{
    portAttr_t *pRet = NULL;

    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(port < COM_PORT_MAX);

    if(para == RX_PORT)
    {
        pRet = &s_slotPortMsg[slot].rxMsg[port];
    }
    else
    {
        pRet = &s_slotPortMsg[slot].txMsg[port];
    }

    return pRet;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-010 (Trace to: SLD-AMC01-010)
 * Function:     glGetMem
 * Description:  分配内存
 * Input:        length 长度
 * Output:       None
 * Return:       分配的内存地址
 * Date:         Author      Modified
 * 2021-11-26    hdq         Create
 *************************************************************************************************/
void *glGetMem(uint32_t length)
{
    uint32_t newStart = 0U;

    if(s_usrMemEnd == 0U)
    {
        s_usrMemEnd = (uint32_t)&_end;
    }

    s_usrMemEnd += 3UL;
    s_usrMemEnd &= (~3UL);

    newStart = s_usrMemEnd;
    s_usrMemEnd += length;

    return (void *)newStart;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-012 (Trace to: SLD-AMC01-012)
 * Function:     glInit
 * Description:  清零槽位端口信息
 * Input:        none
 * Output:       None
 * Return:       none
 * Date:         Author      Modified
 * 2021-11-26    hdq         Create
 *************************************************************************************************/
void glInit(void)
{
    (void)memset((void*)s_slotPortMsg, 0, sizeof(slotPortAttr_t) * (uint32_t)LYNX_SLOT_MAX);
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-065 (Trace to: SLD-AMC01-065)
 * Function:     glMemUsedRatio
 * Description:  计算内存使用率
 * Input:        none
 * Output:       none
 * Return:       计算后的内存使用率
 * Date:         Author      Modified
 * 2021-11-26    hdq         Create
 *************************************************************************************************/
uint32_t glMemUsedRatio(void)
{
    uint32_t value = s_usrMemEnd;

    value -= 0x08000000U;
    value *= 100UL;

    return (value/0x80000U);
}

