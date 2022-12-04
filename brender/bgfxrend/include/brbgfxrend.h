#ifndef _BRBGFXREND_H_
#define _BRBGFXREND_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _BRENDER_H_
#   error Please include brender.h first
#endif

typedef void *BR_CALLBACK br_device_bgfx_createcontext_cbfn(struct br_device *dev, void *user);

typedef void BR_CALLBACK br_device_bgfx_deletecontext_cbfn(struct br_device *dev, void *ctx, void *user);

typedef br_error BR_CALLBACK br_device_bgfx_makecurrent_cbfn(struct br_device *dev, void *ctx, void *user);

typedef void BR_CALLBACK br_device_bgfx_swapbuffers_cbfn(struct br_device *dev, struct br_device_pixelmap *pm,
                                                       void *user);

typedef void * BR_CALLBACK br_device_bgfx_getprocaddress_cbfn(const char *name);

typedef struct br_device_bgfx_procs {

    br_device_bgfx_createcontext_cbfn  *create_context;
    br_device_bgfx_deletecontext_cbfn  *delete_context;
    br_device_bgfx_makecurrent_cbfn    *make_current;
    br_device_bgfx_getprocaddress_cbfn *get_proc_address;
    br_device_bgfx_swapbuffers_cbfn    *swap_buffers;
    void                               *user;

} br_device_bgfx_ext_procs;

/*
 * Function prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _BRBGFXREND_P_H
#   include "brbgfxrend_p.h"
#endif

#endif /* _NO_PROTOTYPES */

#ifdef __cplusplus
};
#endif

#endif /* _BRBGFXREND_H_ */
