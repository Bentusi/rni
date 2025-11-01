/*********************************************************
 * FILENAME:panel.c
 * PURPOSE: 前面板指示灯驱动
 * DATE             ARTHOR             CHANGE
 *2017-10-9         wch                Created
 *********************************************************/
#include "HL_gio.h"
#include "HL_het.h"
#include "HL_spi.h"
#include "panel.h"

typedef struct 
{
    gioPORT_t *port;
    uint32_t idx;
} io_t;

typedef struct 
{
    io_t reds[PanelLedCount];
    io_t greens[PanelLedCount];
} panelled_inst_t;

typedef struct
{
    hcms_spi_t spi;
    hcms_ioport_t ioport;
    hcms_ioidx_t ioidx;
} hcms_inst_t;

/**
 * Panel Led Driver Instance
 *
 * clang-format off
 */
static const panelled_inst_t s_panelled_inst = 
{
    .reds = 
    {
        {gioPORTA, 0U }, /* PWR */
        {hetPORT1, 4U }, /* RUN */
        {gioPORTA, 4U }, /* SYN */
        {gioPORTA, 6U }, /* MAT */
        /* BUG114 */
        {hetPORT1, 2U }, /* ERR */
        {hetPORT1, 4U }, /* MAS */
        {hetPORT1, 7U }, /* SLA */
        {hetPORT1, 9U }, /* TX */
        {hetPORT1, 11U} /* RX */
    },
    .greens = 
    {
        {gioPORTA, 1U }, /* PWR */
        {hetPORT1, 5U }, /* RUN */
        {gioPORTA, 5U }, /* SYN */
        {gioPORTA, 7U }, /* MAT */
        /* BUG114 */
        {hetPORT1, 3U }, /* ERR */
        {hetPORT1, 5U }, /* MAS */
        {hetPORT1, 8U }, /* SLA */
        {hetPORT1, 10U}, /* TX */
        {hetPORT1, 12U}  /* RX */
    }
};

