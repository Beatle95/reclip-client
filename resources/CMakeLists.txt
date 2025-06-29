add_custom_target(deploy_resources)

add_dependencies(${PROJECT_NAME} deploy_resources)

if(APPLE)
    set(actual_app_path ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.app/Contents/MacOS)
else()
    set(actual_app_path ${EXECUTABLE_OUTPUT_PATH})
endif()

add_custom_command(
    TARGET deploy_resources POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/style.qss
    ${actual_app_path}/style.qss
)
