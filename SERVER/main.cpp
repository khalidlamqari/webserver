/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 09:49:46 by klamqari          #+#    #+#             */
/*   Updated: 2024/10/12 18:47:48 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

int main() {
    Server serv ;
    serv.run() ;
    serv.closeServer();
}