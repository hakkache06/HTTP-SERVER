/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnaciri- <hnaciri-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:49:36 by hnaciri-          #+#    #+#             */
/*   Updated: 2022/09/04 19:57:11 by h0naciri-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "webserv.hpp"
# include "classes.hpp"
#include <algorithm>
#include <utility>

void	error_args()
{
	std::cout << "Error" << std::endl << "Only one argument required ****.config" << std::endl;
	exit(1);
}

void	error_port(std::string line)
{
	std::cout << "Error" << std::endl << line << " : invalid host or port" << std::endl;
	exit(1);
}

void	error_dup()
{
	std::cout << "Error" << std::endl << "Duplicated keyword" << std::endl;
	exit(1);
}


void	error_nofile(std::string s)
{
	std::cout << "Error" << std::endl << s << " : No such file or directory" << std::endl;
	exit(1);
}

void	error_extention()
{
	std::cout << "Error" << std::endl << "File extention must be .config" << std::endl;
	exit(1);
}

void	error_inparsing()
{
	std::cout << "Error" << std::endl << "Something happened while parsing" << std::endl;
	exit(1);
}

void	error_keyword_server(std::string s)
{
	std::cout << "Error" << std::endl << s << " : Uknown keyword in server" << std::endl;
	exit (1);
}

void	error_keyword_location(std::string s)
{
	std::cout << "Error" << std::endl << s << " : Uknown keyword in location" << std::endl;
	exit (1);
}

void	error_size(std::string s)
{
	std::cout << "Error" << std::endl << s << " : max_client_body_size contain only numbers" << std::endl;
	exit (1);
}

void	error_error_page()
{
	std::cout << "Error" << std::endl << "something wrong in error_page" << std::endl;
	exit (1);
}

void	error_location_prefix(std::string s)
{
	std::cout << "Error" << std::endl << s << " : invalid location prefix" << std::endl;
	exit (1);
}

void	error_allow_methods(std::string s)
{
	std::cout << "Error" << std::endl << s << " : this is forbidden method" << std::endl;
	exit (1);
}

void	error_auto_index()
{
	std::cout << "Error" << std::endl  << "auto_index accept only (on || off)" << std::endl;
	exit (1);
}

void	error_root(std::string s)
{
	std::cout << "Error" << std::endl << s << " : this root is invalid" << std::endl;
	exit (1);
}

void	error_return(std::string s)
{
	std::cout << "Error" << std::endl << s << " : this return is invalid" << std::endl;
	exit (1);
}

void	error_cgi()
{
	std::cout << "Error" << std::endl << "CGI invalid" << std::endl;
	exit (1);
}

void	error_servers_names(std::string s)
{
	std::cout << "Error" << std::endl << s << " : server name exist in two servers" << std::endl;
	exit (1);
}

void	error_no_root()
{
	std::cout << "Error" << std::endl << "we require root in location" << std::endl;
	exit (1);
}

void	error_upload_cgi()
{
	std::cout << "Error" << std::endl << "location must just one between cgi and upload_pass" << std::endl;
	exit (1);
}

void	get_port(std::string &line, server &serv)
{
	std::string::iterator	it = line.begin();
	while (it != line.end())
	{
		if (!isnumber(*it) || line.length() > 4)
			error_port(line);
		it++;
	}
	serv._port = line;
}

void	get_host(std::string host, server &serv)
{
	int	count = 0;
	std::string::iterator	it = host.begin();

	if (host == "localhost" || host == "*" || host == "127.0.0.1")
	{
		if (host == "localhost" || host == "127.0.0.1")
			serv._host = "localhost";
		return ;
	}
	while (it != host.end())
	{
		if (*it == '.')
			count++;
		if (count == 4 || (!isnumber(*it) && *it != '.'))
			error_port(host);
		it++;
	}
	if (count != 3)
		error_port(host);
	serv._host = host;
}

void	get_host_port(std::string &line, server &serv)
{
	std::string::iterator	it = line.begin();
	std::string	host;

	if (serv._host != "0.0.0.0" && serv._port != "8800")
		error_dup();
	if (line.find_first_of(':') == std::string::npos)
		get_port(line, serv);
	else if (line.find_first_of(':') != line.find_last_of(':'))
		error_port(line);
	else
	{
		while(it != line.end())
		{
			if (*it == ':')
				break ;
			host.push_back(*it);
			it++;
		}
		get_host(host, serv);
		line.erase(0, host.length() + 1);
		get_port(line, serv);
	}
}

void	get_server_names(std::string &line, server &serv)
{
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;

	while ((pos = line.find(delimiter)) != std::string::npos)
	{
		token = line.substr(0, pos);
		serv._names.push_back(token);
		line.erase(0, pos + delimiter.length());
	}
	serv._names.push_back(line);
}

void	get_limit_body_size(std::string &line, server &serv)
{
	std::string::iterator	it = line.begin();

	if (serv._limit_body_size != 1000000)
		error_dup();
	while (it != line.end())
	{
		if (!isnumber(*it))
			error_size(line);
		it++;
	}
	serv._limit_body_size = stoi(line);
}

void	get_error_page(std::string &line, server &serv)
{
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;

	if ((pos = line.find(delimiter)) == std::string::npos)
		error_error_page();
	token = line.substr(0, pos);
	line.erase(0, pos + delimiter.length());
	if (*(line.begin()) != '\'' || *(line.end() - 1) != '\'')
		error_error_page();
	line.erase(0, 1);
	line.erase(line.length() - 1, 1);
	serv._error_page.insert(std::make_pair(stoi(token), line));
}


void	get_index(std::string &line, location &loc)
{
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;

	while ((pos = line.find(delimiter)) != std::string::npos)
	{
		token = line.substr(0, pos);
		loc._index.push_back(token);
		line.erase(0, pos + delimiter.length());
	}
	loc._index.push_back(line);
}

void	get_auto_index(std::string &line, location &loc)
{
	if (line == "on")
		loc._autoindex = true;
	else if (line == "off")
		loc._autoindex = false;
	else
		error_auto_index();
}

void	get_allow_methods(std::string &line, location &loc)
{
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;

	while ((pos = line.find(delimiter)) != std::string::npos)
	{
		token = line.substr(0, pos);
		if (token != "GET" && token != "DELETE" && token != "POST")
			error_allow_methods(token);
		loc._allow_methods.push_back(token);
		line.erase(0, pos + delimiter.length());
	}
	if (line != "GET" && line != "DELETE" && line != "POST")
			error_allow_methods(line);
	loc._allow_methods.push_back(line);
}

void	get_root(std::string &line, location &loc)
{
	int	a = 0;

	for (std::string::iterator it = line.begin(); it != line.end(); it++)
	{
		if (*it == '\'')
			a++;
		if (a > 2)
			error_root(line);
	}
	if (*(line.begin()) != '\'' || *(line.end() - 1) != '\'')
		error_root(line);
	line.erase(0, 1);
	line.erase(line.length() - 1, 1);
	loc._root = line;
}

void	get_upload_pass(std::string &line, location &loc)
{
	int	a = 0;

	for (std::string::iterator it = line.begin(); it != line.end(); it++)
	{
		if (*it == '\'')
			a++;
		if (a > 2)
			error_root(line);
	}
	if (*(line.begin()) != '\'' || *(line.end() - 1) != '\'')
		error_root(line);
	line.erase(0, 1);
	line.erase(line.length() - 1, 1);
	loc._upload_pass = line;
}


void	get_return(std::string &line, location &loc)
{
	int	a = 0;

	for (std::string::iterator it = line.begin(); it != line.end(); it++)
	{
		if (*it == '\'')
			a++;
		if (a > 2)
			error_return(line);
	}
	if (*(line.begin()) != '\'' || *(line.end() - 1) != '\'')
		error_return(line);
	line.erase(0, 1);
	line.erase(line.length() - 1, 1);
	loc._return = line;
}

void	get_cgi(std::string &line, location &loc)
{
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;

	if ((pos = line.find(delimiter)) == std::string::npos)
		error_cgi();
	token = line.substr(0, pos);
	line.erase(0, pos + delimiter.length());
	if (*(line.begin()) != '\'' || *(line.end() - 1) != '\'')
		error_cgi();
	line.erase(0, 1);
	line.erase(line.length() - 1, 1);
	loc._cgi.insert(std::make_pair(token, line));
}

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \n\r\t\f\v;");
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \n\r\t\f\v;");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

std::string	get_line(std::string &file)
{
	std::string	line;
	size_t	p;

	p = file.find_first_of('\n');
	if (p == std::string::npos)
		error_inparsing();
	line = file.substr(0, p);
	file.erase(0, p + 1);
	return (line);
}

std::string	get_setting(std::string &line)
{
	std::string	setting;
	size_t	p;

	p = line.find_first_of(' ');
	if (p == std::string::npos)
		error_inparsing();
	setting = line.substr(0, p);
	line.erase(0, p);
	line = trim(line);
	return (setting);
}

location	get_location(std::string &file, std::string &line)
{
	location	loc;
	std::string	temp;

	loc._prefix = line.substr(0, line.find_first_of(' '));
	if (*loc._prefix.begin() != '/' || std::string::npos != loc._prefix.find_first_of(' '))
		error_location_prefix(loc._prefix);
	skip(line, loc._prefix);
	skip(line, "{");
	while (check_server_end(file))
		switch_cases_location(file, loc);
	if (*file.begin() != '}')
	{
		temp = file.substr(0, file.find_first_of('}')) + "\n";
		switch_cases_location(temp, loc);
		file.erase(0, file.find_first_of('}'));
	}
	skip(file, "}");
	if (loc._root.empty())
		error_no_root();
	if ((loc._upload_pass.empty() && loc._cgi.empty()) || ((!loc._upload_pass.empty()) && (!loc._cgi.empty())))
		error_upload_cgi();
	return (loc);
}

void	switch_cases_location(std::string &file, location &loc)
{
	std::string	line;
	std::string	s;

	line = get_line(file);
	s = get_setting(line);
	if (s == "allow_methods")
	 	get_allow_methods(line, loc);
	else if (s == "return")
		get_return(line, loc);
	else if (s == "root")
		get_root(line, loc);
	else if (s == "index")
		get_index(line, loc);
	else if (s == "autoindex")
		get_auto_index(line, loc);
	else if (s == "cgi_pass")
		get_cgi(line, loc);
	else if (s == "upload_pass")
		get_upload_pass(line, loc);
	else
		error_keyword_location(s);
}


void	switch_cases_server(std::string &file, server &serv)
{
	std::string	line;
	std::string	s;

	line = get_line(file);
	s = get_setting(line);
	if (s == "listen")
		get_host_port(line, serv);
	else if (s == "server_name")
		get_server_names(line, serv);
	else if (s == "max_client_body_size")
		get_limit_body_size(line, serv);
	else if (s == "error_page")
		get_error_page(line, serv);
	else if (s == "location")
		serv._locations.push_back(get_location(file, line));
	else
		error_keyword_server(s);
}
