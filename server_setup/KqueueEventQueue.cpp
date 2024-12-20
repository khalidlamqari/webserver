
#include "KqueueEventQueue.hpp"

KqueueEventQueue::KqueueEventQueue()
{
	queue_created = false;
}

KqueueEventQueue::~KqueueEventQueue()
{
	if (queue_created)
		close(kqueue_fd);
}

void    KqueueEventQueue::create_kqueue( void )
{
	if ((kqueue_fd = kqueue()) == -1)
		throw std::runtime_error(std::string("Webserv : kqueue failed, reason : ") + strerror(errno));

	queue_created = true;
}

void    KqueueEventQueue::register_socket_in_kqueue(Socket * sock_data, short filter)
{
	struct kevent ev;

	// memset(&ev, 0, sizeof(struct kevent));
	EV_SET(&ev, sock_data->get_ident(), filter, EV_ADD | EV_ENABLE, 0, 0, (void *) sock_data);

	if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error(std::string("Webserv : kevent(4) failed, reason : ") + strerror(errno));
}

void    KqueueEventQueue::switch_interest(ClientSocket* client_info, short old_filter, short new_filter)
{
	struct kevent   ev;

	/* Deleting old event, event filter might be either EVFILT_READ or EVFILT_WRITE */
	memset(&ev, 0, sizeof(struct kevent));
	EV_SET(&ev, client_info->get_ident(), old_filter, EV_DELETE, 0, 0, NULL);
	if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("kevent(3) failed!");

	/* Setting the new event we are interested in */
	memset(&ev, 0, sizeof(struct kevent));
	EV_SET(&ev, client_info->get_ident(), new_filter, EV_ADD | EV_ENABLE, 0, 0, (void *) client_info);
	if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
		throw std::runtime_error("kevent(2) failed!");
}

void    KqueueEventQueue::register_listeners_in_kqueue(std::vector<ListenerSocket> & activeListners)
{
	std::vector<ListenerSocket>::iterator it = activeListners.begin();
	std::vector<ListenerSocket>::iterator end = activeListners.end();

	for ( ; it != end; it++)
	{
		it->mark_ident_as_set();
		register_socket_in_kqueue(&(it->get_instance()), EVFILT_READ);
	}
}

int	KqueueEventQueue::poll_events(struct kevent *event_list, int capacity)
{
	int	n_events;	// Number of events fetched.

	if ((n_events = kevent(kqueue_fd, NULL, 0, event_list, capacity, 0)) == -1)
		throw std::runtime_error(std::string("Webserv : kevent(2) failed, reason : ") + strerror(errno));

	return n_events;
}
