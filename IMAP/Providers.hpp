#import <string>

class AuthenticationProvider {

}

//Dataprovider Subclass must provide init() to initialize m_Inst and implement all public functions.
class DataProvider {
public:
	static DataProvider& Inst(){ return *m_Inst; }
	virtual void init() = 0;
	virtual std::string get() = 0;

private:
	static DataProvider* m_Inst;
	DataProvider(DataProvider const&) = delete;
	DataProvider& operator=(DataProvider const&) = delete;
	DataProvider() = delete;

};



namespace IMAPProvider{
	typedef enum IMAPState { UNAUTH, AUTH, SELECTED } IMAPState_t;

	class IMAPClientState {
	private:
		bool authenticated;
		std::string user;
		bool selected;
		std::string mbox;
	public:
		const IMAPState_t state() const{
			if(authenticated){
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
		bool authenticate(const AuthenticationProvider& provider, const std::string& username, const std::string& password){
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

	class IMAPProvider{
	private:
		//ANY STATE
		void CAPABILITY(int rfd);
		void NOOP(int rfd);
		void LOGOUT(int rfd);
		//UNAUTHENTICATED
		void STARTTLS(int rfd);
		void AUTHENTICATE(int rfd);
		void LOGIN(int rfd);
		//AUTENTICATED
		void SELECT(int rfd);
		void EXAMINE(int rfd);
		void CREATE(int rfd);
		void DELETE(int rfd);
		void RENAME(int rfd);
		void SUBSCRIBE(int rfd);
		void UNSUBSCRIBE(int rfd);
		void LIST(int rfd);
		void LSUB(int rfd);
		void STATUS(int rfd);
		void APPEND(int rfd);
		//SELECTED
		void CHECK(int rfd);
		void CLOSE(int rfd);
		void EXPUNGE(int rfd);
		void SEARCH(int rfd);
		void STORE(int rfd);
		void COPY(int rfd);
		void UID(int rfd);


		//RESPONSES
		void OK(int rfd, std::string tag);
		void NO(int rfd, std::string tag);
		void BAD(int rfd, std::string tag);
		void PREAUTH(int rfd, std::string tag);
		void BYE(int rfd, std::string tag);
	public:
		void parse(){

		}
	};
}


