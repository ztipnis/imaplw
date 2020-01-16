/*
 * Copyright [2020] <Zachary Tipnis> – All Rights Reserved
 *
 * The use (including but not limited to modification and
 * distribution) of this source file and its contents shall
 * be governed by the terms of the MIT License.
 *
 * You should have received a copy of the MIT License with
 * this file. If not, please write to "zatipnis@icloud.com"
 * or visit: https://zacharytipnis.com
 *
 */
#include <SocketPool.hpp>
#include <iostream>
#include <boost/locale.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include "IMAP/DataModel.hpp"
#include "IMAP/IMAPProvider.hpp"
#include "cfgParser/Config.hpp"
#include "IMAP/DefaultProviders.hpp"


namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void init_logging(std::string filename, std::string format, std::string severity)
{
    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
    if(format == ""){
      format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%";
    }
    if(severity == ""){
      #ifdef CMAKE_BUILD_TYPE
        #if CMAKE_BUILD_TYPE == Release
          severity = "warning";
        #else
          severity = "trace";
        #endif
      #else
          severity = "debug";
      #endif
    }
    if(filename != "" && filename != "stdout" && filename != "cerr"){
      logging::add_file_log
      (
          keywords::file_name = filename,
          keywords::auto_flush = true,
          keywords::format = format
      );
    }else{
      logging::add_console_log
      (
          (filename == "cerr" ? std::cerr : std::cout),
          keywords::auto_flush = true,
          keywords::format = format
      );
    }
    if(severity != ""){
      boost::log::trivial::severity_level logSeverity;
      std::istringstream{severity} >> logSeverity;
      logging::core::get()->set_filter
      (
          logging::trivial::severity >= logSeverity
      );

    }

    logging::add_common_attributes();
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    BOOST_LOG_TRIVIAL(fatal) << "USAGE: " << argv[0] << " config path" << std::endl;
    exit(1);
  }
  try{
    Config cfg(argv[1]);
    std::string logging_out = cfg.get<String>("Logging", "Output");
    init_logging(logging_out, cfg.get<String>("Logging", "Format"), cfg.get<String>("Logging", "Level"));
    boost::locale::generator gen;
    std::locale loc;
    std::string locale = cfg.get<String>("General", "Time");
    if(locale != ""){
      loc = gen(locale);
    } else{
      loc = gen("en_US.UTF-8");
    }
    std::locale::global(loc); 
    unsigned int port = cfg.get<Numeric>("General", "Port");
    const char* address = cfg.get<String>("General", "Address").c_str();
    IMAPProvider::ConfigModel c(cfg.get<Boolean>("TLS", "Secure"),
                                cfg.get<Boolean>("TLS", "StartTLS"),
                                cfg.get<String>("TLS", "Versions").c_str(),
                                cfg.get<String>("TLS", "Ciphers").c_str(),
                                cfg.get<String>("TLS", "KeyPath").c_str(),
                                cfg.get<String>("TLS", "CertPath").c_str());
    IMAPProvider::IMAPProvider<GAuthP, DAuthP> p(c);
    SocketPool sp(port, address,
                  cfg.get<Numeric>("General", "Threads"),
                  cfg.get<Numeric>("General", "Clients"),
                  p, std::chrono::minutes(5));
    BOOST_LOG_TRIVIAL(trace) << "CONFIG SETTINGS:" << std::endl << cfg.debug();
    BOOST_LOG_TRIVIAL(info) << "IMAPlw Listening on " << cfg.get<String>("General", "Address") << ":" << cfg.get<Numeric>("General", "Port");
    while (1) {
      sp.listen();
    }
  } catch (const std::runtime_error &e){
    BOOST_LOG_TRIVIAL(fatal) << e.what() << std::endl;
    exit(1);
  }
}