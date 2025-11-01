/***************************************************************************************************
 * Filename: project.c
 * Purpose:  源量程机箱中所支持模块定义
 * Date:         Author      Modified 
 * 2021-10-14    hdq         Create  
 * 2024-06-14    WXB         为g_moduleHandle添加注释
***************************************************************************************************/
#include "lynx_types.h"
#include "module_class.h"
#include "io_class.h"
#include "info_class.h"
#include "global.h"
#include "com_hw.h"

#include "project.h"
#include "para_class.h"
#include "cfg_class.h"
#include "logic_class.h"
#include "force_class.h"
#include "net_class.h"

/* 模块功能函数调用指针结构体 */
moduleMethod_t g_moduleHandle[MODULE_TYPE_MAX] =
{
 /*
  *  模块类型    初始化回调函数     接收回调函数    发送回调函数    获取接收通道地址    获取发送通道地址    获取系统变量回调函数     接收勾子回调函数    发送勾子回调函数
  */
    [MPU_TYPE] = {mpuInit, mpuRxHandle, mpuTxHandle, NULL, NULL, infoGetMpuSysVar, mpuRxHook},
    [AI_TYPE] =  {ioMemInit, aiRxHandle, ioTxHandle, aiGetRxChAddr, NULL, infoGetIoSys},
    [DI_TYPE] =  {ioMemInit, diRxHandle, ioTxHandle, diGetRxChAddr, NULL, infoGetIoSys},
    [AO_TYPE] =  {ioMemInit, aoRxHandle, ioTxHandle, NULL, &aoGetTxChAddr, infoGetIoSys},
    [DO_TYPE] =  {ioMemInit, doRxHandle, ioTxHandle, NULL, &doGetTxChAddr, infoGetIoSys},
    [COM3_TYPE] = {com3Init, com3RxHandle, com3TxHandle, NULL, NULL, infoGetComSys},
    [COM4_TYPE] = {com4Init, com4RxHandle, com4TxHandle, NULL, NULL, infoGetComSys},
    [TR_TYPE] = {ioMemInit, trRxHandle, ioTxHandle, trGetRxChAddr, NULL, infoGetIoSys},
    [TC_TYPE] = {ioMemInit, tcRxHandle, ioTxHandle, tcGetRxChAddr, NULL, infoGetIoSys},
    [NPI01_TYPE] = {ioMemInit, npi01RxHandle, ioTxHandle, npi01GetRxChAddr, npi01GetTxChAddr, infoGetIoSys},
    [NHV01_TYPE] = {ioMemInit, nhv01RxHandle, ioTxHandle, nhv01GetRxChAddr, nhv01GetTxChAddr, infoGetIoSys},
    [NAI01_TYPE] =  {ioMemInit, nai01RxHandle, ioTxHandle, nai01GetRxChAddr, nai01GetTxChAddr, infoGetIoSys},
    [NPI02_TYPE] = {ioMemInit, npi02RxHandle, ioTxHandle, npi02GetRxChAddr, npi02GetTxChAddr, infoGetIoSys},
};

