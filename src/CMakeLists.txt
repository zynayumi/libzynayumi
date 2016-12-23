# Zynayumi
add_subdirectory(zynayumi)

# DSSI plugin
if (HAVE_DSSI)
  add_library(zynayumi-dssi SHARED
    dssizynayumi)

  target_link_libraries(zynayumi-dssi
    zynayumi
    dssiplugin)

  add_subdirectory(dssi)

  add_custom_command(TARGET zynayumi-dssi POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:zynayumi-dssi>
    ../zynayumi-dssi.so)

  install(FILES ${CMAKE_BINARY_DIR}/zynayumi-dssi.so DESTINATION lib/dssi)
endif (HAVE_DSSI)

# VST plugin
if (HAVE_VST)
  include_directories("${PROJECT_SOURCE_DIR}/VST3 SDK")

  add_library(zynayumi-vst SHARED
    vstzynayumi
    "${PROJECT_SOURCE_DIR}/VST3 SDK/public.sdk/source/vst2.x/vstplugmain"
    "${PROJECT_SOURCE_DIR}/VST3 SDK/public.sdk/source/vst2.x/audioeffect"
    "${PROJECT_SOURCE_DIR}/VST3 SDK/public.sdk/source/vst2.x/audioeffectx")

  target_link_libraries(zynayumi-vst
    zynayumi)

endif (HAVE_VST)
