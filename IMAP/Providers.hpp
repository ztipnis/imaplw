#import <iostream>
#import <string>
#import <map>
#import <sstream>
#import <uuid/uuid.h>
#import "../SocketPool/SocketPool.hpp"



std::string gen_uuid(){
	uuid_t id;
	uuid_generate( (unsigned char *)&id );
	std::string uu(reinterpret_cast<char*>(id), 16);
	return uu;
}


class AuthenticationProvider {
public:
	virtual bool lookup(std::string username) = 0;
	virtual bool authenticate(std::string username, std::string password) = 0;
	template <class T> static AuthenticationProvider& getInst(){
		static T m_Inst;
		return m_Inst;
	}
private:
	AuthenticationProvider(AuthenticationProvider const&) = delete;
	AuthenticationProvider& operator=(AuthenticationProvider const&) = delete;
	AuthenticationProvider() = delete;
};

//Dataprovider Subclass must provide init() to initialize m_Inst and implement all public functions.
class DataProvider {
public:
	template <class T> static DataProvider& getInst(){
		static T m_Inst;
		return m_Inst;
	}
	virtual std::string get() = 0;
private:
	DataProvider(DataProvider const&) = delete;
	DataProvider& operator=(DataProvider const&) = delete;
	DataProvider() = delete;
};

inline void sendMsg(int fd, const std::string &data){
	#ifndef SO_NOSIGPIPE
		send(fd, &data[0], data.length(), MSG_DONTWAIT | MSG_NOSIGNAL);
	#else
		send(fd, &data[0], data.length(), MSG_DONTWAIT);
	#endif
}


namespace IMAPProvider{
	typedef enum { UNENC, UNAUTH, AUTH, SELECTED } IMAPState_t;

	template <class A>
	class IMAPClientState {
	private:
		std::string uuid;
		bool encrypted;
		bool authenticated;
		std::string user;
		bool selected;
		std::string mbox;
	public:
		IMAPClientState(){
			encrypted = false;
			authenticated = false;
			user = "";
			selected = false;
			mbox = "";
			uuid = gen_uuid();
		}
		const IMAPState_t state() const{
			if(!encrypted){
				return UNENC;
			}else if(authenticated){
				if(selected){
					return SELECTED;
				}else{
					return AUTH;
				}
			}else{
				return UNAUTH;
			}
		}
		const std::string getUser() const{
			return user;
		}
		const std::string getMBox() const{
			return mbox;
		}
		const std::string get_uuid() const{
			return uuid;
		}
		bool authenticate(const std::string& username, const std::string& password){
			AuthenticationProvider& provider = AuthenticationProvider::getInst<A>();
			if(provider.lookup(username) == false){
				return false;
			}
			if(provider.authenticate(username, password)){
				user = username;
				return true;
			}
			return false;
		}
	};

	// template <class D>
	class IMAPProvider: public Pollster::Handler{
	private:
		//ANY STATE
		static void CAPABILITY(int rfd, std::string tag){
			respond(rfd, "*", "CAPABILITY", "IMAP4rev1");
			OK(rfd, tag, "CAPABILITY executed successfully");
		}
		static void NOOP(int rfd);
		static void LOGOUT(int rfd);
		//UNAUTHENTICATED
		static void STARTTLS(int rfd);
		static void AUTHENTICATE(int rfd);
		static void LOGIN(int rfd);
		 //AUTENTICATED
		static void SELECT(int rfd);
		static void EXAMINE(int rfd);
		static void CREATE(int rfd);
		static void DELETE(int rfd);
		static void RENAME(int rfd);
		static void SUBSCRIBE(int rfd);
		static void UNSUBSCRIBE(int rfd);
		static void LIST(int rfd);
		static void LSUB(int rfd);
		static void STATUS(int rfd);
		static void APPEND(int rfd);
		//SELECTED
		static void CHECK(int rfd);
		static void CLOSE(int rfd);
		static void EXPUNGE(int rfd);
		static void SEARCH(int rfd);
		static void STORE(int rfd);
		static void COPY(int rfd);
		static void UID(int rfd);


		//RESPONSES
		static inline void respond(int rfd, std::string tag, std::string code, std::string message){
			sendMsg(rfd, tag + " "+ code +" " + message + "\n");
		}

		static void OK(int rfd, std::string tag, std::string message){
			respond(rfd, tag, "OK", message);
		}
		
		static void NO(int rfd, std::string tag, std::string message){
			respond(rfd, tag, "NO", message);
		}

		static void BAD(int rfd, std::string tag, std::string message){
			respond(rfd, tag, "BAD", message);
		}
		static void PREAUTH(int rfd, std::string tag, std::string message){
			respond(rfd, tag, "PREAUTH", message);
		}

		static void BYE(int rfd, std::string tag, std::string message){
			respond(rfd, tag, "BYE", message);
		}

		// static std::map<int, IMAPClientState> map;

		static void route(int fd, std::string tag, std::string command, std::string args){
			BAD(fd, tag, "Command \"" + command + "\" NOT FOUND");
		}

		static void parse(int fd, std::string message){
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

	public:

		void operator()(int fd) const{
			std::string data(8193, 0);
			int rcvd = recv(fd, &data[0], 8192, MSG_DONTWAIT);
			if( rcvd == -1){
				disconnect(fd, "Unable to read from socket");
			}else{
				data.resize(rcvd);
				parse(fd, data);
			}
		}
		void disconnect(int fd, std::string reason) const{
			BYE(fd, "*", reason);
			close(fd);
		}
		void connect(int fd) const{
			struct sockaddr_in addr;
			socklen_t addrlen = sizeof(addr);
			int getaddr = getpeername(fd, (struct sockaddr *) &addr, &addrlen);
			std::string address(inet_ntoa(addr.sin_addr));
			std::string uuid = gen_uuid();
			OK(fd, "*", "Welcome to IMAPlw. IMAP ready for requests from " + address + " " + uuid);
		}
	};
}


