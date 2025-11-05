/**************************************************************************************************
*Filename:     bsp.c
*Purpose:      平台基础配置，初始化模块
*Log:          Date          Author    Modified
*              2017/8/14     hdq       create
**************************************************************************************************/
#include "HL_adc.h"
#include "HL_spi.h"
#include "HL_sci.h"
#include "HL_emif.h"
#include "HL_gio.h"
#include "HL_het.h" 
#include "panel.h"
#include "wtd.h"
#include "bsp.h"

/**************************************************************************************************
* Identifier:   SCOD-AMC01-001 (Trace to: SLD-AMC01-001)
* Function:     bspInit
* Description:  ARM硬件环境，外设接口初始化
* Input:        none
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
**************************************************************************************************/
void bspInit(void)
{
    rtiInit();         /* 实时时钟初始化 */
    rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);

    sciInit();         /* 串口初始化 */
    adcInit();         /* 片内 ADC 初始化 */

    hetInit();         /* HET 初始化 */
    emif_ASYNC3Init(); /* EMIF FPGA 初始化 */
    gioInit();         /* GPIO 初始化 */
    spiInit();         /* SPI 初始化 */
    crcInit();
    drv2DisplayInit();     /*初始化结束后进行状态显示*/
    wdInit();
}

