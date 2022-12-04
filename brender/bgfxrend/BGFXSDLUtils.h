//
// Created by Arcnor on 26/05/2022.
//

#ifndef B17ME_BGFXSDLUTILS_H
#define B17ME_BGFXSDLUTILS_H

#include <SDL.h>
#include "bgfx/bgfx.h"

namespace BGFXUtils {
	bool sdlSetWindow(SDL_Window *_window);

	void sdlDestroyWindow(SDL_Window *_window);
	bgfx::PlatformData sdlGetPlatformData(SDL_Window *_window);
}

#endif //B17ME_BGFXSDLUTILS_H
