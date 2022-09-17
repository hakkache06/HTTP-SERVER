/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   classes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnaciri- <hnaciri-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/02 12:36:53 by hnaciri-          #+#    #+#             */
/*   Updated: 2022/09/16 17:21:27 by hnaciri-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "classes.hpp"

socketa::socketa(){}

socketa::~socketa(){}

location::location(): _autoindex(true){}

location::~location(){}

server::server(): _host("0.0.0.0"), _port("8080"), _limit_body_size(1000000), _default_root(getcwd(0,0)){}

server::~server(){}
