/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 16:53:06 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/25 10:01:55 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"


void    Response::post_data()
{
    this->_running_post = false ; // when postting file is done set this->_running_post = false
    if ( this->_running_post == false )
    {
        this->read_and_format_msg();
    }
}

void Response::delete_file( )
{
    // std::cout << "delete " << this->_path_ << std::endl;
    struct stat f ;

    if ( stat( this->_path_.c_str(), &f ) == -1)
        throw 404 ;

    int status = remove(this->_path_.c_str()) ;
    if ( status == -1 )
        throw 404 ;
    throw 204 ;
}