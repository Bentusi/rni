/**************************************************************************************************
*Filename:     module_nhv01.h
*Purpose:      公用模块nhv01方法定义
*Log:          Date          Author    Modified
*              2022/1/11     hdq       create
**************************************************************************************************/
#ifndef _MODULE_NHV01_H_
#define _MODULE_NHV01_H_

#define NHV01_TYPE                    (COMMON_TYPE_END + 2U)

#define NHV01_RX_CH_MAX               (3)
#define NHV01_TX_CH_MAX               (4)

#define NHV01_RX_FRAME_RSV_SIZE       (94U)
#define NHV01_TX_FRAME_RSV_SIZE       (64U)
/**************************************************************************************************
 * 所有模块配置信息
 *************************************************************************************************/

/* NHV01私有配置 */
typedef struct
{
    float32_t defValue[1];  /* 高压默认输出值 */
    uint8_t   action[1];    /* 故障安全行为 */
    uint8_t   currentMode;  /* 电流量程选择 */
}nhv01PrivCfg_t;

/**************************************************************************************************
 * 通信数据帧定义
 *************************************************************************************************/

/* 高压控制模块（NHV01）下行数据帧结构 */
typedef struct
{
    uint32_t info;                           /* 4对于工程师站 表示板卡ID */
    binary6B_t hiPreflatCurveDrawEn;         /* 高压坪曲线绘制使能 */
    binary6B_t posPreLossAlarm;              /* 正高压失压报警 */
    binary6B_t posPreOverAlarm;              /* 正高压过压报警 */
    analog6BF_t posPreSetVal;                /* 正高压设定值 */
    uint8_t rsv[NHV01_TX_FRAME_RSV_SIZE];    /* 保留字段 */
    uint8_t rsv1[2];                         /* fpga crc16 使用2B */
    uint16_t comState;                       /* fpga 通信状态使用2B */
    uint64_t crc;                            /* fpga 校验 8B */
}__attribute__((packed)) nhv01FrameTx_t;

/* 高压控制模块（NHV01）上行数据帧结构 */
typedef struct
{
    uint32_t info;                            /* 对于工程师站 表示板卡ID 4B*/
    binary6B_t hiPreflatCurveDrawRec;         /* 高压坪曲线绘制状态反馈 */
    analog6BF_t posPreOutputRec;              /* 高压输出回采 */
    analog6BF_t outputCurrent;                /* 电流值 */
    uint8_t rsv[NHV01_RX_FRAME_RSV_SIZE];     /* 保留字段*/
    uint8_t rsv1[2];                          /* fpga crc16 使用2B */
    uint16_t comState;                        /* fpga 通信状态使用2B */
    uint64_t crc;                             /* fpga 校验8B */
}__attribute__((packed)) nhv01FrameRx_t;

extern int32_t nhv01RxHandle(int32_t slot, int32_t para, void *pBuf);
extern uint8_t *nhv01GetRxChAddr(int32_t slot, int32_t port, int32_t ch);
extern uint8_t *nhv01GetTxChAddr(int32_t slot, int32_t port, int32_t ch);

#endif