static const uint8_t g_hcms3906_charAsciiTbl[128][5] =
{
    /*   - 00 */ { 0x08U, 0x1CU, 0x3EU, 0x7FU, 0x00U },
    /*   - 01 */ { 0x30U, 0x45U, 0x48U, 0x40U, 0x30U },
    /* x - 02 */ { 0x45U, 0x29U, 0x11U, 0x29U, 0x45U },
    /* N - 03 */ { 0x7DU, 0x09U, 0x11U, 0x21U, 0x7DU },
    /* n - 04 */ { 0x7DU, 0x09U, 0x05U, 0x05U, 0x79U },
    /* a - 05 */ { 0x38U, 0x44U, 0x44U, 0x38U, 0x44U },
    /* b - 06 */ { 0x7EU, 0x01U, 0x29U, 0x2EU, 0x10U },
    /* d - 07 */ { 0x30U, 0x4AU, 0x4DU, 0x49U, 0x30U },
    /*   - 08 */ { 0x60U, 0x50U, 0x48U, 0x50U, 0x60U },
    /* h - 09 */ { 0x1EU, 0x04U, 0x04U, 0x38U, 0x40U },
    /*   - 0A */ { 0x3EU, 0x49U, 0x49U, 0x49U, 0x3EU },
    /*   - 0B */ { 0x62U, 0x14U, 0x08U, 0x10U, 0x60U },
    /* m - 0C */ { 0x40U, 0x3CU, 0x20U, 0x20U, 0x1CU },
    /*   - 0D */ { 0x08U, 0x7CU, 0x04U, 0x7CU, 0x02U },
    /*   - 0E */ { 0x38U, 0x44U, 0x44U, 0x3CU, 0x04U },
    /*   - 0F */ { 0x41U, 0x63U, 0x55U, 0x49U, 0x41U },
    /*   - 10 */ { 0x10U, 0x08U, 0x78U, 0x08U, 0x04U },
    /*   - 11 */ { 0x18U, 0x24U, 0x7EU, 0x24U, 0x18U },
    /*   - 12 */ { 0x5EU, 0x61U, 0x01U, 0x61U, 0x5EU },
    /*   - 13 */ { 0x78U, 0x14U, 0x15U, 0x14U, 0x78U },
    /*   - 14 */ { 0x38U, 0x44U, 0x45U, 0x3CU, 0x40U },
    /*   - 15 */ { 0x78U, 0x15U, 0x14U, 0x15U, 0x78U },
    /*   - 16 */ { 0x38U, 0x45U, 0x44U, 0x3DU, 0x40U },
    /*   - 17 */ { 0x3CU, 0x43U, 0x42U, 0x43U, 0x3CU },
    /*   - 18 */ { 0x38U, 0x45U, 0x44U, 0x45U, 0x38U },
    /*   - 19 */ { 0x3CU, 0x41U, 0x40U, 0x41U, 0x3CU },
    /*   - 1A */ { 0x38U, 0x42U, 0x40U, 0x42U, 0x38U },
    /*   - 1B */ { 0x08U, 0x08U, 0x2AU, 0x1CU, 0x08U },
    /*   - 1C */ { 0x20U, 0x7EU, 0x02U, 0x02U, 0x02U },
    /*   - 1D */ { 0x12U, 0x19U, 0x15U, 0x12U, 0x00U },
    /*   - 1E */ { 0x48U, 0x7EU, 0x49U, 0x41U, 0x42U },
    /*   - 1F */ { 0x01U, 0x12U, 0x7CU, 0x12U, 0x01U },
    /* (space)  - 20 */ { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U },
    /* ! - 21 */ { 0x00U, 0x5FU, 0x00U, 0x00U, 0x00U },
    /*   - 22 */ { 0x00U, 0x03U, 0x00U, 0x03U, 0x00U },
    /* # - 23 */ { 0x14U, 0x7FU, 0x14U, 0x7FU, 0x14U },
    /* $ - 24 */ { 0x24U, 0x2AU, 0x7FU, 0x2AU, 0x12U },
    /* % - 25 */ { 0x23U, 0x13U, 0x08U, 0x64U, 0x62U },
    /* & - 26 */ { 0x36U, 0x49U, 0x56U, 0x20U, 0x50U },
    /*   - 27 */ { 0x00U, 0x0BU, 0x07U, 0x00U, 0x00U },
    /* ( - 28 */ { 0x00U, 0x00U, 0x3EU, 0x41U, 0x00U },
    /* ) - 29 */ { 0x00U, 0x41U, 0x3EU, 0x00U, 0x00U },
    /* * - 2A */ { 0x08U, 0x2AU, 0x1CU, 0x2AU, 0x08U },
    /* + - 2B */ { 0x08U, 0x08U, 0x3EU, 0x08U, 0x08U },
    /* , - 2C */ { 0x00U, 0x58U, 0x38U, 0x00U, 0x00U },
    /* - - 2D */ { 0x08U, 0x08U, 0x08U, 0x08U, 0x08U },
    /* . - 2E */ { 0x00U, 0x30U, 0x30U, 0x00U, 0x00U },
    /* / - 2F */ { 0x20U, 0x10U, 0x08U, 0x04U, 0x02U },
    /* 0 - 30 */ { 0x3EU, 0x51U, 0x49U, 0x45U, 0x3EU },
    /* 1 - 31 */ { 0x00U, 0x42U, 0x7FU, 0x40U, 0x00U },
    /* 2 - 32 */ { 0x62U, 0x51U, 0x49U, 0x49U, 0x46U },
    /* 3 - 33 */ { 0x22U, 0x41U, 0x49U, 0x49U, 0x36U },
    /* 4 - 34 */ { 0x18U, 0x14U, 0x12U, 0x7FU, 0x10U },
    /* 5 - 35 */ { 0x27U, 0x45U, 0x45U, 0x45U, 0x39U },
    /* 6 - 36 */ { 0x3CU, 0x4AU, 0x49U, 0x49U, 0x30U },
    /* 7 - 37 */ { 0x01U, 0x71U, 0x09U, 0x05U, 0x03U },
    /* 8 - 38 */ { 0x36U, 0x49U, 0x49U, 0x49U, 0x36U },
    /* 9 - 39 */ { 0x06U, 0x49U, 0x49U, 0x29U, 0x1EU },
    /* : - 3A */ { 0x00U, 0x36U, 0x36U, 0x00U, 0x00U },
    /* ; - 3B */ { 0x00U, 0x5BU, 0x3BU, 0x00U, 0x00U },
    /* < - 3C */ { 0x00U, 0x08U, 0x14U, 0x22U, 0x41U },
    /* = - 3D */ { 0x14U, 0x14U, 0x14U, 0x14U, 0x14U },
    /* > - 3E */ { 0x41U, 0x22U, 0x14U, 0x08U, 0x00U },
    /* ? - 3F */ { 0x02U, 0x01U, 0x51U, 0x09U, 0x06U },
    /* @ - 40 */ { 0x3EU, 0x41U, 0x5DU, 0x55U, 0x1EU },
    /* A - 41 */ { 0x7EU, 0x09U, 0x09U, 0x09U, 0x7EU },
    /* B - 42 */ { 0x7EU, 0x49U, 0x49U, 0x49U, 0x36U },
    /* C - 43 */ { 0x3EU, 0x41U, 0x41U, 0x41U, 0x22U },
    /* D - 44 */ { 0x7FU, 0x41U, 0x41U, 0x41U, 0x3EU },
    /* E - 45 */ { 0x7FU, 0x49U, 0x49U, 0x49U, 0x41U },
    /* F - 46 */ { 0x7FU, 0x09U, 0x09U, 0x09U, 0x01U },
    /* G - 47 */ { 0x3EU, 0x41U, 0x41U, 0x51U, 0x32U },
    /* H - 48 */ { 0x7FU, 0x08U, 0x08U, 0x08U, 0x7FU },
    /* I - 49 */ { 0x00U, 0x41U, 0x7FU, 0x41U, 0x00U },
    /* J - 4A */ { 0x20U, 0x40U, 0x40U, 0x40U, 0x3FU },
    /* K - 4B */ { 0x7FU, 0x08U, 0x14U, 0x22U, 0x41U },
    /* L - 4C */ { 0x7FU, 0x40U, 0x40U, 0x40U, 0x40U },
    /* M - 4D */ { 0x7FU, 0x02U, 0x0CU, 0x02U, 0x7FU },
    /* N - 4E */ { 0x7FU, 0x04U, 0x08U, 0x10U, 0x7FU },
    /* O - 4F */ { 0x3EU, 0x41U, 0x41U, 0x41U, 0x3EU },
    /* P - 50 */ { 0x7FU, 0x09U, 0x09U, 0x09U, 0x06U },
    /* Q - 51 */ { 0x3EU, 0x41U, 0x51U, 0x21U, 0x5EU },
    /* R - 52 */ { 0x7FU, 0x09U, 0x19U, 0x29U, 0x46U },
    /* S - 53 */ { 0x26U, 0x49U, 0x49U, 0x49U, 0x32U },
    /* T - 54 */ { 0x01U, 0x01U, 0x7FU, 0x01U, 0x01U },
    /* U - 55 */ { 0x3FU, 0x40U, 0x40U, 0x40U, 0x3FU },
    /* V - 56 */ { 0x07U, 0x18U, 0x60U, 0x18U, 0x07U },
    /* W - 57 */ { 0x7FU, 0x20U, 0x18U, 0x20U, 0x7FU },
    /* X - 58 */ { 0x63U, 0x14U, 0x08U, 0x14U, 0x63U },
    /* Y - 59 */ { 0x03U, 0x04U, 0x78U, 0x04U, 0x03U },
    /* Z - 5A */ { 0x61U, 0x51U, 0x49U, 0x45U, 0x43U },
    /* [ - 5B */ { 0x00U, 0x00U, 0x7FU, 0x41U, 0x41U },
    /* \ - 5C */ { 0x02U, 0x04U, 0x08U, 0x10U, 0x20U },
    /* ] - 5D */ { 0x41U, 0x41U, 0x7FU, 0x00U, 0x00U },
    /* - - 5E */ { 0x04U, 0x02U, 0x7FU, 0x02U, 0x04U },
    /* _ - 5F */ { 0x40U, 0x40U, 0x40U, 0x40U, 0x40U },
    /* ` - 60 */ { 0x00U, 0x07U, 0x0BU, 0x00U, 0x00U },
    /* a - 61 */ { 0x38U, 0x44U, 0x44U, 0x3CU, 0x40U },
    /* b - 62 */ { 0x7FU, 0x48U, 0x44U, 0x44U, 0x38U },
    /* c - 63 */ { 0x38U, 0x44U, 0x44U, 0x44U, 0x44U },
    /* d - 64 */ { 0x38U, 0x44U, 0x44U, 0x48U, 0x7FU },
    /* e - 65 */ { 0x38U, 0x54U, 0x54U, 0x54U, 0x08U },
    /* f - 66 */ { 0x08U, 0x7EU, 0x09U, 0x02U, 0x00U },
    /* g - 67 */ { 0x08U, 0x14U, 0x54U, 0x54U, 0x3CU },
    /* h - 68 */ { 0x7FU, 0x08U, 0x04U, 0x04U, 0x78U },
    /* i - 69 */ { 0x00U, 0x44U, 0x7DU, 0x40U, 0x00U },
    /* j - 6A */ { 0x20U, 0x40U, 0x44U, 0x3DU, 0x00U },
    /* k - 6B */ { 0x00U, 0x7FU, 0x10U, 0x28U, 0x44U },
    /* l - 6C */ { 0x00U, 0x41U, 0x7FU, 0x40U, 0x00U },
    /* m - 6D */ { 0x78U, 0x04U, 0x18U, 0x04U, 0x78U },
    /* n - 6E */ { 0x7CU, 0x08U, 0x04U, 0x04U, 0x78U },
    /* o - 6F */ { 0x38U, 0x44U, 0x44U, 0x44U, 0x38U },
    /* p - 70 */ { 0x7CU, 0x14U, 0x24U, 0x24U, 0x18U },
    /* q - 71 */ { 0x18U, 0x24U, 0x14U, 0x7CU, 0x40U },
    /* r - 72 */ { 0x00U, 0x7CU, 0x08U, 0x04U, 0x04U },
    /* s - 73 */ { 0x48U, 0x54U, 0x54U, 0x54U, 0x20U },
    /* t - 74 */ { 0x04U, 0x3EU, 0x44U, 0x20U, 0x00U },
    /* u - 75 */ { 0x3CU, 0x40U, 0x40U, 0x20U, 0x7CU },
    /* v - 76 */ { 0x1CU, 0x20U, 0x40U, 0x20U, 0x1CU },
    /* w - 77 */ { 0x3CU, 0x40U, 0x30U, 0x40U, 0x3CU },
    /* x - 78 */ { 0x44U, 0x28U, 0x10U, 0x28U, 0x44U },
    /* y - 79 */ { 0x04U, 0x48U, 0x30U, 0x08U, 0x04U },
    /* z - 7A */ { 0x44U, 0x64U, 0x54U, 0x4CU, 0x44U },
    /* { - 7B */ { 0x00U, 0x08U, 0x36U, 0x41U, 0x00U },
    /* | - 7C */ { 0x00U, 0x00U, 0x77U, 0x00U, 0x00U },
    /* } - 7D */ { 0x00U, 0x41U, 0x36U, 0x08U, 0x00U },
    /* ~ - 7E */ { 0x08U, 0x04U, 0x08U, 0x10U, 0x08U },
    /*   - 7F */ { 0x2AU, 0x55U, 0x2AU, 0x55U, 0x2AU }
};

