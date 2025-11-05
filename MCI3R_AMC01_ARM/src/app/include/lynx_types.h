/**************************************************************************************************
*Filename:     lynx_types.h
*Purpose:      全局数据结构
*Log:          Date          Author    Modified
               2018/8/28     hdq       v1.0 create
               2021/9/20     hdq       v2.0 针对专用项目修改
**************************************************************************************************/
#ifndef _LYNX_TYPES_H_
#define _LYNX_TYPES_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

/***************************************/

typedef float float32_t;
typedef double float64_t;

typedef uint8_t bool_t;

#ifndef NULL
    #define NULL ((void *) 0U)
#endif

#ifndef FALSE
    #define FALSE (0)
#endif
#ifndef TRUE
    #define TRUE (1)
#endif

/***************************************/

#define MODULE_TYPE_MAX                 (0xFFU)
/* 平台支持最大的板卡数量 */
#define LYNX_SLOT_MAX                   (16)  /* 单站支持最大模块数量 */
#define CASE_SLOT_MAX                   (16)  /* 单机箱支持最大模块数量 */

/* 单个文件最大内存定义 */
#define FILE_BUF_SIZE_MAX               (1024U * 16U)

/* 平台使用的CRC长度 */
#define LYNX_CRC_SIZE                   (8U)

/* IO板卡数据缓冲区长度 */
#define IO_FRAME_SIZE_MAX               (128U)

#define IO_FRAME_HEAD_SIZE              (4)

/* 通信帧最大数据长度 */
#define COM_SEG_SIZE_MAX                (1024)                        /* COM数据帧每片段数据长度 */
#define COM_SEG_DATA_SIZE_MAX           (COM_SEG_SIZE_MAX - 12)       /* COM数据帧片段数据净荷长度 */

#define COM_PORT_MAX                    (4)
#define COM2_MAX_CH                     (4)
#define IO_CH_MAX                       (32)

/* 文件属性信息的长度 */
#define SLOT_CFG_MAX_SIZE               (400U)

#define GEN_VERSION(v1,v2,v3,v4)    \
    ((((uint32_t)(v1) << 24U)) | \
    (((uint32_t)(v2) << 16U)) | \
    (((uint32_t)(v3) << 8U))  | \
    (((uint32_t)(v4) << 0U)))

/**************** 全局定义区 ****************/
#define RX_PORT           (0x0)
#define TX_PORT           (0x1)

#define F_NORM            (0x0U)  /* 正常 */
#define F_DEFA            (0x1U)  /* 使用默认值 */
#define F_KEEP            (0x2U)  /* 输出保持 */

/* 板卡类型定义 2.1*/
#define NO_TYPE           (0U)
#define COMMON_TYPE_END   (64U)

/*系统的配置方式*/
#define CFG_SINGLE        (0x10UL)  /* 单配置模式 */
#define CFG_REDUNDANCY    (0x20UL)  /* 热备冗余配置模式 */
#define CFG_PARALLEL      (0x30UL)  /* 并行冗余配置模式 */
#define CFG_ERR           (0x40UL)

/* 站类型定义 */
#define INIT_STATION      (0U) /* 初始化 */
#define RPC_STATION       (1U) /* 主控站 */
#define SVD_STATION       (2U) /* SVDU站 */
#define GW_STATION        (3U) /* 网关端 */
#define MAIT_STATION      (4U) /* 维护站/协议转换模块 */
#define RIC_STATION       (5U) /* 堆芯测量站 */

/* 平台板ID定义 */
typedef union
{
    struct
    {
        uint32_t slot               :5;  /* 槽号 4:0 */
        uint32_t caseNum            :3;  /* 机箱号 7:5 */
        uint32_t stationNum         :8;  /* 站号 13:8; 子站号 15:14 */
        uint32_t type               :8;  /* 板卡类型 23:16 */
        uint32_t aOrb               :2;  /* 主控专用，当前为0槽位或者1槽位 25:24 */
        uint32_t stationType        :4;  /* 站类型 29:26 */
        uint32_t                    :2;  /* rsv 31:30 */
    } detail;
    uint32_t value;
} lynxID_t;

