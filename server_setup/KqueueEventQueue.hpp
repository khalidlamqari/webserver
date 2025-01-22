#ifndef KQUEUE_EVENT_QUEUE_HPP
#define KQUEUE_EVENT_QUEUE_HPP

#include <sys/event.h>
#include <sys/types.h>

#include "KqueueIdent.hpp"

class KqueueEventQueue
{
	public :

	KqueueEventQueue();
	~KqueueEventQueue();

    void	register_listeners_in_kqueue(std::vector<ListenerSocket> & activeListners);
    int		switch_interest(ClientSocket* client_info, short old_filter, short new_filter);
	int    	register_event_in_kqueue(KqueueIdent * data, short filter);
	void	delete_event_from_kqueue(KqueueIdent * data, short filter);
	void	create_kqueue( void );
	int		poll_events(struct kevent *event_list, int capacity);

    private :

        int		kqueue_fd;
		bool	queue_created;
};

#endif