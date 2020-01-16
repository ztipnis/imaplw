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
#include <utility>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <unordered_map>
#include <fstream>  
#include <string>

#ifndef __H_CONFIG_PARSER__
#define __H_CONFIG_PARSER__

enum cfgType {Numeric, String, Time, Boolean};

template <cfgType T>
typename std::enable_if<T == Numeric, float>::type typeCast(std::string value){
	return std::stof(value);
} 

template <cfgType T>
typename std::enable_if<T == String, std::string>::type typeCast(std::string value){
	return value;
}
template <cfgType T>
typename std::enable_if<T == Boolean, bool>::type typeCast(std::string value){
	return value == "True" || value == "true";
} 


template <cfgType T>
typename std::enable_if<T == Time, std::chrono::microseconds>::type typeCast(std::string value){
	std::stringstream ss(value);
	std::string buf;
	while(ss >> std::quoted(buf)){
		size_t nt;
		int num = stoi(buf,&nt);
		std::string suffix(buf.substr(nt));
		if(buf == "" || buf == "s"){
			return std::chrono::seconds(num);
		}else if(buf == "min"){
			return std::chrono::minutes(num);
		}else if(buf == "h"){
			return std::chrono::hours(num);
		}else if(buf == "d"){
			return std::chrono::hours(num * 24); //days introduced in C++20 which is not standard at the time of writing
		}else if(buf == "w"){
			return std::chrono::hours(num * 24 * 7); //weeks introduced in C++20 which is not standard at the time of writing
		}else if(buf == "ms"){
			return std::chrono::milliseconds(num);
		}else if(buf == "us"){
			return std::chrono::microseconds(num);
		}
	}
} 

class Config {
private:
	void parse(std::string);
	std::unordered_map<std::string, std::string> values;
public:
	Config(std::string file){
		parse(file);
	}
	template <cfgType T>
	auto get(std::string section, std::string item){
		return typeCast<T>(values[section + "::" + item]);
	}
};

void Config::parse(std::string filePath){
	std::ifstream cfgFile(filePath);
	if(cfgFile.is_open()){
		thread_local std::string current_section;
		int i = 0;
		for(std::string line; std::getline(cfgFile, line);){
			i++;
			if(line.size() == 0){
				continue;
			}else if(line[0] == '#' || line[0] == ';'){
				continue;
			}else if(line[0] == '[' && line.back() == ']'){
				current_section = line.substr(1, line.size() - 2);
			}else if(line.find("=") != std::string::npos){
				size_t loc = line.find("=");
				std::string val;
				std::stringstream ss;
				ss << line.substr(loc + 1, std::string::npos);
				ss >> std::quoted(val);
				values[current_section + "::" + line.substr(0,loc)] = val;
			}else{
				throw std::runtime_error("Error parsing " + filePath + "\t[Line " + std::to_string(i) + "] " + line);
			}
		}
	}
}

#endif