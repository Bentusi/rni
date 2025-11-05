/**************************************************************************************************
*Filename:     module_class.h
*Purpose:      所有通用模块接收发送处理接口
*Log:          Date          Author    Modified
*              2021/9/18     hdq       create
**************************************************************************************************/
#ifndef _MODULE_CLASS_H_
#define _MODULE_CLASS_H_

#include "lynx_types.h"

#define CUR_STATE_INIT              (0U)
#define CUR_STATE_ENABLE_OUTPUT     (1U)            /* 非闭锁状态 */
#define CUR_STATE_DISABLE_OUTPUT    (2U)            /* 闭锁状态 */
#define CUR_STATE_ERROR             (3U)

#define QUALITY_STATE_AVAILED       (0x00U)         /* 质量位有效 */
#define QUALITY_STATE_INVALID       (0x01U)         /* 质量位无效 */

#define PRE_ACTION                  (0x0)
#define POST_ACTION                 (0x1)

typedef struct
{
    int32_t (*pFunInit)(int32_t slot);
    int32_t (*pFunRx)(int32_t slot, int32_t port, void *pBuf);
    int32_t (*pFunTx)(int32_t slot, int32_t port, void *pBuf);
    uint8_t *(*pFunGetRxChAddr)(int32_t slot, int32_t port, int32_t para2);
    uint8_t *(*pFunGetTxChAddr)(int32_t slot, int32_t port, int32_t para2);
    int32_t (*pFunGetSysInfo)(int32_t slot, int32_t port, int32_t para2);
    int32_t (*pFunRxHook)(int32_t slot, int32_t para);
    int32_t (*pFunTxHook)(int32_t slot, int32_t para);
} moduleMethod_t;

extern moduleMethod_t g_moduleHandle[MODULE_TYPE_MAX];

#endif

