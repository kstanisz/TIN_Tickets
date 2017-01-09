#include<string>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
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
		
		void setIp(std::string ip)
		{
			this->ip=ip;
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
		std::string ip;
		std::string serviceName;
	public:
		Response(std::string message,Ticket* ticket){
			this->message = message;
			this->ip = ticket->ip;
			this->serviceName = ticket->serviceName;
		}
		
		std::string getMessage(){
			return message;
		}
		
		Ticket* getTicket(){
			Ticket* ticket =  new Ticket();
			ticket->ip = ip;
			ticket->serviceName = serviceName;
			return ticket;
		}
		
		std::string serialize(){
			json j;
			j["message"] = message;
			j["ip"] = ip;
			j["serviceName"] = serviceName;
			
			return j.dump();
		}
		
		static Response* deserialize(json j){
			std::string message = j["message"];
			Ticket ticket = Ticket();
			ticket.ip = j["ip"];
			ticket.serviceName = j["serviceName"];
			
			return new Response(message,&ticket);
		}
};