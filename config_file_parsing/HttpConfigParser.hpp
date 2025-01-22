#ifndef HTTP_CONFIG_PARSER_HPP
#define HTTP_CONFIG_PARSER_HPP

#include <queue>
#include <sstream>
#include <fstream>

#include "config_structs.hpp"
#include "../Contexts/HttpContext.hpp"
#include "../Utils/utils.hpp"

///////////////////////

// Config directives :

// Http context Specefic
#define	SERVER_DIR			"server"
#define	MAX_BODY_DIR		"client_max_body_size"

#define	CGI_EXCT_DIR		"cgi_extention"
#define	AUTO_INDX_DIR		"autoindex"
#define	ERR_PAGE_DIR		"error_page"
#define	CGI_RD_TMOUT_DIR	"cgi_read_timeout"

// Server context Specefic
#define	LOCATION_DIR		"location"
#define	LISTEN_DIR			"listen"
#define	SERVER_NAMES_DIR	"server_names"
#define	HOST_DIR			"host"
#define	UPLOAD_DIR			"upload_directory"

#define	ALLOWED_METHODS_DIR	"allowed_methods"
#define	INDEX_DIR			"index"
#define	ROOT_DIR			"root"

// Location context Specefic
#define REDIRECTION_DIR		"return"
#define	ALIAS_DIR			"alias"

/////////////////////

class ConfigValueExtractor {

	public :

		/* Constructors */
		ConfigValueExtractor(std::queue<token_info> & tokens_queue);

		/* Asserts the queue is not empty and Retruns the front token in the queue. */
		token_info &					front_token(const token_info & directive);

		/* Value Extractors */
		std::string						extract_single_string_value(void (ConfigValueExtractor::*validator)(const token_info &));
		std::vector<std::string>		extract_multi_string_value(void (ConfigValueExtractor::*validator)(const token_info &));
		std::vector<unsigned short>		extract_port_nums();
		size_t							extract_max_body_size();
		t_error_page					extract_error_page_info();
		std::string						extract_location();
		t_redirection_info				extract_redirection_info();
		std::pair<extension, execPath>	extract_cgi_info();
		time_t							extract_time_value();

		/* Value Validators */
		void						validate_directive_ending(const token_info & token, const token_info & directive);
		void						validate_port_number(const token_info & token);
		void						validate_max_body_size(const token_info & token);
		void						validate_auto_indx_value(const token_info & token);
		void						validate_http_code_value(const token_info & token);
		void						validate_method(const token_info & token);
		void						validate_redirection_code(const token_info & token);
		void						validate_time_value(const token_info & token);

	private :

		std::queue<token_info> & tokens_queue;

};


class ConfigParser
{
	public :

		static HttpContext	getConfig(const std::string & file_name); // this function will wrapp all the parsing process
		static void			validate_tokens_queue(const std::queue<token_info> & tokens_queue);
		void				storeConfig(const std::string & context);

		void				validate_context_closure(const std::string & context);
		void				validate_config();

		void				storeHttpDirs();
		void				storeServDirs(bool & location_dir_found);	// Tracks if a "location" directive is found to restrict other directives afterward.
		void				storelocationDirs();

		void				find_bad_token_type(token_info & token);
		bool				is_duplicated_http_dir(const std::string & directive);
		bool				is_duplicated_serv_dir(const std::string & directive, const ServerContext & serv);
		bool				is_duplicate_location_dir(const std::string & directive, const LocationContext & location);
		void				setup_new_location();
		void				setup_new_server();

	private :

		// Constructor :
		ConfigParser(std::queue<token_info> & tokens);

		std::queue<token_info>	&	tokens_queue;
		HttpContext					http_config;	// This represents the output of the parsing process
		ConfigValueExtractor		extractor;
};


class ConfigTokenizer {

	public :

		static std::queue<token_info>   tokenize(const std::string & file_name);

    private :

		ConfigTokenizer();

		~ConfigTokenizer();

        void    	open_config_file(std::string file_name);
		std::queue<token_info>		start_tokenization();
		void		process_line_tokens();
		void		extract_quoted_token();
		void		append_token_to_queue();
		std::string	get_quoted_string();
		std::string	capture_chars_after_quote();
		std::string	get_next_chunk();
		token_info	normalize_token(std::string & token);

		/* --------------------- Utils --------------------- */

		void	remove_leading_spaces();
		char	leading_quote_or_wspace(const std::string & line);

        /* ------------------------------------------------------------------------------ */

		std::queue<token_info>	tokens_queue;
        std::ifstream			file;
		std::string				file_name;

		std::string				line;			// Stores the current line from the confing file being parsed
		std::string				token;
        unsigned int			line_num;

		std::stringstream		streamed_line;

		char					quote;
		bool					pair_quote_found;
};

class ConfigException {

	public :

		static void	throwParsingError(const ConfigParseError & e, const token_info & token);
		static void	throwWrongValueError(const std::string & directive, const token_info & token);
		/*	*/

		static std::string  file_name;

};


bool                                    is_http_ctx_dir(const std::string & directive);
bool                                    is_server_ctx_dir(const std::string & directive);
bool                                    is_location_ctx_dir(const std::string & directive);
bool                                    is_a_valid_dir(const std::string & directive);



#endif