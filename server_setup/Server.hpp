/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 16:08:35 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/21 08:16:12 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP


#include "SocketManager.hpp"
#include <fcntl.h>
/// include only needed hraders ! 

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

    private :

	SocketManager		socketManager;
	KqueueEventQueue	kqueueManager;
};


/*                              Cient Management                              */
void    handle_client_request(ClientSocket* client_info);
void    respond_to_client(ClientSocket* client_info, SocketManager& socketManager, KqueueEventQueue & kqueueManager );



#endif