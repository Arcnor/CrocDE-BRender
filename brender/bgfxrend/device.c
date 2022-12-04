//
// Created by Arcnor on 03/12/2022.
//

#define DEVICE_TITLE "BGFX"
#define DEVICE_VERSION BR_VERSION(1,0,0)
#define DEVICE_CREATOR "Edu Garcia <egarcia@reversedgames.com>"
#define DEVICE_PRODUCT "BGFX"

/*
 * Device methods
 */
#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

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

#undef F
#undef A

#define F(f) offsetof(br_device, f)
#define A(a) ((br_uintptr_t)(a))

static const struct br_tv_template_entry deviceTemplateEntries[] = {
		{BRT_IDENTIFIER_CSTR,     NULL, F(identifier),    BRTV_QUERY | BRTV_ALL,            BRTV_CONV_COPY,},
		{BRT_VERSION_U32,         NULL, 0,                BRTV_QUERY | BRTV_ALL,            BRTV_CONV_DIRECT, DEVICE_VERSION},
		{BRT_BRENDER_VERSION_U32, NULL, 0,                BRTV_QUERY | BRTV_ALL,            BRTV_CONV_DIRECT, __BRENDER__},
		{BRT_DDI_VERSION_U32,     NULL, 0,                BRTV_QUERY | BRTV_ALL,            BRTV_CONV_DIRECT, __BRENDER_DDI__},
		{BRT_CREATOR_CSTR,        NULL, A(deviceCreator), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY,},
		{BRT_TITLE_CSTR,          NULL, A(deviceTitle),   BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY,},
		{BRT_PRODUCT_CSTR,        NULL, A(deviceProduct), BRTV_QUERY | BRTV_ALL | BRTV_ABS, BRTV_CONV_COPY,},
//		{BRT_OPENGL_VERSION_CSTR, NULL, F(gl_version),    BRTV_QUERY | BRTV_ALL,            BRTV_CONV_COPY,},
//		{BRT_OPENGL_VENDOR_CSTR,  NULL, F(gl_vendor),     BRTV_QUERY | BRTV_ALL,            BRTV_CONV_COPY,},
//		{BRT_OPENGL_RENDERER_CSTR,NULL, F(gl_renderer),   BRTV_QUERY | BRTV_ALL,            BRTV_CONV_COPY,},
};

#undef F
#undef A

/*
 * Structure used to unpack the device creation tokens/values
 */
typedef struct device_create_tokens {
	void                     *sdl_window;
	br_device_bgfx_ext_procs *ext_procs;
	char                     *vertex_shader;
	char                     *fragment_shader;

} device_create_tokens;

/*
 * Structure used to unpack driver arguments
 */
#define F(f)   offsetof(device_create_tokens, f)

static struct br_tv_template_entry deviceArgsTemplateEntries[] = {
		{BRT_WINDOW_HANDLE_H,            NULL, F(sdl_window),      BRTV_SET, BRTV_CONV_COPY,},
		{BRT_OPENGL_EXT_PROCS_P,         NULL, F(ext_procs),       BRTV_SET, BRTV_CONV_COPY,},
		{BRT_OPENGL_VERTEX_SHADER_STR,   NULL, F(vertex_shader),   BRTV_SET, BRTV_CONV_COPY,},
		{BRT_OPENGL_FRAGMENT_SHADER_STR, NULL, F(fragment_shader), BRTV_SET, BRTV_CONV_COPY,},
};

#undef F

/*
 * Set up a static device object
 */
struct br_device *DeviceBGFXAllocate(const char *identifier, const char *arguments)
{
	br_device      *self;
	br_token_value args_tv[256];
	br_int_32      count;
	br_tv_template *deviceArgs;

	device_create_tokens tokens = {
			.sdl_window      = NULL,
			.ext_procs       = NULL,
			.vertex_shader   = NULL,
			.fragment_shader = NULL,
	};

