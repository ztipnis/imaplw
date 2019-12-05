#import "SocketPool/SocketPool.hpp"
#import "IMAP/Providers.hpp"
#import <iostream>

int main(int argc, char* argv[]){
	if(argc < 3){
		std::cerr << "USAGE: " << argv[0] << " #clients #threads" << std::endl;
		exit(1);
	}
	std::cout << "Note: if # clients is not evently divisible by # threads, # clients will be truncated to be distibuted evenly amongst threads." << std::endl;
	unsigned int port = 8080;
	const char* address = "0.0.0.0";
	IMAPProvider::Config c(true, false, "secure", "secure", "server.key", "server.crt");
	IMAPProvider::IMAPProvider p = IMAPProvider::IMAPProvider(c);
	SocketPool sp(port, address, atoi(argv[1]), atoi(argv[2]), p);
	while(1){
		sp.listen(std::chrono::minutes(0));
	}
}