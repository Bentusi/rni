/**************************************************************************************************
*Filename:     module_com4.h
*Purpose:      公用模块com4方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _MODULE_COM4_H_
#define _MODULE_COM4_H_

#define COM4_TYPE                           (9U)
#define COM4_DATA_SIZE_MAX                  (996U)
#define COM4_PORT_MAX                       (4)

/********FRAME error detail***************/
typedef enum
{
    FRAME_OK = 0,
/*** frame segment error***/
    FRAME_COM_ERR,
    FRAME_ERR_CNT,
    FRAME_ERR_CMD,
    FRAME_ERR_TYPE,
    FRAME_ERR_ID,   /* 5 */
    FRAME_ERR_IDX,
    FRAME_ERR_MODE,

/*** frame segment error***/
    FILE_OK = 10,
    FILE_CRC_ERR,
    FILE_TYPE_ERR,
    FILE_MAGIC_ERR,
    FILE_SAVE_ERR
}ackInfo_e;

/* com4板卡数据帧定义*/
typedef struct
{
    uint32_t id;                        /* 指示的是源ID，方便工作师站区分源模块 */
    int16_t  type;                      /* 文件类型 */
    uint16_t idxMax;                    /* 分片数据帧总数据量 */
    uint16_t idx;                       /* 帧序号 */
    uint16_t size;                      /* 当前帧长度 */
    int32_t cmd;                        /* 控制命令 */
    uint8_t  data[COM4_DATA_SIZE_MAX];  /* 有效数据 */
    uint8_t rsv1[2];                    /* fpga crc16 使用 2B */
    uint16_t comState;                  /* fpga 通信状态使用 2B */
    uint64_t crc;                       /* fpga 校验 8B */
}__attribute__((packed)) com4Frame_t;

extern int32_t com4Init(int32_t slot);
extern int32_t com4RxHandle(int32_t slot, int32_t port, void *pBuf);
extern int32_t com4TxHandle(int32_t slot, int32_t port, void *pBuf);

#endif

