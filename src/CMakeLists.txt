aux_source_directory(. SRC_LIST)
add_library(JayRPC ${SRC_LIST})
target_link_libraries(JayRPC muduo_net muduo_base pthread)