/**************************************************************************************************
*Filename:     module_nai01.h
*Purpose:      公用模块nai01方法定义
*Log:          Date          Author    Modified
*              2022/3/18     hdq       create
**************************************************************************************************/
#ifndef _MODULE_TC_H_
#define _MODULE_TC_H_

#define TC_TYPE                      (10U)

#define TC_CH_MAX                    (7)
#define TC_FRAME_RSV_SIZE            (70U)


/* TC模块私有配置 */
typedef struct
{
    uint32_t compensMode;               /*补偿方式 0:就地补偿 1:远端补偿 */

    float32_t maxValue[TC_CH_MAX];      /*工程量上限值*/
    float32_t minValue[TC_CH_MAX];      /*工程量下限值*/

    float32_t maxViewValue[TC_CH_MAX];  /*监测电气量上限值*/
    float32_t minViewValue[TC_CH_MAX];  /*监测电气量下限值*/

} tcPrivCfg_t;


/* Tc板卡数据帧定义 */
typedef struct
{
    uint32_t info;                    /* 对于工程师站 表示板卡ID 4B */
    analog6BF_t data[TC_CH_MAX];      /* 7通道，每通道一个模拟量，7x6 = 42B */
    uint8_t rsv[TC_FRAME_RSV_SIZE];   /* 保留字段 70B */
    uint8_t rsv1[2];                  /* fpga crc16 使用 2B */
    uint16_t comState;                /* fpga 通信状态使用 2B */
    uint64_t crc;                     /* fpga 校验 8B */
}__attribute__((packed)) tcFrame_t;

extern int32_t tcRxHandle(int32_t slot, int32_t ch, void *pBuf);
extern uint8_t *tcGetRxChAddr(int32_t slot, int32_t port, int32_t ch);

#endif

