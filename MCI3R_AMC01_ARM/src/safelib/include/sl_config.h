/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                                                                          |**
**|         Copyright (c) 2012 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission is hereby granted to licensees of Texas Instruments           |**
**| Incorporated (TI) products to use this computer program for the sole     |**
**| purpose of implementing a licensee product based on TI products.         |**
**| No other rights to reproduce, use, or disseminate this computer          |**
**| program, whether in part or in whole, are granted.                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/
/**
 * @file        sl_config.h
 * @version     2.3.1
 * @brief       SafeTI&trade; Hercules&trade; Diagnostic Library configuration items
 *
 * @details The configuration items of the SafeTI&trade; Hercules&trade; Diagnostic library\n
 * such as enabling of profiling,enabling of the logging,setting of stack\n
 * etc needs to be included in this file.
 */

#ifndef __SL_CONFIG_H__
#define __SL_CONFIG_H__

#define SL_REG_INIT_VAL         0u

#ifndef EXTERNAL_SP_INIT
/**
 *  @defgroup config_group configuration items for SafeTI&trade; Hercules&trade; Diagnostic library
 *  configuration items for the SafeTI&trade; Hercules&trade; Diagnostic library
 */

/** \defgroup grp_armmodes Stack Size for various ARM modes
 *  @ingroup config_group
 *  Following macros define the size of stacks for the ARM modes
 *
 */
/**@{*/
#define STACK_SIZE_MODE_SVC 1024u   /**< Stack size for SVC mode */
#define STACK_SIZE_MODE_FIQ 1024u   /**< Stack size for FIQ mode */
#define STACK_SIZE_MODE_IRQ 1024u   /**< Stack size for IRQ mode */
#define STACK_SIZE_MODE_ABT 1024u   /**< Stack size for ABT mode */
#define STACK_SIZE_MODE_UND 1024u   /**< Stack size for UND mode */
#define STACK_SIZE_MODE_SYS 1024u   /**< Stack size for SYS mode */
/**@}*/
#endif

/** \defgroup nerror_delay delay value for nerror clearing
 *  @ingroup config_group
 *   Delay counter used before clearing nERROR. Delay is
     implemented using a while loop & is not calibrated.
     This function is used by Self test APIs that activate
     nERROR
 *
 */
/**@{*/
 #define NERROR_HOLD_DELAY (uint32)10u /**< Delay counter used before clearing nERROR. Delay is
                                    implemented using a while loop & is not calibrated.
                                    This function is used by Self test APIs that activate
                                    nERROR **/
/**@}*/

/** \defgroup grp_sram_range SRAM Range defines
 *  @ingroup config_group
 *  Following macros are used to define the range of TCM RAM
 *
 */
/**@{*/
#define SRAM_START          (uint32)0x08000000u     /**< Start address of TCM SRAM */
#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#define SRAM_END            (uint32)0x0807FFFFu     /**< End address of SRAM */
#endif
#if defined(_TMS570LS31x_) || defined(_RM48x_)
#define SRAM_END            (uint32)0x0803FFFFu     /**< End address of TCM SRAM */
#endif
#if defined(_TMS570LS12x_) || defined(_RM46x_)
#define SRAM_END            (uint32)0x0802FFFFu     /**< End address of SRAM */
#endif
#if defined(_RM42x_) || defined(_TMS570LS04x_)
#define SRAM_END            (uint32)0x08007FFFu     /**< End address of SRAM */
#endif


/**@}*/

#if 1
/**   !!! BEFORE ENABLING THESE MACROS !!! If the functions are called in non-privilege mode, the buffers must be suitably placed
            (& MPU regions updated) so that the buffers are Read & Writable from non-priv tasks
**/
/** \defgroup log_macros macros for loggin infomration
 *  @ingroup config_group
 *  Following macros are used for enabling or disabling logging and entry and param check in api's
 *
 */
/**@{*/
#define FUNCTION_PARAM_CHECK_ENABLED 			1 /**< When defined enables the parameter check at function entry */
#define FUNC_ENTRY_COND_CHECK_ENABLED 	        1 /**< When defined enables the entry codition check at function entry */
#define FUNC_RESULT_LOG_ENABLED 				0 /**< When enabled logs the function results to a pre-determined buffer */
/**@}*/
#endif

/** \defgroup grp_log Log buffer counts
 *   @ingroup config_group
 *  Following macros defines the size of the log buffers for error & result buffers
 *
 */
/**@{*/
#if(FUNC_RESULT_LOG_ENABLED == 1)
#define LOG_RESULT_BUFFER_COUNT 32u    /**< Size of the result buffer */
#else
#define LOG_RESULT_BUFFER_COUNT 0u    /**< Logging results is disabled */
#endif
#if defined(FUNCTION_PARAM_CHECK_ENABLED) || defined(FUNC_ENTRY_COND_CHECK_ENABLED)
#define LOG_ERROR_BUFFER_COUNT 32u     /**< Size of the error buffer */
#else
#define LOG_ERROR_BUFFER_COUNT 0u     /**< Logging errors is disabled */
#endif
/**@}*/

#if 1
/**   !!! BEFORE ENABLING THESE MACROS !!! If the functions are called in non-privilege mode, the buffers must be suitably placed
            (& MPU regions updated) so that the buffers are Read & Writable from non-priv tasks
**/
/** \defgroup prfile_macros macros for enabling profiling
 *  @ingroup config_group
 *  Following macros are used for enabling or disabling profiling in the code
 *
 */
/**@{*/
#define OPTIMISATION_ENABLED				1u
#define FUNCTION_PROFILING_ENABLED 			0u /**< When defined enables the profiling of the functions */
/**@}*/
#endif
#endif /* __SL_CONFIG_H__ */

