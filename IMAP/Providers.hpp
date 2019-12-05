// #import <iostream>
#import <string>
#import <map>
#import <sstream>
#import <uuid/uuid.h>
#import <cstdlib>
#import <algorithm>
#import <tls.h>
#import "../SocketPool/SocketPool.hpp"
#import "config.hpp"
#import "Helpers.hpp"
#import "IMAPClientState.hpp"

namespace IMAPProvider{
	class IMAPProvider: public Pollster::Handler{
	private:
		const Config &config;
		static std::map<int, IMAPClientState> states;
		struct tls *tls = NULL;
		struct tls_config *t_conf = NULL;
		//ANY STATE
		void CAPABILITY(int rfd, std::string tag) const;
		void NOOP(int rfd, std::string tag) const{ OK(rfd, tag, "NOOP executed successfully"); }
		void LOGOUT(int rfd) const;
		//UNAUTHENTICATED
		void STARTTLS(int rfd, std::string tag) const;
		void AUTHENTICATE(int rfd) const;
		void LOGIN(int rfd) const;
		 //AUTENTICATED
		void SELECT(int rfd) const;
		void EXAMINE(int rfd) const;
		void CREATE(int rfd) const;
		void DELETE(int rfd) const;
		void RENAME(int rfd) const;
		void SUBSCRIBE(int rfd) const;
		void UNSUBSCRIBE(int rfd) const;
		void LIST(int rfd) const;
		void LSUB(int rfd) const;
		void STATUS(int rfd) const;
		void APPEND(int rfd) const;
		//SELECTED
		void CHECK(int rfd) const;
		void CLOSE(int rfd) const;
		void EXPUNGE(int rfd) const;
		void SEARCH(int rfd) const;
		void STORE(int rfd) const;
		void COPY(int rfd) const;
		void UID(int rfd) const;


		//RESPONSES
		inline void respond(int rfd, std::string tag, std::string code, std::string message) const{
			if(states[rfd].tls == NULL){
				sendMsg(rfd, tag + " "+ code +" " + message + "\n");
			}else{
				sendMsg(states[rfd].tls, tag + " "+ code +" " + message + "\n");
			}
		}

		void OK(int rfd, std::string tag, std::string message) const{ respond(rfd, tag, "OK", message + " " + states[rfd].get_uuid()); }
		void NO(int rfd, std::string tag, std::string message) const{ respond(rfd, tag, "NO", message); }
		void BAD(int rfd, std::string tag, std::string message) const{ respond(rfd, tag, "BAD", message); }
		void PREAUTH(int rfd, std::string tag, std::string message) const{ respond(rfd, tag, "PREAUTH", message); }
		void BYE(int rfd, std::string tag, std::string message) const{ respond(rfd, tag, "BYE", message); }
		void route(int fd, std::string tag, std::string command, std::string args) const;
		void parse(int fd, std::string message) const;

	public:
		IMAPProvider(Config &cfg) : config(cfg){
			if(cfg.secure || cfg.starttls){
				t_conf = tls_config_new();
				tls = tls_server();
				unsigned int protocols = 0;
				if(tls_config_parse_protocols(&protocols, cfg.versions) < 0){
					printf("tls_config_parse_protocols error\n");
				}
				tls_config_set_protocols(t_conf, protocols);
				if(tls_config_set_ciphers(t_conf, cfg.ciphers) < 0) {\
					printf("tls_config_set_ciphers error\n");
				}
				if(tls_config_set_key_file(t_conf, cfg.keypath) < 0) {
					printf("tls_config_set_key_file error\n");
				}
				if(tls_config_set_cert_file(t_conf, cfg.certpath) < 0) {
					printf("tls_config_set_cert_file error\n");
				}
				if(tls_configure(tls, t_conf) < 0) {
					printf("tls_configure error: %s\n", tls_error(tls));
				}
			}

		}
		~IMAPProvider(){
			if(t_conf != NULL){
				tls_config_free(t_conf);
			}
		}
		void operator()(int fd) const{
			std::string data(8193, 0);


			int rcvd;
			if(states[fd].state() != UNENC){
				rcvd = tls_read(states[fd].tls, &data[0], 8912);
			}else{
				rcvd= recv(fd, &data[0], 8192, MSG_DONTWAIT);
			}
			if( rcvd == -1){
				disconnect(fd, "Unable to read from socket");
			}else{
				data.resize(rcvd);
				parse(fd, data);
			}
		}
		void disconnect(int fd, std::string reason) const{
			BYE(fd, "*", reason);
			if(states[fd].tls !=  NULL){
				tls_close(states[fd].tls);
				tls_free(states[fd].tls);
			}
			states.erase(fd);
			close(fd);
		}
		void connect(int fd) const;
	};
}

void IMAPProvider::IMAPProvider::connect(int fd) const{
	if(config.secure){
		if(tls_accept_socket(tls, &states[fd].tls, fd) < 0) {
			std::cout << "tls_accept_socket error" << std::endl;
			disconnect(fd, "TLS Negotiation Failed");
		}else{
			if(tls_handshake(states[fd].tls) < 0){
				std::cout << "tls_handshake error" << std::endl;
				disconnect(fd, "TLS Negotiation Failed");
			}else{
				states[fd].starttls();
			}
		}
	}
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int getaddr = getpeername(fd, (struct sockaddr *) &addr, &addrlen);
	std::string address(inet_ntoa(addr.sin_addr));
	OK(fd, "*", "Welcome to IMAPlw. IMAP ready for requests from " + address);
}



std::map<int, IMAPProvider::IMAPClientState> IMAPProvider::IMAPProvider::states;

void IMAPProvider::IMAPProvider::CAPABILITY(int rfd, std::string tag) const{
	if(config.starttls && !config.secure && (states[rfd].state() == UNENC)){
		respond(rfd, "*", "CAPABILITY", "IMAP4rev1 STARTTLS");
	}else{
		respond(rfd, "*", "CAPABILITY", "IMAP4rev1 ");
	}
	OK(rfd, tag, "CAPABILITY executed successfully");
}
void IMAPProvider::IMAPProvider::STARTTLS(int rfd, std::string tag) const{
	if(config.starttls && !config.secure && (states[rfd].state() == UNENC)){
		OK(rfd, tag, "Begin TLS Negotiation Now");
		if(tls_accept_socket(tls, &states[rfd].tls, rfd) < 0) {
			std::cout << "tls_accept_socket error" << std::endl;
		}else{
			if(tls_handshake(states[rfd].tls) < 0){
				std::cout << "tls_handshake error" << std::endl;
			}else{
				states[rfd].starttls();
			}
		}
	}else{
		BAD(rfd, tag, "STARTTLS Disabled");
	}
}
void IMAPProvider::IMAPProvider::route(int fd, std::string tag, std::string command, std::string args) const{
	std::transform(command.begin(), command.end(),command.begin(), ::toupper); //https://stackoverflow.com/questions/735204/convert-a-string-in-c-to-upper-case
	if(command == "CAPABILITY"){
		CAPABILITY(fd, tag);
	}else if(command == "STARTTLS"){
		STARTTLS(fd, tag);
	}else{
		BAD(fd, tag, "Command \"" + command + "\" NOT FOUND");
	}
}
void IMAPProvider::IMAPProvider::parse(int fd, std::string message) const{
	std::stringstream mstream(message);
	std::string tag, command, args;
	if(mstream >> tag >> command){
		if(mstream >> args){
			route(fd, tag, command, args);
		}else{
			route(fd, tag, command, "");
		}
	}else{
		BAD(fd, "*", "Unable to parse command");
	}
}












