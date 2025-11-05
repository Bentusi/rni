/**************************************************************************************************
*FILENAME:     algInterface.c
*PURPOSE:      基础固件与系统应用接口
*DATE          AUTHOR          CHANGE
*2023.05.12    wxb             创建文件
**************************************************************************************************/
#include "algInterface.h"
#include "project.h"
#include "drv2_libs.h"
#include "info_class.h"
#include "io_class.h"
#include "para_class.h"
#include "cfg_class.h"
#include "logic_class.h"
#include "force_class.h"
#include "net_class.h"

typedef int32_t (*pFnAlgLoadInit_t)(algMethod_t *pMethod);
typedef void  (*fnInit_t) (void);
typedef int32_t  (*fnCycle_t)(void);
typedef uint32_t *(*pFnGetOneAddr_t)(uint32_t index);

algMethod_t g_algMethodHandle =
{
    drv2CrcCal,
    lxGetOutChAddr,
    lxGetInChAddr,
    infoGetSysVar,
    lxSetParaHead,
    lxSetCfgHead,
    lxSetImageHead,
    lxSetUsrInAddr,
    lxSetUsrOutAddr,
    lxSetNetPortAddr,
    lxSetForceHead,
    infoGetStation,
};

/*********************************************************************
* Identifier:   SCOD-AMC01-333 (Trace to: SLD-AMC01-333)
* Function:     ifAgLoadInit
* Description:  加载算法环境
* Input:        none
* Output:       none
* Return:       ret 0 加载正确 -1 加载失败
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
int32_t ifAgLoadInit(void)
{
    int32_t ret = -1;
    void** pf = (void*)USER_IMAGE_LOAD_INIT_ADDR;
    uint32_t addr = (uint32_t)*pf;
    if(0xFFFFFFFF != addr)
    {
        ret = ((pFnAlgLoadInit_t)USER_IMAGE_LOAD_INIT_ADDR)(&g_algMethodHandle);
    }
    return ret;
}

/*********************************************************************
* Identifier:   SCOD-AMC01-334 (Trace to: SLD-AMC01-334)
* Function:     ifAgLogicTaskInit
* Description:  算法文件初始化
* Input:        none
* Output:       none
* Return:       none
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
void ifAgLogicTaskInit(void)
{
    ((fnInit_t)USER_IMAGE_INIT_ADDR)();
}

/*********************************************************************
* Identifier:   SCOD-AMC01-335 (Trace to: SLD-AMC01-335)
* Function:     ifAgLogicTaskRun
* Description:  算法周期运行入口函数
* Input:        none
* Output:       none
* Return:       返回 算法运行结果
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
int32_t ifAgLogicTaskRun(void)
{
    return ((fnCycle_t)USER_IMAGE_CYCLE_ADDR)();
}

/*********************************************************************
* Identifier:   SCOD-AMC01-336 (Trace to: SLD-AMC01-336)
* Function:     ifAgGetOneForceAddr
* Description:  获取强制数据接口函数
* Input:        index 强制数据的索引
* Output:       none
* Return:       强制数据地址
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
const uint32_t *ifAgGetOneForceAddr(uint32_t index)
{
    return ((pFnGetOneAddr_t)USER_IMAGE_GET_FORCE_ADDR)(index);
}

/*********************************************************************
* Identifier:   SCOD-AMC01-337 (Trace to: SLD-AMC01-337)
* Function:     ifAgGetOneParaAddr
* Description:  获取参数数据接口函数
* Input:        index 参数数据的索引
* Output:       none
* Return:       参数数据地址
*
* Others:
* Log:          Date          Author    Modified
*               2017/08/14
***********************************************************************/
const uint32_t *ifAgGetOneParaAddr(uint32_t index)
{
    return ((pFnGetOneAddr_t)USER_IMAGE_GET_PARA_ADDR)(index);
}

