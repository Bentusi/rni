/***************************************************************************************************
 * Filename: module_mpu.h
 * Purpose:  主控模块功能定义
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/

#ifndef _MODULE_MPU_H_
#define _MODULE_MPU_H_

#define MPU_TYPE            (1U)

/**************************** Function declaration ****************************/
extern int32_t mpuInit(int32_t slot);
extern int32_t mpuRxHandle(int32_t slot, int32_t port, void *pBuf);
extern int32_t mpuRxHook(int32_t slot, int32_t para);
extern int32_t mpuTxHandle(int32_t slot, int32_t port, void *pBuf);

#endif /* _MODULE_MPU_H_ */

