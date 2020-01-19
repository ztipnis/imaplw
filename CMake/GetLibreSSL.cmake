include(ExternalProject)
ExternalProject_Add(Project_LibreSSL
    URL https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.0.2.tar.gz 
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libressl-3.0.2
)
ExternalProject_Get_Property(Project_LibreSSL INSTALL_DIR)
set(LIBRESSL_INCLUDE_DIR ${INSTALL_DIR}/include)
set(LIBRESSL_CRYPTO_LIBRARY ${INSTALL_DIR}/lib/libcrypto.a CACHE INTERNAL "LibreSSL CRYTO Library Path")
set(LIBRESSL_SSL_LIBRARY ${INSTALL_DIR}/lib/libssl.a CACHE INTERNAL "LibreSSL SSL Library Path")
set(LIBRESSL_TLS_LIBRARY ${INSTALL_DIR}/lib/libtls.a CACHE INTERNAL "LibreSSL TLS Library Path")
# Set LibreSSL::Crypto
if(NOT TARGET LibreSSL::Crypto AND EXISTS "${LIBRESSL_CRYPTO_LIBRARY}")
    # Add Library
    add_library(LibreSSL::Crypto UNKNOWN IMPORTED)
    # Set Properties
    set_target_properties(
        LibreSSL::Crypto
        PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBRESSL_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${LIBRESSL_CRYPTO_LIBRARY}"
    )
    add_dependencies(LibreSSL::Crypto Project_LibreSSL)
endif() # LibreSSL::Crypto

# Set LibreSSL::SSL
if(NOT TARGET LibreSSL::SSL AND EXISTS "${LIBRESSL_SSL_LIBRARY}")
    # Add Library
    add_library(LibreSSL::SSL UNKNOWN IMPORTED)
    # Set Properties
    set_target_properties(
        LibreSSL::SSL
        PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBRESSL_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${LIBRESSL_SSL_LIBRARY}"
            INTERFACE_LINK_LIBRARIES LibreSSL::Crypto
    )
    add_dependencies(LibreSSL::SSL Project_LibreSSL)
endif() # LibreSSL::SSL
# Set LibreSSL::TLS
if(NOT TARGET LibreSSL::TLS AND EXISTS "${LIBRESSL_TLS_LIBRARY}")
    add_library(LibreSSL::TLS UNKNOWN IMPORTED)
    set_target_properties(
        LibreSSL::TLS
        PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${LIBRESSL_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${LIBRESSL_TLS_LIBRARY}"
            INTERFACE_LINK_LIBRARIES LibreSSL::SSL
    )
    add_dependencies(LibreSSL::TLS Project_LibreSSL)
endif() # LibreSSL::TLS
set(LIBRESSL_LIBRARIES LibreSSL::Crypto LibreSSL::SSL LibreSSL::TLS)
mark_as_advanced(LIBRESSL_INCLUDE_DIR LIBRESSL_LIBRARIES LIBRESSL_CRYPTO_LIBRARY LIBRESSL_SSL_LIBRARY LIBRESSL_TLS_LIBRARY)