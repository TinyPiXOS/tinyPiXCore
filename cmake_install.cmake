# Install script for directory: /home/pix/pix_formal/tinyPiXCore

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND /usr/bin/cmake --build . --target clean_install_directory)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/include/tinyPiX/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/include/tinyPiX" TYPE DIRECTORY FILES "/home/pix/pix_formal/tinyPiXCore/src/include/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/include")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64" TYPE DIRECTORY FILES "/home/pix/pix_formal/tinyPiXCore/src/include")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/res/tinyPiX/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/res/tinyPiX" TYPE DIRECTORY FILES "/home/pix/pix_formal/tinyPiXCore/src/res/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/res")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64" TYPE DIRECTORY FILES "/home/pix/pix_formal/tinyPiXCore/src/res")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/data/tinyPiX/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/data/tinyPiX" TYPE DIRECTORY FILES "/home/pix/pix_formal/tinyPiXCore/src/data/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/data")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64" TYPE DIRECTORY FILES "/home/pix/pix_formal/tinyPiXCore/src/data")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/bin/tinyPiX/")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/bin/tinyPiX" TYPE DIRECTORY FILES "/home/pix/pix_formal/tinyPiXCore/src/bin/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64" TYPE DIRECTORY FILES "/home/pix/pix_formal/tinyPiXCore/src/bin")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  
    # 权限修复（可选）
    execute_process(COMMAND chmod -R 0777 /home/pix/pix_formal/tinyPiXCore/install/x86_64/bin)
    execute_process(COMMAND chmod -R 0777 /usr/bin/tinyPiX)

endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin/PiXService" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin/PiXService")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin/PiXService"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin/PiXService")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin" TYPE EXECUTABLE FILES "/home/pix/pix_formal/tinyPiXCore/build/bin/PiXService")
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin/PiXService" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin/PiXService")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin/PiXService"
         OLD_RPATH "/home/pix/pix_formal/tinyPiXCore/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/bin/PiXService")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXSingleGUI.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXSingleGUI.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXSingleGUI.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXSingleGUI.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib" TYPE SHARED_LIBRARY FILES "/home/pix/pix_formal/tinyPiXCore/build/lib/libPiXSingleGUI.so")
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXSingleGUI.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXSingleGUI.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXSingleGUI.so"
         OLD_RPATH "/home/pix/pix_formal/tinyPiXCore/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXSingleGUI.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXExternUtils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXExternUtils.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXExternUtils.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXExternUtils.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib" TYPE SHARED_LIBRARY FILES "/home/pix/pix_formal/tinyPiXCore/build/lib/libPiXExternUtils.so")
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXExternUtils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXExternUtils.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXExternUtils.so"
         OLD_RPATH "/home/pix/pix_formal/tinyPiXCore/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXExternUtils.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXUtils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXUtils.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXUtils.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXUtils.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib" TYPE SHARED_LIBRARY FILES "/home/pix/pix_formal/tinyPiXCore/build/lib/libPiXUtils.so")
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXUtils.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXUtils.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXUtils.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXDesktopGUI.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXDesktopGUI.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXDesktopGUI.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXDesktopGUI.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib" TYPE SHARED_LIBRARY FILES "/home/pix/pix_formal/tinyPiXCore/build/lib/libPiXDesktopGUI.so")
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXDesktopGUI.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXDesktopGUI.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXDesktopGUI.so"
         OLD_RPATH "/home/pix/pix_formal/tinyPiXCore/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libPiXDesktopGUI.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libexampleGUI.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libexampleGUI.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libexampleGUI.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libexampleGUI.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib" TYPE SHARED_LIBRARY FILES "/home/pix/pix_formal/tinyPiXCore/build/lib/libexampleGUI.so")
  if(EXISTS "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libexampleGUI.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libexampleGUI.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libexampleGUI.so"
         OLD_RPATH "/home/pix/pix_formal/tinyPiXCore/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/pix/pix_formal/tinyPiXCore/install/x86_64/lib/libexampleGUI.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/pix/pix_formal/tinyPiXCore/PiXUtils/cmake_install.cmake")
  include("/home/pix/pix_formal/tinyPiXCore/PiXExternUtils/cmake_install.cmake")
  include("/home/pix/pix_formal/tinyPiXCore/PiXSingleGUI/cmake_install.cmake")
  include("/home/pix/pix_formal/tinyPiXCore/PiXGUIFramework/desktopGUI/cmake_install.cmake")
  include("/home/pix/pix_formal/tinyPiXCore/PiXGUIFramework/exampleGUI/cmake_install.cmake")
  include("/home/pix/pix_formal/tinyPiXCore/PiXService/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/pix/pix_formal/tinyPiXCore/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
