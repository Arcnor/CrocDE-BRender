/*
 * Device methods
 */
#include "drv.h"

#define DEVICE_TITLE "OpenGL v3.2"
#define DEVICE_VERSION BR_VERSION(1,0,0)
#define DEVICE_CREATOR "crocguy0688 <crocguy0688@protonmail.com>"
#define DEVICE_PRODUCT "OpenGL"

/*
 * Default dispatch table for device (defined at end of file)
 */
static const struct br_device_dispatch deviceDispatch;

/*
 * Device info. template
 */
static const char deviceTitle[] = DEVICE_TITLE;

static const char deviceCreator[] = DEVICE_CREATOR;

static const char deviceProduct[] = DEVICE_PRODUCT;


#define F(f) offsetof(br_device, f)
#define A(a) ((br_uintptr_t)(a))

static struct br_tv_template_entry deviceTemplateEntries[] = {
    {BRT_IDENTIFIER_CSTR,     NULL, F(identifier),    BRTV_QUERY | BRTV_ALL,            BRTV_CONV_COPY,},
    {BRT_VERSION_U32,         NULL, 0,                BRTV_QUERY | BRTV_ALL,            BRTV_CONV_DIRECT, DEVICE_VERSION},
    {BRT_BRENDER_VERSION_U32, NULL, 0,                BRTV_QUERY | BRTV_ALL,            BRTV_CONV_DIRECT, __BRENDER__},
    {BRT_DDI_VERSION_U32,     NULL, 0,                BRTV_QUERY | BRTV_ALL,            BRTV_CONV_DIRECT, __BRENDER_DDI__},
    {BRT_CREATOR_CSTR,        NULL, A(deviceCreator), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY,},
    {BRT_TITLE_CSTR,          NULL, A(deviceTitle),   BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY,},
    {BRT_PRODUCT_CSTR,        NULL, A(deviceProduct), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY,},
    {BRT_OPENGL_VERSION_CSTR, NULL, F(gl_version),    BRTV_QUERY | BRTV_ALL,            BRTV_CONV_COPY,},
    {BRT_OPENGL_VENDOR_CSTR,  NULL, F(gl_vendor),     BRTV_QUERY | BRTV_ALL,            BRTV_CONV_COPY,},
    {BRT_OPENGL_RENDERER_CSTR,NULL, F(gl_renderer),   BRTV_QUERY | BRTV_ALL,            BRTV_CONV_COPY,},
};

#undef F
#undef A

/*
 * Structure used to unpack the device creation tokens/values
 */
typedef struct device_create_tokens {
    br_device_gl_ext_procs *ext_procs;
    char                   *vertex_shader;
    char                   *fragment_shader;

} device_create_tokens;

/*
 * List of tokens which are not significant in matching (for output facilities)
 */
static br_token insignificantMatchTokens[] = {
    BRT_WIDTH_I32,
    BRT_HEIGHT_I32,
    BRT_PIXEL_BITS_I32,
    BRT_PIXEL_TYPE_U8,
    BRT_WINDOW_MONITOR_I32,
    BRT_MSAA_SAMPLES_I32,
    BR_NULL_TOKEN,
};

/*
 * Structure used to unpack driver arguments
 */
#define F(f)   offsetof(device_create_tokens, f)

static struct br_tv_template_entry deviceArgsTemplateEntries[] = {
    {BRT_OPENGL_EXT_PROCS_P,         NULL, F(ext_procs),       BRTV_SET, BRTV_CONV_COPY,},
    {BRT_OPENGL_VERTEX_SHADER_STR,   NULL, F(vertex_shader),   BRTV_SET, BRTV_CONV_COPY,},
    {BRT_OPENGL_FRAGMENT_SHADER_STR, NULL, F(fragment_shader), BRTV_SET, BRTV_CONV_COPY,},
};

#undef F

static GLuint build_white_texture(void)
{
    const static uint8_t white_rgba[] = {255, 255, 255, 255};

    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white_rgba);
    return tex;
}

static GLuint build_checkerboard_texture(void)
{
    extern const uint8_t checkerboard_rgba[];

    GLuint tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerboard_rgba);

    return tex;
}

