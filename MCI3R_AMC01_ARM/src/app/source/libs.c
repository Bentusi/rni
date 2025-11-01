/*********************************************************
*文件名:   libs.c
*目的  :   公用接口
*日期         作者             修改
*2017.08.14   胡德全          创建文件
*********************************************************/
#include <libs.h>


/****************************************************************************************************
* Identifier:   SCOD-AMC01-083 (Trace to: SLD-AMC01-083)
* Function:     findFirstOne
* Description:  查找故障码
* Input:        value 故障码
* Output:       none
* Return:       ret   返回第一个故障码
*               
* Others:
* Log:          Date          Author    Modified
*               2017/08/14        
****************************************************************************************************/
int32_t findFirstOne(uint64_t value)
{
    int32_t i = 0;
    int32_t ret = 0xFF;

    for(i=0; i<64; i++)
    {
        if((value & ((uint64_t)1U<<i)) != 0U)
        {
            ret = i;
            break;
        }
    }

    return ret;
}

