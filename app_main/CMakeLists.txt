target_sources(
    reclip_app_main PRIVATE
    main.cc
    application_main.h
    application_main.cc
)

target_link_libraries(
    reclip_app_main PUBLIC
    reclip_core
    reclip_ui
    reclip_communication
)
