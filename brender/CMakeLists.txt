set(BRENDER_FOLDER "Brender")

add_library(brender INTERFACE)

add_subdirectory(inc)
add_subdirectory(host)
add_subdirectory(fw)
add_subdirectory(fmt)
add_subdirectory(math)
add_subdirectory(pixelmap)
add_subdirectory(std)
add_subdirectory(v1db)

target_link_libraries(brender INTERFACE v1db)

if(BRENDER_BUILD_DRIVERS)
    add_subdirectory(nulldev)
    add_subdirectory(glrend)

    target_link_libraries(brender INTERFACE glrend)
endif()

##
# Full BRender, with drivers.
##
add_library(BRender::Full ALIAS brender)

##
# Core BRender, no drivers.
##
add_library(BRender::Core ALIAS v1db)
