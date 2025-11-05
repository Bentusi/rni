/**************************************************************************************************
*Filename:     module_ao.h
*Purpose:      公用模块ao方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _MODULE_AO_H_
#define _MODULE_AO_H_

#define AO_TYPE             (4U)
#define AO_CH_MAX           (12)

/* AO模块私有配置 */
typedef struct
{
    float32_t maxValue[AO_CH_MAX];      /* 工程量上限值 */
    float32_t minValue[AO_CH_MAX];      /* 工程量下限值 */

    float32_t maxOriValue[AO_CH_MAX];   /* 电气量上限值 */
    float32_t minOriValue[AO_CH_MAX];   /* 电气量下限值 */

    float32_t defValue[AO_CH_MAX];      /* 默认输出值 */

    uint8_t action[AO_CH_MAX];          /* 异常时动作 */
    uint32_t outType;                   /* 各通道输出类型 */
} aoPrivCfg_t;

typedef aiFrame_t  aoFrame_t;

extern int32_t aoRxHandle(int32_t slot, int32_t para, void *pBuf);
extern uint8_t *aoGetTxChAddr(int32_t slot, int32_t port, int32_t ch);

#endif

