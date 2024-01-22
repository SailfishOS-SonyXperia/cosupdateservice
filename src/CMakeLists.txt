
if(packagekitqt5_FOUND)
  list(APPEND resolver_SRCS CosuPackageKit.cpp)
endif()

if(ZYPP_FOUND)
  list(APPEND resolver_SRCS CosuZypp.cpp)
  # libzypp requires exceptions
  kde_source_files_enable_exceptions(CosuZypp.cpp)
endif()

set(cosupdateservice_SRCS
  main.cpp
  CosuSettings.cpp
  CosuVariables.cpp
  CosuServiceRuntime.cpp
  CosuService.cpp
  CosuZypp.cpp
  CosuResolveAbstract.cpp
  # Need to include CosuService.h manually for CMake to pick it up
  # when calling moc
  ${CMAKE_CURRENT_BINARY_DIR}/../config.h
  ${resolver_SRCS}
)

qt5_generate_dbus_interface(
  CosuService.h
  ${CMAKE_CURRENT_BINARY_DIR}/${COSU_DBUS_INTERFACE}.xml
  OPTIONS -a
)

qt5_add_dbus_adaptor(cosupdateservice_SRCS
  ${CMAKE_CURRENT_BINARY_DIR}/${COSU_DBUS_INTERFACE}.xml
  CosuService.h
  CosuService
  ) #FIXME: register QVersionNumber data type to dbus
ecm_qt_declare_logging_category(cosupdateservice_SRCS
  HEADER cosu_debug.h
  IDENTIFIER COSU
  CATEGORY_NAME org.sailfishos.cosupdateservice
  DESCRIPTION "Community OS update Service"
  EXPORT COSU
)

ecm_qt_declare_logging_category(cosupdateservice_SRCS
  HEADER cosu_debug_packagekit.h
  IDENTIFIER COSU_PACKAGEKIT
  CATEGORY_NAME org.sailfishos.cosupdateservice.packagekit
  DESCRIPTION "Community OS update Service PackageKit"
  EXPORT COSU
)


ecm_qt_declare_logging_category(cosupdateservice_SRCS
  HEADER cosu_debug_zypp.h
  IDENTIFIER COSU_ZYPP
  CATEGORY_NAME org.sailfishos.cosupdateservice.zypp
  DESCRIPTION "Community OS update Service Libzypp"
  EXPORT COSU
)

ecm_qt_declare_logging_category(cosupdateservice_SRCS
  HEADER cosu_debug_service.h
  IDENTIFIER COSU_SERVICE
  CATEGORY_NAME org.sailfishos.cosupdateservice.service
  DESCRIPTION "Community OS update Service Runtime"
  EXPORT COSU
)
add_executable(cosupdateservice_bin ${cosupdateservice_SRCS})
set_target_properties(cosupdateservice_bin PROPERTIES OUTPUT_NAME
  ${COSU_SERVICE_EXECUTABLE_NAME})
set_property(TARGET cosupdateservice_bin PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_link_libraries(cosupdateservice_bin
  Qt5::Core
  Qt5::DBus
  ${PackageKitQt5_LIBRARIES}
  ${ZYPP_LIBRARY}
  ${SSU_LIBRARIES}
)

set(cosupdateservice_proxy_SRCS
  CosuOsUpdateServiceProxyService.cpp
  CosuOsUpdateServiceProxy.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/../config.h
)
ecm_qt_declare_logging_category(cosupdateservice_proxy_SRCS
  HEADER cosu_debug_proxy.h
  IDENTIFIER COSU_PROXY
  CATEGORY_NAME org.sailfishos.cosupdateservice.proxy
  DESCRIPTION "Community OS update Service Runtime"
  EXPORT COSU
)
add_executable(cosupdateservice_proxy_bin ${cosupdateservice_proxy_SRCS})
set_target_properties(cosupdateservice_proxy_bin PROPERTIES OUTPUT_NAME
  ${COSU_SERVICE_PROXY_EXECUTABLE_NAME})
set_property(TARGET cosupdateservice_proxy_bin PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_link_libraries(cosupdateservice_proxy_bin
  Qt5::Core
  Qt5::DBus
)

set(cosupdateservice_trigger_SRCS
  CosuTrigger.cpp
  CosuOsUpdateServiceProxy.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/cosu_debug_proxy.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/../config.h
)
add_executable(cosupdateservice_trigger_bin ${cosupdateservice_trigger_SRCS})
set_target_properties(cosupdateservice_trigger_bin PROPERTIES OUTPUT_NAME
  ${COSU_SERVICE_TRIGGER_EXECUTABLE_NAME})
set_property(TARGET cosupdateservice_trigger_bin PROPERTY POSITION_INDEPENDENT_CODE TRUE)
target_link_libraries(cosupdateservice_trigger_bin
  Qt5::Core
  Qt5::DBus
)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${COSU_DBUS_INTERFACE}.xml DESTINATION ${DBUS_INTERFACES_INSTALL_DIR})
install(TARGETS cosupdateservice_bin DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
install(TARGETS cosupdateservice_trigger_bin DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
install(TARGETS cosupdateservice_proxy_bin DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})
ecm_qt_install_logging_categories(EXPORT COSU
  FILE cosu.categories
  DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})
