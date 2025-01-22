
#ifndef SERVER_HPP
# define SERVER_HPP


#include "SocketManager.hpp"
#include "../response/Response.hpp"
#include <fcntl.h>

#define TMOUT_SEC 1
#define TMOUT_NSEC 2000
#define READ_BUFFER_SIZE 11024
#define	EVENT_QUEUE_CAPACITY 1000

class Server {

	public :

		// Constructor
		Server();

		// Destructor
		~Server();

		void	setup(const HttpContext & http_config);
		void	start();
		void	accept_client_connection(ListenerSocket * listener);
		void	handle_client_event(struct kevent & event);
		void	handle_cgi_output(struct kevent & event);
		void	handle_cgi_process(CgiProcess * cgiProc);

    private :
	SocketManager		socketManager;
	KqueueEventQueue	kqueueManager;
};

int global_status(int sts, bool set);

/*                              Cient Management                              */
void    handle_client_request(ClientSocket* client_info);
void    respond_to_client(ClientSocket* client_info, KqueueEventQueue & kqueueManager );
void setup_request_response(ClientSocket* client_info, SocketManager& socketManager);
bool    checkBrokenIdents(int ident, std::string cmd);

#endif