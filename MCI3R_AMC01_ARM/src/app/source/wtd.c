/****************************************************************************************************
*FILENAME:     wtd.c
*PURPOSE:      看门狗模块
*DATE          AUTHOR          CHANGE
*2017.08.14    彭华军          创建文件
****************************************************************************************************/
#include "bsp.h"
#include "wtd.h"
#include "tm_timer.h"
#include "error_class.h"

/*函数声明*/
static void watchDogWriteRegister16(uint8_t reg, uint16_t data);
static uint16_t watchDogReadRegister16(uint8_t reg);
static void setWatchDog(uint8_t wdIndex , uint16_t time);

/****************************************************************************************************
* Identifier:   SCOD-AMC01-019 (Trace to: SLD-AMC01-019)
* Function:     wdInit
* Description:  看门狗FPGA初始化，判断是否ready.
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/09        
****************************************************************************************************/
void wdInit(void)
{
    uint16_t ready = 0u;
    uint32_t failCnt = 0u;

    while(1)
    {
        ready = watchDogReadRegister16(WD_STATE_ADDR);
        if(ready == 0xAAAAu)
        {
            break;
        }
        else
        {
            failCnt++;
            if(failCnt > 100u) /* 超过1秒 则认为看门狗FPGA未READY */
            {
                errStopHandleInit(WTD_READY_ERROR); /* 增加报看门狗启动失败错误码 */
            }
        }
        tmDelayms(10u);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-278 (Trace to: SLD-AMC01-278)
* Function:     watchDogWriteRegister16
* Description:  写看门狗16bit寄存器
* Input:        reg: 寄存器地址
*               data: 写寄存器数据
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
static void watchDogWriteRegister16(uint8_t reg, uint16_t data)
{
    spiDAT1_t SpiCfg;
    uint16 outData[2] = {0U};

    outData[0] = WATCHDOGCMD_W | ((uint16_t)reg & 0x1FU);
    outData[1] = data;

    SpiCfg.CSNR = (uint8_t)SPI_CS_0;
    SpiCfg.DFSEL   = SPI_FMT_0;         /* 使用数据格式0 配置 */
    SpiCfg.WDEL    = 0;
    SpiCfg.CS_HOLD = 1;

    (void)spiTransmitData(WATCHDOG_SPI, &SpiCfg, 2U, outData);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-279 (Trace to: SLD-AMC01-279)
* Function:     watchDogReadRegister16
* Description:  读看门狗16bit寄存器
* Input:        reg: 寄存器地址
* Output:       none
* Return:       读取的值
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14    hdq       创建
*               2023/03/30    wxb       watchDogReadRegieter16修改为watchDogReadRegister16
****************************************************************************************************/
static uint16_t watchDogReadRegister16(uint8_t reg)
{
    spiDAT1_t SpiCfg;
    uint16_t data[2] = {0U};
    uint16 outData[2] = {0U};

    outData[0] = WATCHDOGCMD_R | ((uint16_t)reg & 0x1FU);
    outData[1] = 0U;

    SpiCfg.CSNR = (uint8_t)SPI_CS_0;
    SpiCfg.DFSEL   = SPI_FMT_0;  /* 使用数据格式0 配置 */
    SpiCfg.WDEL    = 0;
    SpiCfg.CS_HOLD = 1;

    (void)spiTransmitAndReceiveData(WATCHDOG_SPI,&SpiCfg,2U,outData,data);

    return data[1];
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-280 (Trace to: SLD-AMC01-280)
* Function:     setWatchDog
* Description:  读看门狗16bit寄存器
* Input:        wdIndex 看门狗序号
*               time    喂狗时间
* Output:       none
* Return:       读取的值
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
static void setWatchDog(uint8_t wdIndex , uint16_t time)
{
    switch(wdIndex)
    {
        case WD_1:
            watchDogWriteRegister16((uint8_t)WD_TIMER1_ADDR, time);
            break;

        case WD_2:
            watchDogWriteRegister16((uint8_t)WD_TIMER2_ADDR, time);
            break;

        case WD_3:
            watchDogWriteRegister16((uint8_t)WD_TIMER3_ADDR, time);
            break;

        case WD_4:
            watchDogWriteRegister16((uint8_t)WD_TIMER4_ADDR, time);
            break;

        case WD_5:
            watchDogWriteRegister16((uint8_t)WD_TIMER5_ADDR, time);
            break;

        case WD_6:
            watchDogWriteRegister16((uint8_t)WD_TIMER6_ADDR, time);

            break;

        default:
            /* Do nothing */
            break;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-281 (Trace to: SLD-AMC01-281)
* Function:     wdStart
* Description:  启动看门狗
* Input:        wdIndex: 看门狗序号 
*               tick:    看门狗超时时间
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void wdStart(uint8_t wdIndex, uint16_t tick)
{
    setWatchDog(wdIndex, (uint16_t)(tick * 10U)); /* 设置看门狗时间 */
    wdEnable(wdIndex);/* 使能看门狗 */
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-282 (Trace to: SLD-AMC01-282)
* Function:     wdDisable
* Description:  关闭看门狗
* Input:        wdIndex 看门狗序号
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void wdDisable(uint8_t wdIndex)
{
    uint16_t data = 0U;
    uint32_t tmp = 0U;

    data = watchDogReadRegister16((uint8_t)WD_EN_ADDR);
    tmp = 1UL << (wdIndex - 1U);
    tmp = ~tmp;
    data &= (uint16_t)tmp;
    watchDogWriteRegister16((uint8_t)WD_EN_ADDR, data);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-283 (Trace to: SLD-AMC01-283)
* Function:     wdEnable
* Description:  使能看门狗
* Input:        wdIndex 看门狗序号
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void wdEnable(uint8_t wdIndex)
{
    uint16_t data = 0U;
    uint32_t tmp = 0U;

    data = watchDogReadRegister16((uint8_t)WD_EN_ADDR);
    tmp = 1UL << (wdIndex - 1U);
    data |= (uint16_t)tmp;
    watchDogWriteRegister16((uint8_t)WD_EN_ADDR, data);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-284 (Trace to: SLD-AMC01-284)
* Function:     wdFeed
* Description:  向看门狗发送喂狗信号
* Input:        wdIndex 看门狗序号
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void wdFeed(uint8_t wdIndex)
{
    switch(wdIndex)
    {
        case WD_1:
            gioSetBit(WD_SIG1_PORT, WD_SIG1_PIN,1u);
            tmDelayus(10U);
            gioSetBit(WD_SIG1_PORT, WD_SIG1_PIN,0u);
            break;
        case WD_2:
            gioSetBit(WD_SIG2_PORT, WD_SIG2_PIN,1u);
            tmDelayus(10U);
            gioSetBit(WD_SIG2_PORT, WD_SIG2_PIN,0u);
            break;
        case WD_3:
            gioSetBit(WD_SIG3_PORT, WD_SIG3_PIN,1u);
            tmDelayus(10U);
            gioSetBit(WD_SIG3_PORT, WD_SIG3_PIN,0u);
            break;
        case WD_4:
            gioSetBit(WD_SIG4_PORT, WD_SIG4_PIN,1u);
            tmDelayus(10U);
            gioSetBit(WD_SIG4_PORT, WD_SIG4_PIN,0u);
            break;
        case WD_5:
            gioSetBit(WD_SIG5_PORT, WD_SIG5_PIN,1u);
            tmDelayus(10U);
            gioSetBit(WD_SIG5_PORT, WD_SIG5_PIN,0u);
            break;
        case WD_6:
            gioSetBit(WD_SIG6_PORT, WD_SIG6_PIN,1u);
            tmDelayus(10U);
            gioSetBit(WD_SIG6_PORT, WD_SIG6_PIN,0u);
            break;

        default :
            /* Do nothing */
            break;
    }
}

