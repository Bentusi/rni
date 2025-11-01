/****************************************************************************************************
*FILENAME:     standby.h
*PURPOSE:      热备冗余模块
*DATE          AUTHOR          CHANGE
*2017.08.14    胡德全          创建文件
****************************************************************************************************/
#ifndef STANDBY_H
#define STANDBY_H
#include "lynx_types.h"

#ifdef  __cplusplus
extern "C" 
{
#endif

#define SYNC_RETRY_CONT                 (3U)        /* 同步重传次数 */
#define SYNC_FRAME_DATA_MAX             (4034U)     /* 4096 - COM4*12 - CRC8 - HEAD6*/

/* 同步数据 片段 */
#define SYNC_SEG_SIZE                   (1024U)
/* 同步数据 帧片段数 */
#define SYNC_SEG_COUNT                  (4U)
/* 同步数据帧长度 */
#define SYNC_FRAME_SIZE                 (4U * 1024U)

enum DataType
{
    TYPE_CONFIG_FILE    = 1,    /* 组态配置文件 */
    TYPE_USERIMAGE_FILE,        /* 用户算法文件 */
    TYPE_USERPARA_FILE,         /* 参数文件 */
    TYPE_CPU_STATE      = 20,   /* 本CPU状态和时间相关变量 */
    TYPE_MATAIN_DATA            /* 维护数据帧和时间相关变量 */
};

typedef enum
{
    SYNC_CMD_INIT          = 0,

    /* 下装模式请求同步文件头 1 */
    SYNC_DW_REQ_INIT,
    SYNC_DW_REPLY_INIT,

    /* 下装模式同步数据 3 */
    SYNC_DW_REQ_DATA,
    SYNC_DW_REPLY_DATA,

    /* 周期运行同步请求 5 */
    SYNC_REQ_DATA_CYCLE,
    SYNC_REPLY_DATA_CYCLE,

    /* 版本比较请求 7 */
    SYNC_REQ_VERSION,
    SYNC_REPLAY_VERSION,

    /* 请求强制数据 9 */
    SYNC_REQ_FORCE_DATA,
    SYNC_REPLAY_FORCE_DATA,

    /* 请求参数变量 11 */
    SYNC_REQ_PARA_DATA,
    SYNC_REPLAY_PARA_DATA,

    /* 请求时间 相关变量 13 */
    SYNC_REQ_TIME_DATA,
    SYNC_REPLAY_TIME_DATA
}syncPara_e;

typedef struct
{
    uint32_t pfswVer;   /* 平台软件版本与CRC */
    uint64_t pfswCrc;
    uint32_t logicVer;  /* 用户算法版本与CRC */
    uint64_t logicCrc;
    uint32_t cfgVer;    /* 平台配置与CRC */
    uint64_t cfgCrc;
    uint32_t paraVer;   /* 算法参数版本与CRC */
    uint64_t paraCrc;
}__attribute__((packed)) syncVerInfo_t;

typedef struct
{
    int8_t  cmd;           /* 命令 */
    int8_t  type;          /* 数据净荷类型 */
    uint16_t idx;          /* 当前帧序号 */
    uint16_t size;         /* 当前帧有效数据长度 */
}__attribute__((packed)) syncFrameHead_t;

typedef struct
{
    syncFrameHead_t     head;
    uint8_t             data[SYNC_FRAME_DATA_MAX];
    uint64_t            crc64;
}__attribute__((packed)) syncFrame_t;

extern int32_t sbTask(void);
extern void sbTaskInit(void);
extern void sbSlaveRxProcess(void);

extern void errSyncState(void);
extern void sbSyncForceData(const uint8_t *pBuf);
extern void sbSyncLedShow(void);
extern int32_t sbCheckMode(void);

#ifdef  __cplusplus
}
#endif

#endif

