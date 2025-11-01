/***************************************************************************************************
 * Filename: drv2_libs.c
 * Purpose:  常用功能模块库
 * Date:         Author      Modified 
 * 2021-10-16    hdq         Create  
***************************************************************************************************/

/******************************* Custom include *******************************/
#include "lynx_types.h"
#include "bsp.h"

/****************************************************************************************************
* Identifier:   SCOD-AMC01-058 (Trace to: SLD-AMC01-058)
* Function:     drv2CrcCal
* Description:  计算CRC
* Input:        pSrc 待计算的源数据
*               len  待计算数据长度
* Output:       none
* Return:       u64Signature 计算出的CRC值
* Others:
* Log:          Date          Author    Modified
*               2017/08/15
****************************************************************************************************/
uint64_t drv2CrcCal(const void *pSrc, uint32_t len)
{
    crcConfig_t    sCrcParams;
    crcModConfig_t sCrcModConfig;

    volatile uint64 u64Signature = 0U;

    LYNX_ASSERT(NULL != pSrc);

    sCrcParams.crc_channel   = CRC_CH1;
    sCrcParams.mode          = CRC_FULL_CPU;
    sCrcParams.pcount        = 0U; /* All counters are disabled in Full CPU mode */
    sCrcParams.scount        = 0U; /* All counters are disabled in Full CPU mode */
    sCrcParams.wdg_preload   = 0U; /* All counters are disabled in Full CPU mode */
    sCrcParams.block_preload = 0U; /* All counters are disabled in Full CPU mode */
    crcSetConfig(crcREG1, &sCrcParams);

    /* 设置种子 */
    crcREG1->CTRL2 &=0xFFFFFFFCU;
    crcREG1->PSA_SIGREGL1 = 0xFFFFFFFFU;
    crcREG1->PSA_SIGREGH1 = 0xFFFFFFFFU;
    crcREG1->CTRL2 = sCrcParams.mode;

    sCrcModConfig.crc_channel  = CRC_CH1;
    sCrcModConfig.mode         = CRC_FULL_CPU;
    sCrcModConfig.data_length  = len / 8U;
    sCrcModConfig.src_data_pat = (uint64*)pSrc;
    crcSignGen(crcREG1, &sCrcModConfig);

    u64Signature = crcGetPSASig(crcREG1, CRC_CH1);

    return u64Signature;
}

