add_executable(vasSAT vasSAT.cpp)
target_link_libraries (vasSAT PRIVATE vasSATLib)

install(TARGETS vasSAT RUNTIME DESTINATION bin)
