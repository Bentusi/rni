#ifndef __HAL_EPC_H__
#define __HAL_EPC_H__

#include <HL_reg_epc.h>

#define EPC_ERR_CAM_FULL			(uint32)((uint32)1u << 2u)
#define EPC_ERR_CAM_OVRFLW			(uint32)((uint32)1u << 0u)

#define EPC_CTRL_FULL_EN			(uint32)((uint32)1u << 24u)
#define EPC_CTRL_SERR_EN			(uint32)(0xAu)
#define EPC_CTRL_SERR_START			(uint32)(0u)
#define EPC_CTRL_SERR_LENGTH		(uint32)(4u)
#define EPC_CTRL_DIAG_EN			(uint32)(0xAu)
#define EPC_CTRL_DIAG_START			(uint32)(8u)
#define EPC_CTRL_DIAG_LENGTH		(uint32)(4u)
#define EPC_CTRL_DIAG_DIS			(uint32)(0x5u)

#define EPC_FIFO_FULL				(uint32)(0x1Fu)
#define EPC_FIFO_OVRFLW				(uint32)(0x1Fu)

#define EPC_CAM_CONTENT_ADDR		(uint32)(0xFFFFFFF8)

#define EPC_CAM_INDEX_CLEAR			(uint32)(0x5u)
#define EPC_CAM_INDEX_START(n)		(uint32)(8*n)
#define EPC_CAM_INDEX_LENGTH		(uint32)(4u)

#endif /* __HAL_EPC_H__*/
