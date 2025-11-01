/**************************************************************************************************
*Filename:     module_npi02.h
*Purpose:      公用模块npi02方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _MODULE_NPI02_H_
#define _MODULE_NPI02_H_

#define NPI02_TYPE                    (COMMON_TYPE_END + 1U)

#define NPI02_RX_CH_MAX               (8)
#define NPI02_TX_CH_MAX               (10)

#define NPI02_RX_FRAME_RSV_SIZE       (64U)
#define NPI02_TX_FRAME_RSV_SIZE       (60U)
/**************************************************************************************************
 * 所有模块配置信息
 *************************************************************************************************/

/*  NPI02私有配置  */
typedef struct
{
    uint32_t  t1;           /* 低计数率更新时间t1 */
}npi02PrivCfg_t;

/**************************************************************************************************
 * 通信数据帧定义
 *************************************************************************************************/

/* 宽量程脉冲采集模块（NPI02）上行数据帧结构 */
typedef struct
{
    uint32_t info;                          /* IR电流采集模块II自诊断状态反馈,对于工程师站 表示板卡ID 4B */
    binary6B_t channelSwitchRec;            /* 测试通道切换状态 */
    binary6B_t thresholdFlatCurveDraw;      /* 甄别阈坪曲线绘制状态 */
    analog6BF_t countRate;                  /* 脉冲计数率 */
    analog6BF_t MSVChVariance1;             /* MSV通道均方根电压（1倍） */
    analog6BF_t MSVChVariance10;            /* MSV通道均方根电压（10倍） */
    analog6BF_t thresholdOutputRec;         /* 甄别阈输出回采值 */
    analog6BF_t countRateFilterParaRec;     /* 计数率滤波参数反馈 */
    analog6BI_t lowCountRateUpdate_T1;      /* 低计数率更新时间t1 */
    uint8_t rsv[NPI02_RX_FRAME_RSV_SIZE];   /* 保留字段 70B */
    uint8_t rsv1[2];                        /* fpga crc16 使用  2B */
    uint16_t comState;                      /* fpga 通信状态使用  2B */
    uint64_t crc;                           /* fpga 校验 8B */
}__attribute__((packed)) npi02FrameRx_t;

/* 宽量程脉冲采集模块（NPI02）下行数据帧结构 */
typedef struct
{
    uint32_t info;                           /* 4对于工程师站 表示板卡ID */
    binary6B_t channelSwitchEn;              /* 测试通道切换使能 */
    binary6B_t hiPreflatCurveDrawEn;         /* 甄别阈坪曲线绘制使能 */
    analog6BF_t countRateFilterPara;         /* 计数率滤波参数设定值，保留 */
    analog6BF_t thresholdSetVal;             /* 甄别阈设定值 */
    binary6B_t thresholdLowAlarm;            /* 甄别阈低报警 */
    binary6B_t cntRateMode;                  /* 计数率模式 */
    binary6B_t MSVChVariance10Mode;          /* 10倍MSV模式 */
    binary6B_t MSVChVariance1Mode;           /* 1倍MSV模式 */
    binary6B_t currentMode;                  /* 电流模式 */
    uint8_t rsv[NPI02_TX_FRAME_RSV_SIZE];    /* 保留字段 */
    uint8_t rsv1[2];                         /* fpga crc16 使用  2B */
    uint16_t comState;                       /* fpga 通信状态使用  2B */
    uint64_t crc;                            /* fpga 校验 8B */
}__attribute__((packed)) npi02FrameTx_t;

extern int32_t npi02RxHandle(int32_t slot, int32_t para, void *pBuf);
extern uint8_t *npi02GetRxChAddr(int32_t slot, int32_t port, int32_t ch);
extern uint8_t *npi02GetTxChAddr(int32_t slot, int32_t port, int32_t ch);

#endif

