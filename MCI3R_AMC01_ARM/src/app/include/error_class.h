/**************************************************************************************************
*Filename:     error_class.h
*Purpose:      故障处理模块，错误码显示
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _ERROR_CLASS_H_
#define _ERROR_CLASS_H_

#include "panel.h"
#include "diag_class.h"

#define   ERR_CODE_DISP_CONT             (12u)      /* 一个 错误码 最少显示 12个周期 */
#define   NONE_ERR                       (0u)       /* 无错误 */
#define   SHOW_ERR                       (1u)       /* 有错误 */

/*  平台信息故障码     后续扩展最多可扩展至263 即平台信息故障码规划64个错误码  */

/* 三类故障 最多32故障码       */
#define CHECK_PF_CFG_FAIL               (200)     /* 保留     200 */
#define LOAD_USER_IMAGE_FAIL            (201)     /* 加载算法文件失败 201 */
#define CHECK_USER_PARA_FAIL            (202)     /* 参数文件校验失败 202 */
#define CHECK_IMAGE_VERSION_FAIL        (203)     /* 保留     203 */
#define SOFT_VERSION_ERROR              (204)     /* 保留     204 */

#define VERSION_COMPARE_ERROR           (205)     /* 冗余模式  主从模块工程文件不一致     205 -----*/
#define SLOT_NUMBER_ERROR               (206)     /* 运行中机箱号槽号站号错误       206 */
#define MODE_INIT_ERROR                 (207)     /* 系统上电时检测到模式开关故障 207 */
#define LOCK_INIT_ERROR                 (208)     /* 保留       208 */
#define ERROR_TOTAL_TICKS               (209)     /* 任务时间分配异常         209 */
#define TASK_CREAT_ERROR                (210)     /* 任务创建失败           210 */   
#define TASK_ORDER_ERROR                (211)     /* 任务执行顺序异常         211 */ 
#define WTD_READY_ERROR                 (212)     /* 看门狗FPGA启动失败 212 */

#define COM_FPGA_EMIF_READ_CRC          (215)     /* 主控FPGA接收EMIF接口异常               215 */
#define COM_ARM_EMIF_READ_CRC           (216)     /* 主控ARM接收EMIF接口异常                216 */
#define POWER_ON_ERROR                  (217)     /* 开机电压异常                217 */  /* 新增显示 */
#define PERIOD_TIME_ERR                 (218)     /* 组态配置周期超出范围     218 */
#define MPUS_ERR                        (219)     /* 主控模块故障                219 */
#define MPUS_COM_SYNC_INIT_ERROR        (220)     /* 上电同步失败                220 */
#define FPGA_INIT_ERROR                 (221)     /* FPGA启动失败              221 */
#define FLASH_ERROR                     (222)     /* 上电FLASH诊断失败         222 */

/* 四类故障 最多32个故障码 */
#define BACK_MPU_ERROR                  (232)     /* 冗余主控不在位              232 */
#define MPUS_COM_RX_FIAL                (234)     /* 冗余主控接收数据失败    */
#define PF_MODE_ERR                     (236)     /* 主从模式错误 */
#define KEY_MODE_ERR                    (237)     /* 模式开关故障                237 */
#define KEY_LOCK_ERR                    (238)     /* 保留 */
#define MODE_NOT_MATCH                  (239)     /* 主从主控运行模式不一致 */

#define TASK0_TIMEOUT_WARN              (248)     /* 接收数据任务运行超时        248*/
#define TASK1_TIMEOUT_WARN              (249)     /* 用户算法任务超时            249*/
#define TASK2_TIMEOUT_WARN              (250)     /* 热备冗余任务超时            250*/
#define TASK3_TIMEOUT_WARN              (251)     /* 发送数据任务运行超时        251*/
#define TASK4_TIMEOUT_WARN              (252)     /* 维护任务运行超时            252*/
#define CYCLE_TIMEOUT_WARN              (253)     /* 周期超时                253*/
#define BOARD_VOL_WARN                  (254)     /* 板内电压异常              254*/
#define SLOT_NUMBER_WARING              (255)     /* 周期运行子站号异常          */
#define FLASH_WARN                      (256)     /* 周期FLASH诊断失败         256 */

