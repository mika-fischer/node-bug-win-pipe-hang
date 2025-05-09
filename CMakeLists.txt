cmake_minimum_required(VERSION 3.15)
project(node-bug-win-pipe-hang)

add_executable(child child.c)

find_package(libuv CONFIG)
if (libuv_FOUND)
  add_executable(parent parent.c)
  target_link_libraries(parent libuv::uv_a)
  add_executable(parent2 parent2.c)
  target_link_libraries(parent2 libuv::uv_a)
endif ()
