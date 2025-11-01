/**************************************************************************************************
*Filename:     module_npi01.h
*Purpose:      公用模块npi01方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _MODULE_NPI01_H_
#define _MODULE_NPI01_H_

#define NPI01_TYPE                    (COMMON_TYPE_END + 0U)

#define NPI01_RX_CH_MAX               (9)
#define NPI01_TX_CH_MAX               (10)

#define NPI01_RX_FRAME_RSV_SIZE       (58U)  /* PCA脉冲采集模块 */
#define NPI01_TX_FRAME_RSV_SIZE       (52U)
/**************************************************************************************************
 * 所有模块配置信息
 *************************************************************************************************/

/* NPI01私有配置 */
typedef struct
{
    uint32_t  t1;           /* 低计数率更新时间t1 */
    float32_t defValue[1];  /* 高压默认输出值 */
    uint8_t   action[1];    /* 故障安全行为 */
}npi01PrivCfg_t;

/**************************************************************************************************
 * 通信数据帧定义
 *************************************************************************************************/

/* 脉冲采集模块（NPI01）上行数据帧结构 */
typedef struct
{
    uint32_t info;                          /* 脉冲采集模块自诊断状态反馈， 对于工程师站 表示板卡ID 4B */
    binary6B_t hiPreLoseFeedback;           /* 高压切除反馈信号6B */
    binary6B_t channelSwitchRec;            /* 测试通道切换状态反馈6B */
    binary6B_t flatCurveDrawEn;             /* 坪曲线绘制使能6B */
    analog6BI_t lowCountRateUpdate_T1;      /* 低计数率更新时间t16B */
    analog6BF_t countRate;                  /* 脉冲计数率6B */
    analog6BF_t hiPreOutputRec;             /* 高压输出回采6B */
    analog6BF_t thresholdOutputRec;         /* 甄别阀输出回采6B */
    binary6B_t ttlSignalLookRec;            /* TTL信号闭锁反馈 */
    analog6BF_t countRateRec;               /* 计数率滤波参数反馈值6B */
    uint8_t rsv[NPI01_RX_FRAME_RSV_SIZE];   /* 保留字段 58B */
    uint8_t rsv1[2];                        /* fpga crc16 使用  2B */
    uint16_t comState;                      /* fpga 通信状态使用2B */
    uint64_t crc;                           /* fpga 校验 8B */
}__attribute__((packed)) npi01FrameRx_t;

/* 脉冲采集模块（NPI01）下行数据帧结构 */
typedef struct
{
    uint32_t info;                           /* 4对于工程师站 表示板卡ID */
    binary6B_t hiPreLose;                    /* 高压切除信号 */
    binary6B_t hiPreLoseAlarm;               /* 高压失压报警 */
    binary6B_t hiPreOverAlarm;               /* 高压过压报警 */
    binary6B_t thresholdLowAlarm;            /* 甄别阈过低报警 */
    binary6B_t channelSwitchEn;              /* 测试通道切换使能 */
    binary6B_t flatCurveDrawEn;              /* 坪曲线绘制使能 */
    analog6BF_t countRateValue;              /* 计数率滤波参数设定值，保留 */
    analog6BF_t hiPreSetVal;                 /* 高压设定值 */
    analog6BF_t thresholdSetVal;             /* 甄别阈设定值 */
    binary6B_t ttlSignalLook;                /* TTL信号闭锁 */
    uint8_t rsv[NPI01_TX_FRAME_RSV_SIZE];    /* 保留字段 */
    uint8_t rsv1[2];                         /* fpga crc16 使用2B */
    uint16_t comState;                       /* fpga 通信状态使用2B */
    uint64_t crc;                            /* fpga 校验 8B */
}__attribute__((packed)) npi01FrameTx_t;

extern int32_t npi01RxHandle(int32_t slot, int32_t para, void *pBuf);
extern uint8_t *npi01GetRxChAddr(int32_t slot, int32_t port, int32_t ch);
extern uint8_t *npi01GetTxChAddr(int32_t slot, int32_t port, int32_t ch);

#endif

