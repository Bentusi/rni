/***************************************************************************************************
 * Filename: monitor.h
 * Purpose:  板卡电源电压诊断处理
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
***************************************************************************************************/
#ifndef _MONITOR_H_
#define _MONITOR_H_

#include "lynx_types.h"

/* 温度相关     temperature_calibration */
typedef struct OTP_temperature_calibra_data
{
    uint16_t Temperature;
    uint16_t AdcValue;
} OTP_temperature_calibra_data_t;

typedef struct Thermistor_Calibration
{
    float32_t slope;
    float32_t offset;
    float32_t rsquared;
} Thermistor_CAL_t;

typedef struct
{
    uint32_t  chId;
    float32_t value;
} adcValue_t;

#define THERMISTOR_CAL_DATA   (0xF0080310U)     /* OTP Temperature Sensor Data Location */
#define FLOAT_EPS             (0.000001f)       /* 浮点型单精度 最小精度 */
/*************************************************************************************/

#define VCC_REF                  ((float32_t)2.5f)           /* 参考电压 2.5V */
/* 定义采集通道 ID */
#define VCC_MCU_1V2_CHID         (0U)      /* ADC[0] */
#define VCC_FPGA_1V2_CHID        (1U)      /* ADC[1] */
#define VCC_WD_1V5_CHID          (2U)
#define VCC_MCU_3V3_CHID         (3U)
#define VCC_FPGA_3V3_CHID        (4U)
#define VCC_WD_3V3_CHID          (5U)
#define VCC_5V_CHID              (6U)
#define VCC_5VAUX_CHID           (7U)
#define VCC_24V_CHID             (8U)
#define VCC_24V1_CHID            (9U)
#define VCC_24V2_CHID            (10U)

/* 电压故障代码 */
#define VCC_MCU_1V2_ERROR       (0x00000001U)
#define VCC_FPGA_1V2_ERROR      (0x00000002U)
#define VCC_WD_1V5_ERROR        (0x00000004U)
#define VCC_MCU_3V3_ERROR       (0x00000008U)

#define VCC_WD_3V3_ERROR        (0x00000010U)
#define VCC_FPGA_3V3_ERROR      (0x00000020U)
#define VCC_5V_ERROR            (0x00000040U)
#define VCC_5VAUX_ERROR         (0x00000080U)

#define VCC_24V_ERROR           (0x00000100U)
#define VCC_24V1_ERROR          (0x00000200U)
#define VCC_24V2_ERROR          (0x00000400U)

#define CH_NUM                  (11U)  /* 电源电压诊断 通道数目 */
#define MAX_ADC_CH              (32U)  /* ADC最大通道数 */
#define SMAPLE_COUNT            (10U)  /* 电压采集次数 */

/* 电压参考范围 */
#define VCC_FPGA_1V2_MAX        (1.258f)
#define VCC_FPGA_1V2_MIN        (1.137f)

#define VCC_MCU_1V2_MAX         (1.258f)
#define VCC_MCU_1V2_MIN         (1.137f)

#define VCC_WD_1V5_MAX          (1.60f)
#define VCC_WD_1V5_MIN          (1.40f)

#define VCC_MCU_3V3_MAX         (1.15f)
#define VCC_MCU_3V3_MIN         (1.048f)

#define VCC_WD_3V3_MAX          (1.15f)
#define VCC_WD_3V3_MIN          (1.048f)

#define VCC_FPGA_3V3_MAX        (1.15f)
#define VCC_FPGA_3V3_MIN        (1.048f)

#define VCC_5VAUX_MAX           (1.833f)
#define VCC_5VAUX_MIN           (1.5f)

#define VCC_5V_MAX              (1.833f)
#define VCC_5V_MIN              (1.5f)

#define VCC_24V_MAX             (1.319f)
#define VCC_24V_MIN             (0.919f)

#define VCC_24V1_MAX            (1.333f)
#define VCC_24V1_MIN            (0.933f)

#define VCC_24V2_MAX            (1.333f)
#define VCC_24V2_MIN            (0.933f)


#define EVENT_GROUP       (0U)
#define GROUP1            (1U)
#define GROUP2            (2U)

#define CHANNEL1    ((uint32_t)1U << (uint32_t)0U)
#define CHANNEL2    ((uint32_t)1U << (uint32_t)1U)
#define CHANNEL3    ((uint32_t)1U << (uint32_t)2U)
#define CHANNEL4    ((uint32_t)1U << (uint32_t)3U)
#define CHANNEL5    ((uint32_t)1U << (uint32_t)4U)
#define CHANNEL6    ((uint32_t)1U << (uint32_t)5U)
#define CHANNEL7    ((uint32_t)1U << (uint32_t)6U)
#define CHANNEL8    ((uint32_t)1U << (uint32_t)7U)

#define CHANNEL9    ((uint32_t)1U << (uint32_t)8U)
#define CHANNEL10   ((uint32_t)1U << (uint32_t)9U)
#define CHANNEL11   ((uint32_t)1U << (uint32_t)10U)


/* 选用 通道 */
#define CHANNLES            (CHANNEL1  | \
                             CHANNEL2  | \
                             CHANNEL3  | \
                             CHANNEL4  | \
                             CHANNEL5  | \
                             CHANNEL6  | \
                             CHANNEL7  | \
                             CHANNEL8  | \
                             CHANNEL9  | \
                             CHANNEL10 | \
                             CHANNEL11 )

#define TEMPERATURE_SPI           (spiREG5)
#define READ_TEMPER_VALE          (0x50U)

/* function */
extern void volDiganostic(uint8_t flag);

#endif

