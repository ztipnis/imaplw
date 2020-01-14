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
#include "IMAP/DataModel.hpp"
#include "IMAP/IMAPProvider.hpp"
#include "cfgParser/Config.hpp"

class GAuthP : public IMAPProvider::AuthenticationModel {
 public:
  bool lookup(std::string username) { return true; }
  bool authenticate(std::string username, std::string password) { return true; }
  std::string SASL(struct tls* fd, std::string mechanism) { return ""; }
  GAuthP() : AuthenticationModel("AUTH=PLAIN") {}
};
class DAuthP : public IMAPProvider::DataModel {
 public:
  DAuthP() : DataModel() {}
  selectResp select(const std::string& user, const std::string& mailbox) {
    selectResp r;
    return r;
  };
  virtual int messages(const std::string& user, const std::string& mailbox) {
    return 0;
  };
  virtual int recent(const std::string& user, const std::string& mailbox) {
    return 0;
  };
  virtual unsigned long uidnext(const std::string& user,
                                const std::string& mailbox) {
    return 0;
  };
  virtual unsigned long uidvalid(const std::string& user,
                                 const std::string& mailbox) {
    return 0;
  };
  virtual int unseen(const std::string& user, const std::string& mailbox) {
    return 0;
  };
  bool createMbox(const std::string& user, const std::string& mailbox) {
    return true;
  };
  bool hasSubFolders(const std::string& user, const std::string& mailbox) {
    return true;
  };
  bool hasAttrib(const std::string& user, const std::string& mailbox,
                 const std::string& attrib) {
    return true;
  };
  bool addAttrib(const std::string& user, const std::string& mailbox,
                 const std::string& attrib) {
    return true;
  };
  bool rmFolder(const std::string& user, const std::string& mailbox) {
    return true;
  };
  bool clear(const std::string& user, const std::string& mailbox) {
    return true;
  };
  bool rename(const std::string& user, const std::string& mailbox,
              const std::string& name) {
    return true;
  };
  bool addSub(const std::string& user, const std::string& mailbox) {
    return true;
  };
  bool rmSub(const std::string& user, const std::string& mailbox) {
    return true;
  };
  bool list(const std::string& user, const std::string& mailbox,
            std::vector<struct mailbox>& lres) {
    return true;
  };
  bool lsub(const std::string& user, const std::string& mailbox,
            std::vector<struct mailbox>& lres) {
    return true;
  };
  bool mailboxExists(const std::string& user, const std::string& mailbox) {
    return true;
  };
  bool append(const std::string& user, const std::string& mailbox,
              const std::string& messageData) {
    std::cout << "APPEND:" << std::endl << messageData << std::endl;
    return true;
  };
  bool expunge(const std::string& user, const std::string& mailbox, std::vector<std::string>& expunged) {
    return true;
  }
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "USAGE: " << argv[0] << " #clients #threads" << std::endl;
    exit(1);
  }

  Config cfg(argv[1]);

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
  while (1) {
    sp.listen();
  }
}