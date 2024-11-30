/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultInfo.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 15:21:52 by klamqari          #+#    #+#             */
/*   Updated: 2024/11/30 11:32:15 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef DEFAULTINFO_HPP
#define DEFAULTINFO_HPP

# include "../includes/main.hpp"


class DefaultInfo
{
    private:
        // std::map< short, std::string >   error_pages ;
        std::map<short , std::string> error_pages ;

        void init_pages ( void ) ;

    public:

        // const error_page & getDefaultPage( short error ) ;
        std::string getCodeMsg( unsigned short error ) ;
        DefaultInfo();
        ~DefaultInfo();
        std::string  getDefaultPage( const std::string & error_message );
        
};

#endif