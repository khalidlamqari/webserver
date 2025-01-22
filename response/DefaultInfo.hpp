#pragma once
#ifndef DEFAULTINFO_HPP
#define DEFAULTINFO_HPP

# include "includes/main.hpp"

class DefaultInfo
{
    private:
        std::map<short , std::string> error_pages ;
        void init_pages ( void ) ;

    public:

        std::string getCodeMsg( unsigned short error ) ;
        DefaultInfo();
        ~DefaultInfo();
        std::string  getDefaultPage( const std::string & error_message );
        
        
};

static DefaultInfo default_info;

#endif