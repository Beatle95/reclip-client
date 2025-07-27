target_sources(
    reclip_core PRIVATE
    buildflags.h
    log.h
    log.cc
)
target_sources(
  reclip_core
  PUBLIC
    FILE_SET base_modules TYPE CXX_MODULES FILES
    constants.ccm
    byte_swap.ccm
    preferences.ccm
    observer_helper.ccm
    observer.ccm
    observers_list.ccm
    scoped_observation.ccm
)

target_sources(
    unit_tests PRIVATE
    observers_unittests.cc
)
