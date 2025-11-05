/****************************************************************************************************
*FILENAME:     main.c
*PURPOSE:      主函数
*DATE          AUTHOR          CHANGE
*2017.08.14    胡德全          创建文件
****************************************************************************************************/
#include "lynx_types.h"
#include "bsp.h"
#include "env.h"
#include "tm.h"
#include "cfg_class.h"
#include "maintain.h"
#include "task.h"
#include "wtd.h"
#include "com_hw.h"
#include "diag_class.h"
#include "info_class.h"
#include "standby.h"
#include "switch.h"

/*********************************************************************
* Identifier:   SCOD-AMC01-241 (Trace to: SLD-AMC01-241)
* Function:     main
* Description:  主程序入口
* Input:        none
* Output:       none
* Return:       0
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
***********************************************************************/
int main(void)
{
    uint16_t value = 0U;

    bspInit();                    /* 初始化所有    */

    drv2LcdStrDisp((const int8_t *)"DIAG");
    swUserModeInit();
    com1ReportArmState(ARM_OK);   /* 该语句不能再提前 */
    
    diagHardwareInit();           /* 上电初始化 诊断 */
    tmDelayms(500U);
    drv2LcdStrDisp((const int8_t *)"WAIT");
    envInit();                    /* 用户算法环境初始化 */

    value = cfgMpuPeriod();       /* 转化MS到US */
    if((value < 15U)||(value > 200U))
    {
        errStopHandleInit((uint32_t)PERIOD_TIME_ERR);
    }
    tmInit((uint32_t)value * 1000U);
    createAllTask();

    com1EnableFpga(ENABLA_FPGA);  /* 使能FPGA 对ARM丢失检测 */
    wdStart(WD_1,(uint16_t)(value * 3U)); /* 启动看门狗 */

    tmStartSchedule();/* MPU cycle schedule,never exit! */

    return 0;
}

