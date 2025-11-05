/**************************************************************************************************
*Filename:     flash.c
*Purpose:      flash操作接口
*Log:          Date          Author    Modified
*              2021/9/18     hdq       create
**************************************************************************************************/
#include <stddef.h>
#include "flash.h"
#include "bsp.h"

/* 函数声明 */
static void spiTxData(spiBASE_t *spi,const spiDAT1_t *dataconfig_t,uint32 blocksize,const uint16 *srcbuff,uint32 hold);
static void s25flWriteEnable(void);

/****************************************************************************************************
* Identifier:   SCOD-AMC01-112 (Trace to: SLD-AMC01-112)
* Function:     spiTxData
* Description:  SPI发送数据
* Input:        spi          spi基地址
*               dataconfig_t spi配置数据
*               blocksize    发送数据长度
*               srcbuff      发送数据地址
*               hold         连续发送标志
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
static void spiTxData(spiBASE_t *spi,
                        const spiDAT1_t *dataconfig_t,
                        uint32 blocksize,
                        const uint16 *srcbuff,
                        uint32 hold)
{
    volatile uint32 SpiBuf = 0U;
    uint16 Tx_Data = 0U;
    uint32 Chip_Select_Hold = (dataconfig_t->CS_HOLD) ? 0x10000000U : 0U;
    uint32 WDelay = (dataconfig_t->WDEL) ? 0x04000000U : 0U;
    SPIDATAFMT_t DataFormat = dataconfig_t->DFSEL;
    uint8 ChipSelect = dataconfig_t->CSNR;

    while(blocksize != 0U)
    {
        if((spi->FLG & 0x000000FFU) !=0U)
        {
            break;
        }

        if((blocksize == 1U) &&
                (hold == 0U))
        {
            Chip_Select_Hold = 0U;
        }
        /* SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        Tx_Data = *srcbuff;

        spi->DAT1 =  ((uint32)DataFormat << 24U) |
            ((uint32)ChipSelect << 16U) |
            (WDelay)           |
            (Chip_Select_Hold) |
            (uint32)Tx_Data;
        /* SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        srcbuff++;
        /* SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
        while((spi->FLG & 0x00000100U) != 0x00000100U)
        {
        } /* Wait */
        SpiBuf = spi->BUF;
        blocksize--;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-113 (Trace to: SLD-AMC01-113)
