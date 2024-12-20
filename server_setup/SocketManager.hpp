
#ifndef SOCKET_MANAGER_HPP
# define SOCKET_MANAGER_HPP

#include "KqueueEventQueue.hpp"
#include "../Utils/utils.hpp"

#include <vector>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <map>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

class SocketManager
{
    public:

		SocketManager();
		~SocketManager();

		void    		create_listeners(const HttpContext& http_config);
		void    		initialize_sockets_on_port(struct addrinfo *list, const ServerContext& server, unsigned short port);
		struct addrinfo	*my_get_addrinfo(const char * host);
		bool			already_binded(const ServerContext& server, struct in_addr host, unsigned short port);
		void    		create_new_listener(int fd, struct addrinfo *entry, const ServerContext& server);
		void			delete_client(int fd);
		void			add_client(ClientSocket * new_client);

		std::vector<ListenerSocket>&	get_listeners();

	private :

		std::vector<ListenerSocket>	activeListners;
		std::vector<ClientSocket *>	activeClients;
};



#endif