project(reclip_core)

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(
    ${PROJECT_NAME}
    application_main.cc
    clipboard_listener.cc
    moc_clipboard_listener.cc
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
    Qt6::Core
)
