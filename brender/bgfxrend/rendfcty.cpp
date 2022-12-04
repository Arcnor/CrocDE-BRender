/*
 * Renderer type methods
 */
#include <stddef.h>
#include "drv.h"

/*
 * Renderer Type info. template
 */
#define F(f) offsetof(struct br_renderer_facility, f)

static struct br_tv_template_entry rendererFacilityTemplateEntries[] = {
    {BRT_IDENTIFIER_CSTR,  nullptr, F(identifier), BRTV_QUERY | BRTV_ALL, BRTV_CONV_COPY,},
    {BRT_RENDERER_MAX_I32, nullptr, 0,             BRTV_QUERY | BRTV_ALL, BRTV_CONV_DIRECT, 1},
};

#undef F


static void BR_CMETHOD_DECL(br_renderer_facility_bgfx, free)(br_object *_self)
{
    br_renderer_facility *self = (br_renderer_facility *)_self;

    /*
     * Detach renderer from device
     */
    ObjectContainerRemove(self->device, (br_object *)self);

    /*
     * Remove attached objects
     */
    BrObjectContainerFree((br_object_container *)self, BR_NULL_TOKEN, nullptr, nullptr);

    BrResFreeNoCallback(self);
}

static const char *BR_CMETHOD_DECL(br_renderer_facility_bgfx, identifier)(br_object *self)
{
    return ((br_renderer_facility *)self)->identifier;
}

static br_token BR_CMETHOD_DECL(br_renderer_facility_bgfx, type)(br_object *self)
{
    (void)self;
    return BRT_RENDERER_FACILITY;
}

static br_boolean BR_CMETHOD_DECL(br_renderer_facility_bgfx, isType)(br_object *self, br_token t)
{
    (void)self;
    return (t == BRT_RENDERER_FACILITY) || (t == BRT_OBJECT_CONTAINER) || (t == BRT_OBJECT);
}

static struct br_device *BR_CMETHOD_DECL(br_renderer_facility_bgfx, device)(br_object *self)
{
    return ((br_renderer_facility *)self)->device;
}

static br_size_t BR_CMETHOD_DECL(br_renderer_facility_bgfx, space)(br_object *self)
{
    (void)self;
    return sizeof(br_renderer_facility);
}

static struct br_tv_template *BR_CMETHOD_DECL(br_renderer_facility_bgfx, templateQuery)(br_object *_self)
{
    br_renderer_facility *self = (br_renderer_facility *)_self;

    if(self->device->templates.rendererFacilityTemplate == nullptr) {
        self->device->templates.rendererFacilityTemplate = BrTVTemplateAllocate(
            self->device,
            (br_tv_template_entry *)rendererFacilityTemplateEntries,
            BR_ASIZE(rendererFacilityTemplateEntries)
        );
    }

    return self->device->templates.rendererFacilityTemplate;
}

static br_error BR_CMETHOD_DECL(br_renderer_facility_bgfx, validDestination)(br_renderer_facility *self,
                                                                           br_boolean *bp, br_object *h)
{
    (void)self;
    (void)bp;
    (void)h;
    return BRE_OK;
}

/*
 * Structure used to unpack the rendererNew arguments
 */
struct newRendererTokens {
    br_object *dest;
};

#define F(f) offsetof(struct newRendererTokens, f)

static struct br_tv_template_entry rendererNewTemplateEntries[] = {
    {BRT_DESTINATION_O,     nullptr, F(dest), BRTV_SET, BRTV_CONV_COPY,},
    {BRT_OUTPUT_FACILITY_O, nullptr, F(dest), BRTV_SET, BRTV_CONV_COPY,},
};

#undef F

/*
 * Create a new renderer
 */
static br_error BR_CMETHOD_DECL(br_renderer_facility_bgfx, rendererNew)(br_renderer_facility *self,
                                                                      struct br_renderer **prenderer,
                                                                      br_token_value *tv)
{
    struct newRendererTokens rt = {.dest = nullptr};
    br_int_32                count;
    br_renderer              *renderer;

    /*
     * Process any options
     */
    if(self->device->templates.rendererNewTemplate == nullptr) {
        self->device->templates.rendererNewTemplate = BrTVTemplateAllocate(
            self->device,
            rendererNewTemplateEntries,
            BR_ASIZE(rendererNewTemplateEntries)
        );
    }

    BrTokenValueSetMany(&rt, &count, nullptr, tv, self->device->templates.rendererNewTemplate);

    /*
     * Check that the destination is valid
     */
    if(rt.dest == nullptr || ObjectDevice(rt.dest) != self->device || !ObjectIsType(rt.dest, BRT_DEVICE_PIXELMAP))
        return BRE_UNSUPPORTED;

    /*
     * Create Renderer
     */
    renderer = RendererBGFXAllocate(self->device, self, (br_device_pixelmap *)rt.dest);

    if(renderer == nullptr)
        return BRE_FAIL;

    *prenderer = renderer;
    return BRE_OK;
}