* Function:     spiRxData
* Description:  SPI接收数据
* Input:        spi          spi基地址
*               dataconfig_t spi配置数据
*               blocksize    接收数据长度
*               destbuff     接收数据地址
*               hold         连续接收标志
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
static void spiRxData(spiBASE_t *spi,
                        const spiDAT1_t *dataconfig_t,
                        uint32 blocksize,
                        uint16 * destbuff,
                        uint32 hold)
{
    uint32 Chip_Select_Hold = (dataconfig_t->CS_HOLD) ? 0x10000000U : 0U;
    uint32 WDelay = (dataconfig_t->WDEL) ? 0x04000000U : 0U;
    SPIDATAFMT_t DataFormat = dataconfig_t->DFSEL;
    uint8 ChipSelect = dataconfig_t->CSNR;

    while(blocksize != 0U)
    {
        if((spi->FLG & 0x000000FFU) !=0U)
        {
            break;
        }
        if((blocksize == 1U) &&
                        (hold == 0U))
        {
            Chip_Select_Hold = 0U;
        }

        /*SAFETYMCUSW 51 S MR:12.3 <APPROVED> "Needs shifting for 32-bit value" */
        spi->DAT1 = ((uint32)DataFormat << 24U) |
                    ((uint32)ChipSelect << 16U) |
                    (WDelay)            |
                    (Chip_Select_Hold)  |
                    (0x00000000U);
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
        while((spi->FLG & 0x00000100U) != 0x00000100U)
        {
        } /* Wait */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        *destbuff = (uint16)spi->BUF;
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        destbuff++;
        blocksize--;
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-114 (Trace to: SLD-AMC01-114)
* Function:     s25flIsBusy
* Description:  s25Flash忙检测
* Input:        none
* Output:       none
* Return:       ret  忙标志 1 忙碌状态 0 待机状态 
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
static int32_t s25flIsBusy(void)
{
    int32_t ret = 0;
    spiDAT1_t SpiCfg;
    uint16_t outBuf[2U] = {0U};
    uint16_t inData[2U] = {0U};
    uint32_t wrLen = sizeof(inData)/sizeof(uint16_t);

    SpiCfg.CSNR = (uint8_t)SPI_CS_0;    /* SPI片选脚选择 */
    SpiCfg.DFSEL   = SPI_FMT_1;         /* 使用数据格式0 配置*/
    SpiCfg.WDEL    = 1U;
    SpiCfg.CS_HOLD = 1U;

    inData[0U] = FLASH_READ_STATUS_REG;

    (void)spiTransmitAndReceiveData(FLASH_SPI,&SpiCfg, wrLen,inData,outBuf);
    if((outBuf[1U] & 0x01U) == 0x01U)
    {
        ret = 1;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-115 (Trace to: SLD-AMC01-115)
* Function:     s25flWriteEnable
* Description:  s25Flash写使能
* Input:        none
* Output:       none
* Return:       none 
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
static void s25flWriteEnable(void)
{
    spiDAT1_t SpiCfg;
    uint16 inData = FLASH_WRITE_ENABLE;

    SpiCfg.CSNR = (uint8_t)SPI_CS_0;
    SpiCfg.DFSEL   = SPI_FMT_1;         /* 使用数据格式0 配置 */
    SpiCfg.WDEL    = 1U;
    SpiCfg.CS_HOLD = 1U;

    (void)spiTransmitData(FLASH_SPI, &SpiCfg, 1U, &inData);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-116 (Trace to: SLD-AMC01-116)
* Function:     s25flEraseSector
* Description:  擦除扇区，单个扇区大小为4K
* Input:        addr 擦除地址
* Output:       none
* Return:       none 
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
static void s25flEraseSector(uint32_t addr)
{
    spiDAT1_t SpiCfg;
    uint16_t outBuf[4U] = {0U};
    uint16_t inData[4U] = {0U};
    uint32_t wrLen = sizeof(inData)/sizeof(uint16_t);

    SpiCfg.CSNR = (uint8_t)SPI_CS_0;    /* SPI片选脚选择 */
    SpiCfg.DFSEL   = SPI_FMT_1;         /* 使用数据格式0 配置*/
    SpiCfg.WDEL    = 1U;
    SpiCfg.CS_HOLD = 1U;

    inData[0U] = FLASH_SECTOR_ERASE;
    inData[1U] = (uint8_t)(addr >> 16U);
    inData[2U] = (uint8_t)(addr >> 8U);
    inData[3U] = (uint8_t)(addr >> 0U);
    
    /* flash 写使能 */
    s25flWriteEnable();
    (void)spiTransmitAndReceiveData(FLASH_SPI,&SpiCfg, wrLen,inData,outBuf);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-117 (Trace to: SLD-AMC01-117)
* Function:     s25flWritePage
* Description:  写入页数据，页大小为256B
* Input:        addr 写Flash首地址
*               pBuf 数据首地址
*               length 数据长度
* Output:       none
* Return:       none 
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
static void s25flWritePage(uint32_t addr, const uint8_t *pBuf, uint32_t length)
{
    spiDAT1_t SpiCfg;
    uint8_t inData[6] = {0U};

    SpiCfg.CSNR = (uint8_t)SPI_CS_0;   /* 选择片选 */
    SpiCfg.DFSEL   = SPI_FMT_0;        /* 选择16位传输模式 */
    SpiCfg.WDEL    = 1U;
    SpiCfg.CS_HOLD = 1U;

    inData[0] = (uint8_t)(addr >> 16U);
    inData[1] = 0x02;
    inData[2] = (uint8_t)(addr >> 0U);
    inData[3] = (uint8_t)(addr >> 8U);

    s25flWriteEnable();
    spiTxData(spiREG2, &SpiCfg, 2U, (uint16_t *)inData, 1U);
    spiTxData(spiREG2, &SpiCfg, length/2U, (uint16_t *)pBuf, 0U);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-118 (Trace to: SLD-AMC01-118)
* Function:     s25flReadBytes
* Description:  读Flash数据
* Input:        addr 读Flash首地址
*               pBuf 数据缓存首地址
*               length 数据长度
* Output:       none
* Return:       none 
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
static void s25flReadBytes(uint32_t addr, uint8_t *pBuf, uint32_t length)
{
    uint32_t curAddr = addr;

    spiDAT1_t SpiCfg;
    uint8_t inData[4];

    SpiCfg.CSNR = (uint8_t)SPI_CS_0;
    SpiCfg.DFSEL   = SPI_FMT_0;         /* 选择16位传输模式 */
    SpiCfg.WDEL    = 1U;
    SpiCfg.CS_HOLD = 1U;

    inData[0] = (uint8_t)(curAddr >> 16U);
    inData[1] = 0x03;
    inData[2] = (uint8_t)(curAddr >> 0U);
    inData[3] = (uint8_t)(curAddr >> 8U);

    spiTxData(spiREG2, &SpiCfg, 2U, (uint16_t *)inData, 1U);

    spiRxData(spiREG2, &SpiCfg, length/2U, (uint16_t *)pBuf, 0U);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-221 (Trace to: SLD-AMC01-221)
* Function:     flashEraseSector
* Description:  擦除Flash数据接口函数
* Input:        addr 擦除Flash首地址
* Output:       none
* Return:       返回0 
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
void flashEraseSector(uint32_t addr)
{
    s25flEraseSector(addr);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-220 (Trace to: SLD-AMC01-220)
* Function:     flashWriteByBytes
* Description:  写Flash数据接口函数
* Input:        addr 读Flash首地址
*               pBuf 数据缓存首地址
*               length 数据长度
* Output:       none
* Return:       返回0 
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
int32_t flashWriteByBytes(uint32_t addr, const uint8_t *pBuf, uint32_t length)
{
    s25flWritePage(addr, pBuf, length);
    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-119 (Trace to: SLD-AMC01-119)
* Function:     flashReadByBytes
* Description:  读Flash数据接口函数
* Input:        addr 读Flash首地址
*               pBuf 数据缓存首地址
*               length 数据长度
* Output:       none
* Return:       返回0 
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
int32_t flashReadByBytes(uint32_t addr, uint8_t *pBuf, uint32_t length)
{
    s25flReadBytes(addr, pBuf, length);
    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-223 (Trace to: SLD-AMC01-223)
* Function:     flashWriteNoblock
* Description:  保存参数文件
* Input:        dstAddr 参数文件地址
*               pBuf    参数文件
*               bufSize 参数文件大小
* Output:       none
* Return:       0:正常；1：失败
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
int32_t flashWriteNoblock(uint32_t dstAddr, const void *pBuf, uint32_t bufSize)
{
    int32_t ret = 1;
    uint32_t wrSize = 0;

    static uint32_t step = 0U;
    static uint32_t startAddr = 0U, endAddr = 0U;
    static uint32_t curSize = 0U;
    static const uint8_t *pCurBuf = NULL;
    LYNX_ASSERT(NULL != pBuf);
    
    if(s25flIsBusy() == 0)
    {
        switch(step)
        {
        case 0U: /* initiate */
            curSize = bufSize;
            startAddr = dstAddr;
            endAddr = dstAddr + curSize;
            pCurBuf = (const uint8_t *)pBuf;
            startAddr = (startAddr/SECTOR_SIZE)*SECTOR_SIZE; /* align 4K */
            endAddr = (endAddr/SECTOR_SIZE)*SECTOR_SIZE; /* align 4K */
            step = 1U;
            break;

        case 1U: /*erase*/
            s25flEraseSector(startAddr);
            startAddr += SECTOR_SIZE;
            if(startAddr > endAddr)
            {
                curSize = bufSize;
                startAddr = dstAddr;
                endAddr = dstAddr + curSize;
                pCurBuf = (const uint8_t *)pBuf;
                step = 2U;
            }
            break;

        case 2U: /* write */
            if(curSize > 256U)
            {
                wrSize = 256U;
            }
            else
            {
                wrSize = curSize;
            }
            s25flWritePage(startAddr, pCurBuf, wrSize);
            startAddr += wrSize;
            pCurBuf += wrSize;
            curSize -= wrSize;
            if(startAddr >= endAddr)
            {
                step = 0U;
                ret = 0;
            }
            break;

        default:
            break;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

