project(dUQ)


set(DESCRIPTION "dUQ")
set(AUTHOR "Tristan Youngs")
set(VERSION_MAJOR "0")
set(VERSION_MINOR "32")
set(VERSION_PATCH "1")

set(CMAKE_BUILD_TYPE "Release")

if(APPLE)
  cmake_minimum_required(VERSION 2.6.0)
  set(CMAKE_OSX_ARCHITECTURES "i386")
  ADD_DEFINITIONS(-D_MAC)
else(APPLE)
  cmake_minimum_required(VERSION 2.4.8)
endif(APPLE)

if(COMMAND cmake_policy)
  cmake_policy(VERSION 2.6)
endif(COMMAND cmake_policy)

set (target_name NotAten)

#set(Bison_DIR "${CMAKE_SOURCE_DIR}/data")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
find_package(READLINE REQUIRED)
IF(WIN32 OR APPLE)
 find_package(OPENGL REQUIRED)
endif(WIN32 OR APPLE)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(SRCS src/)
#ADD_DEFINITIONS(-DQT_SHARED)

# Add global NOMINMAX define for Windows systems
if(WIN32)
  ADD_DEFINITIONS("-DNOMINMAX")
endif(WIN32)

# tell cmake to process CMakeLists.txt in that subdirectory
add_subdirectory(${SRCS})

If(APPLE)
  set(MACOSX_BUNDLE_ICON_FILE  "duq.icns")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER  "dUQ")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_BUNDLE_NAME  "dUQ" )
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION  "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
  set(MACOSX_BUNDLE_COPYRIGHT "${AUTHOR}")
endif(APPLE)

include_directories(
${CMAKE_SOURCE_DIR} 
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
${SRCS}/templates/
${SRCS}/
${QT_INCLUDE_DIR}
${READLINE_INCLUDE_DIR}
)

add_executable(${target_name}  MACOSX_BUNDLE
${SRCS}/main.cpp 
)

link_directories (${LIBRARY_OUTPUT_PATH})
IF(WIN32 OR APPLE)
target_link_libraries(${target_name} 
base classes gui main math templates
${QT_QTOPENGL_LIBRARY} ${OPENGL_LIBRARIES} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
${READLINE_LIBRARY}
)
ELSE(WIN32 OR APPLE)
target_link_libraries(${target_name} 
base classes gui main math templates
${QT_QTOPENGL_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
${READLINE_LIBRARY}
)
ENDIF(WIN32 OR APPLE)
####################
####install target
####################

if(NOT ${PREFIX})
  MESSAGE("Install folder ${PREFIX}")
  set(CMAKE_INSTALL_PREFIX "${PREFIX}")
endif(NOT ${PREFIX})

if(UNIX AND NOT APPLE)
  configure_file(${CMAKE_SOURCE_DIR}/desktop.cmake NotAten.desktop)
  install(FILES ${CMAKE_BINARY_DIR}/NotAten.desktop 
       DESTINATION "/usr/share/applications/")
  install(FILES ${CMAKE_SOURCE_DIR}/src/gui/icons/duq.png
        DESTINATION share/duq)
endif(UNIX AND NOT APPLE)

if (APPLE)
  install(TARGETS ${target_name}
  BUNDLE DESTINATION .
  )
else(APPLE)
  install(TARGETS ${target_name}
      RUNTIME DESTINATION bin
   )
endif(APPLE)

if (NOT APPLE)
   install(DIRECTORY  "${CMAKE_HOME_DIRECTORY}/data/" 
     DESTINATION share/duq
     REGEX ".svn" EXCLUDE
   )
endif(NOT APPLE)
if(WIN32)
  FIND_FILE(Rline readline5.dll ENV PATH)
  FIND_FILE(HRline history5.dll ENV PATH)
  Find_File(qtgui QtGui4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtgl QtOpenGL4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtcore QtCore4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtsvg QtSvg4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtxml QtXml4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qsvg qsvg4.dll PATHS ${QT_PLUGINS_DIR}/iconengines NO_DEFAULT_PATH)
  if(NOT qsvg) 
    Find_File(qsvg qsvgicon4.dll PATHS ${QT_PLUGINS_DIR}/iconengines NO_DEFAULT_PATH)
  endif(NOT qsvg)
  Find_File(qsvgie qsvg4.dll PATHS ${QT_PLUGINS_DIR}/imageformats NO_DEFAULT_PATH)
  install(FILES ${qtgui} ${qtgl} ${qtcore} ${Rline} ${HRline} ${qtsvg} ${qtxml} ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/icons/duq.ico
   DESTINATION bin
  )
  install(FILES ${qsvg} 
    DESTINATION bin/iconengines
  )
  install(FILES ${qsvgie} 
    DESTINATION bin/imageformats
  )
ENDIF(WIN32)


if(APPLE)
## install the icon for the bundle
  install(FILES "${CMAKE_SOURCE_DIR}/extra/duq.icns" 
        DESTINATION "${target_name}.app/Contents/Resources/")
  install(DIRECTORY  "${CMAKE_HOME_DIRECTORY}/data/"
     DESTINATION ${target_name}.app/Contents/SharedSupport/
     REGEX ".svn" EXCLUDE)
  install(FILES "${CMAKE_SOURCE_DIR}/extra/duq.icns"
        DESTINATION . RENAME ".VolumeIcon.icns")
  install(FILES "${CMAKE_SOURCE_DIR}/extra/background.png"
        DESTINATION .background/) 
  install(FILES "${CMAKE_SOURCE_DIR}/COPYING" 
         DESTINATION . RENAME .COPYING)
  install(FILES "${CMAKE_SOURCE_DIR}/extra/NotAten.DS_Store"
         DESTINATION . RENAME .DS_Store)
#  INSTALL(CODE "set(input_file
#        \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${target_name}.app/Contents/MacOS/${target_name}\")")
#  INSTALL(SCRIPT "${CMAKE_SOURCE_DIR}/CMakeIngestOSXBundleLibraries.cmake")
endif(APPLE)









#######################
## uninstall target
#######################

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
###########################

INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
SET(CPACK_PACKAGE_VENDOR ${AUTHOR})
SET(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})