static hcms_inst_t s_hcms_inst;
static void drv2LedInit(void);
static void drv2HcmsCtrlRegWrite(uint8_t regval);
static void drv2HcmsDotRegWrite(const uint8_t *pDotData);

/****************************************************************************************************
* Identifier:   SCOD-AMC01-089 (Trace to: SLD-AMC01-089)
* Function:     drv2PanelRedLedSet
* Description:  控制前面板红色指示灯
* Input:        led      红led序号
*               value    具体值
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2PanelRedLedSet(panelled_t led, uint8_t value)
{
    const panelled_inst_t *const pinst = &s_panelled_inst;
	gioPORT_t*ledGioPort = NULL;
	uint32_t ledGioId = 0x0U;
    value = value&0x01U;

    if(NULL != pinst->reds[led].port)
    {
        ledGioPort = pinst->reds[led].port;
    }

    ledGioId = pinst->reds[led].idx;
    if(NULL != ledGioPort)
    {
        gioSetBit(ledGioPort, ledGioId, (uint32)value);
    }

    if(NULL != pinst->greens[led].port)
    {
        ledGioPort = pinst->greens[led].port;
    }
    ledGioId = pinst->greens[led].idx;
    if(NULL != ledGioPort)
    {
        gioSetBit(ledGioPort, ledGioId, 0U);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-090 (Trace to: SLD-AMC01-090)
* Function:     drv2PanelGreenLedSet
* Description:  控制前面板绿色指示灯
* Input:        led        绿led序号
*               value      具体值
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2PanelGreenLedSet(panelled_t led, uint8_t value)
{
    const panelled_inst_t *const pinst = &s_panelled_inst;
    gioPORT_t*ledGioPort = NULL;
    uint32_t ledGioId = 0x0U;
	value = value&0x01U;

    if(NULL != pinst->reds[led].port)
    {
        ledGioPort = pinst->reds[led].port;
    }
    ledGioId = pinst->reds[led].idx;
    if(NULL != ledGioPort)
    {
        gioSetBit(ledGioPort, ledGioId, 0U);
    }

    if(NULL != pinst->greens[led].port)
    {
        ledGioPort = pinst->greens[led].port;
    }
    ledGioId = pinst->greens[led].idx;
    if(NULL != ledGioPort)
    {
        gioSetBit(ledGioPort, ledGioId, (uint32)value);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-040 (Trace to: SLD-AMC01-040)
* Function:     drv2PanelSwitchStateGet
* Description:  获取钥匙开关值
* Input:        none
* Output:       none
* Return:       swval 读取到的值
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
*               2021/09/01    hdq       删除闭锁分支
****************************************************************************************************/
uint32_t drv2PanelSwitchStateGet(void)
{
    uint32_t swval = 0x0U;
    uint32_t swvalBak = 0x0U;

    swval  = gioGetPort(gioPORTB) & 0x0000000FU;
    swval &= 0x00000003U;

    swvalBak = gioGetPort(gioPORTA) & 0x0000000FU;
    swvalBak &= 0x00000003U;

    if(swval != swvalBak)
    {
        swval = 0xFFU;
    }
    return swval;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-099 (Trace to: SLD-AMC01-099)
* Function:     drv2HcmsRegWrite
* Description:  写寄存器
* Input:        reg      寄存器地址
*               pdata    数据
*               ndata    数据长度
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2HcmsRegWrite(hcms_reg_t reg, const uint8_t* pdata, uint32_t ndata)
{
    const hcms_inst_t *const pinst = &s_hcms_inst;
    gioPORT_t * port = pinst->ioport.RS;
    uint32_t idx = pinst->ioidx.RS;
    spiBASE_t * spi = pinst->spi.regbase;
    spiDAT1_t spidat = {
        .CS_HOLD = true,
        .WDEL = false,
        .DFSEL = SPI_FMT_0,
        .CSNR = 0U
    };
    uint32_t i = (uint32_t)0x0U;
    uint16_t spibuf[20U] = {0x0U};
    uint16_t destBuf[20U] = {0x0U};
    uint32_t delay = 1000U;

    LYNX_ASSERT(NULL != pdata);
    spidat.DFSEL = pinst->spi.df;
    spidat.CSNR = (uint8_t)(~pinst->spi.csmask);
    (void)memset((void*)spibuf,0x0,40U);
    (void)memset((void*)destBuf,0x0,40U);
    if(ndata <= 20U)
    {
        for (i = 0U; i < ndata; ++i)
        {
            spibuf[i] = pdata[i];
        }

        gioSetBit(port, idx, (uint32)reg);/* pull RS to select register */
        while(delay > 0x0U)
        {
            delay--;
        }
        (void)spiTransmitAndReceiveData(spi, &spidat, ndata, (uint16_t*)spibuf, (uint16_t*)destBuf);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-100 (Trace to: SLD-AMC01-100)
* Function:     drv2HcmsCtrlRegWrite
* Description:  写控制寄存器
* Input:        regval      寄存器值
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
static void drv2HcmsCtrlRegWrite(uint8_t regval)
{
    drv2HcmsRegWrite(HCMS_RegCtrl, &regval, sizeof(regval));/* 设置控制寄存器 */
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-101 (Trace to: SLD-AMC01-101)
* Function:     drv2HcmsDotRegWrite
* Description:  写点阵寄存器
* Input:        pDotData    点阵值
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
static void drv2HcmsDotRegWrite(const uint8_t *pDotData)
{
    drv2HcmsRegWrite(HCMS_RegDot, pDotData, 20U);    /* 设置字符 */
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-102 (Trace to: SLD-AMC01-102)
* Function:     drv2HcmsClear
* Description:  清除所有字符显示
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2HcmsClear(void)
{
    uint8_t dotdata[20U] = {(uint8_t)0U};
    (void)memset((void*)dotdata, 0, 20U);
    drv2HcmsDotRegWrite((const uint8_t*)dotdata);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-103 (Trace to: SLD-AMC01-103)
* Function:     drv2HcmsInit
* Description:  初始化3906
* Input:        mode    初始化模式
*               params  Hcms参数
* Output:       none
* Return:       0
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
int32_t drv2HcmsInit(hcms_int_mode_t mode, const hcms_params_t *params)
{
    hcms_inst_t *const pinst = &s_hcms_inst;

    gioPORT_t *port = NULL;
    uint32 bit = 0U;
    uint32 cnt = 50000U;
    uint32 delay = 10000U;
    LYNX_ASSERT(NULL != params);

    /* 获取上层配置信息 */
    (void)memcpy((void*)&pinst->spi, (const void*)&params->spi, sizeof(hcms_spi_t));
    (void)memcpy((void*)&pinst->ioport, (const void*)&params->ioport, sizeof(hcms_ioport_t));
    (void)memcpy((void*)&pinst->ioidx, (const void*)&params->ioidx, sizeof(hcms_ioidx_t));

    if(mode == HCMS_INIT_MODE_START)
    {
        /* set  pin BL Low */
        port = pinst->ioport.BL;
        bit  = pinst->ioidx.BL;
        gioSetBit(port, bit, 0U);

        /* set pin sel high */
        port = pinst->ioport.SEL;
        bit  = pinst->ioidx.SEL;
        gioSetBit(port, bit, 1U);

        cnt = 1000U;
        while(cnt > 0x0u)
        {
            cnt--;

            delay = 10000U;
            while(delay > 0x0U)
            {
                delay--;
            }
        }
        /* reset */
        port = pinst->ioport.nRST;
        bit  = pinst->ioidx.nRST;
        gioSetBit(port, bit, 0U);
        cnt = 500U;
        while(cnt > 0x0u)
        {
            cnt--;

            delay = 10000U;
            while(delay > 0x0U)
            {
                delay--;
            }
        }
        port = pinst->ioport.nRST;
        bit  = pinst->ioidx.nRST;
        gioSetBit(port, bit, 1U);

        drv2HcmsClear();
    }

    drv2HcmsCtrlRegWrite(CTRLW1 | CTRLW1_DOUT);
    drv2HcmsCtrlRegWrite(CTRLW0 | CTRLW0_NORMAL
        | (uint8_t)(params->peakCurBrt << CTRLW0_PEAKCURBRT_SHIFT)
        | (uint8_t)(params->pwmBrt << CTRLW0_PWMBRT_SHIFT));

    return 0;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-091 (Trace to: SLD-AMC01-091)
* Function:     drv2ModeLedSet
* Description:  模式灯控制接口
* Input:        ledNum led灯索引
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
*               2021/09/01    hdq       删除下载模式分支
****************************************************************************************************/
void drv2ModeLedSet(panelled_t ledNum)
{
    drv2PanelGreenLedSet(PanelLedRun,0U);
    drv2PanelGreenLedSet(PanelLedMaint,0U);

    drv2PanelGreenLedSet(ledNum,1U);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-092 (Trace to: SLD-AMC01-092)
* Function:     drv2LedTurnOn
* Description:  点亮LED
* Input:        ledNum       led序号
*               ledColor     1为红灯，0为绿灯
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2LedTurnOn(panelled_t ledNum, ledColor_t ledColor)
{
    if(RED == ledColor)
    {
        drv2PanelRedLedSet(ledNum,1U); /* 点亮红灯 */
    }
    else
    {
        drv2PanelGreenLedSet(ledNum,1U); /* 点亮绿灯 */
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-093 (Trace to: SLD-AMC01-093)
* Function:     drv2LedTurnOff
* Description:  熄灭LED
* Input:        ledNum      led序号
*               ledColor    1为红灯，0为绿灯
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2LedTurnOff(panelled_t ledNum, ledColor_t ledColor)
{
    if(RED == ledColor)
    {
        drv2PanelRedLedSet(ledNum,0U); /* 熄灭红灯 */
    }
    else
    {
        drv2PanelGreenLedSet(ledNum,0U); /* 熄灭绿灯 */
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-094 (Trace to: SLD-AMC01-094)
* Function:     drv2LcdStrDisp
* Description:  lcd显示字符串
* Input:        str      字符串
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2LcdStrDisp(const int8_t *str)
{
    uint8_t dotdata[(uint8_t)20U];
    uint32_t charcount = (uint32_t)0U;
    uint8_t charidx = (uint8_t)0U;

    /* 设置显示数据 */
    for (charcount = 0U; charcount < 4U; charcount++)
    {
        const uint32_t dotdataIdx = charcount * 5U;
        charidx = (uint8_t)str[charcount];
        /* each character have 5 bytes */
        dotdata[dotdataIdx + 0U] = g_hcms3906_charAsciiTbl[charidx][0U];
        dotdata[dotdataIdx + 1U] = g_hcms3906_charAsciiTbl[charidx][1U];
        dotdata[dotdataIdx + 2U] = g_hcms3906_charAsciiTbl[charidx][2U];
        dotdata[dotdataIdx + 3U] = g_hcms3906_charAsciiTbl[charidx][3U];
        dotdata[dotdataIdx + 4U] = g_hcms3906_charAsciiTbl[charidx][4U];
    }

    drv2HcmsRegWrite(HCMS_RegDot, dotdata, 20U);    /* 发送到3906 */
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-095 (Trace to: SLD-AMC01-095)
* Function:     drv2LcdInit
* Description:  lcd初始化
* Input:        mode   模式
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2LcdInit(hcms_int_mode_t mode)
{
    /* 配置初始化参数 */
    const hcms_params_t params = {
        .spi = {
            .regbase = spiREG3,
            .df      = SPI_FMT_0,
            .csmask  = (uint8_t)0x1U
        },
        .ioport = {
            .nRST = gioPORTB,
            .RS   = gioPORTB,
            .SEL  = gioPORTB,
            .OSC  = hetPORT1,
            .BL   = gioPORTB,
        },
        .ioidx = {
            .nRST = (uint32_t)4U,
            .RS   = (uint32_t)5U,
            .SEL  = 6U,
            .OSC  = 31U,
            .BL   = (uint32_t)7U,
        },
        .peakCurBrt = (uint8_t)0U,
        .pwmBrt     = (uint8_t)8U
    };

    (void)drv2HcmsInit(mode,&params);    /* 设置初始化 */
}

/**************************************************************************************************
* Identifier:   SCOD-AMC01-096 (Trace to: SLD-AMC01-096)
* Function:     drv2LedInit
* Description:  led初始化
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
*               2021/09/01    hdq       删除闭锁/下装模式分支
**************************************************************************************************/
static void drv2LedInit(void)
{
    /* 熄灭所示绿灯 */
    drv2PanelGreenLedSet(PanelLedRun,0U);
    drv2PanelGreenLedSet(PanelLedMaint,0U);
    drv2PanelGreenLedSet(PanelLedErr,0U);

    /* 熄灭所有红灯 */
    drv2PanelRedLedSet(PanelLedRun,0U);
    drv2PanelRedLedSet(PanelLedMaint,0U);
    drv2PanelRedLedSet(PanelLedErr,0U);
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-098 (Trace to: SLD-AMC01-098)
* Function:     drv2DisplayInit
* Description:  初始化显示接口
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2018/12/27    LP        Created
****************************************************************************************************/
void drv2DisplayInit(void)
{
    drv2LedInit();                      /* 调用led初始化接口 */
    drv2LcdInit(HCMS_INIT_MODE_START);  /* 调用lcd初始化接口 */
}