#define GEN_ID(s, ss, box, slot, type)    \
        ((((uint32_t)(slot) << 0U))   |     \
        (((uint32_t)(box) << 5U))     |     \
        (((uint32_t)(s) << 8U))       |     \
        (((uint32_t)(ss) << 14U))     |     \
        (((uint32_t)(type) << 16U)))

/**************** 上下行自定义数据类型区 ****************/

/* 数字量结构 0 */
typedef struct
{
    uint8_t     value;      /* 值 */
    uint8_t     quality;    /* 质量位 */
} __attribute__((packed)) binary2B_t;

/* 数字量结构1 */
typedef struct
{
    uint8_t     value;      /* 值 */
    uint8_t     quality;    /* 质量位 */
    uint8_t     rsv[4];    /* 保留 */
} __attribute__((packed)) binary6B_t;

/* 模拟量结构0 */
typedef struct
{
    float32_t   value;      /* 值 */
    uint8_t     quality;    /* 质量位 */
    uint8_t     rsv;        /* 保留 */
} __attribute__((packed)) analog6BF_t;

/* 模拟量结构1 */
typedef struct
{
    int32_t    value;       /* 值 */
    uint8_t    quality;     /* 质量位 */
    uint8_t    rsv;         /* 保留 */
} __attribute__((packed)) analog6BI_t;

/* IO通用数据帧定义，硬件决定 */
typedef struct
{
    uint32_t info;      /* 对于工程师站 表示板卡ID 4B */
    uint8_t rsv[112];   /* 保留字段 128 - 4 - 2 - 2 - 8 */
    uint8_t rsv1[2];    /* fpga crc16 使用 2B */
    uint16_t comState;  /* fpga 通信状态使用 2B */
    uint64_t crc;       /* fpga 校验 8B */
}__attribute__((packed)) ioFrame_t;

/* 通信通用数据帧定义，硬件决定 */
typedef struct
{
    uint8_t data[COM_SEG_DATA_SIZE_MAX];   /* 有效数据 */
    uint8_t rsv1[2];                       /* fpga crc16 使用2B */
    uint16_t comState;                     /* fpga 通信状态使用2B */
    uint64_t crc;                          /* fpga 校验 8B */
}__attribute__((packed)) comFrame_t;

/* 通信端口详细信息 */
typedef struct
{
    void    *pAddr;    /* 端口数据缓存地址 */
    uint32_t length;   /* 端口数据长度 */
}portAttr_t;

typedef struct
{
    portAttr_t rxMsg[COM_PORT_MAX];
    portAttr_t txMsg[COM_PORT_MAX];
}slotPortAttr_t;

/**************************************************************************************************
 * 单个模块配置类
 *************************************************************************************************/
/* 主控私有配置 */
typedef struct
{
    uint32_t   forceRamSize;            /* 强制区使用量，单位B */
    uint32_t   paraRamSize;             /* 参数区使用量，单位B */
    uint32_t   inputRamSize;            /* 算法输入区使用量，单位B */
    uint32_t   outputRamSize;           /* 算法输出区使用量，单位B */
    uint32_t   logicGlobalStart;        /* 算法全局变量的起始相对地址   时间相关变量 */
    uint32_t   logicGlobalSize;         /* 算法全局变量的长度 */
    int8_t     slotType[16];            /* 主机箱槽位所插板卡类型 */
} cpuPrivCfg_t;

/* 通信端口私用定义,端口数据首先放5字节（值+质量位）变量，紧接着放2字节
 * 变量（值+质量位）*/
typedef struct
{
    uint32_t byte4SignalCnt;   /* 4B带质量位变量个数 */
    uint32_t byte1SignalCnt;   /* 1B带质量位变量 个数 */
    uint32_t totalSize;        /* 表示该端口所有字节数据的总和 */
    lynxID_t targetId;         /* 目标端口所属板卡ID */
    lynxID_t backPortId;       /* 冗余端口ID */
    uint8_t  port;             /* 目标端口号 */
    uint8_t  backPort;         /* 冗余端口号 */
    uint8_t  portType;         /* 端口类型 1 单配置端口 2 冗余配置主端口 3 冗余配置从端口 */
    uint8_t  rsv[1];
}comPortCfg_t;

