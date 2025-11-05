/***************************************************************************************************
*Filename:     error_class.c
*Purpose:      故障处理模块，错误码显示
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
***************************************************************************************************/
#include "global.h"
#include "tm.h"
#include "info_class.h"
#include "libs.h"
#include "panel.h"
#include "cfg_class.h"
#include "error_class.h"
#include "com_hw.h"
#include "wtd.h"
#include "standby.h"
#include "maintain.h"

static int32_t errCodeProcess(mpuPrivInfo_t state,int8_t *errString);
static void normalDisplay(void);

/*********************************************************************
* Identifier:   SCOD-AMC01-059 (Trace to: SLD-AMC01-059)
* Function:     errStopHandleCycle
* Description:  严重故障时，显示故障码后系统停止
* Input:        none
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/16        
***********************************************************************/
void errStopHandleCycle(void)
{
    drv2LedTurnOff(PanelLedRun, GREEN);
    drv2LedTurnOff(PanelLedMaint, GREEN);
    drv2LedTurnOn(PanelLedErr, RED);
    drv2LedTurnOff(PanelLedSync, GREEN);
    
    for(;;)
    {
        com1ReportArmState(ARM_ERROR);
        if(CFG_REDUNDANCY == cfgLocalPfMode())
        {
            infoSetMpuMS(MPU_SLAVE);
        }
        /*停机 喂狗 */
        wdFeed(WD_1);
        tmDelayms(15U);
        (void)errHandle();
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-060 (Trace to: SLD-AMC01-060)
* Function:     errStopHandleInit
* Description:  开机时严重故障停机处理
* Input:        err : 错误码
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/16        
***********************************************************************/
void errStopHandleInit(int32_t err)
{
    int8_t errString[4] = {0};

    drv2LedTurnOff(PanelLedRun, GREEN);
    drv2LedTurnOff(PanelLedMaint, GREEN);
    drv2LedTurnOn(PanelLedErr, RED);
    drv2LedTurnOff(PanelLedSync, GREEN);
    
    for(;;)
    {  
        com1ReportArmState(ARM_ERROR);
        wdFeed(WD_1);
        tmDelayms(15U);
        errString[0] = 'E';
        errString[1] = (int8_t)(err / 100) + '0';
        errString[2] = (int8_t)((err % 100) / 10) + '0';
        errString[3] = (int8_t)(err % 10) + '0';
        drv2LcdStrDisp((const int8_t *)errString);
    }  
}

/*********************************************************************
* Identifier:   SCOD-AMC01-061 (Trace to: SLD-AMC01-061)
* Function:     errCodeProcess
* Description:  错误码数据处理
* Input:        state 主控状态
*               errString 错误码显示地址
* Output:       none
* Return:       errLevel 错误等级
*               
* Others:
* Log:          Date          Author    Modified
*               2019/07/04       
***********************************************************************/
static int32_t errCodeProcess(mpuPrivInfo_t state, int8_t *errString)
{
    int32_t errCodeOffset[5] = {400,300,200,200, 0};
                                /*1级  2级  3级  4级 5级 */                                   
    int32_t errLevel = 0;
    int32_t err = 0;

    uint32_t value = 0U;

    uint64_t cpuErr = 0ULL;
    uint64_t stErr = 0ULL;
    uint64_t pfErr = 0ULL;

    (void)memcpy((void*)&cpuErr, (const void*)state.cpuErr, sizeof(uint64_t));
    (void)memcpy((void*)&stErr, (const void*)state.stErr, sizeof(uint64_t));
    (void)memcpy((void*)&pfErr, (const void*)state.pfErr, sizeof(uint64_t));
    if(cpuErr != 0U) /* 1级故障 */
    {
        err = findFirstOne(cpuErr);
        errLevel = 1;
    }
    else if(stErr != 0U)/* 2级故障 */
    {
        err = findFirstOne(stErr);
        errLevel = 2;
    }
    else if(pfErr != 0U)
    {
        err = findFirstOne(pfErr);
        if(err < 32)                       /* pfErr的 bit[31: 0] 表示 3级故障 最多32个3级故障 */
        {
            errLevel = 3;
        }
        else                                /* pfErr的 bit[63:32] 表示 4级故障  最多32个4级故障*/
        {
            errLevel = 4;
        }
    }
    else if(state.error != 0U)
    {
        err = (int32_t)state.error;
        errLevel = 5;
    }
    else 
    {
        /* 无故障 */
        errLevel = 0;
    }

    if(errLevel != 0)/* 故障索引转换为故障码    */
    {
        err += errCodeOffset[errLevel - 1];
        /* 错误码转换为显示字符串 */
        switch((uint32_t)errLevel)
        {
            case 1U:
            case 2U:
            case 3U:
                errString[0] = 'E';
                errString[1] = (int8_t)(err / 100) + '0';
                errString[2] = (int8_t)((err % 100) / 10) + '0';
                errString[3] = (int8_t)(err % 10) + '0';
                break;
            case 4U:
                errString[0] = 'W';
                errString[1] = (int8_t)(err / 100) + '0';
                errString[2] = (int8_t)((err % 100) / 10) + '0';
                errString[3] = (int8_t)(err % 10) + '0';
                break;
            case 5U:
                value = (uint32_t)err;
                value = (value >> 24U) & 0xFFU;
                errString[0] = (int8_t)value + '0';

                value = (uint32_t)err;
                value = (value >> 16U) & 0xFFU;
                errString[1] = (int8_t)value + '0';

                value = (uint32_t)err;
                value = (value >> 8U) & 0xFFU;
                errString[2] = (int8_t)value + '0';

                value = (uint32_t)err; /* 16 进制显示*/
                value = (value >> 0U) & 0xFFU;
                if(value < 10U)
                {
                    errString[3] = (int8_t)value + '0';
                }
                else
                {
                    errString[3] = (int8_t)(value - 10U) + 'A';
                }
                break;
            default:
                /* do nothing */
                break;
        }
    }
    
    return errLevel;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-062 (Trace to: SLD-AMC01-062)
* Function:     normalDisplay
* Description:  显示无故障标识$
* Input:        none
* Output:       none
* Return:       none
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/16       
***********************************************************************/
static void normalDisplay(void)
{
    static uint32_t cnt500ms = 0U;
    static uint8_t idx = 0U;
    const  int8_t errCode[4][4] =
        {
            {'$','=','=','='},
            {'=','$','=','='},
            {'=','=','$','='},
            {'=','=','=','$'}
        };

    if(cnt500ms >=  ((uint32_t)500000U / tmGetMaxCycleTick()))
    {
        cnt500ms = 0U;
        idx++;
        drv2LcdStrDisp((const int8_t *)errCode[idx % 4U]);
    }
    cnt500ms++;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-063 (Trace to: SLD-AMC01-063)
* Function:     errHandle
* Description:  故障显示处理
* Input:        none
* Output:       none
* Return:       errLevel 错误等级
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/16       
***********************************************************************/
int32_t errHandle(void)
{    
    int8_t errString[4] = {'$','=','=','='};
    static int8_t dispString[4] = {'$','=','=','='};

    int32_t errLevel = 0;   /* 主模块 故障等级 */
    
    static uint8_t dispCnt = 0U;   /* 当前故障码已显示周期数 */
    static uint8_t errFlag = 0U;
    
    /* 更新非主控错误槽位信息 */
    infoUpdateCardError();

    errLevel = errCodeProcess(infoGetAddr()->moduleInfo[g_localSlot].difInfo.mpuInfo, errString);
    if(errLevel != 0)
    { 
        if(NONE_ERR == errFlag)
        {
            (void)memcpy((void*)dispString,(const void*)errString,4U);
            errFlag = SHOW_ERR;
        }
        
        if(memcmp((const void*)dispString, (const void*)errString, 4U) != 0)/* 错误码改变 */
        {
            dispCnt++;
        }
        else
        {
            dispCnt = 0;
        }
        
        if(dispCnt > ERR_CODE_DISP_CONT)
        {
            (void)memcpy((void*)dispString,(const void*)errString,4U);
            dispCnt = 0U;
        } 
    }
    else
    {
        if(SHOW_ERR == errFlag)
        {
            dispCnt++;
        }
        
        if(dispCnt > ERR_CODE_DISP_CONT)
        {
            errFlag = NONE_ERR;
            dispCnt = 0U;
            dispString[0] = '$';
            dispString[1] = '=';
            dispString[2] = '=';
            dispString[3] = '=';
        }
    }

    /* 显示 */
    if(errFlag != NONE_ERR)
    {
        drv2LcdStrDisp((const int8_t *)dispString);
    }
    else
    {
        normalDisplay();
    }
    
    return errLevel;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-064 (Trace to: SLD-AMC01-064)
* Function:     errEmifCrcHandle
* Description:  CRC故障处理函数
* Input:        err 错误码
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2019/9/21     hdq
****************************************************************************************************/
void errEmifCrcHandle(int32_t err)
{
    static uint32_t crcErrCnt = 0U;

    if(EMIF_CRC_ERROR == err)
    {
        crcErrCnt++;
        if(crcErrCnt >= 1U)
        {
            errStopHandleInit(COM_ARM_EMIF_READ_CRC);
        }
    }
    else
    {
        crcErrCnt = 0U;
    }
}