br_device *DeviceGLAllocate(const char *identifier, const char *arguments)
{
    br_device      *self;
    br_token_value args_tv[256];
    br_int_32      count;
    br_tv_template *deviceArgs;

    device_create_tokens tokens = {
        .ext_procs       = NULL,
        .vertex_shader   = NULL,
        .fragment_shader = NULL,
    };

    /*
     * Set up device block and resource anchor
     */
    self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT);
    self->res         = BrResAllocate(self, 0, BR_MEMORY_DRIVER);
    self->identifier  = identifier;
    self->dispatch    = &deviceDispatch;
    self->device      = self;
    self->object_list = BrObjectListAllocate(self);

    if(arguments != NULL) {
        deviceArgs = BrTVTemplateAllocate(
            self,
            deviceArgsTemplateEntries,
            BR_ASIZE(deviceArgsTemplateEntries)
        );
        BrStringToTokenValue(args_tv, sizeof(args_tv), arguments);
        BrTokenValueSetMany(&tokens, &count, NULL, args_tv, deviceArgs);
    }

    if(tokens.ext_procs == NULL) {
        BrResFreeNoCallback(self);
        return NULL;
    }

    /*
     * Make a copy, so they can't switch things out from under us.
     */
    self->ext_procs = *tokens.ext_procs;

    self->vertex_shader   = tokens.vertex_shader;
    self->fragment_shader = tokens.fragment_shader;

    if((self->gl_context = DeviceGLCreateContext(self)) == NULL) {
        BrResFreeNoCallback(self);
        return NULL;
    }

    if(DeviceGLMakeCurrent(self, self->gl_context) != BRE_OK)
        goto cleanup_context;

    if(gladLoadGLLoader(DeviceGLGetGetProcAddress(self)) == 0) {
        BrLogError("GLREND", "Unable to load OpenGL functions.");
        goto cleanup_context;
    }

    self->gl_version  = (const char *)glGetString(GL_VERSION);
    self->gl_vendor   = (const char *)glGetString(GL_VENDOR);
    self->gl_renderer = (const char *)glGetString(GL_RENDERER);

    BrLogTrace("GLREND", "OpenGL Version  = %s", self->gl_version);
    BrLogTrace("GLREND", "OpenGL Vendor   = %s", self->gl_vendor);
    BrLogTrace("GLREND", "OpenGL Renderer = %s", self->gl_renderer);

    if(VIDEO_Open(&self->video, self->vertex_shader, self->fragment_shader) == NULL)
        goto cleanup_context;

    self->tex_white = build_white_texture();
    self->tex_checkerboard = build_checkerboard_texture();

    /*
     * We can't use BRender's fonts directly, so build a POT texture with
     * glyph from left-to-right. All fonts have 256 possible characters.
     */

    BrLogTrace("GLREND", "Building fixed 3x5 font atlas.");
    (void)FontGLBuildAtlas(&self->font_fixed3x5, BrFontFixed3x5, 128, 64);

    BrLogTrace("GLREND", "Building proportional 4x6 font atlas.");
    (void)FontGLBuildAtlas(&self->font_prop4x6, BrFontProp4x6, 128, 64);

    BrLogTrace("GLREND", "Building proportional 7x9 font atlas.");
    (void)FontGLBuildAtlas(&self->font_prop7x9, BrFontProp7x9, 256, 64);

    if((self->renderer_facility = RendererFacilityGLInit(self)) == NULL)
        goto cleanup_context;

    if((self->output_facility = OutputFacilityGLInit(self, self->renderer_facility)) == NULL)
        goto cleanup_context;

    return self;

cleanup_context:
    DeviceGLDeleteContext(self, self->gl_context);
    BrResFree(self);
    return NULL;
}

static void BR_CMETHOD_DECL(br_device_gl, free)(struct br_object *_self)
{
    br_device *self = (br_device *)_self;

    glDeleteTextures(1, &self->font_prop7x9.tex);
    glDeleteTextures(1, &self->font_prop4x6.tex);
    glDeleteTextures(1, &self->font_fixed3x5.tex);
    glDeleteTextures(1, &self->tex_checkerboard);
    glDeleteTextures(1, &self->tex_white);

    VIDEO_Close(&self->video);

    DeviceGLDeleteContext(self, self->gl_context);

    /*
     * Remove attached objects
     */
    BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

    /*
     * Remove resources
     */
    BrResFreeNoCallback(self);
}

static const char *BR_CMETHOD_DECL(br_device_gl, identifier)(struct br_object *self)
{
    return ((br_device *)self)->identifier;
}

static br_token BR_CMETHOD_DECL(br_device_gl, type)(struct br_object *self)
{
    (void)self;
    return BRT_DEVICE;
}

