cmake_minimum_required(VERSION 3.15)

# Configurar el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate position-independent code (-fPIC on UNIX)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

project ("geoposition-system")

IF (WIN32)
    SET(SYSTEM_LIBS ws2_32 crypt32 RpcRT4)
    SET(ALEXT_MQTT_LIBS AlexTMqtt)
    SET(PAHO_MQTT_LIBS paho-mqttpp3-static paho-mqtt3as-static)
ELSEIF (UNIX)
    SET(ALEXT_MQTT_LIBS AlexTMqtt)
    SET(PAHO_MQTT_LIBS paho-mqttpp3 paho-mqtt3as)

    IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
        SET(SYSTEM_LIBS c dl pthread rt)
        # anl is only available with glibc so check if it is found before using
        # it or build will fail on uclibc or musl
        FIND_LIBRARY(LIB_ANL anl)
        IF(LIB_ANL)
            SET(SYSTEM_LIBS "${SYSTEM_LIBS}" anl)
        ENDIF()
        IF(PAHO_WITH_LIBUUID)
            SET(SYSTEM_LIBS "${SYSTEM_LIBS}" uuid)
        ENDIF()
        ADD_DEFINITIONS(-D_GNU_SOURCE -fvisibility=hidden)
    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Android")
        SET(SYSTEM_LIBS c dl)
    ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
        SET(SYSTEM_LIBS compat pthread)
    ELSE()
        SET(SYSTEM_LIBS c pthread)
    ENDIF()
ENDIF()


find_package(OpenSSL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/external/AlexTMqtt/include ${CMAKE_SOURCE_DIR}/external/PahoMqttCpp/include ${OPENSSL_INCLUDE_DIR})

link_directories(${CMAKE_SOURCE_DIR}/external/AlexTMqtt/lib ${CMAKE_SOURCE_DIR}/external/PahoMqttCpp/lib)

add_executable (sitep "src/sitep.cpp" "src/sitep.h")
add_executable (geonumerics "src/geonumerics.cpp" "src/geonumerics.h")

target_link_libraries(sitep ${ALEXT_MQTT_LIBS} ${PAHO_MQTT_LIBS} OpenSSL::SSL OpenSSL::Crypto ${SYSTEM_LIBS})
target_link_libraries(geonumerics ${ALEXT_MQTT_LIBS} ${PAHO_MQTT_LIBS} OpenSSL::SSL OpenSSL::Crypto ${SYSTEM_LIBS})
