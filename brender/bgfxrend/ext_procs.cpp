//
// Created by Arcnor on 03/12/2022.
//

#include <array>
#include "drv.h"
#include "bx/bx.h"
#include "bx/string.h"
#include "bx/file.h"
#include "bgfx/bgfx.h"
#include "bimg/bimg.h"
#include "SDL_video.h"
#include "BGFXSDLUtils.h"

constexpr int MainViewId = 0xFE;

int fbWidth, fbHeight;

struct CallbackStub : public bgfx::CallbackI
{
	~CallbackStub() override
	{
	}

	void fatal(const char* _filePath, uint16_t _line, bgfx::Fatal::Enum _code, const char* _str) override
	{
		// TODO: was bgfx::trace but that's not available outside bgfx_p
		fprintf(stderr, "%s:(%d): BGFX FATAL 0x%08x: %s\n", _filePath, _line, _code, _str);

		if (bgfx::Fatal::DebugCheck == _code) {
			bx::debugBreak();
		} else {
			abort();
		}
	}

	void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) override
	{
		char temp[2048];
		char *out = temp;
		va_list argListCopy;
		va_copy(argListCopy, _argList);
		int32_t len = bx::snprintf(out, sizeof(temp), "%s (%d): ", _filePath, _line);
		int32_t total = len + bx::vsnprintf(out + len, sizeof(temp) - len, _format, argListCopy);
		va_end(argListCopy);
		if ((int32_t) sizeof(temp) < total) {
			out = (char *) alloca(total + 1);
			bx::memCopy(out, temp, len);
			bx::vsnprintf(out + len, total - len, _format, _argList);
		}
		out[total] = '\0';
		//bx::debugOutput(out);
		fprintf(stderr, out);
	}

	void profilerBegin(const char* /*_name*/, uint32_t /*_abgr*/, const char* /*_filePath*/, uint16_t /*_line*/) override
	{
	}

	void profilerBeginLiteral(const char* /*_name*/, uint32_t /*_abgr*/, const char* /*_filePath*/, uint16_t /*_line*/) override
	{
	}

	void profilerEnd() override
	{
	}

	uint32_t cacheReadSize(uint64_t /*_id*/) override
	{
		return 0;
	}

	bool cacheRead(uint64_t /*_id*/, void* /*_data*/, uint32_t /*_size*/) override
	{
		return false;
	}

	void cacheWrite(uint64_t /*_id*/, const void* /*_data*/, uint32_t /*_size*/) override
	{
	}

	void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) override
	{
		BX_UNUSED(_filePath, _width, _height, _pitch, _data, _size, _yflip);

		const int32_t len = bx::strLen(_filePath)+5;
		char* filePath = (char*)alloca(len);
		bx::strCopy(filePath, len, _filePath);
		bx::strCat(filePath, len, ".tga");

		bx::FileWriter writer;
		if (bx::open(&writer, filePath)) {
			bimg::imageWriteTga(&writer, _width, _height, _pitch, _data, false, _yflip);
			bx::close(&writer);
		}
	}

	void captureBegin(uint32_t /*_width*/, uint32_t /*_height*/, uint32_t /*_pitch*/, bgfx::TextureFormat::Enum /*_format*/, bool /*_yflip*/) override
	{
		BX_TRACE("Warning: using capture without callback (a.k.a. pointless).");
	}

	void captureEnd() override
	{
	}

	void captureFrame(const void* /*_data*/, uint32_t /*_size*/) override
	{
	}
};

CallbackStub debugCallback;

void *DeviceBGFXCreateContext(br_device *self) {
	auto *sdlWindow = static_cast<SDL_Window *>(self->ext_procs.user);
//	BGFXUtils::sdlSetWindow(sdlWindow);

	SDL_GL_GetDrawableSize(sdlWindow, &fbWidth, &fbHeight);

	bgfx::Init config;
	config.platformData = BGFXUtils::sdlGetPlatformData(sdlWindow);
#ifdef _DEBUG
//	config.callback = &debugCallback;
#endif
	config.type = bgfx::RendererType::Vulkan;
	config.resolution.width = fbWidth;
	config.resolution.height = fbHeight;
	config.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI;

	bgfx::init(config);

	bgfx::setViewClear(MainViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

	bgfx::setViewRect(MainViewId, 0, 0, fbWidth, fbHeight);

	BrLogTrace("BGFXREND", "Renderer: %s", bgfx::getRendererName(bgfx::getRendererType()));
	static std::array<bgfx::RendererType::Enum, 8> RendererList;

	BrLogTrace("BGFXREND", "Supported renderers:");
	auto numRenderers = bgfx::getSupportedRenderers(RendererList.size(), RendererList.data());
	for (int j = 0; j < numRenderers; ++j) {
		BrLogTrace("BGFXREND", "\t%s", bgfx::getRendererName(RendererList[j]));
	}

	// TODO: Invert ImGUI if used
//	if (bgfx::getCaps()->originBottomLeft) {
//		ImageFBUV0.y = 1.0f;
//		ImageFBUV1.y = 0.0f;
//	}

	return nullptr;
}

void DeviceBGFXDeleteContext(br_device *self, void *ctx) {
	bgfx::shutdown();
	BGFXUtils::sdlDestroyWindow(static_cast<SDL_Window *>(self->ext_procs.user));
}