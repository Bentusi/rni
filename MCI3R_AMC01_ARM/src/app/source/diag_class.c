/***************************************************************************************************
 * Filename: diag_class.c
 * Purpose:  应用层的诊断接口
 * Date:         Author      Modified 
 * 2021-09-23    hdq         Create  
 * 说明（由于产生异常或者中断以下诊断被排除）：
 *              PSCON_ERROR_FORCING_FAULT_INJECT->sl_esm_low_intr_handler中断
 *              PSCON_SELF_TEST_ERROR_FORCING_FAULT_INJECT->sl_esm_low_intr_handler中断
 *              MEMINTRCNT_RESERVED_ACCESS->_excpt_vec_abort_data异常
 *              MAINPERIPHINTRCNT_RESERVED_ACCESS->_excpt_vec_abort_data异常
 *              PERIPHSEGINTRCNT_RESERVED_ACCESS->_excpt_vec_abort_data异常
 *              FLASH_ECC_TEST_MODE_2BIT->sl_esm_high_intr_handler中断
 *              CCMR5F_CPUCOMP_ERROR_FORCING_TEST_FAULT_INJECT->sl_esm_high_intr_handler中断
 *              CCMR5F_PDCOMP_ERROR_FORCING_TEST_FAULT_INJECT->sl_esm_high_intr_handler中断
 *              CCMR5F_INMCOMP_ERROR_FORCING_TEST_FAULT_INJECT->sl_esm_high_intr_handler中断
 *              SRAM_ECC_ERROR_FORCING_1BIT->sl_esm_high_intr_handler中断
 *              SRAM_ECC_ERROR_FORCING_2BIT->sl_esm_high_intr_handler中断
 *              CCMR5F_CPUCOMP_ERROR_FORCING_TEST->sl_esm_high_intr_handler中断
 *              CCMR5F_PDCOMP_ERROR_FORCING_TEST->sl_esm_high_intr_handler中断
 *              CCMR5F_INMCOMP_ERROR_FORCING_TEST->sl_esm_high_intr_handler中断
 ***************************************************************************************************/
#include "bsp.h"
#include "diag_class.h"
#include "info_class.h"
#include "flash.h"
#include "tm.h"
#include "sl_api.h"
#include "monitor.h"
#include "para_class.h"
#include "HL_sys_core.h"

typedef int32_t (*pDiagFun)(void);

static int32_t diagEfuseSelfTest(void);
static int32_t diagEfuseEcc(void);
static int32_t diagSramSelfTest(void);
static int32_t diagPsonSelfTest(void);
static int32_t diagPsonErrorForcing(void);
static int32_t diagPsonSelfTestErrorForcing(void);
static int32_t diagPsonPmaTest(void);
static int32_t diagPerSegIntrCntUnpriAccS1(void);
static int32_t diagPerSegIntrCntUnpriAccS2(void);
static int32_t diagPerSegIntrCntUnpriAccS3(void);
static int32_t diagAdc1SramParityTest(void);
static int32_t diagFlashEccTestMode1bit(void);
static int32_t diagPbistOn2PortMem(uint64_t *p2portMem, uint32_t size);
static int32_t diagPbistOnRomMem(void);
static int32_t diagCcmCpuCompSelfTest(void);
static int32_t diagCcmCpuCompSelfTestErrForce(void);
static int32_t diagCcmPdCompSelfTest(void);
static int32_t diagCcmPdCompSelfTestErrForce(void);
static int32_t diagCcmInmCompSelfTest(void);
static int32_t diagCcmInmCompSelfTestErrForce(void);
static int32_t diagDcc(void);
static int32_t diagSegment1(void);
static int32_t diagSegment2(void);
static int32_t diagSegment3(void);
static int32_t diagSegment4(void);
static int32_t checkDiagResult(SL_SelfTest_Result x, int32_t y);

/*
*  分片段诊断函数指针数组
*/
static pDiagFun s_diagSeg[]=
{
    diagSegment1,
    diagSegment2,
    diagSegment3,
    diagSegment4,
};

