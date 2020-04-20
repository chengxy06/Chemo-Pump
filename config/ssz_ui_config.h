#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//is use simulator's 24 bit depth
#define USE_SIMULATOR_DEPTH 0

#define DISPLAY_WIDTH 256
#define DISPLAY_HEIGHT 64

#if USE_SIMULATOR_DEPTH
#define DISPLAY_BIT_PER_PIXEL 24
#else
#define DISPLAY_BIT_PER_PIXEL 4
#endif


#define SSZ_UI_SUPPORT_ALPHA_COLOR 0


#ifdef __cplusplus
}
#endif
