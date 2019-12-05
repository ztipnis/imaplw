#import "Helpers.hpp"

#ifndef __IMAP_CLIENT_STATE__
#define __IMAP_CLIENT_STATE__

namespace IMAPProvider{
	typedef enum { UNENC, UNAUTH, AUTH, SELECTED } IMAPState_t;
	class IMAPClientState {
	private:
		std::string uuid;
		bool encrypted;
		bool authenticated;
		std::string user;
		bool selected;
		std::string mbox;
	public:
		struct tls *tls = NULL;
		IMAPClientState(){
			encrypted = false;
			authenticated = false;
			user = "";
			selected = false;
			mbox = "";
			uuid = gen_uuid(15);
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
		void starttls(){
			encrypted = true;
		}
		void logout(){
			authenticated = false;
			user = "";
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
		// bool authenticate(const std::string& username, const std::string& password){
		// 	AuthenticationProvider& provider = AuthenticationProvider::getInst<A>();
		// 	if(provider.lookup(username) == false){
		// 		return false;
		// 	}
		// 	if(provider.authenticate(username, password)){
		// 		user = username;
		// 		return true;
		// 	}
		// 	return false;
		// }
	};
}

#endif