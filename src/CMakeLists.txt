
add_library(${OATPP_THIS_MODULE_NAME}
        oatpp-ssdp/SsdpMessage.hpp
        oatpp-ssdp/SsdpMessage.cpp
        oatpp-ssdp/UdpStreamProvider.cpp
        oatpp-ssdp/UdpStreamProvider.hpp
        oatpp-ssdp/SsdpStreamHandler.cpp
        oatpp-ssdp/SsdpStreamHandler.hpp
        oatpp-ssdp/UdpStream.cpp
        oatpp-ssdp/UdpStream.hpp
        oatpp-ssdp/SimpleSsdpUdpStreamProvider.cpp
        oatpp-ssdp/SimpleSsdpUdpStreamProvider.hpp
        oatpp-ssdp/SimpleUdpStreamProvider.cpp
        oatpp-ssdp/SimpleUdpStreamProvider.hpp)

set_target_properties(${OATPP_THIS_MODULE_NAME} PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

if(OATPP_MODULES_LOCATION STREQUAL OATPP_MODULES_LOCATION_EXTERNAL)
    add_dependencies(${OATPP_THIS_MODULE_NAME} ${LIB_OATPP_EXTERNAL})
endif()

target_link_oatpp(${OATPP_THIS_MODULE_NAME})

target_include_directories(${OATPP_THIS_MODULE_NAME}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

## TODO link dependencies here (if some)

#######################################################################################################
## install targets

if(OATPP_INSTALL)
    include("../cmake/module-install.cmake")
endif()
