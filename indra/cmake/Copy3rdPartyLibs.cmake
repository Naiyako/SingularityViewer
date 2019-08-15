# -*- cmake -*-

# The copy_win_libs folder contains file lists and a script used to
# copy dlls, exes and such needed to run the SecondLife from within
# VisualStudio.

include(CMakeCopyIfDifferent)
include(Linking)
include(Variables)
include(LLCommon)

###################################################################
# set up platform specific lists of files that need to be copied
###################################################################
if(WINDOWS)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibrariesAL)

    set(SHARED_LIB_STAGING_DIR_DEBUG            "${SHARED_LIB_STAGING_DIR}/Debug")
    set(SHARED_LIB_STAGING_DIR_RELWITHDEBINFO   "${SHARED_LIB_STAGING_DIR}/RelWithDebInfo")
    set(SHARED_LIB_STAGING_DIR_RELEASE          "${SHARED_LIB_STAGING_DIR}/Release")

    #*******************************
    # VIVOX - *NOTE: no debug version
    set(vivox_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(vivox_files
        SLVoice.exe
        vivoxplatform.dll
        )
    if (ADDRESS_SIZE EQUAL 64)
        list(APPEND vivox_files
            vivoxsdk_x64.dll
            ortp_x64.dll
            )
    else (ADDRESS_SIZE EQUAL 64)
        list(APPEND vivox_files
            vivoxsdk.dll
            ortp.dll
            )
    endif (ADDRESS_SIZE EQUAL 64)

    #*******************************
    # Misc shared libs 

    set(release_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(release_files
        openjpeg.dll
        nghttp2.dll
        glod.dll
        libhunspell.dll
        )

    if(ADDRESS_SIZE EQUAL 64)
      list(APPEND debug_files
           libcrypto-1_1-x64.dll
           libssl-1_1-x64.dll
           )
      list(APPEND release_files
           libcrypto-1_1-x64.dll
           libssl-1_1-x64.dll
           )
    else(ADDRESS_SIZE EQUAL 64)
      list(APPEND debug_files
           libcrypto-1_1.dll
           libssl-1_1.dll
           )
      list(APPEND release_files
           libcrypto-1_1.dll
           libssl-1_1.dll
           )
    endif(ADDRESS_SIZE EQUAL 64)

    if (LLCOMMON_LINK_SHARED)
      list(APPEND debug_files 
        libapr-1.dll
        libaprutil-1.dll
        libapriconv-1.dll
        )
      list(APPEND release_files 
        libapr-1.dll
        libaprutil-1.dll
        libapriconv-1.dll
        )
    endif (LLCOMMON_LINK_SHARED)

    if(USE_TCMALLOC)
      list(APPEND debug_files libtcmalloc_minimal-debug.dll)
      list(APPEND release_files libtcmalloc_minimal.dll)
    endif(USE_TCMALLOC)

    if(USE_TBBMALLOC)
      list(APPEND debug_files tbbmalloc_debug.dll tbbmalloc_proxy_debug.dll)
      list(APPEND release_files tbbmalloc.dll tbbmalloc_proxy.dll)
    endif(USE_TBBMALLOC)

    if(LLWINDOW_SDL2)
      list(APPEND debug_files SDL2.dll)
      list(APPEND release_files SDL2.dll)
    endif(LLWINDOW_SDL2)

    if(OPENAL)
      list(APPEND debug_files alut.dll OpenAL32.dll)
      list(APPEND release_files alut.dll OpenAL32.dll)
    endif(OPENAL)

    if (FMODSTUDIO)
      list(APPEND debug_files fmodL.dll)
      list(APPEND release_files fmod.dll)
    endif (FMODSTUDIO)

    foreach(redistfullfile IN LISTS CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS)
        get_filename_component(redistfilepath ${redistfullfile} DIRECTORY )
        get_filename_component(redistfilename ${redistfullfile} NAME)
        copy_if_different(
            ${redistfilepath}
            "${SHARED_LIB_STAGING_DIR_RELEASE}"
            out_targets
            ${redistfilename}
            )
        set(third_party_targets ${third_party_targets} ${out_targets})

        copy_if_different(
            ${redistfilepath}
            "${SHARED_LIB_STAGING_DIR_RELWITHDEBINFO}"
            out_targets
            ${redistfilename}
            )
        set(third_party_targets ${third_party_targets} ${out_targets})
    endforeach()

elseif(DARWIN)
    set(SHARED_LIB_STAGING_DIR_DEBUG            "${SHARED_LIB_STAGING_DIR}/Debug/Resources")
    set(SHARED_LIB_STAGING_DIR_RELWITHDEBINFO   "${SHARED_LIB_STAGING_DIR}/RelWithDebInfo/Resources")
    set(SHARED_LIB_STAGING_DIR_RELEASE          "${SHARED_LIB_STAGING_DIR}/Release/Resources")

    set(vivox_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(vivox_files
        SLVoice
        libortp.dylib
        libvivoxplatform.dylib
        libvivoxsdk.dylib
       )
    set(debug_src_dir "${ARCH_PREBUILT_DIRS_DEBUG}")
    set(debug_files
       )
    set(release_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(release_files
        libapr-1.0.dylib
        libapr-1.dylib
        libaprutil-1.0.dylib
        libaprutil-1.dylib
        libexception_handler.dylib
        libopenjpeg.dylib
        libfreetype.6.dylib
        libGLOD.dylib
        libndofdev.dylib
        libnghttp2.dylib
        libnghttp2.14.dylib
        libnghttp2.14.14.0.dylib
       )

    if (OPENAL)
      list(APPEND release_files libopenal.dylib libalut.dylib)
    endif (OPENAL)

    if (FMODSTUDIO)
      list(APPEND debug_files libfmodL.dylib)
      list(APPEND release_files libfmod.dylib)
    endif (FMODSTUDIO)

elseif(LINUX)
    # linux is weird, multiple side by side configurations aren't supported
    # and we don't seem to have any debug shared libs built yet anyways...
    set(SHARED_LIB_STAGING_DIR_DEBUG            "${SHARED_LIB_STAGING_DIR}")
    set(SHARED_LIB_STAGING_DIR_RELWITHDEBINFO   "${SHARED_LIB_STAGING_DIR}")
    set(SHARED_LIB_STAGING_DIR_RELEASE          "${SHARED_LIB_STAGING_DIR}")

    set(vivox_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(vivox_files
        libsndfile.so.1
        libortp.so
        libvivoxoal.so.1
        libvivoxplatform.so
        libvivoxsdk.so
        SLVoice
       )
    # *TODO - update this to use LIBS_PREBUILT_DIR and LL_ARCH_DIR variables
    # or ARCH_PREBUILT_DIRS
    set(debug_src_dir "${ARCH_PREBUILT_DIRS_DEBUG}")
    set(debug_files
       )
    # *TODO - update this to use LIBS_PREBUILT_DIR and LL_ARCH_DIR variables
    # or ARCH_PREBUILT_DIRS
    set(release_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    # *FIX - figure out what to do with duplicate libalut.so here -brad
    set(release_files
        libapr-1.so.0
        libaprutil-1.so.0
        libatk-1.0.so
        libexpat.so
        libexpat.so.1
        libfreetype.so.6.14.0
        libfreetype.so.6
        libfreetype.so
        libGLOD.so
        libgmodule-2.0.so
        libgobject-2.0.so
        libopenal.so
        libopenjpeg.so
        libfontconfig.so.1.9.2
        libfontconfig.so.1
        libfontconfig.so
       )

    if (USE_TCMALLOC)
      list(APPEND release_files "libtcmalloc_minimal.so")
    endif (USE_TCMALLOC)

    if (FMODSTUDIO)
      list(APPEND debug_files "libfmodL.so")
      list(APPEND release_files "libfmod.so")
    endif (FMODSTUDIO)

else(WINDOWS)
    message(STATUS "WARNING: unrecognized platform for staging 3rd party libs, skipping...")
    set(vivox_src_dir "${CMAKE_SOURCE_DIR}/newview/vivox-runtime/i686-linux")
    set(vivox_files "")
    # *TODO - update this to use LIBS_PREBUILT_DIR and LL_ARCH_DIR variables
    # or ARCH_PREBUILT_DIRS
    set(debug_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-linux/lib/debug")
    set(debug_files "")
    # *TODO - update this to use LIBS_PREBUILT_DIR and LL_ARCH_DIR variables
    # or ARCH_PREBUILT_DIRS
    set(release_src_dir "${CMAKE_SOURCE_DIR}/../libraries/i686-linux/lib/release")
    set(release_files "")

    set(debug_llkdu_src "")
    set(debug_llkdu_dst "")
    set(release_llkdu_src "")
    set(release_llkdu_dst "")
    set(relwithdebinfo_llkdu_dst "")
endif(WINDOWS)


################################################################
# Done building the file lists, now set up the copy commands.
################################################################

copy_if_different(
    ${vivox_src_dir}
    "${SHARED_LIB_STAGING_DIR_DEBUG}"
    out_targets 
    ${vivox_files}
    )
set(third_party_targets ${third_party_targets} ${out_targets})

copy_if_different(
    ${vivox_src_dir}
    "${SHARED_LIB_STAGING_DIR_RELEASE}"
    out_targets
    ${vivox_files}
    )
set(third_party_targets ${third_party_targets} ${out_targets})

copy_if_different(
    ${vivox_src_dir}
    "${SHARED_LIB_STAGING_DIR_RELWITHDEBINFO}"
    out_targets
    ${vivox_files}
    )
set(third_party_targets ${third_party_targets} ${out_targets})



#copy_if_different(
#    ${debug_src_dir}
#    "${SHARED_LIB_STAGING_DIR_DEBUG}"
#    out_targets
#    ${debug_files}
#    )
#set(third_party_targets ${third_party_targets} ${out_targets})

copy_if_different(
    ${release_src_dir}
    "${SHARED_LIB_STAGING_DIR_RELEASE}"
    out_targets
    ${release_files}
    )
set(third_party_targets ${third_party_targets} ${out_targets})

copy_if_different(
    ${release_src_dir}
    "${SHARED_LIB_STAGING_DIR_RELWITHDEBINFO}"
    out_targets
    ${release_files}
    )
set(third_party_targets ${third_party_targets} ${out_targets})

if(NOT USESYSTEMLIBS)
  add_custom_target(
      stage_third_party_libs ALL
      DEPENDS ${third_party_targets}
      )
endif(NOT USESYSTEMLIBS)
