/* Esm Register Frame Definition */
/** @struct sl_esmBase
*   @brief Esm Register Frame Definition
*
*   This type is used to access the Esm Registers.
*/

#ifndef __HAL_ESM_H__
#define __HAL_ESM_H__

#include <HL_reg_esm.h>

#define ESM_ERROR_PIN_ON		((uint32)0u)
#define ESM_ERROR_PIN_OFF		((uint32)1u)
#define ESM_ESMEPSR_ESF			((uint32)(1u << 0))
#define ESM_NERROR_SET			((uint32)(0xAu))
#define ESM_NERROR_CLEAR		((uint32)(0x5u))
#endif /* __HAL_ESM_H__ */
