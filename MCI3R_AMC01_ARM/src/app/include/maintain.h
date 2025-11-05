/***************************************************************************************************
 * Filename: maintain.h
 * Purpose:  缁存姢妯″紡鎺ュ彛妯″潡
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/
#ifndef _MAINTAIN_H_
#define _MAINTAIN_H_

#include "lynx_types.h"
#include "bsp.h"

extern int32_t mtModeTask(void);
extern void mtSetSlot(int32_t slot);
extern void mtTxHandle(void);
extern void mtUserModeProcess(void);
extern void mtSaveFile(uint8_t *pBuf);
extern void mtMSMtDataProcess(void);

#endif