/* 通信模块私有配置 */
typedef struct
{
    comPortCfg_t rxCfg[COM_PORT_MAX];   /* 接收端口 */
    comPortCfg_t txCfg[COM_PORT_MAX];   /* 发送端口 */
}comPrivCfg_t;

/*通信模块II端口私用定义 */
typedef struct
{
    lynxID_t  id;          /* 目的板卡ID */
    uint8_t port;          /* 目的板卡端口 */
    uint8_t rsv[3u];
}com2PortCfg_t;

/* 通信模块II私有配置 */
typedef struct
{
    com2PortCfg_t rxCfg[COM2_MAX_CH]; /* 接收四端口 端口配置 */
    com2PortCfg_t txCfg[COM2_MAX_CH]; /* 发送四端口 端口配置 */
}Com2PrivCfg_t;

/* 通信配置 */
typedef struct
{
    uint16_t mpuPeriod;  /* 主控周期 */
    uint8_t  pfCfg;      /* 配置模式 */
    uint8_t  select;     /* 使能链路 */
} netCfg_t;

/**
 * 通信卡通道使能表示
 *chEn:          BYTE1 |BYTE0
 COM   0000 T3 T2 T1 T0|0000 R3 R2 R1 R0
 */
/* 基础配置 */
typedef struct
{
    lynxID_t    id;       /* 板卡ID */
    uint32_t    version;  /* 板卡固件号 */
    uint32_t    chEn;     /* 使能通道号 */
} baseCfg_t;

/**************************************************************************************************
 * 单个模块平台信息类
 *************************************************************************************************/
typedef struct
{
    uint32_t cpuErr[2];          /* 处理器状态      0 */
    uint32_t stErr[2];           /* 硬件诊断状态 2 */
    uint32_t pfErr[2];           /* 平台状态 4 */
    uint32_t cycleCnt;           /* 已运行周期数 6 */
    uint32_t cycTime;            /* 最近一周期实际使用时间 7 */
    uint32_t cycTimeT;           /* 周期超时时间 8 */
    uint32_t recTime;            /* 接收任务时间 9 */
    uint32_t recTimeT;           /* 接收任务超时时间 10 */
    uint32_t logicTime;          /* 算法使用时间 11 */
    uint32_t logicTimeT;         /* 算法超时时间 12 */
    uint32_t txTime;             /* 发送任务使用时间 13 */
    uint32_t txTimeT;            /* 发送任务超时时间 14 */
    lynxID_t id;                 /* 当前主控ID 15 */
    uint32_t version;            /* 当前主控软件版本 16 */

    /* 针对工程师站回读版本号功能添加的数据结构 */
    uint32_t userImageVer;       /* 当前算法文件版本 17 */
    uint32_t userParaVer;        /* 当前参数文件版本 18 */
    uint32_t pfCfgVer;           /* 当前设备配置版本 19 */

    uint32_t error;              /* 出错槽号  出错详细信息 20 */
    uint32_t mtSize;             /* 维护数据总长度 强制区 参数区 输入区 输出区 平台信息 IO值 21 */

    /*热备专有，防止接口变化*/
    uint32_t switched;           /* 切换计数器 22 */
    uint32_t cpuRate;            /* CPU 使用率    23 */
    uint32_t netRate;            /* 保留     24 */
    uint32_t ramRate;            /* 内存使用率 25 */

    uint32_t pfCfg;              /* 当前平台配置 26 */
    uint32_t curTrueMode;        /* 当前真实模式 27 */
    uint32_t outputState;        /* 当前输出状态 28 */
    uint32_t curMSState;         /* 0xFF: 当前状态为主，0x00：当前状态为从 29 */
    uint32_t fpgaVersion;        /* 当前主控软件版本 30 */
}mpuPrivInfo_t;

typedef struct
{
    uint8_t  chInfo[32]; /* 通道状态 */
}ioPrivInfo_t;

