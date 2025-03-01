target_sources(
    reclip_communication PRIVATE
    server_connection.h
    server_connection_impl.h
    server_connection_impl.cc
    server_impl.h
    server_impl.cc
)

target_link_libraries(
    reclip_communication PUBLIC
    Qt6::Core
)

target_sources(
    unit_tests PRIVATE
    communication_unittests.cc
)

target_link_libraries(
    unit_tests PUBLIC
    reclip_communication
)
