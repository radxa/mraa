add_subdirectory (c)
add_subdirectory (platform)

if ( (DEFINED ENV{JAVA_HOME_NATIVE}) OR (DEFINED ENV{JAVA_HOME}) )
  #add_subdirectory (java)
else ()
  #message ( WARNING "You haven't configured the Java environment variables yet!")
endif ()

if (NOT ANDROID_TOOLCHAIN)
  add_subdirectory (c++)
endif ()

install (DIRECTORY ${PROJECT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_DATADIR}/mraa/examples)
