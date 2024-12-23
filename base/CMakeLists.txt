target_sources(
    reclip_core PRIVATE
    constants.h
    log.h
    log.cc
    observers_list.h
    observer.h
    observer.cc
    scoped_observation.h
)

target_sources(
    unit_tests PRIVATE
    observers_unittests.cc
)
