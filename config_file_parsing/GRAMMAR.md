<Config>    ::= <Context>*

<Context>   ::= <HTTP> | <Server> | <Location>

<HTTP>      ::= 'http {' <Directive>* <Server>* '}'

<Server>    ::= 'server {' <Directive>* <Location>* '}'

<Location>  ::= 'location' '='? <PATH> '{' <Directive>* '}'

<PATH>      ::= '/' STR;

<Directive> ::= 'root'
                    | 'autoindex'
                    | 'cgi_extention'
                    | 'error_page'
                    | 'upload_dir'
                    | 'index'
                    | 'listen'
                    | 'server_names'
                    | 'allowed_methods'
                    | 'return'

# Using the '=' in the location uri will help optimizing the search of the appropriate block for the requested uri
# Location will be spliten to two categories : one's having an exact URI (using '=') and others having prefix match
# When searching for a location for a requested URI, first the exact ones are going to be cheked, if none found, then the 
# search will move to prefix matching.