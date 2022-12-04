add_library(bgfxrend
		include/brbgfxrend.h
		include/brbgfxrend_p.h

		device.c
		driver.c

		renderer.h
		renderer.cpp

		ext_procs.cpp

		outfcty.cpp
		devpixmp.cpp
		rendfcty.cpp
		gv1buckt.cpp
		gv1model.cpp

		gstored.cpp

		BGFXSDLUtils.cpp
		BGFXSDLUtils.h
		)

# FIXME: Remove C++20 requirement (mainly used for designated initializers)
set_target_properties(bgfxrend
		PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
		)

target_include_directories(bgfxrend PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(bgfxrend PUBLIC include)

target_link_libraries(bgfxrend PRIVATE bgfx bx bimg SDL2::SDL2)
target_link_libraries(bgfxrend PRIVATE fw v1db nulldev)