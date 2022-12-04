/*
 * Private device driver structure
 */
#ifndef _DRV_H_
#define _DRV_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include <stdalign.h>
#ifndef __STDALIGN_H
#define __STDALIGN_H

#if defined(_MSC_VER) && !defined(__cplusplus)

#define alignof             _Alignof
#define alignas(nBytes)     __declspec(align(nBytes))

#endif

#endif
#include <assert.h>

#define BR_DEVICE_PRIVATE
#define BR_OUTPUT_FACILITY_PRIVATE
#define BR_DEVICE_PIXELMAP_PRIVATE
#define BR_RENDERER_FACILITY_PRIVATE
#define BR_BUFFER_STORED_PRIVATE
#define BR_GEOMETRY_V1_MODEL_PRIVATE
#define BR_GEOMETRY_V1_BUCKETS_PRIVATE
#define BR_GEOMETRY_STORED_PRIVATE
#define BR_RENDERER_STATE_STORED_PRIVATE
#define BR_RENDERER_PRIVATE

#include "brddi.h"
#include "brbgfxrend.h"
#include "formats.h"
#include "pm.h"
//#include "font.h"
//#include "video.h"
//#include "glstate.h"

#include "template.h"
#include "device.h"
#include "outfcty.h"
#include "devpixmp.h"
#include "rendfcty.h"
#include "renderer.h"
//#include "sstate.h"
//#include "sbuffer.h"
#include "gstored.h"
#include "gv1model.h"
#include "gv1buckt.h"

/*
 * Pull in private prototypes
 */
#ifndef _NO_PROTOTYPES

#include "drv_ip.h"

#endif

#ifdef __cplusplus
};
#endif
#endif /* _DRV_H_ */
