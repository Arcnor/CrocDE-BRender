//
// Created by Arcnor on 04/12/2022.
//

#include "drv.h"

/*
 * Output Type info. template
 */
#define F(f) offsetof(br_output_facility, f)

static struct br_tv_template_entry outputFacilityTemplateEntries[] = {
		{BRT_WIDTH_I32,               nullptr, F(width),             BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_WIDTH_MIN_I32,           nullptr, F(width),             BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_WIDTH_MAX_I32,           nullptr, F(width),             BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_HEIGHT_I32,              nullptr, F(height),            BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_HEIGHT_MIN_I32,          nullptr, F(height),            BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_HEIGHT_MAX_I32,          nullptr, F(height),            BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_PIXEL_BITS_I32,          nullptr, F(colour_bits),       BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_PIXEL_TYPE_U8,           nullptr, F(colour_type),       BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_INDEXED_B,               nullptr, F(indexed),           BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_WINDOW_MONITOR_I32,      nullptr, F(monitor),           BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_IDENTIFIER_CSTR,         nullptr, F(identifier),        BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_RENDERER_FACILITY_O,     nullptr, F(renderer_facility), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
		{BRT_TEMPORARY_B,             nullptr, F(temporary),         BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},

		/*
		 * We don't use these, but we need BrDevBeginVar to accept them.
		 * These are passed to pixelmapNew, which will handle them.
		 */
		{BRT_WINDOW_HANDLE_H,         nullptr, 0,                    BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT,},
		{BRT_MSAA_SAMPLES_I32,        nullptr, 0,                    BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT,},
};

#undef F

static int BrType_BPP(int type) {
	switch (type) {
		case BR_PMT_INDEX_8:
			return 8;
		case BR_PMT_RGB_555:
			return 15;
		case BR_PMT_RGB_565:
			return 16;
		case BR_PMT_RGB_888:
			return 24;
		case BR_PMT_RGBX_888:
		case BR_PMT_RGBA_8888:
		case BR_PMT_ARGB_8888:
			return 32;
		default:
			return 0;
	}
}


/*
 * Common object methods
 */
static void BR_CMETHOD_DECL(br_output_facility_bgfx, free)(br_object *_self)
{
	br_output_facility *self = (br_output_facility *)_self;

	ObjectContainerRemove(self->device, (br_object *)self);

	/*
	 * Remove attached objects
	 */
	BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, nullptr, nullptr);

	BrResFreeNoCallback(self);
}

static const char *BR_CMETHOD_DECL(br_output_facility_bgfx, identifier)(br_object *self)
{
	return ((br_output_facility *)self)->identifier;
}

static br_token BR_CMETHOD_DECL(br_output_facility_bgfx, type)(br_object *self)
{
	(void)self;
	return BRT_OUTPUT_FACILITY;
}

