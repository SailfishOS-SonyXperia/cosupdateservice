set(usr_confdir ${CMAKE_INSTALL_PREFIX}/lib/cosupdateservice/conf.d)
set(preconfigure_settings
  conf.d/05-mandatoryVariables.ini
  conf.d/10-defaults.ini
)

install(FILES ${preconfigure_settings}
  DESTINATION ${usr_confdir})

set(maplauncher_privileges_dir ${CMAKE_INSTALL_DATADIR}/mapplauncherd/privileges.d)
set(maplauncher_privileges
  cosupdateservice.privileges
)
set(SYSTEMDUSERPRESETDIR ${CMAKE_INSTALL_PREFIX}/lib/systemd/user-preset)

install(FILES ${maplauncher_privileges} DESTINATION ${maplauncher_privileges_dir})


ecm_generate_dbus_service_file(
  NAME ${COSU_DBUS_SERVICENAME}
  EXECUTABLE ${KDE_INSTALL_FULL_BINDIR}/${COSU_SERVICE_EXECUTABLE_NAME}
  DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR}
  SYSTEMD_SERVICE ${COSU_SERVICE_EXECUTABLE_NAME}.service
  RENAME ${COSU_DBUS_SERVICENAME}.service
)


configure_file(${COSU_SERVICE_EXECUTABLE_NAME}.service.in
  ${CMAKE_CURRENT_BINARY_DIR}/${COSU_SERVICE_EXECUTABLE_NAME}.service @ONLY)
configure_file(${COSU_SERVICE_TRIGGER_EXECUTABLE_NAME}.service.in
  ${CMAKE_CURRENT_BINARY_DIR}/${COSU_SERVICE_TRIGGER_EXECUTABLE_NAME}.service @ONLY)
configure_file(${COSU_SERVICE_TRIGGER_EXECUTABLE_NAME}.timer.in
  ${CMAKE_CURRENT_BINARY_DIR}/${COSU_SERVICE_TRIGGER_EXECUTABLE_NAME}.timer @ONLY)
configure_file(${COSU_SERVICE_PROXY_EXECUTABLE_NAME}.service.in
  ${CMAKE_CURRENT_BINARY_DIR}/${COSU_SERVICE_PROXY_EXECUTABLE_NAME}.service @ONLY)

install(FILES cosupdateservice.preset DESTINATION ${SYSTEMDUSERPRESETDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${COSU_SERVICE_EXECUTABLE_NAME}.service
  DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${COSU_SERVICE_TRIGGER_EXECUTABLE_NAME}.service
  DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${COSU_SERVICE_TRIGGER_EXECUTABLE_NAME}.timer
  DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${COSU_SERVICE_PROXY_EXECUTABLE_NAME}.service
  DESTINATION ${KDE_INSTALL_SYSTEMDUSERUNITDIR})
