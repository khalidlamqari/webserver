

#ifndef KQUEUE_IDENT_HPP
#define KQUEUE_IDENT_HPP

#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../Contexts/HttpContext.hpp"

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



#endif