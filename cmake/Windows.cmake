if(WIN32)
    if(MSVC AND GORGON_TARGET_ARCH EQUAL 64)
    	ADD_DEFINITIONS(/bigobj)
    endif()
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:glut32.lib" CACHE STRING "" FORCE)
endif()
