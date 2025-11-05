/**************************************************************************************************
*Filename:     module_ai.h
*Purpose:      公用模块ai方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _MODULE_AI_H_
#define _MODULE_AI_H_

#define AI_TYPE                         (2U)

#define AI_CH_MAX                       (12)
#define AI_FRAME_RSV_SIZE               (40)

/* AI模块私有配置 */
typedef struct
{
    float32_t maxValue[AI_CH_MAX];      /* 工程量上限值 */
    float32_t minValue[AI_CH_MAX];      /* 工程量下限值 */

    float32_t maxOriValue[AI_CH_MAX];   /* 电气量上限值 */
    float32_t minOriValue[AI_CH_MAX];   /* 电气量下限值 */

    float32_t maxViewValue[AI_CH_MAX];  /* 监测工程量上限值 */
    float32_t minViewValue[AI_CH_MAX];  /* 监测工程量下限值 */
} aiPrivCfg_t;

/* Ai板卡数据帧定义 */
typedef struct
{
    uint32_t info;                    /* 对于工程师站 表示板卡ID 4B */
    analog6BF_t data[AI_CH_MAX];      /* 12通道，每通道一个模拟量，12x6 = 72B */
    uint8_t rsv[AI_FRAME_RSV_SIZE];   /* 保留字段 40B */
    uint8_t rsv1[2];                  /* fpga crc16 使用 2B */
    uint16_t comState;                /* fpga 通信状态使用 2B */
    uint64_t crc;                     /* fpga 校验 8B */
}__attribute__((packed)) aiFrame_t;

extern int32_t aiRxHandle(int32_t slot, int32_t ch, void *pBuf);
extern uint8_t *aiGetRxChAddr(int32_t slot, int32_t ch, int32_t para2);

#endif

