/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 16:53:06 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/13 03:15:20 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"


void    Response::post_data()
{
    // size_t i ;
    // std::string delimiter ;
    // std::string content_desc ;
    // std::string content_type ;

    // std::cout << "//----\n" << this->request.get_request_body() << "----//" << std::endl;
    // // Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryJBhO3luKAdVyS4OY
    // std::string header_value = "multipart/form-data; boundary=----WebKitFormBoundaryJBhO3luKAdVyS4OY";

    // if ( header_value.find('multipart/form-data;') != std::string::npos )
    // {
    //     i = header_value.find('boundary=') ;
    //     if ( i == std::string::npos )
    //         throw 400 ;
    //     size_t pos = 0 ;
        
    //     delimiter = header_value.substr(i + 9) ;
    //     while (true)
    //     {
    //         pos = this->request.get_request_body().find( delimiter, pos ) ;
    //         if ( pos == std::string::npos )
    //             break ;
                
    //         content_desc = this->request.get_request_body().substr( ); 
            
    //     }
    // }

    this->_running_post = false ; // when postting file is done set this->_running_post = false
    if ( this->_running_post == false )
    {
        this->read_and_format_msg();
    }
}

void Response::delete_file( )
{
    std::cout << "delete " << this->_path_ << std::endl;
    struct stat f ;

    if ( stat( this->_path_.c_str(), &f ) == -1)
        throw 404 ;

    int status = remove(this->_path_.c_str()) ;
    if ( status == -1 )
        throw 404 ;
    throw 204 ;

    // this->_end_of_response = true ;

}



