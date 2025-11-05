/***************************************************************************************************
 * Filename: para_class.h
 * Purpose:  为算法提供 参数变量 读写/保存接口
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/
#ifndef _PARA_CLASS_H_
#define _PARA_CLASS_H_

#include "lynx_types.h"

/* 函数声明 */
extern void paraMemCpyTo(uint32_t idx, const void *pSrc, uint32_t size);
extern fileHead_t *lxGetParaHead(void);
extern int32_t paraSave(void);
extern void lxSetParaHead(void *pAddr);
extern fileHead_t *lxGetParaHead(void);
extern void *lxGetPara(void);
extern int32_t paraLockFile(int32_t req);

#endif