/**************************************************************************************************
* Identifier:   SCOD-AMC01-285 (Trace to: SLD-AMC01-285)
* Function:     checkDiagResult
* Description:  校验诊断结果
* Input:        x                   --- 诊断结果
*               y                   --- 诊断过程是否成功
* Output:       none
* Return:       TRUE                --- 诊断成功
*               FALSE               --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
**************************************************************************************************/
static int32_t checkDiagResult(SL_SelfTest_Result x, int32_t y)
{
    int32_t ret = FALSE;/* for misra2004 */

    /* x为ST_PASS，y为TRUE则返回真，否则为假 */
    if ((ST_PASS == (x)) && (TRUE == (y)))
    {
        ret = TRUE;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-288 (Trace to: SLD-AMC01-288)
* Function:     diagEfuseSelfTest
* Description:  efus自诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                            --- 诊断通过
*                ERR_EFUSE_SELF_TEST              --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagEfuseSelfTest(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t retVal = FALSE;       /* For function return values */
    SL_EFuse_Config stConfigEFuse;         /* EFuse self test configuration */

    /* Run EFuse self tests */
    stConfigEFuse.numPatterns      = (uint32)600u;
    stConfigEFuse.seedSignature    = (uint32)0x5362F97Fu;
    stConfigEFuse.failInfo.stResult= ST_FAIL;
    stConfigEFuse.failInfo.failInfo= EFUSE_ERROR_NONE;
    retVal = SL_SelfTest_EFuse(EFUSE_SELF_TEST_STUCK_AT_ZERO, TRUE, &stConfigEFuse);

    if(0 == checkDiagResult(stConfigEFuse.failInfo.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_EFUSE_SELF_TEST;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-289 (Trace to: SLD-AMC01-289)
* Function:     diagEfuseEcc
* Description:  efuse ecc诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                诊断通过
*               ERR_EFUSE_ECC         诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagEfuseEcc(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t         retVal = FALSE;     /* For function return values */
    SL_EFuse_Config          stConfigEFuse;      /* EFuse self test configuration */

    stConfigEFuse.numPatterns      = (uint32)600U;
    stConfigEFuse.seedSignature    = (uint32)0x5362F97FU;
    stConfigEFuse.failInfo.stResult= ST_FAIL;
    stConfigEFuse.failInfo.failInfo= EFUSE_ERROR_NONE;
    retVal = SL_SelfTest_EFuse(EFUSE_SELF_TEST_ECC, TRUE, &stConfigEFuse);

    while (0 == SL_SelfTest_Status_EFuse(&stConfigEFuse.failInfo))/*等待诊断结束 */
    {
        ;
    }

    if(0 == checkDiagResult(stConfigEFuse.failInfo.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_EFUSE_ECC;
    }
    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-290 (Trace to: SLD-AMC01-290)
* Function:     diagSramSelfTest
* Description:  sram自诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                      诊断通过
*               ERR_SRAM_SELF_TEST          诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagSramSelfTest(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t         retVal = FALSE;     /* For function return values */
    SL_SelfTest_Result       failInfoTCMRAM;     /* TCM RAM Failure  information */

    /* Run RAD Self tests on sRAM */
    failInfoTCMRAM = ST_FAIL;
    retVal = SL_SelfTest_SRAM(SRAM_RADECODE_DIAGNOSTICS, TRUE, &failInfoTCMRAM);

    if(0 == checkDiagResult(failInfoTCMRAM, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_SRAM_SELF_TEST;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-291 (Trace to: SLD-AMC01-291)
* Function:     diagPsonSelfTest
* Description:  pson自测试
* Input:        none
* Output:       none
* Return:       ERR_NO                     诊断通过
*               ERR_PSON_SELF_TEST         诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagPsonSelfTest(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t         retVal = FALSE;     /* For function return values */
    SL_PSCON_FailInfo        failInfoPSCON;      /* PSCON failure information */

    /* PSCON Selftests */
    failInfoPSCON.stResult = ST_FAIL;
    failInfoPSCON.pdFailure = (uint32)0x0U;
    failInfoPSCON.mdFailure = (uint32)0x0U;
    retVal = SL_SelfTest_PSCON(PSCON_SELF_TEST, TRUE, &failInfoPSCON);

    if(0 == checkDiagResult(failInfoPSCON.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_PSON_SELF_TEST;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-292 (Trace to: SLD-AMC01-292)
* Function:     diagPsonErrorForcing
* Description:  pson 错误强制测试
* Input:        none
* Output:       none
* Return:       ERR_NO                            --- 诊断通过
*               ERR_PSCON_ERR_FORCE               --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagPsonErrorForcing(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t         retVal = FALSE;             /* For function return values */
    SL_PSCON_FailInfo        failInfoPSCON;      /* PSCON failure information */

    failInfoPSCON.stResult = ST_FAIL;
    failInfoPSCON.pdFailure = (uint32)0x0U;
    failInfoPSCON.mdFailure = (uint32)0x0U;
    retVal = SL_SelfTest_PSCON(PSCON_ERROR_FORCING, TRUE, &failInfoPSCON);

    if(0 == checkDiagResult(failInfoPSCON.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_PSCON_ERR_FORCE;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-293 (Trace to: SLD-AMC01-293)
* Function:     diagPsonSelfTestErrorForcing
* Description:  pson自测试错误强制测试
* Input:        none
* Output:       none
* Return:       ERR_NO                            ---  诊断通过
*               ERR_PSCON_SL_ERR_FORCE            --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagPsonSelfTestErrorForcing(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t         retVal = FALSE;             /* For function return values */
    SL_PSCON_FailInfo        failInfoPSCON;      /* PSCON failure information */

    failInfoPSCON.stResult = ST_FAIL;
    failInfoPSCON.pdFailure = (uint32)0x0U;
    failInfoPSCON.mdFailure = (uint32)0x0U;
    retVal = SL_SelfTest_PSCON(PSCON_SELF_TEST_ERROR_FORCING, TRUE, &failInfoPSCON);

    if(0 == checkDiagResult(failInfoPSCON.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_PSCON_SL_ERR_FORCE;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-294 (Trace to: SLD-AMC01-294)
* Function:     diagPsonPmaTest
* Description:  pson pma测试
* Input:        none
* Output:       none
* Return:       ERR_NO                       --- 诊断通过
*               ERR_PSON_PMA_TEST            --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagPsonPmaTest(void)
{
    int32_t        ret = ERR_NO;
    SL_PSCON_FailInfo        failInfoPSCON;      /* PSCON failure information */

    failInfoPSCON.stResult = ST_FAIL;
    failInfoPSCON.pdFailure = (uint32)0x0U;
    failInfoPSCON.mdFailure = (uint32)0x0U;
    if(0 != SL_SelfTest_PSCON(PSCON_PMA_TEST, TRUE, &failInfoPSCON))
    {
        ret =  ERR_PSON_PMA_TEST;/* 检测诊断结果 */
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-295 (Trace to: SLD-AMC01-295)
* Function:     diagPerSegIntrCntUnpriAccS1
* Description:  外围段S1特权模式访问诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                --- 诊断通过
*               ERR_PERIPHT_UNPRI_ACC_S1              --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagPerSegIntrCntUnpriAccS1(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t         retVal = FALSE;             /* For function return values */

    /* Peripheral Segment1 interconnect - unprivileged access of protected location -
     * Note: Segment Instance is not used. */
    retVal = SL_SelfTest_PeripheralSegmentInterconnect(PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS,
                                                       SL_PERIPH_SEGMENT1,
                                                       (volatile uint32*)0xFFFFF000U,
                                                       (volatile uint32*)0xFFFF1020u,
                                                       16u);

    if(0 == checkDiagResult(ST_PASS, !retVal))/* 检测诊断结果 */
    {
        ret =  ERR_PERIPHT_UNPRI_ACC_S1;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-296 (Trace to: SLD-AMC01-296)
* Function:     diagPerSegIntrCntUnpriAccS2
* Description:  外围段S2连接诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                  --- 诊断通过
*               ERR_PERIPH_UNPRI_ACC_S2                 --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagPerSegIntrCntUnpriAccS2(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t         retVal = FALSE;             /* For function return values */

    /* Peripheral Segment2 interconnect - unprivileged access of protected location -
     * Note: Segment Instance is not used. */
    retVal = SL_SelfTest_PeripheralSegmentInterconnect(PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS, SL_PERIPH_SEGMENT2, (volatile uint32*)0xFFFFF000U, (volatile uint32*)0xFFFF1020u, 16u);

    if(0 == checkDiagResult(ST_PASS, !retVal))/* 检测诊断结果 */
    {
        ret =  ERR_PERIPH_UNPRI_ACC_S2;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-297 (Trace to: SLD-AMC01-297)
* Function:     diagPerSegIntrCntUnpriAccS3
* Description:  外围段S3权模式访问诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                  --- 诊断通过
*               ERR_PERIPH_UNPRI_ACC_S3                 --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagPerSegIntrCntUnpriAccS3(void)
{
    int32_t        ret = ERR_NO;
    volatile int32_t         retVal = FALSE;             /* For function return values */

    /* Peripheral Segment3 interconnect - unprivileged access of protected location -
     * Note: Segment Instance is not used. */
    retVal = SL_SelfTest_PeripheralSegmentInterconnect(PERIPHSEGINTRCNT_UNPRIVELEGED_ACCESS, SL_PERIPH_SEGMENT3, (volatile uint32*)0xFFFFF000U, (volatile uint32*)0xFFFF1020u, 16u);

    if(0 == checkDiagResult(ST_PASS, !retVal))/* 检测诊断结果 */
    {
        ret =  ERR_PERIPH_UNPRI_ACC_S3;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-298 (Trace to: SLD-AMC01-298)
* Function:     diagAdc1SramParityTest
* Description:  adc1诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                    --- 诊断通过
*               ERR_ADC1_SRAM_PARITY_TEST                 --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagAdc1SramParityTest(void)
{
    int32_t   ret = ERR_NO;
    volatile int32_t    retVal = FALSE;          /* For function return values */
    SL_ADC_Config       adcConfig;
    SL_ADC_Pinstatus    pinStatus;

    /* ADC SRAM Parity Test */
    adcConfig.adc_channel = (uint8)0u;
    adcConfig.adcbase = adcREG1;

    retVal = SL_SelfTest_ADC(ADC_SRAM_PARITY_TEST, FALSE, &adcConfig, &pinStatus);
    if(0 == checkDiagResult(ST_PASS, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_ADC1_SRAM_PARITY_TEST;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-299 (Trace to: SLD-AMC01-299)
* Function:     dCacheFlush
* Description:  flush cache
* Input:        addr         --- 地址
*               len          --- 长度
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2019/01/04    LP        Created
****************************************************************************************************/
void dCacheFlush(uint32 addr,uint32 len)
{
    uint32 count = 0x0U;
	uint32 i = 0x0U;
	uint32 address = 0x0U;

    if(len > 0x0U)
    {
        count =  len / 0x20u;/*取0x20的整数倍*/
        if((len % 0x20u) != 0x0u)/*不满0x20的设置为0x20*/
        {
            count = count + 0x1U;
        }

		for(i = 0x0U; i < count; i++)
		{
			address = addr + (i * 0x20u);
        	_dCacheFlush_(address);/*flush cache*/
		}
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-300 (Trace to: SLD-AMC01-300)
* Function:     diagFlashEccTestMode1bit
* Description:  FLASH ECC 模式1诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                       --- 诊断通过
*               ERR_FLASH_ECC_TEST_MODE_1BIT                 --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagFlashEccTestMode1bit(void)
{
    int32_t   ret = ERR_NO;
    volatile int32_t    retVal = FALSE;          /* For function return values */
    SL_SelfTest_Result       failInfoFlash;      /* Flash Self test failure information */

    /* Run 1Bit ECC test on Flash */
    failInfoFlash = ST_FAIL;
    dCacheFlush(0U,32U);
    retVal = SL_SelfTest_Flash(FLASH_ECC_TEST_MODE_1BIT, TRUE, &failInfoFlash);
    if(0 == checkDiagResult(failInfoFlash, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_FLASH_ECC_TEST_MODE_1BIT;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-301 (Trace to: SLD-AMC01-301)
* Function:     diagPbistOn2PortMem
* Description:  双口ram诊断
* Input:        p2portMem 内存地址
*               size 长度
* Output:       none
* Return:       ERR_NO                                            --- 诊断通过
*               ERR_PBIST_MARCH13N_2PORT_MEMORY                   --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
*               2021/10/21    hdq       添加参数、可重入
****************************************************************************************************/
static int32_t diagPbistOn2PortMem(uint64_t *p2portMem, uint32_t size)
{
    int32_t ret = ERR_NO;
    volatile boolean retVal = FALSE;                /* For function return values */
    SL_PBIST_FailInfo   failInfoPBISTOthers;        /* PBIST Failure information for non-TCM memories */
    int32_t i= 0;

    for(i = 0; i < size; i++)
    {

        retVal = SL_SelfTest_PBIST( PBIST_EXECUTE, p2portMem[i], PBISTALGO_MARCH13N_2PORT);/* only run March13N algorithm. */

        while (0 == SL_SelfTest_Status_PBIST(&failInfoPBISTOthers))
        {
            ; /* 等待检测完成*/
        }

        retVal &= SL_SelfTest_PBIST_StopExec();/*停止运行pbist*/
        if(0 == checkDiagResult(failInfoPBISTOthers.stResult, retVal))/* 检测诊断结果 */
        {
            ret =  ERR_PBIST_MARCH13N_2PORT_MEMORY;
            break;
        }
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-302 (Trace to: SLD-AMC01-302)
* Function:     diagPbistOnRomMem
* Description:  rom诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                            --- 诊断通过
*               ERR_PBIST_MARCH13N_ROM_MEMORY                     --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagPbistOnRomMem(void)
{
    int32_t   ret = ERR_NO;
    volatile boolean retVal = FALSE;                /* For function return values */
    SL_PBIST_FailInfo   failInfoPBISTOthers;        /* PBIST Failure information for non-TCM memories */
    uint32 i = 0U;
    uint32 j = 0U;

    uint64 allRomMem[4] = {PBIST_RAMGROUP_01_PBIST_ROM,PBIST_RAMGROUP_02_STC1_1_ROM_R5,PBIST_RAMGROUP_03_STC1_2_ROM_R5, PBIST_RAMGROUP_04_STC2_ROM_NHET};
    uint32 allRomAlgos[2] = {PBISTALGO_TRIPLE_READ_SLOW_READ,PBISTALGO_TRIPLE_READ_FAST_READ};

    /*running pbist on all ROM memories*/
    for(i = 0U;i < (sizeof(allRomMem)/sizeof(uint64));i++)
    {
        for(j =0U;j<(sizeof(allRomAlgos)/sizeof(uint32));j++)
        {
            retVal = SL_SelfTest_PBIST( PBIST_EXECUTE,
                                        allRomMem[i],
                                        allRomAlgos[j]);

            while (0 == SL_SelfTest_Status_PBIST(&failInfoPBISTOthers))
            {
                ; /* 等待检测完成*/
            }

            retVal &= SL_SelfTest_PBIST_StopExec();/*停止运行pbist*/
            if(0 == checkDiagResult(failInfoPBISTOthers.stResult, retVal))/* 检测诊断结果 */
            {
                ret =  ERR_PBIST_MARCH13N_ROM_MEMORY;
                break;
            }
        }
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-306 (Trace to: SLD-AMC01-306)
* Function:     diagCcmCpuCompSelfTest
* Description:  ccm cpu自诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                    --- 诊断通过
*               ERR_CCMR5F_CPUCOMP_SL                     --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagCcmCpuCompSelfTest(void)
{
    int32_t   ret = ERR_NO;
    volatile int32_t    retVal = FALSE;             /* For function return values */
    SL_CCMR5F_FailInfo  failInfoCCMR5F;             /* CCMR5 Self Test fail info */

    failInfoCCMR5F.stResult = ST_FAIL;
    failInfoCCMR5F.failInfo = CCMR5F_ST_ERR_COMPARE_MATCH;

    retVal = SL_SelfTest_CCMR5F(CCMR5F_CPUCOMP_SELF_TEST,  TRUE, &failInfoCCMR5F);

    if(0 == checkDiagResult(failInfoCCMR5F.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_CCMR5F_CPUCOMP_SL;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-307 (Trace to: SLD-AMC01-307)
* Function:     diagCcmCpuCompSelfTestErrForce
* Description:  ccm cpu自测试错误强制诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                    --- 诊断通过
*               ERR_CCMR5F_CPUCOMP_SL_ERR_FORCE           --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagCcmCpuCompSelfTestErrForce(void)
{
    int32_t   ret = ERR_NO;
    volatile int32_t    retVal = FALSE;             /* For function return values */
    SL_CCMR5F_FailInfo  failInfoCCMR5F;             /* CCMR5 Self Test fail info */

    failInfoCCMR5F.stResult = ST_FAIL;
    failInfoCCMR5F.failInfo = CCMR5F_ST_ERR_COMPARE_MATCH;

    retVal = SL_SelfTest_CCMR5F(CCMR5F_CPUCOMP_SELF_TEST_ERROR_FORCING, TRUE, &failInfoCCMR5F);
    if(0 == checkDiagResult(failInfoCCMR5F.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_CCMR5F_CPUCOMP_SL_ERR_FORCE;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-308 (Trace to: SLD-AMC01-308)
* Function:     diagCcmPdCompSelfTest
* Description:  ccm pd自诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                            --- 诊断通过
*               ERR_CCMR5F_PDCOMP_SL              --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagCcmPdCompSelfTest(void)
{
    int32_t   ret = ERR_NO;
    volatile int32_t    retVal = FALSE;             /* For function return values */
    SL_CCMR5F_FailInfo  failInfoCCMR5F;             /* CCMR5 Self Test fail info */

    failInfoCCMR5F.stResult = ST_FAIL;
    failInfoCCMR5F.failInfo = CCMR5F_ST_ERR_COMPARE_MATCH;

    retVal = SL_SelfTest_CCMR5F(CCMR5F_PDCOMP_SELF_TEST,  TRUE, &failInfoCCMR5F);

    if(0 == checkDiagResult(failInfoCCMR5F.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_CCMR5F_PDCOMP_SL;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-309 (Trace to: SLD-AMC01-309)
* Function:     diagCcmPdCompSelfTestErrForce
* Description:  ccm pd自测试错误强制诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                      --- 诊断通过
*               ERR_CCMR5F_PDCOMP_SL_ERR_FORCE              --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagCcmPdCompSelfTestErrForce(void)
{
    int32_t   ret = ERR_NO;
    volatile int32_t    retVal = FALSE;             /* For function return values */
    SL_CCMR5F_FailInfo  failInfoCCMR5F;             /* CCMR5 Self Test fail info */

    failInfoCCMR5F.stResult = ST_FAIL;
    failInfoCCMR5F.failInfo = CCMR5F_ST_ERR_COMPARE_MATCH;

    retVal = SL_SelfTest_CCMR5F(CCMR5F_PDCOMP_SELF_TEST_ERROR_FORCING, TRUE, &failInfoCCMR5F);

    if(0 == checkDiagResult(failInfoCCMR5F.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_CCMR5F_PDCOMP_SL_ERR_FORCE;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-310 (Trace to: SLD-AMC01-310)
* Function:     diagCcmInmCompSelfTest
* Description:  ccm inm自诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                             --- 诊断通过
*               ERR_CCMR5F_INMCOMP_SL              --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagCcmInmCompSelfTest(void)
{
    int32_t   ret = ERR_NO;
    volatile int32_t    retVal = FALSE;             /* For function return values */
    SL_CCMR5F_FailInfo  failInfoCCMR5F;             /* CCMR5 Self Test fail info */

    failInfoCCMR5F.stResult = ST_FAIL;
    failInfoCCMR5F.failInfo = CCMR5F_ST_ERR_COMPARE_MATCH;

    retVal = SL_SelfTest_CCMR5F(CCMR5F_INMCOMP_SELF_TEST,  TRUE, &failInfoCCMR5F);

    if(0 == checkDiagResult(failInfoCCMR5F.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_CCMR5F_INMCOMP_SL;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-311 (Trace to: SLD-AMC01-311)
* Function:     diagCcmInmCompSelfTestErrForce
* Description:  ccm inm自诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                                       --- 诊断通过
*               ERR_CCMR5F_INMCOMP_SL_ERR_FORCE              --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
****************************************************************************************************/
static int32_t diagCcmInmCompSelfTestErrForce(void)
{
    int32_t   ret = ERR_NO;
    volatile int32_t    retVal = FALSE;             /* For function return values */
    SL_CCMR5F_FailInfo  failInfoCCMR5F;             /* CCMR5 Self Test fail info */

    failInfoCCMR5F.stResult = ST_FAIL;
    failInfoCCMR5F.failInfo = CCMR5F_ST_ERR_COMPARE_MATCH;

    retVal = SL_SelfTest_CCMR5F(CCMR5F_INMCOMP_SELF_TEST_ERROR_FORCING, TRUE, &failInfoCCMR5F);

    if(0 == checkDiagResult(failInfoCCMR5F.stResult, retVal))/* 检测诊断结果 */
    {
        ret =  ERR_CCMR5F_INMCOMP_SL_ERR_FORCE;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-312 (Trace to: SLD-AMC01-312)
* Function:     diagDcc
* Description:  DCC诊断
* Input:        none
* Output:       none
* Return:       ERR_NO                --- 诊断通过
*               ERR_DCC               --- 诊断失败
* Others:
* Log:          Date          Author    Modified
*               2019/01/03    LP        Created
*               2024/06/14    WXB       删除注释保存的代码
****************************************************************************************************/
static int32_t diagDcc(void)
{
    int32_t   ret = ERR_NO;

    if(dccGetErrStatus(dccREG1) == (uint32)1U)

    {
        ret = ERR_DCC;
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-313 (Trace to: SLD-AMC01-313)
* Function:     diagSegment1
* Description:  ARM分片自诊断1
* Input:        none
* Output:       none
* Return:       none
* Others:       实际测试 耗时时间 约 1580 us
* Log:          Date          Author    Modified
*               2017/01/22    zql        
****************************************************************************************************/
static int32_t diagSegment1(void)
{
    int32_t ret = ERR_NO;  
    
    ret = diagEfuseSelfTest();    /* 0 */
    
    if(ERR_NO == ret)
    {
        ret = diagSramSelfTest();   /* 2 */
    }
    
    /*pson*/
    if(ERR_NO == ret)
    {
        ret = diagPsonSelfTest(); /* 3 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagPsonErrorForcing(); /* 4 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagPsonSelfTestErrorForcing(); /* 6 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagPsonPmaTest();/* 8 */
    }
    
    /*s1*/
    if(ERR_NO == ret)
    {
        ret = diagPerSegIntrCntUnpriAccS1(); /* 12 */
    }
    
    /*s2*/
    if(ERR_NO == ret)
    {
        ret = diagPerSegIntrCntUnpriAccS2();/* 14 */
    }
    
    /*s3*/
    if(ERR_NO == ret)
    {
        ret = diagPerSegIntrCntUnpriAccS3();/* 16 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagAdc1SramParityTest();    /* 17 */
    }
    /*adc*/
    if(ERR_NO == ret)
    {
        ret = diagFlashEccTestMode1bit();  /* 18 */
    }

    if(ERR_NO == ret)
    {
        ret = diagPbistOnRomMem();     /* 22 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagCcmCpuCompSelfTest();     /* 23 */
    }

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-303 (Trace to: SLD-AMC01-303)
* Function:     diagSegment2
* Description:  自诊断接口
* Input:        none
* Output:       none
* Return:       none
* Others:       实际测试 耗时时间 约 1830 us
* Log:          Date          Author    Modified
*               2019/01/22    zql   
****************************************************************************************************/
static int32_t diagSegment2(void)
{
    int32_t ret = ERR_NO;

    /*common 2 port memories and algos*/
    uint64 all2portMem[4] =
    {
        PBIST_RAMGROUP_05_AWM1,
        PBIST_RAMGROUP_08_DMA,
        PBIST_RAMGROUP_10_MIBSPI1,
        PBIST_RAMGROUP_12_MIBSPI3,
    };

    ret = diagPbistOn2PortMem(all2portMem, sizeof(all2portMem)/sizeof(uint64_t)); /* 20 */

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-304 (Trace to: SLD-AMC01-304)
* Function:     diagSegment3
* Description:  自诊断接口－双口RAM诊断分片1
* Input:        none
* Output:       none
* Return:       none
* Others:       实际测试 耗时时间 约 1751 us
* Log:          Date          Author    Modified
*               2021/10/21    hdq
****************************************************************************************************/
static int32_t diagSegment3(void)
{
    int32_t ret = ERR_NO;

    /*common 2 port memories and algos*/
    uint64 all2portMem[4] =
    {
        PBIST_RAMGROUP_09_HTU1,
        PBIST_RAMGROUP_13_NHET1,
        PBIST_RAMGROUP_14_VIM,
        PBIST_RAMGROUP_17_ATB,
    };

    ret = diagPbistOn2PortMem(all2portMem, sizeof(all2portMem)/sizeof(uint64_t)); /* 20 */

    return ret;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-305 (Trace to: SLD-AMC01-305)
* Function:     diagSegment4
* Description:  自诊断接口－双口RAM诊断分片4
* Input:        none
* Output:       none
* Return:       none
* Others:       实际测试 耗时时间 约 1141 us
* Log:          Date          Author    Modified
*               2021/10/21    hdq
****************************************************************************************************/
static int32_t diagSegment4(void)
{
    int32_t ret = ERR_NO;

    /*common 2 port memories and algos*/
    uint64 all2portMem[3] =
    {
        PBIST_RAMGROUP_18_AWM2,
        PBIST_RAMGROUP_21_HTU2,
        PBIST_RAMGROUP_24_NHET2,
    };

    ret = diagPbistOn2PortMem(all2portMem, sizeof(all2portMem)/sizeof(uint64_t)); /* 20 */

    if(ERR_NO == ret)
    {
        ret = diagEfuseEcc();   /* 1 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagCcmCpuCompSelfTestErrForce();  /* 24 */
    }
    
    /*ccm for inm*/
    if(ERR_NO == ret)
    {
        ret = diagCcmPdCompSelfTest();   /* 26 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagCcmPdCompSelfTestErrForce(); /* 27 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagCcmInmCompSelfTest();/* 29 */
    }
    
    if(ERR_NO == ret)
    {
        ret = diagCcmInmCompSelfTestErrForce(); /* 30 */
    }

    /*dcc*/
    if(ERR_NO == ret)
    {
        ret = diagDcc();  /* 35 */
    }

    /*  对lcd 屏初始化一次 代替lcd屏诊断 */
    drv2LcdInit(HCMS_INIT_MODE_RUN);
    
    return ret;
}/* 实际测试 耗时时间 约 1570 us */

/****************************************************************************************************
* Identifier:   SCOD-AMC01-286 (Trace to: SLD-AMC01-286)
* Function:     diagHardwareInit
* Description:  上电初始化诊断
* Input:        none
* Output:       none
* Return:       none
* Others:       none
* Log:          Date          Author    Modified
*               2021/10/21    hdq
****************************************************************************************************/
void diagHardwareInit(void)
{
    int32_t diagRet = 0;
    int32_t i = 0;

    dccInit();        /*dcc初始化*/

    /* Initialise ADC SRAM */
    SL_Init_Memory(RAMTYPE_MIBADC1_RAM);

    /* Initialise MIBSPI SRAM */
    SL_Init_Memory(RAMTYPE_MIBSPI1_RAM);
    SL_Init_Memory(RAMTYPE_MIBSPI3_RAM);

    for(i = 0; i < (sizeof(s_diagSeg)/sizeof(pDiagFun)); i++)
    {
        SL_Clear_nERROR();/*清理ESM错误*/
        diagRet = s_diagSeg[i]();
        if(ERR_NO != diagRet)
        {
            errStopHandleInit(diagRet);
        }
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-287 (Trace to: SLD-AMC01-287)
* Function:     diagHardwareCycle
* Description:  自诊断接口
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/15     hdq
****************************************************************************************************/
void diagHardwareCycle(void)
{
    static uint32_t   digIndex = 0U; /* 静态变量 */
    int32_t diagRet = 0U;

    SL_Clear_nERROR();    /* 诊断前 清除错误标记 */
    diagRet = s_diagSeg[digIndex]();
    digIndex++;
    if(digIndex >= (sizeof(s_diagSeg)/sizeof(pDiagFun)))
    {
        digIndex = 0U;
    }

    if(ERR_NO != diagRet)
    {
        infoSetStError(diagRet);
    }
}

/********************************************************************* 
* Identifier:   SCOD-AMC01-222 (Trace to: SLD-AMC01-222) 
* Function:     diagFlash 
* Description:  诊断外部flash是否正常 
* Input:        flag 诊断方式 0 周期诊断 1 上电诊断
* Output:       none 
* Return:       none 
* Others: 
* Log:          Date          Author    Modified 
*               2022/08/16    wxb       create 
***********************************************************************/ 
static void diagFlash(uint8_t flag)
{ 
    uint16_t wValue = 0xAA55U; 
    uint16_t rValue = 0U;

    if(1 == flag) 
    {
        flashEraseSector(FLASH_DIAG_BASE_ADDRESS);
        tmDelayTick(1000 * 50U);
        flashWriteByBytes(FLASH_DIAG_BASE_ADDRESS,(const uint8_t*)&wValue,sizeof(uint16_t));
        tmDelayTick(1000 * 10U);
    } 
    flashReadByBytes(FLASH_DIAG_BASE_ADDRESS,(uint8_t*)&rValue,sizeof(uint16_t));
    if(wValue != rValue)
    {
        if(1 == flag)
        {
            errStopHandleInit(FLASH_ERROR); 
        }
        else
        {
            if(paraLockFile(3) == 0)
            {
                infoSetPfError(FLASH_WARN);
            }
        }
    }
    else
    {
        infoClearPfError(FLASH_WARN);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-088 (Trace to: SLD-AMC01-088)
* Function:     diagHardwareCyclePlus
* Description:  诊断ARM电源及片外设备
* Input:        flag 诊断方式 0 周期诊断 1 上电诊断
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/15     xd
****************************************************************************************************/
void diagHardwareCyclePlus(uint8_t flag)
{
    volDiganostic(flag);
    diagFlash(flag);
}

