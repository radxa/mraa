if (NOT DEFINED ENV{JAVA_HOME_NATIVE})
    if (NOT DEFINED ENV{JAVA_HOME})
        message (FATAL_ERROR "Neither JAVA_HOME nor JAVA_HOME_NATIVE are set")
    endif()
    set (JAVA_HOME_NATIVE $ENV{JAVA_HOME})
    set (JAVAC $ENV{JAVA_HOME}/bin/javac)
    set (JAR $ENV{JAVA_HOME}/bin/jar)
else ()
    set (JAVAC $ENV{JAVA_HOME_NATIVE}/bin/javac)
    set (JAR $ENV{JAVA_HOME_NATIVE}/bin/jar)
endif ()

set (Main_Class_Name_list
        AioA0 BlinkIO BlinkOnboard Bmp85
        CyclePwm3 Example FTDITest GpioMmapped
        GpioRead6 HelloEdison I2cCompass Isr
        SpiMAX7219 SpiMCP4261 UartExample)

foreach (Main_Class_Name ${Main_Class_Name_list})
    configure_file (
        ${CMAKE_CURRENT_SOURCE_DIR}/manifest.txt.in
        ${CMAKE_CURRENT_BINARY_DIR}/${Main_Class_Name}/manifest.txt
        @ONLY
    )

    if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../../src/java/mraa.jar")
        set (mraa_jar_path ${CMAKE_CURRENT_BINARY_DIR}/../../src/java/mraa.jar)
    elseif (EXISTS "/usr/local/lib/java/mraa.jar")
        set (mraa_jar_path /usr/local/lib/java/mraa.jar)
    endif()

    add_custom_target (${Main_Class_Name}_jar ALL
        COMMAND ${JAVAC} -cp ${mraa_jar_path} -d ${CMAKE_CURRENT_BINARY_DIR}/${Main_Class_Name} ${CMAKE_CURRENT_SOURCE_DIR}/${Main_Class_Name}.java
        COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/${Main_Class_Name}/ && ${JAR} -cmvf manifest.txt ${Main_Class_Name}.jar ${Main_Class_Name}.class
    )
endforeach ()