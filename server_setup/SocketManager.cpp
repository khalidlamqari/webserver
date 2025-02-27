#include "SocketManager.hpp"

SocketManager::SocketManager()
{

}

SocketManager::~SocketManager()
{
    // Deleting the activeClients objects (This will result in calling the ClientSocket then Socket destructors!)
    std::vector<ClientSocket *>::iterator it = activeClients.begin();
    std::vector<ClientSocket *>::iterator end = activeClients.end();

    for ( ; it != end; it++ )
    {
        delete *it;
    }
}

bool SocketManager::already_binded(const ServerContext& server, struct in_addr host, unsigned short port)
{
    std::vector<ListenerSocket>::iterator  it = activeListners.begin();
    std::vector<ListenerSocket>::iterator  end = activeListners.end();

    for ( ; it != end; it++)
    {
        if ((it->get_host().s_addr == host.s_addr))
        if ((it->get_host().s_addr == host.s_addr) && (it->get_port() == port))
        {
            it->add_server(&server); // link the server to the socket already created for this host:port
            return true;
        }
    }
    return false;
}

struct addrinfo *   SocketManager::my_get_addrinfo(const char * host)
{
    int             ec;             // error code;
    struct addrinfo *res;
    struct addrinfo hints;

    memset(&hints, 0,sizeof(hints));
    hints.ai_family = AF_INET; // TODO : Try specifying AF_INET6 as address family hint and see if ipv4 addresses are gonna be returned as well.
    hints.ai_socktype = SOCK_STREAM;

    if ((ec = getaddrinfo(host, NULL, &hints, &res)) != 0)
    {
        if (ec == 8)
            std::cerr << BOLD << RED << "Unknown host \"" << host << "\"." << RESET << std::endl;
        return NULL;
    }
    return res;
}

void    SocketManager::create_new_listener(int fd, struct addrinfo *entry, const ServerContext& server)
{
    ListenerSocket  new_s;

    new_s.set_ident(fd);
    new_s.set_host(((struct sockaddr_in *)entry->ai_addr)->sin_addr);
    new_s.set_port(((struct sockaddr_in*)entry->ai_addr)->sin_port);
    new_s.add_server(&server);

    activeListners.push_back(new_s);

    new_s.set_ident(-1);
}

void    SocketManager::initialize_sockets_on_port(struct addrinfo *list, const ServerContext& server, unsigned short port)
{
    int                 fd;
    const char          *cause = NULL;
    unsigned int        n_sock = 0;
    int                 opt = 1;
    struct sockaddr_in *ip_access; 

    for (struct addrinfo *entry = list; entry ; entry = entry->ai_next)
    {
        if (already_binded(server, ((struct sockaddr_in *) entry->ai_addr)->sin_addr, htons(port)))
        {
            n_sock++;
            continue ;
        }

        if ((fd = socket(entry->ai_family, entry->ai_socktype, entry->ai_protocol)) == -1)
        {
            cause = "socket";
            continue ;
        }

        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));

        ip_access = (struct sockaddr_in *) entry->ai_addr;
        ip_access->sin_family = entry->ai_family;;
        ip_access->sin_port = htons(port);

        if (bind(fd, (struct sockaddr *) ip_access, sizeof(*ip_access)) == -1)
        {
            close(fd);
            cause = "bind";
            continue ;
        }

        if (listen(fd, 128) == -1)
        {
            close(fd);
            cause = "listen";
            continue ;
        }

        create_new_listener(fd, entry, server);

        n_sock++;
    }

    if (n_sock == 0)
        throw cause;
}

void    SocketManager::create_listeners(const HttpContext& http_config)
{
    struct addrinfo *res;
    int             valid_servers = 0;

    std::vector<ServerContext>::const_iterator serv_it = http_config.get_servers().begin();
    std::vector<ServerContext>::const_iterator end = http_config.get_servers().end();

    for ( ; serv_it != end; serv_it++)
    {
        if ((res = my_get_addrinfo(serv_it->get_host().c_str())) == NULL)
            continue ;

        {
            std::vector<unsigned short>::const_iterator ports_it = serv_it->get_ports().begin();
            std::vector<unsigned short>::const_iterator p_end = serv_it->get_ports().end();
            int                                         valid_ports = 0;

            for ( ; ports_it != p_end; ports_it++ )
            {
                try {
                    initialize_sockets_on_port(res, *serv_it, *ports_it);
                    valid_ports++;
                }
                catch(const char * err) {}
            }

            if (valid_ports)
                valid_servers++;
        }

        if (res) freeaddrinfo(res);
    }

    if (valid_servers)
        show_listeners_info();
    else
        throw "❌ ERROR: No valid server could be created!\n   Please check your configuration and try again.";
}

std::vector<ListenerSocket>&	SocketManager::get_listeners()
{
    return this->activeListners;
}

void    SocketManager::delete_client(int fd)
{
    std::vector<ClientSocket *>::iterator it = activeClients.begin();
    std::vector<ClientSocket *>::iterator end = activeClients.end();

    for ( ; it != end; it++)
    {
        if ((*it)->get_ident() == fd)
        {
            delete (*it);
            activeClients.erase(it);
            return ;
        }
    }
}

void    SocketManager::add_client(ClientSocket * new_client)
{
    activeClients.push_back(new_client);
}

void    SocketManager::show_listeners_info( void )
{

    std::cout << BOLD << CYAN << "Created Listeners: " << RESET << std::endl;

    for (std::vector<ListenerSocket>::iterator l_it = activeListners.begin(); l_it != activeListners.end(); l_it++)
    {
        std::cout << BOLD << CYAN << "  Server Names: " << RESET << "[";

        for (std::vector<const ServerContext*>::const_iterator s_it = l_it->get_servers().begin(); s_it != l_it->get_servers().end(); s_it++)
        {
            if (s_it != l_it->get_servers().begin())
                std::cout << ", ";
            (*s_it)->show_server_names();
        }

        std::cout << "]   " << BOLD << CYAN << "Listening on: " << RESET;

        std::cout	<< GREEN
                    << inet_ntoa(l_it->get_host())
                    << ":" << ntohs(l_it->get_port())
                    << RESET
                    << std::endl;
    }
}