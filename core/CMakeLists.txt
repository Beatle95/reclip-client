target_sources(
    reclip_core PRIVATE
    application_main.h
    application_main.cc
    clipboard_model.h
    clipboard_model.cc
    clipboard.h
    clipboard.cc
    server.h
    server.cc
    client_data.h
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
    clipboard_model_unittests.cc
)
