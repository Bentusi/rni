/**************************************************************************************************
*Filename:     io_class.h
*Purpose:      IO模块处理相关方法
*Log:          Date          Author    Modified
*              2021/08/31    hdq       create
**************************************************************************************************/
#ifndef _IO_CLASS_H_
#define _IO_CLASS_H_

typedef void (*pFunRxFrameHandle_t)(int32_t slot, int32_t copyFlag, void *pDst, const void *pSrc);

extern uint8_t *lxGetInChAddr(int32_t slot, int32_t ch);
extern uint8_t *lxGetOutChAddr(int32_t slot, int32_t ch);
extern int32_t ioComRecvData(int32_t slot, uint32_t size, void *pBuf);
extern void ioComSendData(int32_t slot, uint32_t size, const void *pBuf);
extern void ioRxHandle(int32_t slot, void *pBuf, pFunRxFrameHandle_t pFrameHandle);
extern int32_t ioTxHandle(int32_t slot, int32_t para, void *pBuf);
extern int32_t ioMemInit(int32_t slot);

#endif

