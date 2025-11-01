/**************************************************************************************************
*Filename:     module_nhv01.h
*Purpose:      公用模块nhv01方法定义
*Log:          Date          Author    Modified
*              2022/1/11     hdq       create
**************************************************************************************************/
#ifndef _MODULE_TR_H_
#define _MODULE_TR_H_

#define TR_TYPE                    (11U)

#define TR_CH_MAX                  (6)
#define TR_FRAME_RSV_SIZE          (76U)

/**************************************************************************************************
 * 所有模块配置信息
 *************************************************************************************************/

/* TR私有配置 */
typedef struct
{
    float32_t maxValue[TR_CH_MAX];     /*工程量上限值*/
    float32_t minValue[TR_CH_MAX];     /*工程量下限值*/

    float32_t maxViewValue[TR_CH_MAX]; /*监测电气量上限值*/
    float32_t minViewValue[TR_CH_MAX]; /*监测电气量下限值*/
}trPrivCfg_t;


/**************************************************************************************************
 * 通信数据帧定义
 *************************************************************************************************/

/* TR板卡数据帧定义 */
typedef struct
{
    uint32_t info;                    /* 对于工程师站 表示板卡ID 4B */
    analog6BF_t data[TR_CH_MAX];      /* 6通道，每通道一个模拟量，6x6 = 36B */
    uint8_t rsv[TR_FRAME_RSV_SIZE];   /* 保留字段 76B */
    uint8_t rsv1[2];                  /* fpga crc16 使用 2B */
    uint16_t comState;                /* fpga 通信状态使用 2B */
    uint64_t crc;                     /* fpga 校验 8B */
}__attribute__((packed)) trFrame_t;


extern int32_t trRxHandle(int32_t slot, int32_t para, void *pBuf);
extern uint8_t *trGetRxChAddr(int32_t slot, int32_t port, int32_t ch);

#endif

