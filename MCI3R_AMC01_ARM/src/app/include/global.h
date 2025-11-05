/**************************************************************************************************
*Filename:     global.h
*Purpose:      与模块无关全局变量定义
*Log:          Date          Author    Modified
*              2021/9/22     hdq       create
**************************************************************************************************/
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "lynx_types.h"

/***************************************************************************************************
 * v1.3.x.x 初始版本
 **************************************************************************************************/

#define LYNX_VERSION    (GEN_VERSION('3','0','0','0'))

/* temp global buffer */
extern uint8_t g_fastRxBuffer[4096];
extern uint8_t g_fastTxBuffer[4096];

extern uint8_t g_flieBuf[FILE_BUF_SIZE_MAX];
extern int32_t g_localSlot;    /* 实际槽位号 */
extern uint8_t* g_psStation;

extern void glInit(void);
extern void *glGetMem(uint32_t length);
extern uint32_t glMemUsedRatio(void);

extern portAttr_t *glGetPortAttr(int32_t slot, int32_t port, int32_t para);

#endif

