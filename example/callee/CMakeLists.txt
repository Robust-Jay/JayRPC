# set(SRC_LIST userservice.cc ../user.pb.cc)
# add_executable(callee ${SRC_LIST})
# target_link_libraries(callee JayRPC protobuf)

set(SRC_LIST friendservice.cc ../friend.pb.cc)
add_executable(callee ${SRC_LIST})
target_link_libraries(callee JayRPC protobuf)