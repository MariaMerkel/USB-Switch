cmake_minimum_required(VERSION 3.27)
project(USBSwitchButton)

set(CMAKE_CXX_STANDARD 17)

add_executable(USBSwitchButton main.cpp CH9326DLL.h)

target_link_libraries(USBSwitchButton ${CMAKE_SOURCE_DIR}/CH9326DLL64.lib Dxva2.lib -static)

add_custom_command(TARGET USBSwitchButton POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:USBSwitchButton> ${CMAKE_SOURCE_DIR}/driver)
add_custom_command(TARGET USBSwitchButton POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/CH9326DLL64.dll ${CMAKE_SOURCE_DIR}/driver)


project(USBSwitchICConfigurator)

add_executable(USBSwitchICConfigurator configurator.cpp CH9326DLL.h)

target_link_libraries(USBSwitchICConfigurator ${CMAKE_SOURCE_DIR}/CH9326DLL64.lib)


project(USBSwitchSwitcher)

add_executable(USBSwitchSwitcher switcher_main.cpp)

target_link_libraries(USBSwitchSwitcher Dxva2.lib -static)