cmake_minimum_required (VERSION 3.5.0)
project (cosupdateservice)



option(WITH_PACKAGEKIT "Enable PackagekitQt backend" ON)
option(WITH_ZYPP "Enable Zypp backend" ON)

set(COSU_SERVICE_EXECUTABLE_NAME cosupdateservice)
set(COSU_SERVICE_TRIGGER_EXECUTABLE_NAME cosupdatesearch)
set(COSU_SERVICE_PROXY_EXECUTABLE_NAME cosupdateproxy)
set(COSU_DBUS_SERVICENAME org.sailfishos.CosUpdater) #fixme extract name from header
set(COSU_DBUS_PATH /org/sailfishos/CosUpdater)
set(COSU_DBUS_INTERFACE org.sailfishos.CosUpdater)

set(CONF_DIRECTORIES
  /usr/lib/cosupdateservice/conf.d
  /etc/cosupdateservice/conf.d
)

set(CMAKE_AUTOMOC TRUE)
set(QT_MIN_VERSION "5.6.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Convert lists to variable to be used during config.h
string(REGEX REPLACE "\(.*\);" "\"\\1\",\"" COSU_CONF_DIRECTORIES "${CONF_DIRECTORIES}")


include(FindPkgConfig)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH}  "${CMAKE_SOURCE_DIR}/cmake")

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMSetupVersion)
include(ECMMarkAsTest)
include(ECMGenerateHeaders)
include(ECMGenerateDBusServiceFile)
include(ECMQtDeclareLoggingCategory)
include(ECMAddQch)
include(ECMPoQmTools)
include(FeatureSummary)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(KDEClangFormat)
include(KDEGitCommitHooks)
#include(ECMDeprecationSettings)

# CONFIG FIXME
find_package (Qt5 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS
  Core
  DBus
)

# FIXME implement solely zypper and packageKit builds
if (WITH_PACKAGEKIT)
  find_package(packagekitqt5 REQUIRED COMPONENTS)
endif()

if (WITH_ZYPP)
  find_package(Zypp REQUIRED COMPONENTS)
  if( ZYPP_FOUND )
    include_directories(${ZYPP_INCLUDE_DIR})
    # libzypp uses not operator and so on
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -foperator-names")
  endif( ZYPP_FOUND )
endif()

if(NOT ZYPP_FOUND OR NOT packagekitqt5_FOUND)
  message(FATAL_ERROR "Either PackageKit or Zypp required at least")
endif()
if(NOT ZYPP_FOUND AND PACKAGEKIT_FOUND)
  WARNING("PackageKit doesn't support to search only target repository")
endif()

pkg_check_modules(SSU ssu REQUIRED)


configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
if (BUILD_TESTING)
  include(ECMAddTests)
  find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED Test)
  add_subdirectory(tests)
endif()
add_subdirectory(data)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)


file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
list(FILTER ALL_CLANG_FORMAT_SOURCE_FILES EXCLUDE REGEX ".*\.\(ccls-cache\|build\).*")
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
