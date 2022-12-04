#ifndef _DEVICE_H_
#define _DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BR_DEVICE_PRIVATE

/*
 * Private state of device
 */
typedef struct br_device {
    /*
     * Dispatch table
     */
    const struct br_device_dispatch *dispatch;

    /*
     * Standard object identifier
     */
    const char *identifier;

    /*
     * Pointer to owning device
     */
    struct br_device *device;

    /*
     * List of objects associated with this device
     */
    void *object_list;

    /*
     * Anchor for all device's resources
     */
    void *res;

    /*
     * Driver-wide template store
     */
    struct device_templates templates;

	/**
	 * BGFX specific
	 */
	br_device_bgfx_ext_procs ext_procs;

	char *vertex_shader;
	char *fragment_shader;
	struct br_renderer_facility *renderer_facility;
} br_device;

#endif /* BR_DEVICE_PRIVATE */

#ifdef __cplusplus
};
#endif
#endif /* _DEVICE_H_ */
