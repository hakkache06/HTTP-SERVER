/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   classes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnaciri- <hnaciri-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 16:36:13 by hnaciri-          #+#    #+#             */
/*   Updated: 2022/09/13 17:13:17 by hnaciri-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLASSES_HPP
# define CLASSES_HPP

# include <iostream>
# include <string>
# include <vector>
# include <fstream>
# include <map>
# include "webserv.hpp"

class socketa
{
public:
	int		socket_listen;
	socketa();
	~socketa();
};

class server
{
public:
	std::string					_host;
	std::string					_port;
	std::vector<std::string>	_names;
	int							_limit_body_size;
	std::map<int, std::string>	_error_page;
	std::vector<location>		_locations;
	std::string					_default_root;
	socketa						_socket;
	fd_set						_reads;
	struct client_info			*client_list;
	server();
	~server();
};

struct client_info
{
    socklen_t											address_length; // the adresse length
    struct sockaddr_storage								address;  //  client address stored here
    char												address_buffer[2000];
    int													socket;
    char												request[BUFFER + 1];  // all of data receivd from the client stored here
    int													received; //the number of bytes stored int array (requst)
	bool												read_all_header;
	bool												read_all_body;
	std::string											header;
	std::map<std::string, std::vector<std::string> >	headers;
	std::string											method;
	server												serv;
	std::vector<server>									servers;			
	std::string											body;
    struct client_info									*next;
};

class location
{
public:
	std::string							_prefix;
	std::vector<std::string>			_allow_methods;
	std::string							_return;
	std::string							_root;
	std::vector<std::string>			_index;
	bool								_autoindex;
	std::map<std::string, std::string>	_cgi;
	std::string							_upload_pass;
	location();
	~location();
};

# endif
