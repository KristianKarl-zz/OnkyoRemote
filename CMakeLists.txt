project(OnkyoRemote)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(OnkyoRemote_SRCS ISCP.cpp Network.cpp OnkyoRemote.cpp main.cpp)
qt4_automoc(${OnkyoRemote_SRCS})
add_executable(OnkyoRemote ${OnkyoRemote_SRCS})
target_link_libraries(OnkyoRemote ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY})
install(TARGETS OnkyoRemote RUNTIME DESTINATION bin)
