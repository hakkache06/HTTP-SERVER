/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnaciri- <hnaciri-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/27 16:11:16 by hnaciri-          #+#    #+#             */
/*   Updated: 2022/09/13 11:00:41 by hnaciri-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

# include <iostream>
# include <string>
# include <vector>
# include <fstream>
# include <map>
# include <fcntl.h>
# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <errno.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>

# define BUFFER 1024
# define BSIZE 1024

class 	server;
class	location;

void		error_args();
void		error_nofile(std::string s);
void		error_extention();
void	    error_dup();
void    	error_nofile(std::string s);
void	    error_extention();
void	    error_inparsing();
void	    error_keyword_server(std::string s);
void	    error_servers_names(std::string s);
std::string	get_file(int ac, char **av);
std::string	get_line(std::string	&file);
void		switch_cases_server(std::string &file, server &serv);
void	    switch_cases_location(std::string &file, location &loc);
std::string trim(const std::string &s);
bool		check_server_end(std::string file);
void    	skip(std::string &file, std::string str);

#endif
