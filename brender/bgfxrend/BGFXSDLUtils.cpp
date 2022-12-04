//
// Created by Arcnor on 26/05/2022.
//

#include "BGFXSDLUtils.h"

#include <SDL.h>
#include <SDL_syswm.h>
#include <bx/platform.h>
#include <bgfx/platform.h>

namespace BGFXUtils {

static void *sdlNativeWindowHandle(SDL_Window *_window) {
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(_window, &wmi)) {
		return nullptr;
	}

#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#		if ENTRY_CONFIG_USE_WAYLAND
	wl_egl_window *win_impl = (wl_egl_window*)SDL_GetWindowData(_window, "wl_egl_window");
	if(!win_impl)
	{
		int width, height;
		SDL_GetWindowSize(_window, &width, &height);
		struct wl_surface* surface = wmi.info.wl.surface;
		if(!surface)
			return nullptr;
		win_impl = wl_egl_window_create(surface, width, height);
		SDL_SetWindowData(_window, "wl_egl_window", win_impl);
	}
	return (void*)(uintptr_t)win_impl;
#		else
	return (void*)wmi.info.x11.window;
#		endif
#	elif BX_PLATFORM_OSX || BX_PLATFORM_IOS
	return wmi.info.cocoa.window;
#	elif BX_PLATFORM_WINDOWS
	return wmi.info.win.window;
#   elif BX_PLATFORM_ANDROID
	return wmi.info.android.window;
#	endif // BX_PLATFORM_

	return nullptr;
}

bgfx::PlatformData sdlGetPlatformData(SDL_Window *_window) {
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(_window, &wmi)) {
		return {};
	}

	bgfx::PlatformData pd;
#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#		if ENTRY_CONFIG_USE_WAYLAND
	pd.ndt          = wmi.info.wl.display;
#		else
	pd.ndt          = wmi.info.x11.display;
#		endif
#	else
	pd.ndt = nullptr;
#	endif // BX_PLATFORM_
	pd.nwh = sdlNativeWindowHandle(_window);

	pd.context = nullptr;
	pd.backBuffer = nullptr;
	pd.backBufferDS = nullptr;

	return pd;
}


bool sdlSetWindow(SDL_Window *_window) {
	auto pd = sdlGetPlatformData(_window);
	bgfx::setPlatformData(pd);
	return pd.nwh != nullptr && pd.ndt != nullptr;
}

void sdlDestroyWindow(SDL_Window *_window) {
	if (!_window)
		return;
#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#		if ENTRY_CONFIG_USE_WAYLAND
	wl_egl_window *win_impl = (wl_egl_window*)SDL_GetWindowData(_window, "wl_egl_window");
	if(win_impl)
	{
		SDL_SetWindowData(_window, "wl_egl_window", nullptr);
		wl_egl_window_destroy(win_impl);
	}
#		endif
#	endif
	SDL_DestroyWindow(_window);
}

}