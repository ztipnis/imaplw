add_executable(IMAPlw IMAP.cpp)
find_package(LibreSSL REQUIRED)
target_link_libraries(IMAPlw LibreSSL::TLS)