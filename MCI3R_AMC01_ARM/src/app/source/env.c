/****************************************************************************************************
*FILENAME:     env.c
*PURPOSE:      系统环境初始化
*DATE          AUTHOR          CHANGE
*2017.08.14    胡德全          创建文件
****************************************************************************************************/
#include <string.h>
#include "env.h"
#include "lynx_types.h"
#include "bsp.h"
#include "flash.h"
#include "global.h"
#include "error_class.h"
#include "force_class.h"
#include "info_class.h"
#include "logic_class.h"
#include "para_class.h"
#include "module_class.h"
#include "cfg_class.h"
#include "net_class.h"
#include "com_hw.h"
#include "algInterface.h"

/* 全局变量数据定义 */
static int8_t s_arFlashFlag[USER_FLAG_SIZE];
static void loadParameter(void);
static void platformInit(void);
static int32_t checkFlashData(void *pFile, uint32_t maxSize);
static int32_t verifyRamData(const fileHead_t *info, const void *data);

/*********************************************************************
* Identifier:   SCOD-AMC01-020 (Trace to: SLD-AMC01-020)
* Function:     verifyRamData
* Description:  根据文件头信息对文件进行CRC校验
* Input:        info  参数文件头信息
*               data  参数文件数据
* Output:       none
* Return:       成功 0 失败 1
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
***********************************************************************/
static int32_t verifyRamData(const fileHead_t *info, const void *data)
{
    int32_t ret = 0;
    uint64_t crc = 0ULL;
    uint32_t size = info->length;

    crc = drv2CrcCal(data,size);
    if(crc != info->crc)
    {
        ret = 1;
    }

    return ret;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-021 (Trace to: SLD-AMC01-021)
* Function:     checkFlashData
* Description:  对指定的存储数据进行校验
* Input:        pFile     参数文件指针
*               maxSize   校验数据的最大长度
* Output:       none
* Return:       
*               ret: 1 数据长度越界
                     2 数据校验失败
                     0 校验正确
* Others:
* Log:          Date          Author    Modified
*               2023/03/09    wxb       删除参数baseAddr
***********************************************************************/
static int32_t checkFlashData(void *pFile, uint32_t maxSize)
{
    uint32_t size = 0U;
    uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;
    uint32_t addr = (uint32_t)pFile + sizeof(fileHead_t);
    int32_t ret = 0;
    fileHead_t *pHead = (fileHead_t *)pFile;

    LYNX_ASSERT(NULL != pFile);

    size = pHead->length;
    if(size > (maxSize - sizeof(fileHead_t)))
    {
        ret = 0x01;
    }

    if(ret == 0)
    {
        crc = drv2CrcCal((const void*)addr, size);
        if(crc != pHead->crc)
        {
            /* 校验时失败 */
            ret = 0x02;
        }
    }

    return ret;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-022 (Trace to: SLD-AMC01-022)
* Function:     loadParameter
* Description:  校验加载更新RAM参数文件
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2021/08/17    hdq       create
***********************************************************************/
static void loadParameter(void)
{
    uint32_t ret = 0U;
    int32_t res = 0;
    fileHead_t *pHead = (fileHead_t *)g_flieBuf;
    uint8_t *pData = (uint8_t *)g_flieBuf + sizeof(fileHead_t);

    (void)memset(s_arFlashFlag, 0xFF, USER_FLAG_SIZE);
    (void)flashReadByBytes(USER_PARA_BASE_ADDRESS, (uint8_t*)pHead, USER_PARA_MAX_SIZE);
    if(0 == memcmp((const void *)s_arFlashFlag, (const void *)pHead, USER_FLAG_SIZE))
    {
        /* new flash, need save */
        ret = 1U;
    }
    else
    {
        /* 校验参数数据 */
        res = checkFlashData((void*)pHead, USER_PARA_MAX_SIZE);
        if(0 != res)
        {
            ret = 2U;
        }
        else
        {
            /* compare header */
            res = memcmp((const void *)lxGetParaHead(), (const void *)pHead, sizeof(fileHead_t) - sizeof(uint64_t));
            if(0 != res)
            {
                /* update the para, need save */
                ret = 1U;
            }
        }
    }

    switch(ret)
    {
        case 0U:
            /* 加载flash数据 */
            (void)memcpy((void*)lxGetParaHead(), (const void*)pHead, sizeof(fileHead_t));
            (void)memcpy(lxGetPara(), (const void*)pData, (size_t)pHead->length);
            ret = (uint32_t)verifyRamData((const fileHead_t *)lxGetParaHead(), (const void*)lxGetPara());
            if(ret != 0U)
            {
                errStopHandleInit(CHECK_USER_PARA_FAIL);
            }
            break;
            
        case 1U:
            /* save default parameter to flash */
            do 
            {
                ret = (uint32_t)paraSave();
            } while(ret != 0U);
            break;
        default:
            errStopHandleInit(CHECK_USER_PARA_FAIL);
            break;
    }
}

/*********************************************************************
* Identifier:   SCOD-AMC01-003 (Trace to: SLD-AMC01-003)
* Function:     platformInit
* Description:  初始化所有模块
* Input:        none
* Output:       none
* Return:       none
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
static void platformInit(void)
{
    int32_t slot = 0;
    uint32_t type = 0u;

    /* 发送主控配置信息 */
    for(slot = 0; slot < LYNX_SLOT_MAX; slot++)
    {
        type = cfgCardTypeOnSlot(slot);
        LYNX_ASSERT(type < MODULE_TYPE_MAX);
        if(NULL != g_moduleHandle[type].pFunInit)
        {
            (void)g_moduleHandle[type].pFunInit(slot);
        }
    }
}

/****************************************************************************************************
* Identifier:   SCOD-AMC01-004 (Trace to: SLD-AMC01-004)
* Function:     initMpuSates
* Description:  初始化平台信息
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
*               2019/06/20       函数名变更: 原函数名lxInitCardStates变更为initMpuSates
****************************************************************************************************/
void initMpuSates(void)
{
    int32_t curSlot = g_localSlot;
    infoGetAddr()->moduleInfo[curSlot].difInfo.mpuInfo.pfCfg = cfgLocalPfMode();
    infoGetAddr()->moduleInfo[curSlot].difInfo.mpuInfo.id.value = cfgLocalMpuId();
    infoGetAddr()->moduleInfo[curSlot].difInfo.mpuInfo.version = LYNX_VERSION;

    /*  工程文件 版本号 */
    infoGetAddr()->moduleInfo[curSlot].difInfo.mpuInfo.userImageVer = lxGetImageHead()->version;
    infoGetAddr()->moduleInfo[curSlot].difInfo.mpuInfo.userParaVer  = lxGetParaHead()->version;
    infoGetAddr()->moduleInfo[curSlot].difInfo.mpuInfo.pfCfgVer     = lxGetCfgHead()->version;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-002 (Trace to: SLD-AMC01-002)
* Function:     envInit
* Description:  运行环境初始化
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
void envInit(void)
{
    int32_t ret = -99;

    glInit();
    ret = ifAgLoadInit();
    if(0 != ret)
    {
        errStopHandleInit(LOAD_USER_IMAGE_FAIL);
    }

    diagHardwareCyclePlus(1U);

    /* check the user parameter. */
    loadParameter();

    platformInit();
}


