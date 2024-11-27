/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 16:53:06 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/26 04:05:21 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

void Response::delete_file()
{
    struct stat f ;

    if ( stat( this->_path_.c_str(), &f ) == -1)
        throw 404 ;

    int status = remove(this->_path_.c_str()) ;
    if ( status == -1 )
        throw 404 ;
    throw 204 ;
}