/***************************************************************************************************
 * Filename: monitor.c
 * Purpose:  板卡电源电压诊断处理
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/
#include "monitor.h"
#include "bsp.h"
#include "error_class.h"
#include "info_class.h"

static uint32_t volProcess(const adcValue_t voltage[]);
static void sampleVol(adcValue_t voltage[]);

static const float32 volmin[CH_NUM] = 
{
    VCC_MCU_1V2_MIN,
    VCC_FPGA_1V2_MIN,
    VCC_WD_1V5_MIN,
    VCC_MCU_3V3_MIN,
    VCC_FPGA_3V3_MIN,
    VCC_WD_3V3_MIN,
    VCC_5V_MIN,
    VCC_5VAUX_MIN,
    VCC_24V_MIN,
    VCC_24V1_MIN,
    VCC_24V2_MIN
};

static const float32 volmax[CH_NUM] = 
{
    VCC_MCU_1V2_MAX,
    VCC_FPGA_1V2_MAX,
    VCC_WD_1V5_MAX,
    VCC_MCU_3V3_MAX,
    VCC_FPGA_3V3_MAX,
    VCC_WD_3V3_MAX,
    VCC_5V_MAX,
    VCC_5VAUX_MAX,
    VCC_24V_MAX,
    VCC_24V1_MAX,
    VCC_24V2_MAX
};

/****************************************************************************************************
* Identifier:   SCOD-AMC01-084 (Trace to: SLD-AMC01-084)
* Function:     volProcess
* Description:  处理判断各路电压是否正常
* Input:        voltage 采集数组
* Output:       none
* Return:       ret 故障代码
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
static uint32_t volProcess(const adcValue_t voltage[])
{
    uint32_t ret = 0U;
    uint32_t i = 0U;
    
    for(i = 0U ; i< CH_NUM; i++)
    {
        LYNX_ASSERT(voltage[i].chId < MAX_ADC_CH);
        if((voltage[voltage[i].chId].value > volmax[voltage[i].chId]) ||
           (voltage[voltage[i].chId].value < volmin[voltage[i].chId]))
        {
            ret |= ((uint32_t)1U << voltage[i].chId) ;
        }
    }

    /* VCC_5VAUX_CHID 通道不使用 */
    ret &= ~((uint32_t)1U << VCC_5VAUX_CHID);

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-085 (Trace to: SLD-AMC01-085)
* Function:     volDiganostic
* Description:  各路电源电压诊断
* Input:        flag 检测标志 0 周期调用（故障警告） 1 上电调用（故障停机）
* Output:       none
* Return:       
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void volDiganostic(uint8_t flag)
{
    uint32_t ret = 0U;
    uint32_t  mask = ((uint32_t)1U << VCC_24V1_CHID) | ((uint32_t)1U << VCC_24V2_CHID);
    adcValue_t volValue[MAX_ADC_CH] = {{0U, 0.0f}};

    /* 采集电压值 */
    sampleVol(volValue);
    
    /* 比较 各诊断电压 是否在正常范围内 */
    ret = volProcess(volValue);

    if(ret != 0u)
    {
        if(((ret & mask) == mask)&&(flag == 1U)) /* 开机时两路24V电压 都异常 */
        {
            errStopHandleInit(POWER_ON_ERROR);
        }
        else
        {
            infoSetPfError(BOARD_VOL_WARN);
        }
    }
    else
    {
        infoClearPfError(BOARD_VOL_WARN);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-086 (Trace to: SLD-AMC01-086)
* Function:     sampleVol
* Description:  采集各通道电压值，ADC采样10次，取平均值计算出电压值
* Input:        none
* Output:       voltage : 存放电压最终转换的电压值和对应通道ID
* Return:       
*               
* Others:
* Log:          Date          Author    Modified
*               2019/07/04    wxb       smapleVol修改为sampleVol
****************************************************************************************************/
static void sampleVol(adcValue_t voltage[])
{
    uint32_t i = 0U;
    uint32_t j = 0U;
    uint32_t tmpData = 0U;
    float32_t tmpVoltage = 0.0f;

    adcData_t averageData[32U] = {{0U,0U}};  /* ADC数字量 */
    uint32_t  totalData[32U] = {0U};

    for(i = 0U; i< SMAPLE_COUNT; i++)
    {
        adcStartConversion(adcREG1, GROUP1);/* 启动ADC转换器 */
        while(adcIsConversionComplete(adcREG1, GROUP1) == 0u)/* 等待转换完成 */
        {
            ; /* Do nothing */
        }
        /* ADC RAM区读取 ADC值 */
        (void)adcGetData(adcREG1, GROUP1, (adcData_t *)averageData);

        /* 累加各通道ADC值 */
        for(j = 0U; j< CH_NUM; j++)
        {
            totalData[j] += averageData[j].value;
        }
    }

    for(i = 0U; i < CH_NUM; i++)
    {
        averageData[i].value = (uint16_t)(totalData[i] / SMAPLE_COUNT);
        tmpData = averageData[i].value;
        tmpVoltage = (float32_t)tmpData * VCC_REF ;
        voltage[i].value = tmpVoltage / 4096.0f;
        voltage[i].chId  = averageData[i].id;
    }
}

