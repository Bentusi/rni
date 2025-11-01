/**************************************************************************************************
*Filename:     module_nai01.h
*Purpose:      公用模块nai01方法定义
*Log:          Date          Author    Modified
*              2022/3/18     hdq       create
**************************************************************************************************/
#ifndef _MODULE_NAI01_H_
#define _MODULE_NAI01_H_

#define NAI01_TYPE                      (67U)

#define NAI01_RX_CH_MAX                 (8)
#define NAI01_TX_CH_MAX                 (2)

#define NAI01_RX_FRAME_RSV_SIZE         (70U)
#define NAI01_TX_FRAME_RSV_SIZE         (100U)

/* NAI01模块私有配置 */
typedef struct
{
    uint32_t  gear;      /* 档位选择*/
} nai01PrivCfg_t;

/* PR电流采集模块（NAI01）上行数据帧结构 */
typedef struct
{
    uint32_t info;                         /* PR电流采集模块自诊断状态反馈,4对于工程师站 表示板卡ID */
    binary6B_t channelSwitchRec;           /* 测试通道切换状态反馈 */
    analog6BI_t channelChoiceRec;          /* 测试通道选择反馈 */
    analog6BI_t gearSignalRec;             /* 档位信号反馈 */
    analog6BI_t rsv2;                      /* 保留 */
    analog6BF_t upCurrent_1;               /* 1段电流 */
    analog6BF_t upCurrent_2;               /* 2段电流 */
    analog6BF_t upCurrent_3;               /* 3段电流 */
    analog6BF_t upCurrent_4;               /* 4段电流 */
    uint8_t rsv[NAI01_RX_FRAME_RSV_SIZE];  /* 保留字段 */
    uint8_t rsv1[2];                       /* fpga crc16 使用2B */
    uint16_t comState;                     /* fpga 通信状态使用2B */
    uint64_t crc;                          /* fpga 校验 8B*/
}__attribute__((packed)) nai01RxFrame_t;

/* PR电流采集模块（NAI01）下行数据帧结构 */
typedef struct
{
    uint32_t info;                         /* 4对于工程师站 表示板卡ID */
    binary6B_t channelSwitchEn;            /* 测试通道切换使能 */
    analog6BI_t channelChoice;             /* 测试通道选择 */
    uint8_t rsv[NAI01_TX_FRAME_RSV_SIZE];  /* 保留字段 */
    uint8_t rsv1[2];                       /* fpga crc16 使用2B */
    uint16_t comState;                     /* fpga 通信状态使用2B */
    uint64_t crc;                          /* fpga 校验8B */
}__attribute__((packed)) nai01TxFrame_t;

extern int32_t nai01RxHandle(int32_t slot, int32_t ch, void *pBuf);
extern uint8_t *nai01GetRxChAddr(int32_t slot, int32_t port, int32_t ch);
extern uint8_t *nai01GetTxChAddr(int32_t slot, int32_t port, int32_t ch);

#endif

