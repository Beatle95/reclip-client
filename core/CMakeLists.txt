target_sources(
  reclip_core
  PUBLIC
    FILE_SET core_modules TYPE CXX_MODULES FILES
    host_types.ccm
    shortcut_listener.ccm
    clipboard.ccm
    hosts_model/hosts_list_model.ccm
    hosts_model/host_model.ccm
)

if(APPLE)
    target_sources(
    reclip_core
    PUBLIC
        FILE_SET core_modules TYPE CXX_MODULES FILES
        clipboard_mac.ccm
    )
else()
    message(FATAL_ERROR "Not implemented")
endif()

target_link_libraries(
    reclip_core 
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        qhotkey
)

target_sources(
    unit_tests PRIVATE
    hosts_model/hosts_model_unittests.cc
)
target_sources(
  unit_tests
  PUBLIC
    FILE_SET core_modules TYPE CXX_MODULES FILES
    test_with_event_loop_base.ccm
    test_server_process.ccm
)
target_sources(
    server_integration_tests PRIVATE
    test_with_event_loop_base.ccm
    test_server_process.ccm
)
target_sources(
  server_integration_tests
  PUBLIC
    FILE_SET core_modules TYPE CXX_MODULES FILES
    test_with_event_loop_base.ccm
    test_server_process.ccm
)

target_sources(
    server_integration_tests PRIVATE
    full_client_sitest.cc
)
target_link_libraries(
    server_integration_tests PUBLIC
    reclip_core
    reclip_communication
)
