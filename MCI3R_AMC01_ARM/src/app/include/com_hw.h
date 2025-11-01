/**************************************************************************************************
*Filename:     com_hw.h
*Purpose:      fpga读写接口
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/

#ifndef _COM_HW_H_
#define _COM_HW_H_

#include "lynx_types.h"

#define EMIF_CRC_ERROR          (0x10000) /* 通信状态 是16 位要与其区分开 */

#define  COM_CFG_ADDR           (0)
#define  COM_RX1_ADDR           (1)
#define  COM_TX1_ADDR           (2)

#define FPGA_BASE_ADDR                      (0x68000000U)       /* EMIF异步基地址 cs3 */

#define FPGA_WRITE_REG_ADDR                 (FPGA_BASE_ADDR + 0x004000U)   /* FPGA寄存器地址 */
#define MPU_HADR_INFO_ADDR                  (FPGA_BASE_ADDR + 0x804000U)   /* 数据缓存状态 + 主控模块自身硬件信息 */

#define MAIN_BOX_STATUS_ADDR                (FPGA_BASE_ADDR + 0x800000U)   /* 主控机箱  各槽位数据 状态信息 */

#define SYNC_WRITE_BASE_ADDR                (FPGA_BASE_ADDR + 0x018000U)
#define SYNC_READ_BASE_ADDR                 (FPGA_BASE_ADDR + 0x818000U)

#define ASYNC_EMIF_COM_STEP_ADDR            (0x100000U)         /* 通信卡 （ 2 3 ），四个 port 间的地址步进  */

#define ASYNC_EMIF_READ_MIAN_ADDR           (0x808000U)         /* 主控机箱读数据偏移首地址 */
#define ASYNC_EMIF_READ_EXTEND_ADDR         (0xC08000U)         /* 扩展机箱读数据偏移首地址 */
#define ASYNC_EMIF_READ_STEP                (0x010000U)         /* 读数据偏移地址步进 */

#define ASYNC_EMIF_WRITE_MIAN_ADDR          (0x008000U)         /* 主控机箱写数据偏移首地址 */
#define ASYNC_EMIF_WRITE_EXTEND_ADDR        (0x408000U)         /* 扩展机箱写数据偏移首地址 */
#define ASYNC_EMIF_WRITE_STEP               (0x010000U)         /* 写数据偏移地址步进 */

#define ASYNC_EMIF_CONFIG_MIAN_ADDR         (0x000000U)         /* 主控机箱读写配置偏移首地址 */
#define ASYNC_EMIF_CONFIG_EXTEND_ADDR       (0x400000U)         /* 扩展机箱读写配置数据偏移首地址 */
#define ASYNC_EMIF_CONFIG_STEP              (0x010000U)         /* 写配置偏移地址步进 */

/**************************************************************************************************
 * FPGA写寄存器定义
 *************************************************************************************************/
typedef struct
{
    uint8_t    enable;
    uint8_t    fpgaReset;
    uint8_t    mpuState;    /* ARM  状态      */
    uint8_t    mpuMode;     /* ARM  运行模式 */
    uint8_t    msState;     /* 主从状态 */
    uint8_t    res[11];     /* 保留 */
    uint64_t   crc;
}__attribute__((packed))fpgaWriteReg_t;

/**************************************************************************************************
 * FPGA读寄存器定义
 *************************************************************************************************/
#define ARM_OK                (0x55U)   /* ARM 无停机故障 */
#define ARM_ERROR             (0xAAU)   /* ARM 停机故障 */
#define RESET_FPGA            (0x55U)   /* 复位 FPGA */
#define NONRESET_FPGA         (0xAAU)   /* 不复位 FPGA */
#define ENABLA_FPGA           (0x55U)
#define DISABLE_FPGA          (0xAAU)

typedef struct
{
    uint8_t portState[8];
    uint8_t rsv_8;
    uint8_t slotAndcaseNum;  /* 机箱号 槽号 */

    uint8_t stationNum;
    uint8_t isMaster;
    uint8_t msg1;
    uint32_t netLode; 
    uint8_t msg2;
    uint32_t fpgaVersion;
    uint8_t rsv[42];
    uint64_t crc;
}__attribute__((packed)) fpgaReadReg_t;

typedef struct
{
    uint16_t comInfo[16];
    uint16_t softInfo[16];
    uint64_t crc;
} fpgaModuleState_t;


extern void com1UpdateMpuState(void);
extern int32_t com1ReadData(void *pBuf, uint32_t addr, uint32_t length);
extern void com1WriteData(uint32_t addr, void *pBuf, uint32_t length);
extern void com1ReportArmState(uint8_t state);
extern void com1UpdateCaseState(int32_t box);
extern uint32_t com1GetAddr(int32_t slot, int32_t idx);
extern void com1EnableFpga(uint8_t state);
extern void com1CalConfigData(int32_t slot);
extern void com1CalReadData(int32_t slot);
extern void com1CalWriteData(int32_t slot);
extern void com1SetArmMS(uint8_t state);

#endif /* _COM_HW_H_ */

