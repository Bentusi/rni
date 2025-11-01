#ifndef __HAL_EFC_H__
#define __HAL_EFC_H__

#include <HL_reg_efc.h>

#define EFC_BOUNDARY_INPUT_EN           (uint32)(0xFu << 0u)

#define EFC_BOUNDARY_SELF_TEST_ERR      (uint32)(1u << 21u)
#define EFC_BOUNDARY_SINGLE_BIT_ERR     (uint32)(1u << 20u)
#define EFC_BOUNDARY_INSTR_ERR          (uint32)(1u << 19u)
#define EFC_BOUNDARY_AUTOLOAD_ERR       (uint32)(1u << 18u)
#define EFC_BOUNDARY_OUTPUT_EN          (uint32)(0xFu << 14)
#define EFC_BOUNDARY_SELF_TEST_EN       (uint32)(1u << 13u)

#define EFC_PINS_SELF_TEST_DONE         (uint32)(1u << 15u)
#define EFC_PINS_SELF_TEST_ERROR        (uint32)(1u << 14u)
#define EFC_PINS_SINGLE_BIT_ERR         (uint32)(1u << 12u)
#define EFC_PINS_INSTR_ERR              (uint32)(1u << 11u)
#define EFC_PINS_AUTOLOAD_ERR           (uint32)(1u << 10u)

#define EFC_SELF_TEST_SIGNATURE         (uint32)0x5362F97Fu
#define EFC_ERROR_SIGNATURE				(uint32)0x18U
#define EFC_NO_ERROR					(uint32)0U
#define EFC_ERROR_CODE					(uint32)(0x1FU)
#endif /* __HAL_EFC_H__ */

