target_sources(
    reclip_communication PRIVATE
    connection.h
    connection_impl.h
    connection_impl.cc
    server_impl.h
    server_impl.cc
    messages.h
    messages.cc
    communication_host.h
    communication_host.cc
    client_server_types.h
)

target_link_libraries(
    reclip_communication PUBLIC
    Qt6::Core
    Qt6::Network
)

target_sources(
    unit_tests PRIVATE
    communication_unittests.cc
)

target_link_libraries(
    unit_tests PUBLIC
    reclip_communication
)
