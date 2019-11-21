#include <iostream>
#include "poll.hpp"
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include "ThreadPool/ThreadPool.h"


class PrintHandler : Pollster::Handler {
	protected:
		void operator()(int fd){
			std::string data(8193, 0);
			int rcvd = read(fd, &data[0], 8192);
			if( rcvd == -1){
				throw std::runtime_error("Unable to read from socket");
			}else{
				std::cout << data << std::endl;
			}
		}
		void disconnect(int fd, std::string reason){
			std::cout << reason << std::endl;
			close(fd);
		}
};
class SocketPool {
public:
	SocketPool(unsigned int port, const char* addr, int max_Clients, int max_Threads, Pollster::Handler& T):sock(socket(PF_INET, SOCK_STREAM, 0)), handler(T), cliPerPollster(max_Clients/max_Threads), pollsters(max_Threads){
		sockaddr_in sockopt;
		if(sock < 0){
			throw std::runtime_error("Unable to create socket");
		}
		sockopt.sin_family = AF_INET;
		sockopt.sin_port = port;
		if(inet_aton(addr, &(sockopt.sin_addr)) < 0){
			throw std::runtime_error("Listen Address Invalid");
		}
		if(bind(sock, reinterpret_cast<sockaddr*>(&sockopt), sizeof(sockopt)) < 0){
			throw std::runtime_error("Unable to bind to port");
		}
		int opt = 1;
			setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt));

	}
	void listen(std::chrono::seconds gcInterval){
		if(::listen(sock, 5) < 0){
			throw std::runtime_error("Unable to listen on socket");
		}
		if(gcInterval.count() > 0){
			timeval tv;
			fd_set rfds;
			tv.tv_sec = gcInterval.count();
			tv.tv_usec = 0;
			FD_ZERO(&rfds);
           	FD_SET(sock, &rfds);
           	int retval = select(1, &rfds, NULL, NULL, &tv);
           	if(retval < 0){
           		throw std::runtime_error("Polling Listening Socket Failed");
           	}else if(retval > 0){
           		this->accept();
           	}
		}else{
			this->accept();
		}
	}
private:
	void accept(){
			sockaddr_in address;
       		socklen_t adrlen = static_cast<socklen_t>(sizeof(address));
       		int cli_fd = ::accept(sock, reinterpret_cast<sockaddr*>(&address), &adrlen);
       		if(cli_fd > 0){
       			bool assigned = false;
       			for(int i = 0; i < p.size(); i++){
       				if(p[i].canAddClient()){
       					if(p[i].addClient(cli_fd)){
       						assigned = false;
       						break;
       					}
       				}
       			}
       			if(!assigned){
       				if(p.size() < pollsters){
       					Pollster::Pollster p_ = Pollster::Pollster(cliPerPollster,handler);
	       				if(!p_.addClient(cli_fd)){
	       					throw std::runtime_error("Unable to add client to new Pollster");
	       				}
	       				p.push_back(p_);
	       				//TODO: Thread the loop
       				}else{
       					handler.disconnect(cli_fd, "Too many simultaneos connections...");
       				}
       			}
       		}
	}
	int sock;
	Pollster::Handler& handler;
	std::vector<Pollster::Pollster> p;
	int cliPerPollster;
	int pollsters;
	static ThreadPool pool;
};