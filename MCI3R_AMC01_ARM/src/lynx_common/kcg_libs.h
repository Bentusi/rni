#ifndef _KCG_LIBS_H_
#define _KCG_LIBS_H_

#include "kcg_types.h"

/* app/source/lynx.c */


/* app/source/in_out.c */
extern void lxCpyToIo(int32_t slot, int32_t ch, void *pSrc, size_t size, void *pForce, uint8_t *pFlag);
extern void lxCpyFromIo(void *pDst, int32_t slot, int32_t ch, size_t size, void *pForce, uint8_t *pFlag);
extern void lxCpyFromInfo(void *pDst, int32_t slot, int32_t para, size_t size, const void *pForce, const uint8_t *pFlag);

extern void lxCpyNetToInput(void *pDst, const void *pSrc, size_t size, void *pForce, uint8_t *pFlag);
extern void lxCpyParaToInput(void *pDst, void *pSrc, size_t size, void *pForce, uint8_t *pFlag);
extern void lxCpyOutputToNet(void *pDst, void *pSrc, size_t size, void *pForce, uint8_t *pFlag);

#endif
