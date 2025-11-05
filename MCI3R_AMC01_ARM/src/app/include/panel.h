/*********************************************************
 * FILENAME:panel.h
 * PURPOSE: 前面板指示灯驱动
 * DATE             ARTHOR             CHANGE
 *2017-10-9         wch                Created
 *********************************************************/
#ifndef _PANEL_H_
#define _PANEL_H_

#include "lynx_types.h"
#include "HL_sys_common.h"
#include "HL_gio.h"
#include "HL_spi.h"

/* definitions for control word 0 */

#define CTRLW0                      (0x0U)
#define CTRLW0_NORMAL               (0x40U)
#define CTRLW0_PEAKCURBRT_SHIFT     (4U)
#define CTRLW0_PWMBRT_SHIFT         (0U)

/* definitions for control word 1 */
#define CTRLW1                      (0x80U)
#define CTRLW1_DOUT                 (0x1U)

typedef enum 
{
    PanelLedPwr = 0,        /* 电源 指示灯 */
    PanelLedRun,
    PanelLedSync,
    PanelLedMaint,
    PanelLedErr,
    PanelLedMaster,
    PanelLedSlave,
    PanelLedTx,
    PanelLedRx,
    PanelLedCount
} panelled_t;

typedef enum 
{
    GREEN = 0,
    RED
} ledColor_t;

typedef enum 
{
    HCMS_RegDot  = 0U,
    HCMS_RegCtrl = 1U
} hcms_reg_t;

typedef struct 
{
    spiBASE_t *regbase;
    SPIDATAFMT_t df;
    uint8_t csmask;
} hcms_spi_t;

typedef struct 
{
    gioPORT_t *nRST;
    gioPORT_t *RS;
    gioPORT_t *SEL;
    gioPORT_t *OSC;
    gioPORT_t *BL;
} hcms_ioport_t;

typedef struct 
{
    uint32_t nRST;
    uint32_t RS;
    uint32_t SEL;
    uint32_t OSC;
    uint32_t BL;
} hcms_ioidx_t;

typedef struct 
{
    hcms_spi_t spi;
    hcms_ioport_t ioport;
    hcms_ioidx_t ioidx;
    uint8_t peakCurBrt;
    uint8_t pwmBrt;
} hcms_params_t;

typedef enum 
{
    HCMS_INIT_MODE_START = 0U,
    HCMS_INIT_MODE_RUN   = 1U
} hcms_int_mode_t;

extern void drv2HcmsRegWrite(hcms_reg_t reg, const uint8_t* pdata, uint32_t ndata);
extern void drv2HcmsClear(void);
extern int32_t drv2HcmsInit(hcms_int_mode_t mode, const hcms_params_t *params);
extern void drv2PanelRedLedSet(panelled_t led, uint8_t value);
extern void drv2PanelGreenLedSet(panelled_t led, uint8_t value);
extern uint32_t drv2PanelSwitchStateGet(void);
extern void drv2LedTurnOn(panelled_t ledNum, ledColor_t ledColor);
extern void drv2LedTurnOff(panelled_t ledNum, ledColor_t ledColor);
extern void drv2LcdStrDisp(const int8_t *str);
extern void drv2DisplayInit(void);
extern void drv2ModeLedSet(panelled_t ledNum);
extern void drv2LcdInit(hcms_int_mode_t mode);

#endif

