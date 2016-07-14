if(APPLE)
    option(GORGON_ENABLE_CROSS_COMPILE_MAC "enable cross-compile for Mac OSX")
    set(GORGON_OSX_SDK_ROOT)
    set(GORGON_OSX_SDK_PREFIX)
    set(GORGON_TARGET_OSX_VERSION)
    
    mark_as_advanced(GORGON_ENABLE_CROSS_COMPILE_MAC)
    mark_as_advanced(GORGON_OSX_SDK_ROOT)
    mark_as_advanced(GORGON_OSX_SDK_PREFIX)
    mark_as_advanced(GORGON_TARGET_OSX_VERSION)
    
    if(GORGON_ENABLE_CROSS_COMPILE_MAC)
        set(GORGON_OSX_SDK_ROOT   "" CACHE PATH "Mac OSX SDK root folder")
        set(GORGON_OSX_SDK_PREFIX "" CACHE PATH "Mac OSX SDK folder prefix")
        
        set(GORGON_TARGET_OSX_VERSION "" CACHE STRING "Target OSX version")
        set_property(CACHE GORGON_TARGET_OSX_VERSION PROPERTY STRINGS 10.10 10.9 10.8 10.7)
        
        set(CMAKE_OSX_SYSROOT "${GORGON_OSX_SDK_ROOT}/${GORGON_OSX_SDK_PREFIX}${GORGON_TARGET_OSX_VERSION}.sdk" CACHE PATH "" FORCE)
        set(MACOSX_DEPLOYMENT_TARGET ${CMAKE_OSX_SYSROOT} CACHE PATH "" FORCE)
        
        #set(CMAKE_OSX_DEPLOYMENT_TARGET ${GORGON_TARGET_OSX_VERSION} CACHE STRING "" FORCE)
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.7 CACHE STRING "" FORCE)
        set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")
        
        set(GORGON_OS_VERSION "/MacOSX_${GORGON_TARGET_OSX_VERSION}" CACHE INTERNAL "" )
    else()
        unset(GORGON_OSX_SDK_ROOT         CACHE)
        unset(GORGON_OSX_SDK_PREFIX       CACHE)
        unset(GORGON_TARGET_OSX_VERSION   CACHE)
        unset(CMAKE_OSX_SYSROOT           CACHE)
        unset(MACOSX_DEPLOYMENT_TARGET    CACHE)
        unset(CMAKE_OSX_DEPLOYMENT_TARGET CACHE)
        unset(GORGON_OS_VERSION           CACHE)
    endif()
endif()