#define PF_ERR_MAX                      (264)


/* cpu 状态故障 码  */
/* group1*/
#define ADC2_PARITY_ERR                 (400)      /* MibADC2 - parity                               Group1  1  */
#define EPC_ERR                         (401)      /* EPC - Correctable Error                        Group1  4  */
#define L2FMC_ERR                       (402)      /* L2FMC - correctable error (implicit OTP read). Group1  6  */
#define PLL1_ERR                        (403)      /* PLL1 - slip                                    Group1  10 */
#define LPO_CLK_MONITOR_ERR             (404)      /* LPO Clock Monitor - interrupt                  Group1  11 */

#define SPI1_ECC_ERR                    (405)      /*MibSPI1 - ECC uncorrectable error              Group1  17 */
#define SPI3_ECC_ERR                    (406)      /*MibSPI3 - ECC uncorrectable error              Group1  18 */
#define SPI5_ECC_ERR                    (407)      /*MibSPI5 - ECC uncorrectable error              Group1  24 */
#define L2RAMW_ERR                      (408)      /*L2RAMW - correctable error                     Group1  26 */
#define R5F_SELF_TEST_ERR               (409)      /*Cortex-R5F CPU - self-test                     Group1  27 */

#define DCC1_ERR                        (410)      /*DCC1 - error                                   Group1  30*/
#define BACK_R5F_SELF_TEST_ERR          (411)      /*CCM-R5F - self-test                            Group1  31*/
#define IOMM_MUX_CFG_ERR                (412)      /*IOMM - Mux configuration error                 Group1  37*/
#define POWER_CMP_ERR                   (413)      /*Power domain compare error                     Group1  38*/
#define POWER_SELF_TEST_ERR             (414)      /*Power domain self-test error                   Group1  39*/

#define EFUSE_FARM_EFC_ERR              (415)      /*eFuse farm – EFC error                         Group1  40 */
#define EFUSE_SELF_TEST_ERR             (416)      /*eFuse farm - self-test error                    Group1  41 */
#define PLL2_ERR                        (417)      /*PLL2 - slip                                     Group1  42 */
#define R5F_CACHE_ERR                   (418)      /*Cortex-R5F Core - cache correctable error event Group1  46*/
#define ACP_D_CACHE_ERR                 (419)      /*ACP d-cache invalidate                          Group1  47*/

#define SPI2_ECC_ERR                    (420)      /*MibSPI2 - ECC uncorrectable error               Group1 49 */
#define SPI4_ECC_ERR                    (421)      /*MibSPI4 - ECC uncorrectable error               Group1 50*/
#define CPU_INT_SUBSYS_GLOBAL_ERR       (422)      /*CPU Interconnect Subsystem - Global error          Group1 52 */
#define CPU_INT_SUBSYS_GLOBAL_PAR_ERR   (423)      /*CPU Interconnect Subsystem - Global Parity Error   Group1 53 */
#define NMPU_PS_SCR_S_MPU_ERR           (424)      /*NMPU - PS_SCR_S MPU Error Group1 61 */

#define DCC2_ERR                        (425)      /*DCC2 - error                                           Group1 62 */
#define SPI1_ECC_SINGLE_BIT_ERR         (426)      /*MIBSPI1 - ECC single bit error                         Group1 77 */
#define SPI2_ECC_SINGLE_BIT_ERR         (427)      /*MIBSPI2 - ECC single bit error                         Group1 78 */
#define SPI3_ECC_SINGLE_BIT_ERR         (428)      /*MIBSPI3 - ECC single bit error                         Group1 79 */
#define SPI4_ECC_SINGLE_BIT_ERR         (429)      /*MIBSPI4 - ECC single bit error                         Group1 80*/

