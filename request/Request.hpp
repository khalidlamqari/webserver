/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klamqari <klamqari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 17:07:06 by ymafaman          #+#    #+#             */
/*   Updated: 2024/12/21 11:52:17 by klamqari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>

typedef struct  s_part {

    std::string     file_name;
    std::string     file_content;
    std::ofstream   file;
    std::string     content_type;
	bool		    header_parsed;
	bool		    is_complete;
    bool            is_new;

    std::string unparsed_bytes;

    s_part() {};
    s_part( const s_part & other )
    {
        std::cout << "Copy called!" <<std::endl;
        this->file_name = other.file_name;
        this->content_type = other.content_type;
        this->header_parsed = other.header_parsed;
        this->is_complete = other.is_complete;
        this->is_new = other.is_new;
    }
} t_part;

class Request {

    public :

        size_t                              total_chunks_length;
        


        bool                    first_chunk_fixed;


        /* Constructors */
        Request( void );

        /* Destructor */
        ~Request();

        /* Getters */
		t_part &	get_latest_part();
        size_t      get_total_chunks_length();
        std::string get_target();
        std::string get_query();
        std::string get_method();
        std::string get_body();
        size_t      getContentLength();
        std::string get_boundary();

        // struct ClientSocket * get_ClientSocket() { return this->clientsocket ;}
        // void set_ClientSocket(struct ClientSocket * clientsocket) { this->clientsocket = clientsocket ;}
        const std::map<std::string, std::string> & get_headers() const ;

        bool    hasParsedStartLine();
        bool    hasParsedHeaders();
        bool    hasParsedBody();
        bool    hasIncompletePart();
        bool    hostIsSet();
        bool    ContentLengthIsSet();
        bool    isBadRequest();
        bool    isReady();
        bool    isChunked();
        bool    isPersistent();
        bool    isMultipart();
        std::string getUnparsedMsg();
        std::string get_version();

        bool    hasReachedFirstPart( void );
        bool    hasReachedLastPart( void );
        size_t   hasAnUndoneChunk( void );

        /* Setters */
        void      set_total_chunks_length(size_t total_length);
		void	set_new_part( t_part & new_part );

        void    set_method( const std::string & method );
        void    set_version( const std::string & version );
        void    set_target( const std::string & target );
        void    set_query( std::string query );
        void    set_body( const std::string & body );
		void	set_content_length( const size_t & length );
        void    set_boundary( const std::string & boundary );
        void    setHasIncompletePart( bool & incomplete );
        void    markStartLineParsed( const bool & parsed ); // TODO: remove argument ?
        void    markHeadersParsed( const bool & parsed );
        void    markBodyParsed( const bool & parsed );
        void    markAsReady( const bool & ready );
        void    markAsBad( int i );
        void    markAsChunked();
        void    markAsPersistent( const bool& persist);
        void    markContentLengthAsSet();
        void    markAsMultipart();
        void    markHostAsSet();
        void    markFirstPartAsReached();
        void    markLastPartAsReached();
        void    set_parsingErrorCode( short code );
        void    storeUnparsedMsg(const std::string & msg );
        void    markAsHasUndoneChunk( bool undone, size_t size_left );

        /* Methods */
        void    resetUnparsedMsg();
        void    setHeader( const std::string& name, const std::string& value );

        void    print_headrs();
        void    print_files();

        void    drop_last_part();

        std::string    build_boundary(int type);

        
    private : 

        std::map<std::string, std::string>  headers;
        std::string                         method;
        std::string                         version;
        std::string                         target;
        std::string                         body;
        std::string                         query;
        std::string                         content_type;

        bool                                start_line_is_parsed;
        bool                                headers_parsed;
        bool                                body_is_parsed;
        bool                                content_length_is_set;
        bool                                is_ready;

        bool                                is_chunked;
        bool                                is_persistent;
        bool                                is_multipart;
        bool                                host_is_set;

        /*  */
        bool                                is_bad;
        short                               parsingErrorCode;

        /* */
        std::string                         unparsed_msg;
        size_t		                        content_length;

        std::string                         boundary;

        // Content parsing :
        std::vector <t_part>    parts;     			// This vector stores the parts of the body, only parts with the type of file are stored.
        std::string				content;            // Where normal content will be stored, this content is gonna be passed to the cgi program.
        bool					first_part_reached; // In case of a multipart body.
        bool					last_part_reached;
        bool					has_incomplete_part;
        bool                    undone_chunk;
        size_t                  size_left;
};



#endif