static br_boolean BR_CMETHOD_DECL(br_output_facility_bgfx, isType)(br_object *self, br_token t)
{
	(void)self;
	return (t == BRT_OUTPUT_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static br_device *BR_CMETHOD_DECL(br_output_facility_bgfx, device)(br_object *self)
{
	return ((br_output_facility *)self)->device;
}

static br_size_t BR_CMETHOD_DECL(br_output_facility_bgfx, space)(br_object *self)
{
	(void)self;
	return sizeof(br_output_facility);
}

static struct br_tv_template *BR_CMETHOD_DECL(br_output_facility_bgfx, queryTemplate)(br_object *_self)
{
	br_output_facility *self = (br_output_facility *)_self;

	if(self->device->templates.outputFacilityTemplate == nullptr)
		self->device->templates.outputFacilityTemplate = BrTVTemplateAllocate(
				self->device,
				outputFacilityTemplateEntries,
				BR_ASIZE(outputFacilityTemplateEntries)
		);

	return self->device->templates.outputFacilityTemplate;
}

static br_error BR_CMETHOD_DECL(br_output_facility_bgfx, validSource)(br_output_facility *self,
                                                                     br_boolean *bp, br_object *h)
{
	(void)self;
	(void)bp;
	(void)h;
	return BRE_OK;
}

/*
 * Instantiate an output pixelmap from the output type
 */
static br_error BR_CMETHOD_DECL(br_output_facility_bgfx, pixelmapNew)
		(br_output_facility *self, br_device_pixelmap **ppmap, br_token_value *tv)
{
	br_device_pixelmap *pm;

	/*
	 * Create a device pixelmap structure representing display memory
	 */
	pm = DevicePixelmapBGFXAllocate(self->device, self, tv);

	if(pm == nullptr)
		return BRE_FAIL;

	*ppmap = pm;

	return BRE_OK;
}

/*
 * Cannot create new CLUTs, stuck with the single hardware one
 */
static br_error BR_CMETHOD_DECL(br_output_facility_bgfx, clutNew)(br_output_facility *self,
                                                                 br_device_clut **pclut, br_token_value *tv)
{
	(void)self;
	(void)pclut;
	(void)tv;
	return BRE_FAIL;
}

static br_error BR_CMETHOD_DECL(br_output_facility_bgfx, queryCapability)(
		br_output_facility *self, br_token_value *buffer_in, br_token_value *buffer_out, br_size_t size_buffer_out)
{
	(void)self;
	(void)buffer_in;
	(void)buffer_out;
	(void)size_buffer_out;
	return BRE_FAIL;
}


static void *BR_CMETHOD_DECL(br_output_facility_bgfx, listQuery)(struct br_object_container *self)
{
	return ((br_output_facility *)self)->object_list;
}

/*
 * Output facility dispatch table
 */
static const struct br_output_facility_dispatch outputFacilityDispatch = {
		.__reserved0        = nullptr,
		.__reserved1        = nullptr,
		.__reserved2        = nullptr,
		.__reserved3        = nullptr,
		._free              = BR_CMETHOD_REF(br_output_facility_bgfx, free),
		._identifier        = BR_CMETHOD_REF(br_output_facility_bgfx, identifier),
		._type              = BR_CMETHOD_REF(br_output_facility_bgfx, type),
		._isType            = BR_CMETHOD_REF(br_output_facility_bgfx, isType),
		._device            = BR_CMETHOD_REF(br_output_facility_bgfx, device),
		._space             = BR_CMETHOD_REF(br_output_facility_bgfx, space),

		._templateQuery     = BR_CMETHOD_REF(br_output_facility_bgfx, queryTemplate),
		._query             = BR_CMETHOD_REF(br_object, query),
		._queryBuffer       = BR_CMETHOD_REF(br_object, queryBuffer),
		._queryMany         = BR_CMETHOD_REF(br_object, queryMany),
		._queryManySize     = BR_CMETHOD_REF(br_object, queryManySize),
		._queryAll          = BR_CMETHOD_REF(br_object, queryAll),
		._queryAllSize      = BR_CMETHOD_REF(br_object, queryAllSize),

		._listQuery         = BR_CMETHOD_REF(br_output_facility_bgfx, listQuery),
		._tokensMatchBegin  = BR_CMETHOD_REF(br_object_container, tokensMatchBegin),
		._tokensMatch       = BR_CMETHOD_REF(br_object_container, tokensMatch),
		._tokensMatchEnd    = BR_CMETHOD_REF(br_object_container, tokensMatchEnd),
		._addFront          = BR_CMETHOD_REF(br_object_container, addFront),
		._removeFront       = BR_CMETHOD_REF(br_object_container, removeFront),
		._remove            = BR_CMETHOD_REF(br_object_container, remove),
		._find              = BR_CMETHOD_REF(br_object_container, find),
		._findMany          = BR_CMETHOD_REF(br_object_container, findMany),
		._count             = BR_CMETHOD_REF(br_object_container, count),

		._validSource       = BR_CMETHOD_REF(br_output_facility_bgfx, validSource),
		._pixelmapNew       = BR_CMETHOD_REF(br_output_facility_bgfx, pixelmapNew),
		._clutNew           = BR_CMETHOD_REF(br_output_facility_bgfx, clutNew),
		._queryCapability   = BR_CMETHOD_REF(br_output_facility_bgfx, queryCapability),
};


static br_output_facility *OutputFacilityBGFXCreateMode(br_device *dev, br_uint_16 type,
                                                        br_int_32 width, br_int_32 height,
                                                        br_int_32 monitorIndex, br_boolean temporary) {
	br_output_facility *self;
	char tmp[64];

	self = static_cast<br_output_facility *>(BrResAllocate(dev, sizeof(*self), BR_MEMORY_OBJECT));
	self->dispatch = &outputFacilityDispatch;
	self->device = dev;
	self->object_list = BrObjectListAllocate(self);

	/* Fill in display format */
	self->colour_type = type;
	self->colour_bits = BrType_BPP(type);
	self->indexed = (type <= BR_PMT_INDEX_8);
	self->monitor = monitorIndex;

	self->width = width;
	self->height = height;
	self->temporary = temporary;

	/* Attach a descriptive identifier */
	BrSprintfN(tmp, sizeof(tmp) - 1, "%dx%dx%d", self->width, self->height, self->colour_bits);
	BrLogTrace("BGFXREND", "Registering %sscreen mode %s.", temporary ? "temporary " : "", tmp);
	self->identifier = BrResStrDup(self, tmp);

	self->renderer_facility = dev->renderer_facility;

	ObjectContainerAddFront(dev, (br_object *) self);
	return self;
}

br_output_facility *OutputFacilityBGFXCreateTemporary(br_device *dev, br_token_value *tv) {
	int width = -1, height = -1, bpp = -1;

	for (br_token_value *v = tv; v->t != 0; ++v) {
		if (v->t == BRT_WIDTH_I32)
			width = v->v.i32;
		else if (v->t == BRT_HEIGHT_I32)
			height = v->v.i32;
		else if (v->t == BRT_PIXEL_BITS_I32)
			bpp = v->v.i32;
	}

	if (width <= 0 || height <= 0)
		return nullptr;

	br_uint_16 pmt;
	if (bpp == 16)
		pmt = BR_PMT_RGB_565;
	else if (bpp == 24)
		pmt = BR_PMT_RGB_888;
	else if (bpp == 32)
		pmt = BR_PMT_RGBA_8888;
	else
		return nullptr;

	return OutputFacilityBGFXCreateMode(dev, pmt, width, height, -1, 1);
}