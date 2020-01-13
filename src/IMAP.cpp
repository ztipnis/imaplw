/*
 * Copyright [2020] <Zachary Tipnis> – All Rights Reserved
 *
 * The use (including but not limited to modification and
 * distribution) of this source file and its contents shall
 * be governed by the terms of the MIT License.
 *
 * You should have received a copy of the MIT License with
 * this file. If not, please write to "zatipnis@icloud.com"
 * or visit: https://zachrytipnis.com
 *
 */
#import <SocketPool.hpp>
#import <iostream>
#include <boost/locale.hpp>
#import "IMAP/DataModel.hpp"
#import "IMAP/IMAPProvider.hpp"

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
  if (argc < 3) {
    std::cerr << "USAGE: " << argv[0] << " #clients #threads" << std::endl;
    exit(1);
  }

  boost::locale::generator gen;
  std::locale loc = gen("en_US.UTF-8"); 
  std::locale::global(loc); 

  std::cout
      << "Note: if # clients is not evently divisible by # threads, # clients "
         "will be truncated to be distibuted evenly amongst threads."
      << std::endl;
  unsigned int port = 8080;
  const char* address = "0.0.0.0";
  IMAPProvider::ConfigModel c(true, false, "secure", "secure", "server.key",
                              "server.crt");
  IMAPProvider::IMAPProvider<GAuthP, DAuthP> p(c);
  SocketPool sp(port, address, atoi(argv[1]), atoi(argv[2]), p,
                std::chrono::minutes(5));
  while (1) {
    sp.listen();
  }
}