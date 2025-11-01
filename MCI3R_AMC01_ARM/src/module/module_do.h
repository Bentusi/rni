/**************************************************************************************************
*Filename:     module_do.h
*Purpose:      公用模块do方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _MODULE_DO_H_
#define _MODULE_DO_H_

#define DO_TYPE             (5U)
#define DO_CH_MAX           (32)

/* DO 私有配置 */
typedef struct
{
    uint8_t action[DO_CH_MAX];          /* 异常处理方式 */
    uint8_t defValue[DO_CH_MAX];        /* 输出默认 */
}doPrivCfg_t;

/* DO板卡数据帧定义 */
typedef diFrame_t doFrame_t;

extern int32_t doRxHandle(int32_t slot, int32_t para, void *pBuf);
extern uint8_t *doGetTxChAddr(int32_t slot, int32_t port, int32_t ch);

#endif

