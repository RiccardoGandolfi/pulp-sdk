#include "pmsis.h"
#include "stdio.h"

#include "idma_parameters.h"
#include "idma_presets.h"
#include "idma_defines.h"

#ifndef _DMA_FRONTEND_REG_DEFS_
#define _DMA_FRONTEND_REG_DEFS_

#ifdef __cplusplus
extern "C" {
#endif

#define DMA_CONF_DECOUPLE 0
#define DMA_CONF_DEBURST 0
#define DMA_CONF_SERIALIZE 0

#define CORE_SPACE 2048

#ifdef QUICK_MODE
#define TRANSFERS 7
#else
#define TRANSFERS NB_TRANSFERS
#endif

#ifdef DEBUG_TEST
    #define PRINTF(...) printf(__VA_ARGS__)
#else
    #define PRINTF(...)
#endif

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // _DMA_FRONTEND_REG_DEFS_