	/*
	 * Set up device block and resource anchor
	 */
	self = BrResAllocate(NULL, sizeof(*self), BR_MEMORY_OBJECT);
	self->res = BrResAllocate(self, 0, BR_MEMORY_DRIVER);
	if(self->res == NULL) {
		BrResFreeNoCallback(self);
		return NULL;
	}

	self->identifier = identifier;
	self->dispatch = &deviceDispatch;
	self->device = self;

	self->object_list = BrObjectListAllocate(self);
	if(self->object_list == NULL) {
		BrResFreeNoCallback(self);
		return NULL;
	}

	self->templates.deviceTemplate = BrTVTemplateAllocate(
			self,
			deviceTemplateEntries,
			BR_ASIZE(deviceTemplateEntries)
	);

	if(self->templates.deviceTemplate == NULL) {
		BrResFreeNoCallback(self);
		return NULL;
	}

	if(arguments != NULL) {
		deviceArgs = BrTVTemplateAllocate(
				self,
				deviceArgsTemplateEntries,
				BR_ASIZE(deviceArgsTemplateEntries)
		);
		BrStringToTokenValue(args_tv, sizeof(args_tv), arguments);
		BrTokenValueSetMany(&tokens, &count, NULL, args_tv, deviceArgs);
	}


	/*
	 * If we're directly given an SDL_Window pointer, force SDL.
	 */
	if(tokens.sdl_window != NULL) {
		extern const br_device_bgfx_ext_procs sdl_procs;

		self->ext_procs      = sdl_procs;
		self->ext_procs.user = tokens.sdl_window;
	} else if(tokens.ext_procs == NULL) {
		BrResFreeNoCallback(self);
		return NULL;
	} else {
		/*
		 * Make a copy, so they can't switch things out from under us.
		 */
		self->ext_procs = *tokens.ext_procs;
	}

	self->vertex_shader   = tokens.vertex_shader;
	self->fragment_shader = tokens.fragment_shader;

	DeviceBGFXCreateContext(self);

	self->renderer_facility = RendererFacilityBGFXInit(self);
	if(self->renderer_facility == NULL) {
		goto cleanup_context;
	}

	return self;

cleanup_context:
	DeviceBGFXDeleteContext(self, NULL);
	BrResFree(self);
	return NULL;
}

static void BR_CMETHOD_DECL(br_device_bgfx, free)(struct br_object *_self)
{
	br_device *self = (br_device *)_self;

	DeviceBGFXDeleteContext(self, NULL);

	/*
	 * Remove attached objects
	 */
	BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, NULL, NULL);

	/*
	 * Remove resources
	 */
	BrResFreeNoCallback(self);
}

static const char *BR_CMETHOD_DECL(br_device_bgfx, identifier)(struct br_object *self)
{
	return ((br_device*)self)->identifier;
}

static br_token BR_CMETHOD_DECL(br_device_bgfx, type)(struct br_object *self)
{
	(void)self;
	return BRT_DEVICE;
}

static br_boolean BR_CMETHOD_DECL(br_device_bgfx, isType)(struct br_object *self, br_token t)
{
	(void)self;
	return (t == BRT_DEVICE) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_device *BR_CMETHOD_DECL(br_device_bgfx, device)(struct br_object *self)
{
	return ((br_device *)self)->device;
}

static br_size_t BR_CMETHOD_DECL(br_device_bgfx, space)(struct br_object *self)
{
	(void)self;
	return sizeof(br_device);
}

static struct br_tv_template *BR_CMETHOD_DECL(br_device_bgfx, templateQuery)(struct br_object *_self)
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

static void *BR_CMETHOD_DECL(br_device_bgfx, listQuery)(struct br_object_container *self)
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
	br_int_32      n;
	void           *extra;
	br_size_t      extra_size;
};

/*
 * List of tokens which are not significant in matching (for output facilities)
 */
static br_token insignificantMatchTokens[] = {
		BRT_WINDOW_HANDLE_H,
		BRT_MSAA_SAMPLES_I32,
		BRT_TEMPORARY_B,
		BR_NULL_TOKEN,
};

