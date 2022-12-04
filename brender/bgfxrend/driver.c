//
// Created by Arcnor on 03/12/2022.
//

#include "drv.h"

struct br_device *BR_EXPORT BrDrvBGFXBegin(const char *arguments)
{
	return DeviceBGFXAllocate("bgfx", arguments);
}

#ifdef DEFINE_BR_ENTRY_POINT
br_device *BR_EXPORT BrDrv1Begin(const char *arguments)
{
    return BrDrvBGFXBegin(arguments);
}
#endif