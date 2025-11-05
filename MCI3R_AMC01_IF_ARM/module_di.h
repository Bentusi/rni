/**************************************************************************************************
*Filename:     module_di.h
*Purpose:      公用模块di方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _MODULE_DI_H_
#define _MODULE_DI_H_

#define DI_TYPE                         (3U)

#define DI_CH_MAX                       (32)
#define DI_FRAME_RSV_SIZE               (48)

/* DI私有配置 */
typedef struct
{
    uint32_t jitter[DI_CH_MAX];        /* 消抖时间:us */
} diPrivCfg_t;

/* DI板卡数据帧定义*/
typedef struct
{
    uint32_t info;                    /* 对于工程师站 表示板卡ID 4B */
    binary2B_t data[DI_CH_MAX];       /* 32通道，每通道一个数字量 64B */
    uint8_t rsv[DI_FRAME_RSV_SIZE];   /* 保留字段 48B */
    uint8_t rsv1[2];                  /* fpga crc16 使用 2B */
    uint16_t comState;                /* fpga 通信状态使用 2B */
    uint64_t crc;                     /* fpga 校验 8B */
}__attribute__((packed)) diFrame_t;

extern int32_t diRxHandle(int32_t slot, int32_t para, void *pBuf);
extern uint8_t *diGetRxChAddr(int32_t slot, int32_t para1, int32_t ch);

#endif

