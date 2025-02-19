project(communication_lib)

target_sources(
    reclip_communication PRIVATE
    server_connection.h
    server_connection.cc
    server_connection_impl.h
    server_connection_impl.cc
)
