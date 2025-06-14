target_sources(
    reclip_ui PRIVATE
    content_window.h
    content_window.cc
    text_view.h
    text_view.cc
    clipboard_controller.h
    clipboard_controller.cc
    host_clipboard_view.h
    host_clipboard_view.cc
)

target_link_libraries(
    reclip_ui PUBLIC
    reclip_core
    Qt6::Core
    Qt6::Widgets
)

target_sources(
    unit_tests PRIVATE
    clipboard_controller_unittest.cc
)
target_link_libraries(
    unit_tests PUBLIC
    reclip_ui
)
