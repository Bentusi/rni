/**************************************************************************************************
*Filename:     com_hw.c
*Purpose:      fpga读写接口
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#include "lynx_types.h"
#include "bsp.h"
#include "error_class.h"
#include "com_hw.h"
#include "info_class.h"
#include "module_class.h"
#include "cfg_class.h"

/* FPGA接口地址 */
static uint32_t s_comAllAddr[LYNX_SLOT_MAX][3];  /* 0 配置 1 接收 2 发送 */

static fpgaWriteReg_t s_fpgaWriteReg;
static fpgaReadReg_t  s_fpgaReadReg;

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-344 (Trace to: SLD-AMC01-344)
 * Function:     com1ReadData
 * Description:  从FPGA读出到指定长度的数据并计算CRC
 * Input:        addr   源地址
 *               pBuf   目的地址
 *               length 数据长度
 * Output:       None
 * Return:       ret EMIF_CRC_ERROR：CRC错误， 0：无错误
 * Date:         Author      Modified
 * 2021-10-14    hdq         Create
 *************************************************************************************************/
int32_t com1ReadData(void *pBuf, uint32_t addr, uint32_t length)
{
    int32_t ret = 0;

    LYNX_ASSERT(NULL != pBuf);
    (void)memcpy(pBuf, (const void*)addr, (size_t)length);

    if(0ULL != drv2CrcCal(pBuf, length))
    {
        ret = EMIF_CRC_ERROR;
        errEmifCrcHandle(ret); /* CRC异常处理方式 */
    }

    return ret;
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-345 (Trace to: SLD-AMC01-345)
 * Function:     com1WriteData
 * Description:  向FPGA写入指定长度的数据并伴随着CRC
 * Input:        addr   目的地址
 *               pBuf   源地址
 *               length 数据长度
 * Output:       None
 * Return:       None
 * Date:         Author      Modified
 * 2021-10-14    hdq         Create
 *************************************************************************************************/
void com1WriteData(uint32_t addr, void *pBuf, uint32_t length)
{
    uint64_t crc = 0ULL;
    uint8_t *pSrc = (uint8_t*)pBuf;

    LYNX_ASSERT(NULL != pBuf);

    crc = drv2CrcCal((const void *)pSrc, length - LYNX_CRC_SIZE);
    pSrc += (length - LYNX_CRC_SIZE);
    (void)memcpy((void *)pSrc, (const void *)&crc, LYNX_CRC_SIZE);

    (void)memcpy((void *)addr, (const void *)pBuf, (size_t)length);
}

/***************************************************************************************
* Identifier:   SCOD-AMC01-052 (Trace to: SLD-AMC01-052)
* Function:     com1UpdateCaseState
* Description:  机箱0接收数据状态信息收集
* Input:        box 机箱号
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***************************************************************************************/
void com1UpdateCaseState(int32_t box)
{
    int32_t ret = 0;
    int32_t i = 0;
    uint16_t softInfo = 0U;

    uint32_t limitCnt = 1UL;
    static uint32_t failCnt = 0U;
    fpgaModuleState_t *pState = (fpgaModuleState_t *)g_fastRxBuffer;

    LYNX_ASSERT(0 == box);
    /* 读取数据 */
    ret = com1ReadData((void*)pState, MAIN_BOX_STATUS_ADDR, sizeof(fpgaModuleState_t));
    if(0 == ret)
    {
        for(i = 0; i < CASE_SLOT_MAX; i++)
        {
            if(i < 2)
            {
                /*主控FPGA上传状态信息在索引0*/
                infoSetCom(g_localSlot, (uint32_t)pState->comInfo[0]);
                infoSetsoft(g_localSlot, (uint32_t)pState->softInfo[0]);
            }
            else
            {
                infoSetCom(i, (uint32_t)pState->comInfo[i]);
                infoSetsoft(i, (uint32_t)pState->softInfo[i]);
            }
        }
    }
    else
    {
        failCnt++;
        if(failCnt > limitCnt)
        {
            failCnt = limitCnt;
            infoSetPfError(COM_ARM_EMIF_READ_CRC);
        }
    }
    softInfo = pState->softInfo[0];
    if((softInfo & 0x01U) != 0)
    {
        infoSetPfError(COM_FPGA_EMIF_READ_CRC);
    }
    else
    {
        infoClearPfError(COM_FPGA_EMIF_READ_CRC);
    }
    infoIOComPatch();
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-053 (Trace to: SLD-AMC01-053)
* Function:     com1SetArmMS
* Description:  设置 ARM 当前主从状态到fpga缓冲区
* Input:        state  当前状态；MPU_MASTER 主 MPU_SLAVE 从
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2021/9/20
****************************************************************************************************/
void com1SetArmMS(uint8_t state)
{
    s_fpgaWriteReg.msState = state;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-054 (Trace to: SLD-AMC01-054)
* Function:     com1ReportArmState
* Description:  向FPGA 写ARM 当前状态
* Input:        state  当前状态
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void com1ReportArmState(uint8_t state)
{
    uint32_t mode = 0U;

    mode = infoCurSysMode();
    s_fpgaWriteReg.mpuState = state;
    s_fpgaWriteReg.mpuMode = (uint8_t)mode;

    com1WriteData(FPGA_WRITE_REG_ADDR, (void*)&s_fpgaWriteReg, sizeof(fpgaWriteReg_t));
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-055 (Trace to: SLD-AMC01-055)
* Function:     com1EnableFpga
* Description:  使能或关闭FPGA超时检测
* Input:        state  使能或关闭
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
****************************************************************************************************/
void com1EnableFpga(uint8_t state)
{
    s_fpgaWriteReg.enable = state;
    com1WriteData(FPGA_WRITE_REG_ADDR, (void*)&s_fpgaWriteReg, sizeof(fpgaWriteReg_t));
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-005 (Trace to: SLD-AMC01-005)
* Function:     com1CalReadData
* Description:  计算上行地址 （数据流向FPGA到ARM    读数据地址）
* Input:        slot         槽位号
* Output:       s_comAllAddr FPGA接口地址
* Return:       none
*
* Others:       
* Log:          Date          Author    Modified
*               2017/09/07
****************************************************************************************************/
void com1CalReadData(int32_t slot)
{
    uint32_t tmpAddr = 0U;
    
    /* 判断模块类型是否合法 */
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    
    tmpAddr = (((uint32_t)slot) * ASYNC_EMIF_READ_STEP) + ASYNC_EMIF_READ_MIAN_ADDR;
    if(slot == 0)  /*主控机箱两块主控卡 RX地址同为0x68818000ul*/
    {
        tmpAddr = (ASYNC_EMIF_READ_STEP) + ASYNC_EMIF_READ_MIAN_ADDR;
    }

    /* 实际地址等于偏移地址加上基地址 */
    tmpAddr = (uint32_t)FPGA_BASE_ADDR + tmpAddr;

    s_comAllAddr[slot][COM_RX1_ADDR] = tmpAddr;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-006 (Trace to: SLD-AMC01-006)
* Function:     com1CalWriteData
* Description:  计算下行地址 （数据流向 ARM 到 FPGA    写数据地址）
* Input:        slot 槽位号
* Output:       s_comAllAddr FPGA接口地址
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/09/07
****************************************************************************************************/
void com1CalWriteData(int32_t slot)
{
    uint32_t tmpAddr = 0U;

    /* 判断模块类型是否合法 */
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    tmpAddr = (((uint32_t)slot) * ASYNC_EMIF_WRITE_STEP) + ASYNC_EMIF_WRITE_MIAN_ADDR;
    if(slot == 0)/*主控机箱两块主控卡 TX地址同为0x68018000ul*/
    {
        tmpAddr = (ASYNC_EMIF_WRITE_STEP) + ASYNC_EMIF_WRITE_MIAN_ADDR;
    }

    /* 实际地址等于偏移地址加上基地址 */
    tmpAddr = (uint32_t)FPGA_BASE_ADDR + tmpAddr;

    s_comAllAddr[slot][COM_TX1_ADDR] = tmpAddr;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-007 (Trace to: SLD-AMC01-007)
* Function:     com1CalConfigData
* Description:  计算配置地址 （数据流向 ARM 到 FPGA   写配置地址）
* Input:        slot 槽位号
* Output:       s_comAllAddr FPGA接口地址
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/09/07
****************************************************************************************************/
void com1CalConfigData(int32_t slot)
{
    uint32_t tmpAddr = 0U;

    /* 判断模块类型是否合法 */
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    tmpAddr = (((uint32_t)slot) * ASYNC_EMIF_CONFIG_STEP) + ASYNC_EMIF_CONFIG_MIAN_ADDR;

    /* 实际地址等于偏移地址加上基地址 */
    tmpAddr = (uint32_t)FPGA_BASE_ADDR + tmpAddr;
    
    s_comAllAddr[slot][COM_CFG_ADDR] = tmpAddr;
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-056 (Trace to: SLD-AMC01-056)
* Function:     checkSlotCycle
* Description:  周期校验子站号
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/09/07
****************************************************************************************************/
static void checkSlotCycle(void)
{
    lynxID_t cfgId;

    cfgId = infoCurMpuId();

    if((s_fpgaReadReg.stationNum & 0xC0U) != ((uint8_t)cfgId.detail.stationNum & 0xC0U))
    {
        infoSetPfError(SLOT_NUMBER_WARING);
    }
    else
    {
        infoClearPfError(SLOT_NUMBER_WARING);
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-057 (Trace to: SLD-AMC01-057)
* Function:     com1UpdateMpuState
* Description:  读取主控模块状态信息，并校验子站是否正确
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2021/9/20     hdq       create
****************************************************************************************************/
void com1UpdateMpuState(void)
{
    int32_t i = 0;
    int32_t ret = 0;
    fpgaReadReg_t *pFpgaReadReg = &s_fpgaReadReg;
    LYNX_ASSERT(NULL != pFpgaReadReg);
    
    /* 读取 硬件信息 */
    ret = com1ReadData((void*)pFpgaReadReg, MPU_HADR_INFO_ADDR, sizeof(fpgaReadReg_t));
    if(0 == ret)
    {
        checkSlotCycle();
        /* 取主机箱 0 ~ 最大槽位数据空满状态 */
        for(i = 0; i < CASE_SLOT_MAX; i += 2)
        {
            int32_t j = 0;
            for(j = 0; j < COM_PORT_MAX; j++)
            {
                portState_t *pState0 = infoGetPortState(i, j, RX_PORT);
                portState_t *pState1 = infoGetPortState(i + 1, j, RX_PORT);
                if(i == 0)
                {
                    if(i == g_localSlot)
                    {
                        pState0->full = (pFpgaReadReg->portState[i/2] >> (j + 0)) & 0x01U;
                    }
                    else
                    {
                        pState1->full = (pFpgaReadReg->portState[i/2] >> (j + 0)) & 0x01U;
                    }
                }
                else
                {
                    pState0->full = (pFpgaReadReg->portState[i/2] >> (j + 0)) & 0x01U;
                    pState1->full = (pFpgaReadReg->portState[i/2] >> (j + 4)) & 0x01U;
                }
            }
        }

        if(pFpgaReadReg->isMaster == 0xFFU)
        {/* 主模式 */
            infoSetMpuMS(MPU_MASTER);
            infoGetAddr()->moduleInfo[1 - g_localSlot].difInfo.mpuInfo.curMSState = MPU_SLAVE;
        }
        else
        {/* 从模式 */
            infoSetMpuMS(MPU_SLAVE);
            infoGetAddr()->moduleInfo[1 - g_localSlot].difInfo.mpuInfo.curMSState = MPU_MASTER;
        }

        /* 主控模块故障 */
        if(((pFpgaReadReg->msg1 & (1UL << 5U)) != 0U) ||
           ((pFpgaReadReg->msg1 & (1UL << 7U)) != 0U))
        {
            infoSetPfError(MPUS_ERR);
        }
        else
        {
            infoClearPfError(MPUS_ERR);
        }
        
        /* 组态配置硬件信息是否匹配 */
        if((pFpgaReadReg->msg1 & (1UL << 4U)) != 0x10U)
        {
            infoSetPfError(SLOT_NUMBER_ERROR);
        }
        else
        {
            infoClearPfError(SLOT_NUMBER_ERROR);
        }

        /* 热备模式下 才显示冗余端口号 故障 */
        if((pFpgaReadReg->msg2 & (1UL << 5U)) != 0U)
        {
            infoSetPfError(BACK_MPU_ERROR);
        }
        else
        {
            infoClearPfError(BACK_MPU_ERROR);
        }
        
        /* 该函数执行后FPGA才真正切换完成 */
        com1ReportArmState(ARM_OK);
    }  
}

/**************************************************************************************************
 * Identifier:   SCOD-AMC01-008 (Trace to: SLD-AMC01-008)
 * Function:     com1GetAddr
 * Description:  获取每个槽位的IO模式下三地址
 * Input:        slot 槽位号
 *               idx  地址类型
 * Output:       None
 * Return:       s_comAllAddr[slot][idx] 指定类型地址
 * Date:         Author      Modified
 * 2021-09-23    hdq         Create
 *************************************************************************************************/
uint32_t com1GetAddr(int32_t slot, int32_t idx)
{
    LYNX_ASSERT(slot < LYNX_SLOT_MAX);
    LYNX_ASSERT(idx <= COM_TX1_ADDR);
    return s_comAllAddr[slot][idx];
}

