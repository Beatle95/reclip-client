target_sources(
    reclip_core PRIVATE
    clipboard_model.h
    clipboard_model.cc
    clipboard.h
    clipboard.cc
    host_types.h
    host_types.cc
)

if(APPLE)
    target_sources(
        reclip_core PRIVATE
        clipboard_mac.cc
    )
else()
    message(FATAL_ERROR "Not implemented")
endif()

target_link_libraries(
    reclip_core PUBLIC
    Qt6::Core
    Qt6::Widgets
)

target_sources(
    unit_tests PRIVATE
    clipboard_model_unittests.cc
    test_with_event_loop_base.h
    test_with_event_loop_base.cc
)
target_sources(
    server_integration_tests PRIVATE
    test_with_event_loop_base.h
    test_with_event_loop_base.cc
)
