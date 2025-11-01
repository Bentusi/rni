/**************************************************************************************************
*Filename:     flash.h
*Purpose:      flash操作接口
*Log:          Date          Author    Modified
*              2021/9/18     hdq       create
**************************************************************************************************/
#ifndef _FLASH_H_
#define _FLASH_H_

#include "lynx_types.h"

#define USER_PARA_BASE_ADDRESS          (0x00U)
#define USER_PARA_MAX_SIZE              (1024U * 4U)
#define FLASH_DIAG_BASE_ADDRESS         (USER_PARA_MAX_SIZE)

#define USER_FLAG_SIZE                  (64U)
#define FLASH_SPI                       (spiREG2)

#define SECTOR_SIZE                     (4096U)                /* 扇区大小 4KB */
#define FLASH_MAX_SIZE                  (1024U*1024U*16U)      /* Flash最大容量 */

/* 指令表  */
#define FLASH_WRITE_ENABLE               (0x06U)  /* 写使能 */
#define FLASH_WRITE_DISABLE              (0x04U)  /* 写禁能 */
#define FLASH_READ_STATUS_REG            (0x05U)  /* 读状态寄存器 */
#define FLASH_READ_DATA                  (0x03U)  /* 普通读数据 */
#define FLASH_PAGE_PROGRAM               (0x02U)  /* 页编程 */
#define FLASH_SECTOR_ERASE               (0x20U)  /* 擦除扇区，扇区大小4K */
#define FLASH_CHIP_ERASE                 (0x60U)  /* 擦除整个芯片 */

extern int32_t flashReadByBytes(uint32_t addr, uint8_t *pBuf, uint32_t length);
extern int32_t flashWriteNoblock(uint32_t dstAddr, const void *pBuf, uint32_t bufSize);
extern void flashEraseSector(uint32_t addr);
extern int32_t flashWriteByBytes(uint32_t addr, const uint8_t *pBuf, uint32_t length);

#endif /* _FLASH_H_ */

