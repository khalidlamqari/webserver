

#ifndef KQUEUE_IDENT_HPP
#define KQUEUE_IDENT_HPP

#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <exception>
#include "../Contexts/HttpContext.hpp"
#include <unistd.h>
#include "../response/Response.hpp"
#include "../Request/request_parse.hpp"
#include <signal.h>

class Response;
extern char **environ;

typedef enum	e_ident_type
{
	CHILD_ID,
	LISTENER_SOCK,
	CLIENT_SOCK,
	CGI_PAIR_SOCK
}	t_ident_type;

class KqueueIdent
{
	public :

		virtual void			set_ident( const int & id ) = 0;
		virtual void			set_type( const t_ident_type & type ) = 0;

		virtual int				get_ident( void ) = 0;
		virtual t_ident_type	get_type( void ) = 0;

	protected :

		int				_ident;
		t_ident_type	_type;
};



/* --------------- Cgi Process -------------- */
class CgiProcess : public KqueueIdent {

    public :

		/* Constructors */
		CgiProcess(Response *response);

		/* Destructor */
        virtual ~CgiProcess();

		/* Setters */
		void			set_ident( const int & id );
		void			set_type( const t_ident_type & type );
		void			set_response( Response * response );
		
		/* Getters */
		int				get_ident( void );
		t_ident_type	get_type( void );
		Response*		get_response( void );
		void			setup_environment();
		void			execute_cgi();

		void			markAsStoped();
		void                        set_start_time(std::time_t tm);
        std::time_t                 get_start_time();

    private :

		CgiProcess( const CgiProcess & ref );

		std::time_t	start_time;

		Response	*response;

		bool		is_running;
};

/* --------------- Cgi Pair Socket --------------- */
class CgiPairSocket : public KqueueIdent {

    public :

		/* Constructors */
		CgiPairSocket(Response *response);

		/* Destructor */
        virtual ~CgiPairSocket();

		/* Setters */
		void			set_ident( const int & id );
		void			set_type( const t_ident_type & type );
		void			set_response( Response * response );

		/* Getters */
		int				get_ident( void );
		t_ident_type	get_type( void );
		Response*		get_response( void );

		/* Pair socket creation */
		void			create_socket_pair( void );
		int *                       get_pair_fds();

    private :

		CgiPairSocket(const CgiPairSocket & ref);

		int			s_fds[2] ;
		Response 	*response;
};

class Socket : public KqueueIdent {

    public :

		Socket();
		// Socket( Socket & other );
        virtual ~Socket();

        virtual void    									set_host( const struct in_addr & host ) = 0;
        virtual void    									set_port( const unsigned short & port ) = 0;
        virtual void    									set_servers( const std::vector<const ServerContext*> & servers ) = 0;
        virtual void    									add_server( const ServerContext* server ) = 0;

        virtual struct in_addr    							get_host( void ) = 0;
        virtual unsigned short    							get_port( void ) = 0;
		virtual const std::vector<const ServerContext*>&	get_servers( void ) = 0;


    protected :

        struct in_addr                      _host;       // In the host representation
        unsigned short                      _port;       // In the host representation

        std::vector<const ServerContext*>   _related_servers;    // List of server contexts that can serve requests received on this socket. // TODO : remove pointer

		bool								_ident_is_set;

};

class ListenerSocket : public Socket {

    public :

        /* Constructors */
		ListenerSocket();
		ListenerSocket( const bool & is_temp );

		/* Destructor */
		~ListenerSocket();

		/* Setter methods inherited from the KqueueIdent class */
		void										set_ident( const int & id );
		void										set_type( const t_ident_type & type );
		void										mark_ident_as_set();

		/* Setter methods inherited from the Socket class */
        void    									set_host( const struct in_addr & host );
        void    									set_port( const unsigned short & port );
        void    									set_servers( const std::vector<const ServerContext*> & servers );
        void    									add_server( const ServerContext* server );


		// Getter methods inherited from the KqueueIdent class
		int											get_ident( void );
		t_ident_type								get_type( void );

		/* Getter methods inherited from the Socket class */
        struct in_addr								get_host( void );
        unsigned short								get_port( void );
		const std::vector<const ServerContext*>&	get_servers( void );

		/* ------------------------------------------------------------------- */

        ListenerSocket& 							get_instance( void );

	private :

		bool	is_temp;
};

class ClientSocket : public Socket {

    public :

		/* Constructors */
		ClientSocket();

		/* Destructor */
		~ClientSocket();

		/* Setter methods inherited from the KqueueIdent class */
		void										set_ident( const int & id );
		void										set_type( const t_ident_type & type );


		/* Setter methods inherited from the Socket class */
        void    									set_host( const struct in_addr & host );
        void    									set_port( const unsigned short & port );
        void    									set_servers( const std::vector<const ServerContext*> & servers );
        void    									add_server( const ServerContext* server );


		// Getter methods inherited from the KqueueIdent class
		int											get_ident( void );
		t_ident_type								get_type( void );

		/* Getter methods inherited from the Socket class */
        struct in_addr								get_host( void );
        unsigned short								get_port( void );
		const std::vector<const ServerContext*>&	get_servers( void );

		/* ------------------------------------------------------------------- */

        ClientSocket&   							get_instance( void );

		void										set_request( Request *new_request );
		void										set_response( Response *new_response );

		Request*									get_request( void );
		Response*									get_response( void );

		void										delete_request( void );
		void    									delete_response( void );

    private :

        Request*		_request;
		Response*		_response;

};


#endif