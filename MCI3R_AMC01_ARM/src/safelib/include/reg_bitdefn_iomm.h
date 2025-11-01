#ifndef __HAL_IOMM_H__
#define __HAL_IOMM_H__

/** @struct pinMuxKicker
*   @brief Pin Muxing Kicker Register Definition
*
*   This structure is used to access the Pin Muxing Kicker registers.
*/
typedef volatile struct pinMuxKicker
{
    uint32 KICKER0;       /* kicker 0 register */
    uint32 KICKER1;       /* kicker 1 register */
} pinMuxKICKER_t;

/** @struct pinMuxBase
*   @brief PINMUX Register Definition
*
*   This structure is used to access the PINMUX module egisters.
*/
/** @typedef pinMuxBASE_t
*   @brief PINMUX Register Frame Type Definition
*
*   This type is used to access the PINMUX Registers.
*/
typedef volatile struct pinMuxBase
{
    uint32 PINMUX0;           /**< 0xEB10 Pin Mux 0 register*/
    uint32 PINMUX1;           /**< 0xEB14 Pin Mux 1 register*/
    uint32 PINMUX2;           /**< 0xEB18 Pin Mux 2 register*/
    uint32 PINMUX3;           /**< 0xEB1C Pin Mux 3 register*/
    uint32 PINMUX4;           /**< 0xEB20 Pin Mux 4 register*/
    uint32 PINMUX5;           /**< 0xEB24 Pin Mux 5 register*/
    uint32 PINMUX6;           /**< 0xEB28 Pin Mux 6 register*/
    uint32 PINMUX7;           /**< 0xEB2C Pin Mux 7 register*/
    uint32 PINMUX8;           /**< 0xEB30 Pin Mux 8 register*/
    uint32 PINMUX9;           /**< 0xEB34 Pin Mux 9 register*/
    uint32 PINMUX10;          /**< 0xEB38 Pin Mux 10 register*/
    uint32 PINMUX11;          /**< 0xEB3C Pin Mux 11 register*/
    uint32 PINMUX12;          /**< 0xEB40 Pin Mux 12 register*/
    uint32 PINMUX13;          /**< 0xEB44 Pin Mux 13 register*/
    uint32 PINMUX14;          /**< 0xEB48 Pin Mux 14 register*/
    uint32 PINMUX15;          /**< 0xEB4C Pin Mux 15 register*/
    uint32 PINMUX16;          /**< 0xEB50 Pin Mux 16 register*/
    uint32 PINMUX17;          /**< 0xEB54 Pin Mux 17 register*/
    uint32 PINMUX18;          /**< 0xEB58 Pin Mux 18 register*/
    uint32 PINMUX19;          /**< 0xEB5C Pin Mux 19 register*/
    uint32 PINMUX20;          /**< 0xEB60 Pin Mux 20 register*/
    uint32 PINMUX21;          /**< 0xEB64 Pin Mux 21 register*/
    uint32 PINMUX22;          /**< 0xEB68 Pin Mux 22 register*/
    uint32 PINMUX23;          /**< 0xEB6C Pin Mux 23 register*/
    uint32 PINMUX24;          /**< 0xEB70 Pin Mux 24 register*/
    uint32 PINMUX25;          /**< 0xEB74 Pin Mux 25 register*/
    uint32 PINMUX26;          /**< 0xEB78 Pin Mux 26 register*/
    uint32 PINMUX27;          /**< 0xEB7C Pin Mux 27 register*/
    uint32 PINMUX28;          /**< 0xEB80 Pin Mux 28 register*/
    uint32 PINMUX29;          /**< 0xEB84 Pin Mux 29 register*/
    uint32 PINMUX30;          /**< 0xEB88 Pin Mux 30 register*/
}pinMuxBASE_t;


/** @def kickerReg
*       @brief Pin Muxing Kicker Register Frame Pointer
*
*               This pointer is used to enable and disable muxing accross the device.
*/
#define kickerReg ((pinMuxKICKER_t *) 0xFFFFEA38U)

/** @def pinMuxReg
*       @brief Pin Muxing Control Register Frame Pointer
*
*               This pointer is used to set the muxing registers accross the device.
*/
#define pinMuxReg ((pinMuxBASE_t *) 0xFFFFEB10U)

#endif /* __HAL_IOMM_H__ */
