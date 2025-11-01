/**************************************************************************************************
*Filename:     net_class.h
*Purpose:      为算法提供网络相关功能接口
*Log:          Date          Author    Modified
*Log:          2021/08/31    hdq       create
**************************************************************************************************/
#ifndef _NET_CLASS_H_
#define _NET_CLASS_H_

extern void netCpyFromTxPort(void *dst, int32_t slot, int32_t port, uint32_t offset, uint32_t size);
extern void lxSetNetPortAddr(int32_t slot, int32_t port, void *pAddr, int32_t para);

#endif

