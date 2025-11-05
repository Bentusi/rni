/**************************************************************************************************
*Filename:     logic_class.h
*Purpose:      算法相关接口
*Log:          Date          Author    Modified
*              2021/08/31    hdq       create
**************************************************************************************************/
#ifndef _LOGIC_CLASS_H_
#define _LOGIC_CLASS_H_

extern void *logicGetUsrInAddr(void);
extern void *logicGetUsrOutAddr(void);
extern void lxSetImageHead(void *pAddr);
extern fileHead_t *lxGetImageHead(void);
extern void lxSetUsrInAddr(void *pAddr);
extern void lxSetUsrOutAddr(void *pAddr);

#endif

