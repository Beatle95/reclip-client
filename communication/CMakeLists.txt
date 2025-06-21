target_sources(
  reclip_communication
  PUBLIC
    FILE_SET base_modules TYPE CXX_MODULES FILES
    message_types.ccm
    communication_manager.ccm
    message_reassembler.ccm
    serialization.ccm
    connection_info_provider.ccm
    server.ccm
    server_impl.ccm
    server_connection.ccm
    server_connection_impl.ccm
)

target_link_libraries(
    reclip_communication PUBLIC
    reclip_core
    Qt6::Core
    Qt6::Network
)

target_sources(
    unit_tests PRIVATE
    communication_unittests.cc
    message_reassembler_unittest.cc
)
target_link_libraries(
    unit_tests PUBLIC
    reclip_communication
)

target_sources(
    server_integration_tests PRIVATE
    communication_protocol_sitest.cc
)
target_link_libraries(
    server_integration_tests PUBLIC
    reclip_communication
)