typedef struct
{
    uint8_t full;      /* 0:有数据 ；!0:无数据 */
    int8_t avail;      /* 0:数据有效; 1:数据无效 */
}portState_t;

typedef struct
{
    portState_t rxPort[COM_PORT_MAX]; /* 端口接收到的数据有效性 */
    portState_t txPort[COM_PORT_MAX];
}portDataInfo_t;

/**************************************************************************************************
 * 专用模块 数据定义接口
 *************************************************************************************************/

#include "project.h"

/**************************************************************************************************
 * 所有模块配置信息
 *************************************************************************************************/

/* 单卡配置 */
typedef struct
{
    baseCfg_t       baseCfg; /*基础配置*/
    netCfg_t        netCfg;  /*通信专用配置*/
    privateCfg_t    difCfg;  /*板卡差异性配置*/
} moduleCfg_t;

/* 所有卡组态配置集合 */
typedef struct
{
    uint32_t  magic[2];                   /* magic */
    moduleCfg_t cardCfg[LYNX_SLOT_MAX];    /* 每个槽位一个配置，不包括电源卡 */
}lynxCfg_t;

/**************************************************************************************************
 * 所有模块状态信息
 *************************************************************************************************/
typedef struct
{
    uint32_t hwInfo;              /* 公用状态 硬件状态 */
    uint32_t softInfo;            /* 公用状态 运行状态 */
    uint32_t comInfo;             /* 公用状态 通信状态 */
    uint32_t chInfo;              /* 公用状态 通道状态 */
    portDataInfo_t portDataInfo;  /* 公用状态 端口数据状态 */
    privateInfo_t difInfo;        /* 板卡差异性状态 */
}moduleInfo_t;

/* platform information struct. */
typedef struct
{
    moduleInfo_t moduleInfo[LYNX_SLOT_MAX];  /* 所有板卡的状态 */
}lynxInfo_t;

/**************************************************************************************************
 *
 *************************************************************************************************/
/* 强制相关枚举 */
#define FORCE_N             (0U)    /* 不强制 */
#define FORCE_V             (1U)    /* 只强制值 */
#define FORCE_Q             (2U)    /* 只强制质量位 */
#define FORCE_A             (3U)    /* 强制所有 */

/* 文件类型定义 */
#define FILE_INIT           (0U)    /* 初始值 */
/* 下载数据类型定义 */
#define FILE_PF_CONFIG      (1U)    /* 设备配置文件 */
#define FILE_USER_IMAGE     (2U)    /* 用户算法 */
#define FILE_USER_PARA      (3U)    /* 算法参数 */
#define FILE_MODIFY_PARA    (4U)    /* 参数修改文件 */
#define FILE_MODIFY_FORCE   (5U)    /* 变量强制文件 */
#define FILE_MODIFY_T2      (6U)    /* T2定期实验文件 */

/* 上传数据类型定义 */
#define FILE_PF_INFO        (7U)    /* 平台状态信息 */
#define FILE_LOGIC_INPUT    (8U)    /* 算法输入 */
#define FILE_LOGIC_OUTPUT   (9U)    /* 算法输出 */
#define FILE_LOGIC_FORCE    (10U)   /* 强制数据 */
#define FILE_LOGIC_PARA     (11U)   /* 算法参数区 */
#define FILE_ORIGIN_VALUE   (12U)   /* IO原始数据 */

/* 文件属性数据结构 */
typedef struct
{
    uint32_t magic;   /* magic */
    uint32_t length;  /* 文件长度，不包括头 */
    uint32_t version; /* 文件版本号 */
    uint32_t type;    /* 文件类型 */
    uint32_t cnt;     /* 文件中包含强制/参数修改/T2实验修改变量数目 */
    uint32_t para;
    uint64_t crc;     /* 文件校验 */
}fileHead_t;

/* for debug */
#define LYNX_ASSERT(expr)                   \
    do                                      \
    {                                       \
        if(!(expr))                         \
        {                                   \
            while(1)                        \
            {                               \
                ;                           \
            }                               \
        }                                   \
    }while(0)

#endif

