/**************************************************************************************************
*Filename:     project.h
*Purpose:      基础平台与专用平台接口
*Log:          Date          Author    Modified
*              2021/9/18     hdq       create
**************************************************************************************************/
#ifndef _PROJECT_H_
#define _PROJECT_H_

#include "module_mpu.h"
#include "module_ai.h"
#include "module_di.h"
#include "module_ao.h"
#include "module_do.h"
#include "module_com3.h"
#include "module_com4.h"
#include "module_tc.h"
#include "module_tr.h"
#include "module_npi01.h"
#include "module_nhv01.h"
#include "module_nai01.h"
#include "module_npi02.h"

/**************************************************************************************************
 * 所有模块配置信息
 *************************************************************************************************/
typedef union
{
    cpuPrivCfg_t    cpuCardCfg;
    comPrivCfg_t    comCardCfg;
    Com2PrivCfg_t   com2CardCfg;
    aiPrivCfg_t     aiCardCfg;
    diPrivCfg_t     diCardCfg;
    aoPrivCfg_t     aoCardCfg;
    doPrivCfg_t     doCardCfg;
    npi01PrivCfg_t  npi01CardCfg;
    nhv01PrivCfg_t  nhv01CardCfg;
    nai01PrivCfg_t  nai01CardCfg;
    npi02PrivCfg_t  npi02CardCfg;
    tcPrivCfg_t     tcCardCfg;
    trPrivCfg_t     trCardCfg;
    uint8_t         data[SLOT_CFG_MAX_SIZE - 16U];
} privateCfg_t;

/**************************************************************************************************
 * 本工程使用到的模块
 *************************************************************************************************/

/*card information*/
typedef union
{
    mpuPrivInfo_t mpuInfo;
    ioPrivInfo_t  ioInfo;
    uint8_t       data[144];
}privateInfo_t;

#endif

