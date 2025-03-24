#include "../includes/Server.hpp"

Server::Server()
{ }

Server::Server(const std::string port, const std::string pass) 
	: _port(port), _pass(pass), _host("127.0.0.1")
{ }

Server::Server(const Server& other)
{
	(void)other;
}

Server& Server::operator=(const Server& other)
{
	(void)other;
	return *this;
}

Server::~Server()
{ }

int Server::start()
{
	//Create sockets
	int listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1)
	{
		std::cerr << "Can't create socket! Quitting" << std::endl;
		return -1;
	}

	//Bind the ip adress and port to socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(std::atoi(_port.c_str()));
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
	{
		std::cout << "Can't bind\n";
		return -2;
	}
	
	// Tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);	

	// Wait for a connection
	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i. e. port) the client is connect on
	
	memset(host, 0, NI_MAXHOST);	//same as memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0))
		std::cout << host << " connected on port " << service << std::endl;
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connect on " << ntohs(client.sin_port) << std::endl;
	}

	// Close listening socket
	close(listening);
		
	// While loop: accept and message back to client
	char buff[4096];

	while (true)
	{
		memset(buff, 0, 4096);

		// Whait for client to send data
		int bytesReceived = recv(clientSocket, buff, 4096, 0);
		if (bytesReceived == -1)
		{
			std::cerr << "Error in recv(). Quitting\n";
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected\n";
			break;
		}

		std::cout << "Rceived: " << std::string(buff, 0, bytesReceived) << std::endl;
		
		// Echo message back to client
		send(clientSocket, ("I said: " + std::string(buff)).c_str(), bytesReceived + 9, 0);
	}

	// Close socket
	close(clientSocket);

	return 0;
}
