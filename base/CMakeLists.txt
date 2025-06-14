target_sources(
    reclip_core PRIVATE
    constants.h
    buildflags.h
    byte_swap.h
    log.h
    log.cc
    observers_list.h
    observer.h
    observer.cc
    scoped_observation.h
)
target_sources(
  reclip_core
  PUBLIC
    FILE_SET base_modules TYPE CXX_MODULES FILES
    preferences.ccm
)

target_sources(
    unit_tests PRIVATE
    observers_unittests.cc
)
