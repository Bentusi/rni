/****************************************************************************************************
*FILENAME:     wtd.h
*PURPOSE:      看门狗模块
*DATE          AUTHOR          CHANGE
*2017.08.14    彭华军          创建文件
****************************************************************************************************/
#ifndef _WTD_H_
#define _WTD_H_
#include "lynx_types.h"
#include "HL_spi.h"
#include "HL_reg_gio.h"
#include "HL_reg_het.h"

#define WATCHDOG_SPI           (spiREG1)
#define WATCHDOG_CS            ((uint8_t)SPI_CS_0)


#define WATCHDOGCMD_R              ((uint16_t)0x8000U)   /* spi 读命令*/
#define WATCHDOGCMD_W              ((uint16_t)0x0000U)   /* spi 写命令*/



/* 看门狗序号 */
#define WD_1            (1U)        /* 看门狗1 */
#define WD_2            (2U)        /* 看门狗2 */
#define WD_3            (3U)        /* 看门狗3 */
#define WD_4            (4U)        /* 看门狗4 */
#define WD_5            (5U)        /* 看门狗5 */
#define WD_6            (6U)        /* 看门狗6 */

/* 看门狗寄存器 */
#define WD_STATE_ADDR       (0U)        /* wd 状态寄存器 */
#define WD_EN_ADDR          (3U)        /* wd 使能寄存器地址 */
#define WD_TIMER1_ADDR      (4U)        /* wd1 时间寄存器地址 */
#define WD_TIMER2_ADDR      (5U)        /* wd2 时间寄存器地址 */
#define WD_TIMER3_ADDR      (6U)        /* wd3 时间寄存器地址 */
#define WD_TIMER4_ADDR      (7U)        /* wd4 时间寄存器地址 */
#define WD_TIMER5_ADDR      (8U)        /* wd5 时间寄存器地址 */
#define WD_TIMER6_ADDR      (9U)        /* wd6 时间寄存器地址 */

/* 喂狗信号 引脚分配 */
#define WD_SIG1_PORT    (hetPORT1)
#define WD_SIG1_PIN     (24U)           /* 喂狗信号1       GIOA[0]   */

#define WD_SIG2_PORT    (hetPORT1)
#define WD_SIG2_PIN     (25U)           /* 喂狗信号2       GIOA[1]   */

#define WD_SIG3_PORT    (hetPORT1)
#define WD_SIG3_PIN     (26U)           /* 喂狗信号3       GIOA[2]   */

#define WD_SIG4_PORT    (hetPORT1)
#define WD_SIG4_PIN     (27U)           /* 喂狗信号4       GIOA[3]   */

#define WD_SIG5_PORT    (hetPORT1)
#define WD_SIG5_PIN     (28U)           /* 喂狗信号5       GIOA[4]   */

#define WD_SIG6_PORT    (hetPORT1)
#define WD_SIG6_PIN     (29U)           /* 喂狗信号6       GIOA[5]   */

#define WD_EVT_PORT     (gioPORTA)
#define WD_EVT_PIN      (6U)            /* EVT(预留)       GIOA[6]   */

/* 函数声明 */
extern void wdStart(uint8_t wdIndex, uint16_t tick);
extern void wdDisable(uint8_t wdIndex);
extern void wdEnable(uint8_t wdIndex);
extern void wdFeed(uint8_t wdIndex);
extern void wdInit(void);

#endif /* INC_APP_WTD_H_ */