#define SPI5_ECC_SINGLE_BIT_ERR         (430)      /*MIBSPI5 - ECC single bit error                         Group1 81 */
#define EMIF_64_BIT_BRIDGE_ECC_ERR      (431)      /*EMIF 64-bit Bridge I/F ECC uncorrectable error         Group1 84 */
#define EMIF_64_BIT_BRI_ECC_SIN_BIT_ERR (432)      /* EMIF 64-bit Bridge I/F ECC single bit error           Group1 85 */
#define L2FMC_REG_SOFT_ERR              (433)      /* L2FMC - Register Soft Error                           Group1 89 */
#define SYS_REG_SOFT_ERR                (434)      /* SYS - Register Soft Error                             Group1 90 */

#define SCM_TIME_OUT_ERR                (435)      /* SCM - Time-out Error                                  Group1 91 */
#define CCM_R5F_OPERATING_STATUS_ERR    (436)      /* CCM-R5F - Operating status                            Group1 92 */
/* GROUP2 */                               
#define CCM_R5F_CPU_COMPARE_ERR         (437)      /* CCM-R5F - CPU compare error                           Group2 2  */
#define CORTEX_R5F_CORE_BUS_ERR         (438)      /* Cortex-R5F Core - All fatal bus error events. [Commonly caused by improperor incomplete ECC values in Flash.] Group2 3*/
#define L2FMC_UNCORRECTABLE_TYPE_B_ERR  (439)      /* L2RAMW - Uncorrectable error type B Group2 7 */

#define L2FMC_PARITY_ERR                (440)      /* L2FMC - parity error Group2 17*/
#define L2FMC_DOUBLE_BIT_ECC_ERR        (441)      /* L2FMC - double bit ECC error-error due to implicit OTP reads Group2 19 */
#define EPC_UNCORRECTABLE_ERR           (442)      /* EPC - Uncorrectable Error Group2 21       */
#define RTI_WWD_NMI                     (443)      /* RTI_WWD_NMI Group2 24                     */
#define CCM_R5F_VIM_COMPARE             (444)      /* CCM-R5F VIM compare error Group2 25       */

#define CPU1_AXIM_BUS_MONTOR_ERR        (445)      /* CPU1 AXIM Bus Monitor failure Group2 26 */
#define CCM_R5F_POWER_MONITOR_ERR       (446)      /* CCM-R5F - Power Domain monitor error Group2 28 */
/* group3*/
#define EFUSE_FARM_ATUOLOAD_ERR         (447)      /* eFuse Farm - autoload error Group3 1 */
#define L2RAMW_DOUBLE_BIT_ECC_ERR       (448)      /* L2RAMW - double bit ECC uncorrectable error Group3 3 */
#define CORTEX_R5F_CORE_FATAL_ERR       (449)      /*Cortex-R5F Core - All fatal events   Group3 9 */

#define CPU_INT_SUBSYS_DIAGNOSTIC_ERR   (450)      /* CPU Interconnect Subsystem - Diagnostic Error Group3 12 */

#define L2FMC_UNCORRECTABLE_ERR         (451)      /*L2FMC - uncorrectable error due to:  1) address parity/internal parity error
                                                                                        2) address tag
                                                                                        3) internal switch time-out     */
#define L2FMC_UNCORRECTABLE_TYPE_A_ERR  (452)      /* L2RAMW - Uncorrectable error Type A Group3 14    */
#define L2FMC_ADDR_CTL_PARITY_ERR       (453)      /*L2RAMW - Address/Control parity error Group3 15   */

extern void errStopHandleCycle(void);
extern int32_t errHandle(void);
extern void errStopHandleInit(int32_t err);
extern void errEmifCrcHandle(int32_t err);

#endif

