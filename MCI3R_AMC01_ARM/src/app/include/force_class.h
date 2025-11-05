/****************************************************************************************************
*FILENAME:     force_class.h
*PURPOSE:      强制数据驱动接口文件
*DATE          AUTHOR          CHANGE
*2017.08.14    胡德全          创建文件
****************************************************************************************************/
#ifndef _FORCE_CLASS_H_
#define _FORCE_CLASS_H_

#include "lynx_types.h"

/**********modify parament frame detail***********/
typedef struct
{
    uint32_t index;
    uint16_t valueSize;
}__attribute__((packed)) modifyHead_t;

extern void forceCopyTo(uint32_t index, const void *pSrc, uint32_t size);
extern void lxSetForceHead(void *pAddr);
extern fileHead_t *lxGetForceHead(void);
extern void *lxGetForce(void);
extern fileHead_t *lxGetForceHead(void);
extern const uint32_t *agGetOneForceAddr(uint32_t index);

#endif

