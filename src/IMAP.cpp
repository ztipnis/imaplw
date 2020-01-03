#import "IMAP/DataModel.hpp"
#import <SocketPool.hpp>
#import "IMAP/IMAPProvider.hpp"
#import <iostream>

class GAuthP : public IMAPProvider::AuthenticationModel{
public:
	bool lookup(std::string username){ return true; }
	bool authenticate(std::string username, std::string password){ return true; }
	std::string SASL(struct tls* fd, std::string mechanism){ return ""; }
	GAuthP() : AuthenticationModel("AUTH=PLAIN"){}
};
class DAuthP : public IMAPProvider::DataModel{
public:
	DAuthP() : DataModel() {}
	selectResp select(const std::string& user, const std::string& mailbox){ selectResp r; return r;};
	bool createMbox(const std::string& user, const std::string& mailbox){ return true; };
	bool hasSubFolders(const std::string& user, const std::string& mailbox){return true;};
	bool hasAttrib(const std::string& user, const std::string& mailbox, const std::string& attrib){return true;};
	bool addAttrib(const std::string& user, const std::string& mailbox, const std::string& attrib){return true;};
	bool rmFolder(const std::string& user, const std::string& mailbox){return true;};
	bool clear(const std::string& user, const std::string& mailbox){return true;};
	bool rename(const std::string& user, const std::string& mailbox, const std::string& name){return true;};
	bool addSub(const std::string& user, const std::string& mailbox){return true;};
	bool rmSub(const std::string& user, const std::string& mailbox){return true;};
	bool list(const std::string& user, const std::string& mailbox, std::vector<struct mailbox>& lres){return true;};
	bool lsub(const std::string& user, const std::string& mailbox, std::vector<struct mailbox>& lres){return true;};
	bool mailboxExists(const std::string& user, const std::string& mailbox){return true;};
	bool append(const std::string& user, const std::string& mailbox, const std::string& messageData){return true;};
};

int main(int argc, char* argv[]){
	if(argc < 3){
		std::cerr << "USAGE: " << argv[0] << " #clients #threads" << std::endl;
		exit(1);
	}
	std::cout << "Note: if # clients is not evently divisible by # threads, # clients will be truncated to be distibuted evenly amongst threads." << std::endl;
	unsigned int port = 8080;
	const char* address = "0.0.0.0";
	IMAPProvider::ConfigModel c(true, false, "secure", "secure", "server.key", "server.crt");
	IMAPProvider::IMAPProvider<GAuthP, DAuthP> p(c);
	SocketPool sp(port, address, atoi(argv[1]), atoi(argv[2]), p, std::chrono::minutes(5));
	while(1){
		sp.listen();
	}
}