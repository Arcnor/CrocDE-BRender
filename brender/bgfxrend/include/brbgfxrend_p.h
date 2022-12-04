#ifndef _BRBGFXREND_P_H
#define _BRBGFXREND_P_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NO_PROTOTYPES

/*
 * Main entry point for device.
 */
struct br_device *BR_EXPORT BrDrvBGFXBegin(const char *arguments);

#endif /* _NO_PROTOTYPES */

#ifdef __cplusplus
};
#endif
#endif
