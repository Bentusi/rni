/**************************************************************************************************
*Filename:     bsp.h
*Purpose:      平台基础配置，初始化模块
*Log:          Date          Author    Modified
*              2017/8/14     hdq       create
**************************************************************************************************/
#ifndef _BSP_H_
#define _BSP_H_

#include "HL_adc.h"
#include "HL_esm.h"
#include "HL_dcc.h"
#include "HL_crc.h"
#include "HL_rti.h"
#include "HL_spi.h"
#include "HL_gio.h"
#include "drv2_libs.h"

extern void bspInit(void);

#endif
