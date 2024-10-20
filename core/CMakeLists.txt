target_sources(
    reclip_core PRIVATE
    application_main.cc
    clipboard_listener.cc
    clipboard_model.cc
    server.cc
)

if(APPLE)
    target_sources(
        reclip_core PRIVATE
        clipboard_listener_mac.cc
    )
else()
    message(FATAL_ERROR "Not implemented")
endif()

target_link_libraries(
    reclip_core PUBLIC
    Qt6::Core
    Qt6::Gui
)

target_sources(
    unit_tests PRIVATE
    # TODO:
)