void *BR_CMETHOD_DECL(br_device_bgfx, tokensMatchBegin)(struct br_object_container *_self, br_token t, br_token_value *tv)
{
	br_device          *self = (br_device *)_self;
	struct token_match *tm;
	br_int_32          i;

	(void)t;

	if(tv == NULL)
		return NULL;

	tm = BrResAllocate(self->res, sizeof(*tm), BR_MEMORY_APPLICATION);
	tm->original = tv;

	for(i = 0; tv[i].t != BR_NULL_TOKEN; i++);

	tm->n     = i + 1;
	tm->query = BrResAllocate(tm, tm->n * sizeof(br_token_value), BR_MEMORY_APPLICATION);
	BrMemCpy(tm->query, tv, i * sizeof(br_token_value));
	return (void *)tm;
}

br_boolean BR_CMETHOD_DECL(br_device_bgfx, tokensMatch)(struct br_object_container *self, br_object *h, void *arg)
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
	return BrTokenValueComparePartial(tm->original, tm->query, insignificantMatchTokens);
}

void BR_CMETHOD_DECL(br_device_bgfx, tokensMatchEnd)(struct br_object_container *self, void *arg)
{
	(void)self;

	if(arg)
		BrResFree(arg);
}

/*
 * Custom find implementation to create an appropriate output facility on-demand.
 */
br_error BR_CMETHOD_DECL(br_device_bgfx, find)(br_object_container *self, br_object **ph, br_token type,
                                               const char *pattern, br_token_value *tv)
{
	br_error r = BR_CMETHOD(br_object_container, find)(self, ph, type, pattern, tv);
	if(r == BRE_OK)
		return BRE_OK;

	if(r == BRE_FAIL && type != BRT_OUTPUT_FACILITY)
		return BRE_FAIL;

	if(OutputFacilityBGFXCreateTemporary((br_device *)self, tv) == NULL)
		return BRE_FAIL;

	return BR_CMETHOD(br_object_container, find)(self, ph, type, pattern, tv);
}

/*
 * Default dispatch table for device
 */
static const struct br_device_dispatch deviceDispatch = {
		.__reserved0        = NULL,
		.__reserved1        = NULL,
		.__reserved2        = NULL,
		.__reserved3        = NULL,
		._free              = BR_CMETHOD_REF(br_device_bgfx, free),
		._identifier        = BR_CMETHOD_REF(br_device_bgfx, identifier),
		._type              = BR_CMETHOD_REF(br_device_bgfx, type),
		._isType            = BR_CMETHOD_REF(br_device_bgfx, isType),
		._device            = BR_CMETHOD_REF(br_device_bgfx, device),
		._space             = BR_CMETHOD_REF(br_device_bgfx, space),

		._templateQuery     = BR_CMETHOD_REF(br_device_bgfx, templateQuery),
		._query             = BR_CMETHOD_REF(br_object, query),
		._queryBuffer       = BR_CMETHOD_REF(br_object, queryBuffer),
		._queryMany         = BR_CMETHOD_REF(br_object, queryMany),
		._queryManySize     = BR_CMETHOD_REF(br_object, queryManySize),
		._queryAll          = BR_CMETHOD_REF(br_object, queryAll),
		._queryAllSize      = BR_CMETHOD_REF(br_object, queryAllSize),

		._listQuery         = BR_CMETHOD_REF(br_device_bgfx, listQuery),
		._tokensMatchBegin  = BR_CMETHOD_REF(br_device_bgfx, tokensMatchBegin),
		._tokensMatch       = BR_CMETHOD_REF(br_device_bgfx, tokensMatch),
		._tokensMatchEnd    = BR_CMETHOD_REF(br_device_bgfx, tokensMatchEnd),
		._addFront          = BR_CMETHOD_REF(br_object_container, addFront),
		._removeFront       = BR_CMETHOD_REF(br_object_container, removeFront),
		._remove            = BR_CMETHOD_REF(br_object_container, remove),
		._find              = BR_CMETHOD_REF(br_device_bgfx, find),
		._findMany          = BR_CMETHOD_REF(br_object_container, findMany),
		._count             = BR_CMETHOD_REF(br_object_container, count),
};
