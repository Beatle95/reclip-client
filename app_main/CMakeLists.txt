target_sources(
    reclip_app_main PRIVATE
    main.cc
)
target_sources(
  reclip_app_main
  PUBLIC
    FILE_SET base_modules TYPE CXX_MODULES FILES
    application_main.ccm
)

target_link_libraries(
    reclip_app_main PUBLIC
    reclip_core
    reclip_ui
    reclip_communication
)
