
#include "utils.hpp"

bool is_all_digits(const std::string& str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

bool    is_space(char c)
{
    return (c == 32 || (c >= 9 && c <= 13));
}

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned int	i;

	i = 0;
	while (i < len)
	{
		*((unsigned char *)(b) + i) = (unsigned char) c;
		i++;
	}
	return (b);
}

void    err_throw( const char * message )
{
    std::string err_msg;
    
    err_msg += "Webserv : ";
    err_msg += message;

    throw err_msg;
}


bool is_all_WS( const std::string & str )
{
    for ( size_t i = 0; i < str.length() ; i++ )
        if ( ! std::iswspace(str[i]) )
            return ( false ) ;
    return ( true ) ;
}

long    my_stoul(const std::string & str)
{
    std::stringstream   strm(str);
    long                value;

    strm >> value;

    return value;
}

int		my_stoi(const std::string & str)
{
    std::stringstream   strm(str);
    int                 value;

    strm >> value;

    return value;
}