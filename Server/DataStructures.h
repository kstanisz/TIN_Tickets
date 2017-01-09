#include<string>
#include"json.hpp"

using json = nlohmann::json;

struct Ticket
{
	std::string ip;
	std::string serviceName;
};

inline bool operator<(const Ticket& ticket1, const Ticket& ticket2)
{
	return ticket1.serviceName < ticket2.serviceName;
}

class Request
{
	private:
		bool releaseTicket;
		std::string ip;
		std::string password;
		std::string serviceName;
		std::string message;
	public:
	
		Request(bool releaseTicket,std::string ip, std::string password, std::string serviceName, std::string message){
			this->releaseTicket = releaseTicket;
			this->ip = ip;
			this->password = password;
			this->serviceName = serviceName;
			this->message = message;
		}
		
		bool isReleaseTicket(){
			return releaseTicket;
		}
		
		std::string getIp(){
			return ip;
		}
		
		std::string getPassword(){
			return password;
		}
		
		std::string getServiceName(){
			return serviceName;
		}
		
		std::string getMessage(){
			return message;
		}
		
		std::string serialize(){
			json j;
			j["releaseTicket"] = releaseTicket;
			j["ip"] = ip;
			j["password"] = password;
			j["serviceName"] = serviceName;
			j["message"] = message;
			
			return j.dump();
		} 
		
		static Request* deserialize(json j){
			bool releaseTicket = j["releaseTicket"];
			std::string ip = j["ip"];
			std::string password = j["password"];
			std::string serviceName = j["serviceName"];
			std::string message = j["message"];
			
			return new Request(releaseTicket,ip,password,serviceName,message);
 		}
};

class Response
{
	private:
		std::string message;
		Ticket ticket;
	public:
		Response(std::string message,Ticket* ticket){
			this->message = message;
			this->ticket = (*ticket);
		}
		
		std::string getMessage(){
			return message;
		}
		
		std::string serialize(){
			json j;
			j["message"] = message;
			j["ticket"] = {{"ip",ticket.ip},{"serviceName",ticket.serviceName}};
			
			return j.dump();
		}
		
		static Response* deserialize(json j){
			std::string message = j["message"];
			Ticket ticket = Ticket();
			ticket.ip = "test";
			ticket.serviceName = "test2";
			
			return new Response(message,&ticket);
		}
};