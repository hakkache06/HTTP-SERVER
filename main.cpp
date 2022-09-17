/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnaciri- <hnaciri-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:46:08 by hnaciri-          #+#    #+#             */
/*   Updated: 2022/09/16 19:04:00 by hnaciri-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"
# include "classes.hpp"
#include <algorithm>
#include <cstddef>
#include <string>
# include <utility>

std::string::iterator	skip_white_spaces(std::string::iterator it)
{
	while (isspace(*it))
		it++;
	while (*it == '\n')
		it++;
	return (it);
}

void	skip(std::string &file, std::string str)
{
	std::string::iterator	it = file.begin();
	if (file.compare(0, str.length(), str))
	{
		std::cout << "Error!" << std::endl << "Expected | " << str << " | keyword" << std::endl;
		exit (1);
	}
	it += str.length();
	it = skip_white_spaces(it);
	file.erase(file.begin(), it);
}

void	delete_line(std::string::iterator &it, std::string &file)
{
	std::string::iterator	temp = it;

	while (*it != '\n')
		it++;
	file.erase(temp, it + 1);
	it = temp;
}

void	delete_comment(std::string &file)
{
	std::string::iterator	it = file.begin();
	
	while (it < file.end())
	{
		if (*it == '\n' && *(it - 1) == '\n')
		{
			file.erase(it - 1, it);
			it--;
		}
		if (*it == '#')
			delete_line(it, file);
		else
			it++;
	}
}

bool		check_server_end(std::string file)
{
	std::string	line;
	size_t	p;

	p = file.find_first_of('\n');
	if (p == std::string::npos)
		error_inparsing();
	line = file.substr(0, p);
	if (std::string::npos == line.find('}'))
		return (true);
	return (false);
}

server	fill_server(std::string &file)
{
	server	serv;
	std::string	temp;

	while (check_server_end(file))
		switch_cases_server(file, serv);
	if (*file.begin() != '}')
	{
		temp = file.substr(0, file.find_first_of('}')) + "\n";
		switch_cases_server(temp, serv);
		file.erase(0, file.find_first_of('}'));
	}
	return(serv);
}

std::vector<server>	parse(std::string file)
{
	std::vector<server>	config;

	delete_comment(file);
	do
	{
		skip(file, "server");
		skip(file, "{");
		config.push_back(fill_server(file));
		skip(file, "}");
	} while (!file.empty());
	return (config);
}

std::string	get_file(int ac, char **av)
{
	std::string		file_name;
	std::string		line;
	std::ifstream	file;
	std::string		all_file;

	if (ac != 2)
		error_args();
	file_name = av[1];
	if ((std::string::npos == file_name.find_last_of('.')) || file_name.substr(file_name.find_last_of('.')) != ".config")
		error_extention();
	file.open(av[1]);
	if (file.fail())
		error_nofile(av[1]);
	while (!file.eof())
	{
		std::getline (file, line);
		line = trim(line);
		all_file += line + "\n";
	}
	file.close();
	return (all_file);
}

std::map<std::string, std::string>	get_mime_types()
{
	std::fstream						file;
	std::string							line;
	std::map<std::string, std::string>	map;

	file.open("mime/mime_type.txt");
	while(!file.eof())
	{
		std::getline (file, line);
		map.insert(std::make_pair(line.substr(line.find_first_of(" ") + 1), line.substr(0, line.find_first_of(" "))));
	}
	return (map);
}

void	check_server_hosts(std::vector<server> servers, server serv)
{
	for (std::vector<server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (it->_host == serv._host && it->_port == serv._port)
		{
			for (std::vector<std::string>::iterator i = it->_names.begin(); i != it->_names.end(); i++)
			{
				for (std::vector<std::string>::iterator j = serv._names.begin(); j != serv._names.end(); j++)
				{
					if (*j == *i)
						error_servers_names(*i);
				}
			}
		}
	}
}

std::map<std::string, std::map<std::string, std::vector<server> > >	get_serv(std::vector<server> serv)
{
	std::map<std::string, std::map<std::string, std::vector<server> > >				servers;
	std::map<std::string, std::map<std::string, std::vector<server> > >::iterator	it;
	std::map<std::string, std::vector<server> >::iterator							_it;

	for (std::vector<server>::iterator i = serv.begin(); i != serv.end(); i++)
	{
		if (i->_locations.empty())
		{
			std::cout << "Server should have location" << std::endl;
			exit (1);
		}
		it = servers.find(i->_host);
		if (it == servers.end())
			servers.insert(std::make_pair(i->_host, std::map<std::string, std::vector<server> >()));
		it = servers.find(i->_host);
		_it = it->second.find(i->_port);
		if (_it == it->second.end())
			it->second.insert(std::make_pair(i->_port, std::vector<server>()));
		_it = it->second.find(i->_port);
		check_server_hosts(_it->second, *i);
		_it->second.push_back(*i);
	}
	return (servers);
}

server	find_server(std::vector<server>	servers, std::string server_name)
{
	size_t	pos;

	if ((pos = server_name.find_last_of(':')) != std::string::npos)
		server_name = server_name.substr(0, pos);
	for(std::vector<server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		for (std::vector<std::string>::iterator _it = it->_names.begin(); _it != it->_names.end(); _it++)
		{
			if (*_it == server_name)
				return (*it);
		}
	}
	return (*(servers.begin()));
}

int  create_socket(const char* host, const char *port)
{

    struct addrinfo	hints;
    int				socket_listen;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // ipv4
    hints.ai_socktype = SOCK_STREAM; // Use tcp
    hints.ai_flags = AI_PASSIVE; // for getaddrinfo() to generates an address for bind

    struct addrinfo *bind_address; 
    getaddrinfo(host, port, &hints, &bind_address);
	// Creates and initialize an new socket
    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol); 
    if (socket_listen == -1)
	{
        printf("socket Error \n");
        exit(1);
    }
    int opt = 1;
    if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	if(setsockopt(socket_listen, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt))<0)
    {
        perror("setsockopt");
		exit(EXIT_FAILURE);
    }
    // associates a socket with localport
    if (bind(socket_listen,bind_address->ai_addr, bind_address->ai_addrlen)) 
    {
        printf("Binding Error \n");
        exit(1);
    }
    freeaddrinfo(bind_address);
	// listen new connection  100000 connections it is allowed to queue up
    if (listen(socket_listen, 100000) < 0)
	{
        printf("listen() \n");
        exit(1);
    }
    return (socket_listen);
}

fd_set wait_on_clients(struct client_info **client_list, fd_set reads, int max_socket, fd_set *writes)
{
    struct client_info *ci = *client_list;

	while(ci)
	{
		if (ci->socket > max_socket)
			max_socket = ci->socket;
		ci = ci->next;
	}
    if (select(max_socket + 1, &reads, writes, 0, 0) < 0)
    {
        printf("Error select \n");
        exit(1);
    }
    return (reads); /// return the number of ready descriptors that are contained in the descriptor sets
}

/// drop client from linked list and close socket 
void drop_client(struct client_info **client_list, struct client_info *client)
{
    close(client->socket);
    struct client_info	**head = client_list;
	int					position = -1;
	int					i = 0;

    while(*head)
	{
        if (*head == client)
		{
			position = i;
            break ;
        }
		i++;
        head = &(*head)->next;
	}
	head = client_list;
    struct client_info* temp;
    struct client_info* prev;
    temp = *head;
    prev = *head;
    for (int i = 0; i < position; i++)
	{
        if (i == 0 && position == 1)
		{
            *head = (*head)->next;
            free(temp);
        }
        else
		{
            if (i == position - 1 && temp)
			{
                prev->next = temp->next;
                free(temp);
            }
            else
			{
                prev = temp;
                if (prev == NULL)
                    break;
                temp = temp->next;
            }
        }
	}
}

void send_error(struct client_info *client, int code)
{
	FILE											*fp;
	std::string										path;
	std::map<int, std::string>::iterator	_it;

	_it = client->serv._error_page.find(code);
	if (_it != client->serv._error_page.end())
		path = _it->second;
	else
		path = "./error_pages/" + std::to_string(code) + ".html";
	fp = fopen(path.c_str(), "rb");
	if (!fp)
	{
		path = "./error_pages/404.html";
		code = 404;
		fp = fopen(path.c_str(), "rb");
		if (!fp)
		{
			path = "HTTP/1.1 400\r\nConnection: close\r\nContent-Length: 11\r\nContent-Type: text/html\r\n\r\nbad request";
			send(client->socket, path.c_str(), path.length(), 0);
			return ;
		}
	}
	fseek(fp, 0L, SEEK_END);
    size_t cl = ftell(fp);
    rewind(fp);
	char buffer[BSIZE];
	std::string	s = "HTTP/1.1 " + std::to_string(code) + "\r\nConnection: close\r\nContent-Length: " + std::to_string((int)cl) + "\r\nContent-Type: text/html\r\n\r\n";
    send(client->socket, s.c_str(), s.length(), 0);
	int r = fread(buffer, 1, BSIZE, fp);
    while (r)
	{
        send(client->socket, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }
}

const char *get_client_address(struct client_info *ci)
{
	getnameinfo((struct sockaddr*)&ci->address,ci->address_length,ci->address_buffer, sizeof(ci->address_buffer), 0, 0,NI_NUMERICHOST);
    return (ci->address_buffer);
}

struct client_info *get_client(struct client_info **client_list, int s)
{
    struct client_info *ci = *client_list;

    while(ci)
	{
        if (ci->socket == s)
            break;
        ci = ci->next;
    }
    if (ci)
		return (ci);
    struct client_info *n = new client_info();
    if (!n)
	{
        printf("Out of memory.\n");
        exit(1);
    }
	n->read_all_header = false;
	n->read_all_body = false;
    n->address_length = sizeof(n->address);
    n->next = *client_list;
    *client_list = n;
    return (n);
}

void	parse_header(struct client_info **clienta, std::vector<server> servers)
{
	struct client_info	*client = *clienta;
	std::string	first_line = client->header.substr(0, client->header.find_first_of('\n') - 1);
	std::string	line, token, key;
	std::vector<std::string> values;
	std::map<std::string, std::vector<std::string> >	my_map;
	size_t		pos_1, pos_2;

	client->header = client->header.substr(client->header.find_first_of('\n') + 1);
	client->method = first_line.substr(0, first_line.find_first_of(' '));
	line = client->header;
	client->header = first_line;
	while ((pos_1 = line.find("\r\n")) != std::string::npos)
	{
		token = line.substr(0, pos_1);
		key = token.substr(0, token.find_first_of(": "));
		token.erase(0, token.find_first_of(": ") + 2);
		if (key == "Host")
			client->serv = find_server(servers, token);
		while((pos_2 = token.find(",")) != std::string::npos)
		{
			values.push_back(token.substr(0, pos_2));
			token.erase(0, pos_2 + 1);
		}
		if (!token.empty())
			values.push_back(token);
		my_map.insert(std::make_pair(key, values));
		key.clear();
		values.clear();
		line.erase(0, pos_1 + 2);
	}
	if (!line.empty())
	{
		token = line;
		key = token.substr(0, token.find_first_of(": "));
		token.erase(0, token.find_first_of(": ") + 2);
		while((pos_2 = token.find(",")) != std::string::npos)
		{
			values.push_back(token.substr(0, pos_2));
			token.erase(0, pos_2 + 1);
		}
		if (!token.empty())
			values.push_back(token);
		my_map.insert(std::make_pair(key, values));
		key.clear();
		values.clear();
		line.erase(0, pos_1 + 1);
	}
	if (!client->serv._locations.empty())
		client->serv = *(servers.begin());
	client->headers = my_map;
}

std::string     get_time_string()
{
    time_t curr_time;
	tm * curr_tm;
	struct timeval tv;
	char time_string[100];
	std::string		_time;

	time(&curr_time);
	gettimeofday(&tv, NULL);
	curr_tm = localtime(&curr_time);
	strftime(time_string, 50, "%Y_%m_%d_%H_%M_%S_", curr_tm);
	_time = std::string(time_string) + std::to_string(tv.tv_usec);
    return (_time);
}

bool	check_request_errors(struct client_info *client)
{
	{
		std::map<std::string, std::vector<std::string> >::iterator	_it = client->headers.find("Transfer-Encoding");
		if (_it != client->headers.end())
		{
			if (*(_it->second.begin()) != "chunked")
			{
				send_error(client, 501);
				return (true);
			}
		}
	}
	{
		std::map<std::string, std::vector<std::string> >::iterator	_it = client->headers.find("Transfer-Encoding");
		std::map<std::string, std::vector<std::string> >::iterator	__it = client->headers.find("Content-Length");
		if (_it == client->headers.end() && __it == client->headers.end() && client->method == "POST")
		{
			send_error(client, 400);
			return (true);
		}
	}
	{
		std::map<std::string, std::vector<std::string> >::iterator	_it = client->headers.find("Content-Length");
		if ((_it == client->headers.end() || std::stoi(*(_it->second.begin())) > client->serv._limit_body_size) && client->method == "POST")
		{
			send_error(client, 413);
			return (true);
		}
	}
	return (false);
}

void	request_handler(struct client_info **client_list, struct client_info *client, std::vector<server> servers, fd_set *save_r, fd_set *save_w)
{
	size_t	p;

	p = client->body.find("\r\n\r\n");
	if ((p != std::string::npos) && (client->read_all_header == false))
	{	
		p += 4;
		client->read_all_header = true;
		client->header = client->body.substr(0, p - 4);
		client->body = client->body.substr(p);
		parse_header(&client, servers);
		if (check_request_errors(client))
		{
			delete_client(client_list, &client, save_r, save_w);
			return ;
		}
		if (client->method != "POST")
		{
			client->read_all_body = true;
			return ;
		}
		else
		{
			client->file_name = ".tmp/" + get_time_string();
			client->file.open(client->file_name, std::ios_base::out);
		}
	}
	if (client->read_all_header == true && client->method == "POST")
	{
		client->file << client->body;
		client->body.clear();
		client->file.seekg(0, std::ios::end);
		if (std::stoi(*(client->headers.find("Content-Length")->second.begin())) == client->file.tellp())
		{
			client->read_all_body = true;
			client->file.close();
		}
	}
}

void	socket_handler(struct client_info **client_list, struct client_info **_client, std::vector<server> servers, fd_set *save_r, fd_set *save_w)
{
	struct	client_info	*client = *_client;

	size_t r = recv(client->socket, client->request + client->received, BUFFER, 0);
	client->body += std::string(client->request, r);
	std::cout << client->body << std::endl;
	if (client->read_all_body == true)
	{
		client->body.clear();
		return ;
	}
	request_handler(client_list, client, servers, save_r, save_w);
}

void	delete_client(struct client_info **client_list, struct client_info **ci, fd_set *save_r, fd_set *save_w)
{
	struct client_info	*client = *ci;
	FD_CLR(client->socket, save_r);
	FD_CLR(client->socket, save_w);
	drop_client(client_list, client);
}

location	get_location(server serv, std::string uri)
{
	std::vector<location>::iterator	it = serv._locations.begin();
	int		match = 1;

	if (serv._locations.size() == 1)
		return (*it);
	for (std::vector<location>::iterator _it = serv._locations.begin(); _it != serv._locations.end(); _it++)
	{
		size_t	i;
		int	count = 0;
		for (i = 0; i < _it->_prefix.length(); i++)
		{
			if (_it->_prefix[i] != uri[i])
				break ;
			if (_it->_prefix[i] == '/' || ((_it->_prefix[i + 1] == '\0') && (uri[i + 1 ] == '\0' || uri[i + 1] == '/')))
				count++;
		}
		if (count > match)
		{
			match = count;
			it = _it;
		}
	}
	return (*it);
}

void	post_request(struct client_info *client)
{
	std::string	uri = client->header.substr(client->header.find_first_of(" ") + 1), extention;
	std::string	protocol = client->header.substr(client->header.find_last_of(" ") + 1);
	uri = uri.substr(0, uri.find_first_of(" "));
	location	loc = get_location(client->serv, uri);

	if (std::find(loc._allow_methods.begin(), loc._allow_methods.end(), "POST") == loc._allow_methods.end())
	{
		send_error(client, 405);
		return ;
	}
	if (protocol != "HTTP/1.1")
	{
		send_error(client, 505);
		return ;
	}
	extention = "";
	std::map<std::string, std::vector<std::string> >::iterator	_it = client->headers.find("Content-Type");
	if (_it != client->headers.end())
	{
		std::map<std::string, std::string>::iterator it = client->mime_types.find(*(_it->second.begin()));
		if (it != client->mime_types.end())
			extention = it->second;
	}
	if (!loc._upload_pass.empty())
		protocol = loc._upload_pass + client->file_name.substr(4);
	else
	{
		send_error(client, 400);
		return ;
	}
	protocol += extention;
	if (std::rename(client->file_name.c_str(), protocol.c_str()))
		send_error(client, 400);
	else
		send_error(client, 201);
}

void	send_response(struct client_info **client_list, struct client_info **ci, fd_set *save_r, fd_set *save_w)
{
	struct client_info	*client = *ci;

	if (client->read_all_body == false)
		return ;
	// if (client->method == "GET")
	// 	get_request(client);
	/*else */if (client->method == "POST")
		post_request(client);
	// else if (client->method == "DELETE")
	// 	delete_request(client);
	else
		send_error(client, 405);
	delete_client(client_list, &client, save_r, save_w);
}

