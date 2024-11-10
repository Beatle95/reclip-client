target_sources(
    reclip_core PRIVATE
    application_main.cc
    clipboard_model.cc
    clipboard_controller.cc
    clipboard.cc
    server.cc
)

if(APPLE)
    target_sources(
        reclip_core PRIVATE
        clipboard_mac.cc
    )
else()
    message(FATAL_ERROR "Not implemented")
endif()

target_sources(
    unit_tests PRIVATE
    # TODO:
)