static void *BR_CMETHOD_DECL(br_renderer_facility_bgfx, listQuery)(br_object_container *self)
{
    return ((br_renderer_facility *)self)->object_list;
}

/*
 * Default dispatch table for renderer type (defined at and of file)
 */
static const struct br_renderer_facility_dispatch rendererFacilityDispatch = {
    .__reserved0        = nullptr,
    .__reserved1        = nullptr,
    .__reserved2        = nullptr,
    .__reserved3        = nullptr,
    ._free              = BR_CMETHOD_REF(br_renderer_facility_bgfx, free),
    ._identifier        = BR_CMETHOD_REF(br_renderer_facility_bgfx, identifier),
    ._type              = BR_CMETHOD_REF(br_renderer_facility_bgfx, type),
    ._isType            = BR_CMETHOD_REF(br_renderer_facility_bgfx, isType),
    ._device            = BR_CMETHOD_REF(br_renderer_facility_bgfx, device),
    ._space             = BR_CMETHOD_REF(br_renderer_facility_bgfx, space),

    ._templateQuery     = BR_CMETHOD_REF(br_renderer_facility_bgfx, templateQuery),
    ._query             = BR_CMETHOD_REF(br_object, query),
    ._queryBuffer       = BR_CMETHOD_REF(br_object, queryBuffer),
    ._queryMany         = BR_CMETHOD_REF(br_object, queryMany),
    ._queryManySize     = BR_CMETHOD_REF(br_object, queryManySize),
    ._queryAll          = BR_CMETHOD_REF(br_object, queryAll),
    ._queryAllSize      = BR_CMETHOD_REF(br_object, queryAllSize),

    ._listQuery         = BR_CMETHOD_REF(br_renderer_facility_bgfx, listQuery),
    ._tokensMatchBegin  = BR_CMETHOD_REF(br_object_container, tokensMatchBegin),
    ._tokensMatch       = BR_CMETHOD_REF(br_object_container, tokensMatch),
    ._tokensMatchEnd    = BR_CMETHOD_REF(br_object_container, tokensMatchEnd),
    ._addFront          = BR_CMETHOD_REF(br_object_container, addFront),
    ._removeFront       = BR_CMETHOD_REF(br_object_container, removeFront),
    ._remove            = BR_CMETHOD_REF(br_object_container, remove),
    ._find              = BR_CMETHOD_REF(br_object_container, find),
    ._findMany          = BR_CMETHOD_REF(br_object_container, findMany),
    ._count             = BR_CMETHOD_REF(br_object_container, count),

    ._validDestination  = BR_CMETHOD_REF(br_renderer_facility_bgfx, validDestination),

    ._rendererNew       = BR_CMETHOD_REF(br_renderer_facility_bgfx, rendererNew),
};


br_renderer_facility *RendererFacilityBGFXInit(br_device *device)
{
	br_renderer_facility *self;

	self = static_cast<br_renderer_facility *>(BrResAllocate(device, sizeof(*self), BR_MEMORY_OBJECT));
	self->dispatch    = &rendererFacilityDispatch;
	self->identifier  = "BGFX Renderer facility";
	self->device      = device;
	self->object_list = BrObjectListAllocate(self);

	/* Create geometry objects */
	if(GeometryV1BucketsBGFXAllocate(self, "V1Buckets") == nullptr ||
	   GeometryPrimitivesNullAllocate(self, "Primitives") == nullptr ||
	   GeometryLightingNullAllocate(self, "Lighting") == nullptr ||
	   GeometryV1ModelBGFXAllocate(self, "V1Model") == nullptr) {
		BrLogError("BGFXREND", "Error creating geometry objects.");
		BrResFree(self);
		return nullptr;
	}

	ObjectContainerAddFront(device, (br_object *)self);
	return (br_renderer_facility *)self;
}