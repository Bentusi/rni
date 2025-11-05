/**************************************************************************************************
*Filename:     module_com3.h
*Purpose:      公用模块com3方法定义
*Log:          Date          Author    Modified
*              2021/9/21     hdq       create
**************************************************************************************************/
#ifndef _MODULE_COM3_H_
#define _MODULE_COM3_H_

#define COM3_TYPE                       (8U)

#define COM3_PORT_MAX                   (1)

#define COM3_DATA_MAX_SIZE              (4040U)

/* COM3 主从端口 */
#define COM3_SLAVE_PORT                 (0x03U) /* COM3端口为从端口 */
#define COM3_MASTER_PORT                (0x02U) /* COM3端口为主端口 */
#define COM3_PORT                       (0x01U) /* COM3端口单配置 */

typedef portDataInfo_t com3PrivInfo_t;

/* com3板卡数据帧定义, 应用层无校验 */
typedef struct
{
    uint8_t  data[COM3_DATA_MAX_SIZE]; /* 有效数据, 由于fpga原因，最大只支持4048，净荷4048－sizeof(ctrl)-sizeof(magic) */
    uint32_t ctrl;                     /* 使能控制位 */
    uint32_t magic;                    /* 配置文件CRC校验值 */
}__attribute__((packed)) com3Frame_t;

extern int32_t com3Init(int32_t slot);
extern int32_t com3RxHandle(int32_t slot, int32_t port, void *pBuf);
extern int32_t com3TxHandle(int32_t slot, int32_t port, void *pBuf);
extern int32_t com3RxHook(int32_t slot, int32_t para);
extern int32_t com3TxHook(int32_t slot, int32_t para);

#endif

