include(ECMMarkAsTest)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/../src # fixme replace with the right variables
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

set(testCosuSettings_SRCS
  ../src/CosuSettings.cpp
  testCosuSettings.cpp
)
add_executable(testCosuSettings ${testCosuSettings_SRCS})
target_link_libraries(testCosuSettings
  ${SSU_LIBRARIES}
  Qt5::Test
)
add_test(NAME CosuSettings COMMAND testCosuSettings)
ecm_mark_as_test(testCosuSettings)

set(testCosuVariables_SRCS
  ../src/cosu_debug.cpp
  ../src/CosuVariables.cpp
  testCosuVariables.cpp
)
add_executable(testCosuVariables ${testCosuVariables_SRCS})
target_link_libraries(testCosuVariables
  Qt5::Test
)
add_test(NAME CosuVariables COMMAND testCosuVariables)
ecm_mark_as_test(testCosuVariables)


set(testCosuServiceRuntime_SRCS
  ../src/cosu_debug.cpp
  ../src/CosuServiceRuntime.cpp
  ../src/CosuVariables.cpp
  ../src/CosuSettings.cpp
  testCosuServiceRuntime.cpp
)
add_executable(testCosuServiceRuntime ${testCosuServiceRuntime_SRCS})
target_link_libraries(testCosuServiceRuntime
  Qt5::Test
)

add_test(NAME CosuServiceRuntime COMMAND testCosuServiceRuntime)
ecm_mark_as_test(testCosuServiceRuntime)


if(packagekitqt5_FOUND)
  list(APPEND resolver_SRCS
    ../src/cosu_debug_packagekit.cpp
    ../src/CosuPackageKit.cpp)
endif()

if(ZYPP_FOUND)
  list(APPEND resolver_SRCS
    ../src/cosu_debug_zypp.cpp
    ../src/CosuZypp.cpp)
  # libzypp requires exceptions
  kde_source_files_enable_exceptions(../src/CosuZypp.cpp)
endif()

set(testCosuService_SRCS
  ../src/cosu_debug.cpp
  ../src/cosu_debug_service.cpp
  ../src/CosuSettings.cpp
  ../src/CosuVariables.cpp
  ../src/CosuService.cpp
  ../src/CosuResolveAbstract.cpp
  ../src/CosuServiceRuntime.cpp
  testCosuService.cpp
  ${resolver_SRCS}
)

qt5_generate_dbus_interface(
  ../src/CosuService.h
  ${COSU_DBUS_INTERFACE}.xml
  OPTIONS -a
)
qt5_add_dbus_adaptor(testCosuService_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/${COSU_DBUS_INTERFACE}.xml
  ../src/CosuService.h
  CosuService
)
add_executable(testCosuService ${testCosuService_SRCS})

target_link_libraries(testCosuService
  Qt5::Test
  Qt5::DBus
  ${PackageKitQt5_LIBRARIES}
  ${ZYPP_LIBRARY}
)
add_test(NAME CosuService COMMAND testCosuService)
ecm_mark_as_test(testCosuService)

set(testCosuZypp_SRCS
  ../src/cosu_debug_zypp.cpp
  ../src/CosuZypp.cpp
  ../src/CosuResolveAbstract.cpp
  testCosuZypp.cpp
)
add_executable(testCosuZypp ${testCosuZypp_SRCS})
target_link_libraries(testCosuZypp
  Qt5::Test
  ${ZYPP_LIBRARY}
)
add_test(NAME CosuZypp COMMAND testCosuZypp)
ecm_mark_as_test(testCosuZypp)
