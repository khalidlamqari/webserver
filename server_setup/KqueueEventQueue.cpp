
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

int    KqueueEventQueue::register_event_in_kqueue(KqueueIdent * data, short filter)
{
	struct kevent ev;

	memset(&ev, 0, sizeof(struct kevent));
	if (dynamic_cast<CgiProcess *> (data))
		EV_SET(&ev, data->get_ident(), filter, EV_ADD | EV_ENABLE, NOTE_EXITSTATUS | NOTE_EXIT, 0, (void *) data);
	else
		EV_SET(&ev, data->get_ident(), filter, EV_ADD | EV_ENABLE, 0, 0, (void *) data);

	if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
		return 1;
	return 0;
}

void	KqueueEventQueue::delete_event_from_kqueue(KqueueIdent * data, short filter)
{
	struct kevent   ev;

	memset(&ev, 0, sizeof(struct kevent));
	EV_SET(&ev, data->get_ident(), filter, EV_DELETE, 0, 0, NULL);
	kevent(kqueue_fd, &ev, 1, NULL, 0, NULL);
}

int    KqueueEventQueue::switch_interest(ClientSocket* client_info, short old_filter, short new_filter)
{
	struct kevent   ev;

	/* Deleting old event, event filter might be either EVFILT_READ or EVFILT_WRITE */
	memset(&ev, 0, sizeof(struct kevent));
	EV_SET(&ev, client_info->get_ident(), old_filter, EV_DELETE, 0, 0, NULL);
	if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
		return 1;

	/* Setting the new event we are interested in */
	memset(&ev, 0, sizeof(struct kevent));
	EV_SET(&ev, client_info->get_ident(), new_filter, EV_ADD | EV_ENABLE, 0, 0, (void *) client_info);
	if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
		return 1;
	return 0;
}

void    KqueueEventQueue::register_listeners_in_kqueue(std::vector<ListenerSocket> & activeListners)
{
	std::vector<ListenerSocket>::iterator it = activeListners.begin();
	std::vector<ListenerSocket>::iterator end = activeListners.end();

	for ( ; it != end; it++)
	{
		it->mark_ident_as_set();
		register_event_in_kqueue(&(it->get_instance()), EVFILT_READ);
	}
}

int	KqueueEventQueue::poll_events(struct kevent *event_list, int capacity)
{
	int	n_events;	// Number of events fetched.

	if ((n_events = kevent(kqueue_fd, NULL, 0, event_list, capacity, 0)) == -1)
		throw std::runtime_error(std::string("Webserv : kevent(2) failed, reason : ") + strerror(errno));

	return n_events;
}
