/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   KqueueWrapper.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 11:33:49 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/12 11:28:06 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int create_kqueue( void )
{ 
    int fd; 
    if ((fd = kqueue()) == -1)
        throw std::runtime_error(std::string("Webserv : kqueue failed, reason : ") + strerror(errno)); // TODO : close all sockets on error
    return fd;
}

void    register_socket_in_kqueue(int kqueue_fd, Socket * sock_data, short filter)
{
    struct kevent ev;

    ft_memset(&ev, 0, sizeof(struct kevent));
    EV_SET(&ev, sock_data->get_sock_fd(), filter, EV_ADD | EV_ENABLE, 0, 0, (void *) sock_data);

    if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
        throw std::runtime_error(std::string("Webserv : kevent(4) failed,  reason : ") + strerror(errno));
}

void    register_listeners_in_kqueue(int kqueue_fd, std::vector<struct ListenerSocket> & activeListners)
{
    std::vector<struct ListenerSocket>::iterator it = activeListners.begin();
    std::vector<struct ListenerSocket>::iterator end = activeListners.end();

    for ( ; it != end; it++)
        register_socket_in_kqueue(kqueue_fd, &it->get_instance(), EVFILT_READ);
}

void    switch_interest(ClientSocket* client_info, int kqueue_fd, short old_filter, short new_filter)
{
    struct kevent   ev;

    /* Deleting old event, event filter might be either EVFILT_READ or EVFILT_WRITE */
    ft_memset(&ev, 0, sizeof(struct kevent));
    EV_SET(&ev, client_info->get_sock_fd(), old_filter, EV_DELETE, 0, 0, NULL);
    if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
        throw std::runtime_error("kevent(3) failed!");

    /* Setting the new event we are interested in */
    ft_memset(&ev, 0, sizeof(struct kevent));
    EV_SET(&ev, client_info->get_sock_fd(), new_filter, EV_ADD | EV_ENABLE, 0, 0, (void *) client_info);
    if (kevent(kqueue_fd, &ev, 1, NULL, 0, NULL) == -1)
        throw std::runtime_error("kevent(2) failed!");
}
