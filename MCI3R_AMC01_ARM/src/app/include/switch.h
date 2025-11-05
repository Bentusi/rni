/**************************************************************************************************
 * Filename: switch.h
 * Purpose:  on-board Switch Drvier Header File
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
**************************************************************************************************/
#ifndef _SWITCH_H_
#define _SWITCH_H_

#include "lynx_types.h"

/*******************************************************************************
* MPU Work Mode Definition
*******************************************************************************/
typedef enum
{
    SYS_INIT = 0,
    SYS_NOR,
    SYS_MAIT = 3,
    SYS_ERR
}SysMode_t;  /* 杩愯�屾ā寮� */

extern int32_t swUserStateCheck(void);
extern void swUserHandle(void);
extern void swUserModeInit(void);

#endif /*_SWITCH_H__*/

