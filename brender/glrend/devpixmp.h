/*
 * Private device pixelmap structure
 */
#ifndef _DEVPIXMP_H_
#define _DEVPIXMP_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z;
    float r, g, b;
    float u, v;
} br_device_pixelmap_gl_tri;

typedef struct br_device_pixelmap_gl_quad {
    br_device_pixelmap_gl_tri tris[4];
    GLuint                    defaultVao;
    GLuint                    textVao;
    GLuint                    buffers[2];
} br_device_pixelmap_gl_quad;

#ifdef BR_DEVICE_PIXELMAP_PRIVATE

/*
 * Private state of device pixelmap
 */
typedef struct br_device_pixelmap {

    /*
     * Dispatch table
     */
    const struct br_device_pixelmap_dispatch *dispatch;

    /*
     * Standard handle identifier
     */
    const char *pm_identifier;

    /** Standard pixelmap members (not including identifier) **/

    BR_PIXELMAP_MEMBERS

    /** End of br_pixelmap fields **/

    struct br_device          *device;
    struct br_output_facility *output_facility;

    /*
     * Type of buffer (when matched)
     */
    br_token use_type;

    /*
     * No. MSAA samples
     */
    br_int_32 msaa_samples;

    /*
     * Pointer to renderer currently opened on this pixelmap (N.B. This is only set on the screen
     * pixelmap)
     */
    struct br_renderer *renderer;

    /* OpenGL crap */
    union {
        struct {
            struct br_device_pixelmap *depthbuffer;
            GLuint                    glFbo;
            GLuint                    glTex;
            GLfloat                   clearColour[4];

            br_device_pixelmap_gl_quad quad;
        } asBack;
        struct {
            struct br_device_pixelmap *backbuffer;
            GLuint                    glDepth;
            GLfloat                   clearValue;
        } asDepth;
    };

} br_device_pixelmap;

#endif

#ifdef __cplusplus
};
#endif
#endif


