#import "IMAP/DataProvider.hpp"
#import <SocketPool.hpp>
#import "IMAP/Providers.hpp"
#import <iostream>

class GAuthP : public AuthenticationProvider{
public:
	bool lookup(std::string username){ return true; }
	bool authenticate(std::string username, std::string password){ return true; }
	GAuthP() : AuthenticationProvider("AUTH=PLAIN"){}
};

int main(int argc, char* argv[]){
	if(argc < 3){
		std::cerr << "USAGE: " << argv[0] << " #clients #threads" << std::endl;
		exit(1);
	}
	std::cout << "Note: if # clients is not evently divisible by # threads, # clients will be truncated to be distibuted evenly amongst threads." << std::endl;
	unsigned int port = 8080;
	const char* address = "0.0.0.0";
	IMAPProvider::Config c(true, false, "secure", "secure", "server.key", "server.crt");
	IMAPProvider::IMAPProvider<GAuthP, DataProvider> p(c);
	SocketPool sp(port, address, atoi(argv[1]), atoi(argv[2]), p, std::chrono::minutes(5));
	while(1){
		sp.listen();
	}
}