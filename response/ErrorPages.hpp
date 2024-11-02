/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPages.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 15:21:52 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/02 10:08:09 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ERRORPAGES_HPP
#define ERRORPAGES_HPP

# include "../includes/main.hpp"

typedef struct s_error_page {
    int         error_num ;
    std::string error_name ;
    std::string error_html ;

} error_page ;




class ErrorPages
{
    private:
        // std::map< short, std::string >   error_pages ;
        std::map<short , std::string> error_pages ;

        void init_pages ( void ) ;

    public:

        // const error_page & getErrorPage( short error ) ;
        const std::string & getErrorMsg( unsigned short error ) ;
        ErrorPages();
        ~ErrorPages();
        std::string  getErrorPage( const std::string & error_message );
        
};





#endif