static br_boolean BR_CMETHOD_DECL(br_device_gl, isType)(struct br_object *self, br_token t)
{
    (void)self;
    return (t == BRT_DEVICE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_device *BR_CMETHOD_DECL(br_device_gl, device)(struct br_object *self)
{
    return ((br_device *)self)->device;
}

static br_size_t BR_CMETHOD_DECL(br_device_gl, space)(struct br_object *self)
{
    (void)self;
    return sizeof(br_device);
}

static struct br_tv_template *BR_CMETHOD_DECL(br_device_gl, templateQuery)(struct br_object *_self)
{
    br_device *self = (br_device *)_self;

    if(self->templates.deviceTemplate == NULL) {
        self->templates.deviceTemplate = BrTVTemplateAllocate(
            self,
            (br_tv_template_entry *)deviceTemplateEntries,
            BR_ASIZE(deviceTemplateEntries)
        );
    }

    return self->templates.deviceTemplate;
}

static void *BR_CMETHOD_DECL(br_device_gl, listQuery)(struct br_object_container *self)
{
    return ((br_device *)self)->object_list;
}


/*
 * We can't use the default token matching unfortunately, as we need to ignore
 * the window handle and MSAA tokens.
 *
 * Taken from objectc.c
 */
struct token_match {
    br_token_value *original;
    br_token_value *query;
    br_token       type;
    br_int_32      n;
    void           *extra;
    br_size_t      extra_size;
};

void *BR_CMETHOD_DECL(br_device_gl, tokensMatchBegin)(struct br_object_container *_self, br_token t, br_token_value *tv)
{
    br_device          *self = (br_device *)_self;
    struct token_match *tm;
    br_int_32          i;

    if(tv == NULL)
        return NULL;

    tm = BrResAllocate(self->res, sizeof(*tm), BR_MEMORY_APPLICATION);
    tm->original = tv;
    tm->type     = t;

    for(i = 0; tv[i].t != BR_NULL_TOKEN; i++);

    tm->n     = i + 1;
    tm->query = BrResAllocate(tm, tm->n * sizeof(br_token_value), BR_MEMORY_APPLICATION);
    BrMemCpy(tm->query, tv, i * sizeof(br_token_value));
    return (void *)tm;
}

br_boolean BR_CMETHOD_DECL(br_device_gl, tokensMatch)(struct br_object_container *self, br_object *h, void *arg)
{
    struct token_match *tm = arg;
    br_size_t          s;
    br_int_32          n;

    (void)self;

    if(arg == NULL)
        return BR_TRUE;

    /*
     * Make a query on the object and then compare with the original tokens
     */
    ObjectQueryManySize(h, &s, tm->query);

    if(s > tm->extra_size) {
        if(tm->extra)
            BrResFree(tm->extra);
        tm->extra      = BrResAllocate(tm, s, BR_MEMORY_APPLICATION);
        tm->extra_size = s;
    }

    ObjectQueryMany(h, tm->query, tm->extra, tm->extra_size, &n);

    /*
     * Ensure that all tokens were found
     */
    if(tm->query[n].t != BR_NULL_TOKEN)
        return BR_FALSE;

    /*
     * Compare the two token lists
     */
    if(tm->type == BRT_OUTPUT_FACILITY) {
        return BrTokenValueComparePartial(tm->original, tm->query, insignificantMatchTokens);
    } else {
        return BrTokenValueCompare(tm->original, tm->query);
    }
}

void BR_CMETHOD_DECL(br_device_gl, tokensMatchEnd)(struct br_object_container *self, void *arg)
{
    (void)self;

    if(arg)
        BrResFree(arg);
}

/*
 * Default dispatch table for device
 */
static const struct br_device_dispatch deviceDispatch = {
    .__reserved0        = NULL,
    .__reserved1        = NULL,
    .__reserved2        = NULL,
    .__reserved3        = NULL,
    ._free              = BR_CMETHOD_REF(br_device_gl, free),
    ._identifier        = BR_CMETHOD_REF(br_device_gl, identifier),
    ._type              = BR_CMETHOD_REF(br_device_gl, type),
    ._isType            = BR_CMETHOD_REF(br_device_gl, isType),
    ._device            = BR_CMETHOD_REF(br_device_gl, device),
    ._space             = BR_CMETHOD_REF(br_device_gl, space),

    ._templateQuery     = BR_CMETHOD_REF(br_device_gl, templateQuery),
    ._query             = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer       = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany         = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize     = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll          = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize      = BR_CMETHOD_REF(br_object, queryAllSize),

    ._listQuery         = BR_CMETHOD_REF(br_device_gl, listQuery),
    ._tokensMatchBegin  = BR_CMETHOD_REF(br_device_gl, tokensMatchBegin),
    ._tokensMatch       = BR_CMETHOD_REF(br_device_gl, tokensMatch),
    ._tokensMatchEnd    = BR_CMETHOD_REF(br_device_gl, tokensMatchEnd),
    ._addFront          = BR_CMETHOD_REF(br_object_container, addFront),
    ._removeFront       = BR_CMETHOD_REF(br_object_container, removeFront),
    ._remove            = BR_CMETHOD_REF(br_object_container, remove),
    ._find              = BR_CMETHOD_REF(br_object_container, find),
    ._findMany          = BR_CMETHOD_REF(br_object_container, findMany),
    ._count             = BR_CMETHOD_REF(br_object_container, count),
};