int		ft_lst_size(struct client_info *lst)
{
	int	total;
	total = 0;
	while (lst != NULL)
	{
		total++;
		lst = lst->next;
	}
	return (total);
}

int	main(int ac, char **av)
{
	std::string															file = get_file(ac, av);
	std::map<std::string, std::map<std::string, std::vector<server> > >	servers = get_serv(parse(file));
	std::map<std::string, std::string>									mime_types = get_mime_types();
	fd_set																reads, save_r, writes, save_w;
	int max_server = 0;
	bool			new_request;

	FD_ZERO(&reads);
	FD_ZERO(&writes);
	for (std::map<std::string, std::map<std::string, std::vector<server> > >::iterator it = servers.begin(); it != servers.end(); it++)
	{
		for (std::map<std::string, std::vector<server> >::iterator i = it->second.begin(); i != it->second.end(); i++)
		{
			int	a = create_socket(i->second.begin()->_host.c_str(), i->second.begin()->_port.c_str());
			for (std::vector<server>::iterator j = i->second.begin(); j != i->second.end(); j++)
				j->_socket.socket_listen = a;
			FD_SET(a, &reads);
			fcntl(a, F_SETFL, O_NONBLOCK);
			if (a > max_server)
				max_server = a;
		}
	}
	save_r = reads;
	struct client_info	*client_list = 0;
	while (1)
	{
		new_request = true;
		reads = save_r;
		writes = save_w;
		reads = wait_on_clients(&client_list, reads, max_server, &writes);
		for (std::map<std::string, std::map<std::string, std::vector<server> > >::iterator it = servers.begin(); it != servers.end(); it++)
		{
			for (std::map<std::string, std::vector<server> >::iterator i = it->second.begin(); i != it->second.end(); i++)
			{
				if (FD_ISSET(i->second.begin()->_socket.socket_listen, &reads))
				{
					struct client_info *client = get_client(&client_list, -1);
					client->socket = accept(i->second.begin()->_socket.socket_listen,(struct sockaddr*) &(client->address), &(client->address_length));
					if (!client->socket)
					{
						printf("accept() failed.");
						exit(1);
					}
					client->servers = i->second;
					client->mime_types = mime_types;
					FD_SET(client->socket, &save_r);
					FD_SET(client->socket, &save_w);
					fcntl(client->socket, F_SETFL, O_NONBLOCK);
				}
			}
		}
		struct client_info *ci = client_list;
		while(ci)
		{
			if (FD_ISSET(ci->socket, &reads))
			{
				socket_handler(&client_list, &ci, ci->servers, &save_r, &save_w);
				new_request = false;
				break ;
			}
			else if (FD_ISSET(ci->socket, &writes))
			{
				send_response(&client_list, &ci, &save_r, &save_w);
				new_request = false;
				break ;
			}
			//sleep (1);
			ci = ci->next;
		}
	}
}
