target_sources(
    reclip_ui PRIVATE
    content_window.h
    content_window.cc
    text_view.h
    text_view.cc
    host_clipboard_view.h
    host_clipboard_view.cc
)
target_sources(
  reclip_ui
  PUBLIC
    FILE_SET ui_modules TYPE CXX_MODULES FILES
    content_window_controller.ccm
    host_controller.ccm
)

target_link_libraries(
    reclip_ui PUBLIC
    reclip_core
    Qt6::Core
    Qt6::Widgets
)

target_sources(
    unit_tests PRIVATE
    content_window_unittest.cc
)
target_link_libraries(
    unit_tests PUBLIC
    reclip_ui
)
