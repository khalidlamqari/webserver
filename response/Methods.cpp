/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 16:53:06 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/12 01:53:24 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"


void    Response::post_data()
{
    // size_t i;
    // std::string delimiter ;
    std::cout << "//----\n" << this->request.get_request_body() << "----//" << std::endl;
    // Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryJBhO3luKAdVyS4OY
    // std::string header_value = "multipart/form-data; boundary=----WebKitFormBoundaryJBhO3luKAdVyS4OY";
    
    // if ( header_value.find('multipart/form-data;') != std::string::npos )
    // {
    //     i = header_value.find('boundary=');
    //     if ( i == std::string::npos )
    //         throw 400 ;
    //     delimiter = header_value.substr(i + 9) ;
        
        
    // }
    
    
    
}

void Response::delete_file(const std::string & path )
{
    std::cout << "delete " << path << std::endl;
    struct stat f ;

    if ( stat( path.c_str(), &f ) == -1)
        throw 404 ;

    int status = remove(path.c_str()) ;
    if ( status == -1 )
        throw 404 ;
    throw 204 ;

    // this->_end_of_response = true ;

}