IF (APPLE)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_BINARY_DIR}/README.txt COPYONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING.txt COPYONLY) 
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_BINARY_DIR}/README.txt")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/COPYING.txt")
ELSE(APPLE)
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING") 
ENDIF(APPLE)

SET(CPACK_PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "NotAten${VERSION_MAJOR}.${VERSION_MINOR}")
IF(WIN32)
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "NotAten-${VERSION_MAJOR}.${VERSION_MINOR}")
  SET(CPACK_GENERATOR "NSIS;ZIP")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/extra/icons\\\\duq.ico")
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/extra/icons\\\\uninstall.ico")
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/extra/icons\\\\duq.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\duq.ico")  
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} NotAten")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.duq.net")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.duq.net")
  SET(CPACK_NSIS_CONTACT "tris@duq.net")
  SET(CPACK_NSIS_MODIFY_PATH ON)  
  SET(CPACK_PACKAGE_EXECUTABLES "duq" "duq")
  set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\duq.lnk\\\" \\\"$INSTDIR\\\\bin\\\\duq.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\duq.ico\\\"
      StrCmp $INSTALL_DESKTOP \\\"1\\\" 0 +2
      CreateShortCut \\\"$DESKTOP\\\\duq.lnk\\\" \\\"$INSTDIR\\\\bin\\\\duq.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\duq.ico\\\"
"
  )
set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    Push \\\"DUQDATA\\\"
    Push \\\"$INSTDIR\\\\share\\\\duq\\\"
    Call WriteEnvStr      
"
  )
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "    
    Push \\\"DUQ`DATA\\\"
    Call un.DeleteEnvStr 
    StrCmp $INSTALL_DESKTOP \\\"1\\\" 0 +2
    Delete \\\"$DESKTOP\\\\duq.lnk\\\"    
"
)
Endif(WIN32)

IF(UNIX)
  SET(CPACK_GENERATOR "TGZ;TBZ2;RPM;DEB")
Endif(UNIX)

IF(APPLE)
  SET(CPACK_GENERATOR "TGZ")
Endif(APPLE)

SET(CPACK_CMAKE_GENERATOR ${CMAKE_GENERATOR})

INCLUDE(CPack)


