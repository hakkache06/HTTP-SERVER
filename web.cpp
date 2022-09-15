/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   web.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hnaciri- <hnaciri-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/02 04:52:34 by yhakkach          #+#    #+#             */
/*   Updated: 2022/09/13 10:24:36 by hnaciri-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <iomanip>

///// Content Type MIME Registerd with  (IANA)
const char *get_content_type(const char* path) {
    const char *last_dot = strrchr(path, '.');
    if (last_dot) {
        if (strcmp(last_dot, ".css") == 0) return "text/css";
        if (strcmp(last_dot, ".csv") == 0) return "text/csv";
        if (strcmp(last_dot, ".gif") == 0) return "image/gif";
        if (strcmp(last_dot, ".htm") == 0) return "text/html";
        if (strcmp(last_dot, ".html") == 0) return "text/html";
        if (strcmp(last_dot, ".ico") == 0) return "image/x-icon";
        if (strcmp(last_dot, ".jpeg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".jpg") == 0) return "image/jpeg";
        if (strcmp(last_dot, ".js") == 0) return "application/javascript";
        if (strcmp(last_dot, ".json") == 0) return "application/json";
        if (strcmp(last_dot, ".png") == 0) return "image/png";
        if (strcmp(last_dot, ".pdf") == 0) return "application/pdf";
        if (strcmp(last_dot, ".svg") == 0) return "image/svg+xml";
        if (strcmp(last_dot, ".txt") == 0) return "text/plain";
    }
    return "application/octet-stream";
}
///////  
 /// create bind and listen 
int  create_socket(const char* host, const char *port) {

    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // ipv4
    hints.ai_socktype = SOCK_STREAM; // Use tcp
    hints.ai_flags = AI_PASSIVE; // for getaddrinfo() to generates an address for bind

    struct addrinfo *bind_address; 
    getaddrinfo(host, port, &hints, &bind_address);

    printf("Creating socket...\n");
    int socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol); // Creates and initialize an new socket
    if (socket_listen == -1) {
        printf("socket Error \n");
        exit(1);
    }
    printf("Binding socket to local address...\n");   // associates a socket with localport
    int opt = 1;
    if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0) {perror("setsockopt");exit(EXIT_FAILURE);}if(setsockopt(socket_listen, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt))<0)
    {
           perror("setsockopt");exit(EXIT_FAILURE);
    }
    if (bind(socket_listen,bind_address->ai_addr, bind_address->ai_addrlen)) 
    {
        printf("Binding Error \n");
        exit(1);
    }
    freeaddrinfo(bind_address);
    printf("Listening...\n");
    if (listen(socket_listen, 100000) < 0) { // listen new connection  10 connections it is allowed to queue up
        printf("listen() \n");
        exit(1);
    }
    return socket_listen;
}

////////////// is useful to define struct to store information on each connected client  // /////////////////

struct client_info {
    socklen_t address_length; // the adresse length
    struct sockaddr_storage address;  //  client address stored here
    char address_buffer[128];
    int socket;
    char request[MAX_REQUEST_SIZE + 1];  // all of data receivd from the client stored here
    int received; //the number of bytes stored int array (requst)
    struct client_info *next; 
};
///////////////////////////////////


///  get_client () take socket varibale en searches our linked list  
//     serves two purposes—it can find an existing client_info, or it can create a new client_info.
// takes a socketa as its input and return a client_info structure
struct client_info *get_client(struct client_info **client_list, int  s) {
    struct client_info *ci = *client_list;
    while(ci) {
        if (ci->socket == s)
            break;
        ci = ci->next;
    }
    if (ci) return ci;
    struct client_info *n = (struct client_info*) calloc(1, sizeof(struct client_info));
    if (!n) {
        printf("Out of memory.\n");
        exit(1);
    }
    n->address_length = sizeof(n->address);
    n->next = *client_list;
    *client_list = n;
    return n;
}


/// drop client from linked list and close socket 
void drop_client(struct client_info **client_list,
        struct client_info *client) {
    close(client->socket);
    struct client_info **p = client_list;
    while(*p) {
        if (*p == client) {
            *p = client->next;
            free(client);
            return;
        }
        p = &(*p)->next;
    }
    printf("drop_client not found.\n");
    exit(1);
}

/// uses the select() function to wait until either a client has data available or a new client is attempting to connect.
/// server must have a way to wait for data from multiple clients at once
/// select () preferred technique for multiplexing , give select set of socket and it tells ys wich ones are ready
fd_set wait_on_clients(struct client_info **client_list, int server) {

    fd_set reads; // our sockets;
    FD_ZERO(&reads); /// initializes a descriptor set fdset to the null set
    FD_SET(server, &reads); /// includes a particular descriptor fd in fdset.
    int max_socket = server;
    
    struct client_info *ci = *client_list;
    
    while(ci)
    {
        FD_SET(ci->socket, &reads); /// includes a particular descriptor fd in fdset.
        if (ci->socket > max_socket)
            max_socket = ci->socket;
        ci = ci->next;
    }
    if (select(max_socket+1, &reads, 0, 0, 0) < 0)
    {
        printf("Error select \n");
        exit(1);
    }
    return reads; /// return the number of ready descriptors that are contained in the descriptor sets
}




const char *get_client_address(struct client_info *ci) {
    //It first allocates a char array to store the IP address in. This char array is declared static, which ensures that its memory //is available after the function returns
    //static char address_buffer[100];
getnameinfo((struct sockaddr*)&ci->address,ci->address_length,ci->address_buffer, sizeof(ci->address_buffer), 0, 0,NI_NUMERICHOST);
    return ci->address_buffer;
}

void send_400(struct client_info **client_list,
        struct client_info *client) {
    const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
        "Connection: close\r\n"
        "Content-Length: 11\r\n\r\nBad Request";
    send(client->socket, c400, strlen(c400), 0);
    drop_client(client_list, client);
}

void send_404(struct client_info **client_list,
        struct client_info *client) {
    const char *c404 = "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Length: 9\r\n\r\nPage Not Found";
    send(client->socket, c404, strlen(c404), 0);
    drop_client(client_list, client);
}


///// serve_resource() function takes as arguments a connected client and a requested resource path.
void serve_resource(struct client_info **client_list, struct client_info *client, const char *path) {

    printf("serve_resource %s %s\n", get_client_address(client), path);

    if (strcmp(path, "/") == 0) path = "/cpp.html";

    if (strlen(path) > 100) {
        send_400(client_list, client);
        return;
    }
    if (strstr(path, "..")) {
        send_404(client_list, client);
        return;
    }
    char full_path[128];
    sprintf(full_path, "public%s", path);


    FILE *fp = fopen(full_path, "rb");

    if (!fp) {
        send_404(client_list, client);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    size_t cl = ftell(fp);
    rewind(fp);

    const char *ct = get_content_type(full_path);

#define BSIZE 1024
    char buffer[BSIZE];

    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(client->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: close\r\n");
    send(client->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %zu\r\n", cl);
    send(client->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Type: %s\r\n", ct);
    send(client->socket, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send(client->socket, buffer, strlen(buffer), 0);
    int r = fread(buffer, 1, BSIZE, fp);
    while (r) {
        send(client->socket, buffer, r, 0);
        r = fread(buffer, 1, BSIZE, fp);
    }
    fclose(fp);
    drop_client(client_list, client);
}

int main() {

    int server = create_socket("localhost", "8080");

    std::cout << "Server : " << server << std::endl;

    struct client_info *client_list = 0;

    while(1) {

        fd_set reads = wait_on_clients(&client_list, server);

        if (FD_ISSET(server, &reads))
        {
            struct client_info *client = get_client(&client_list, -1);

            client->socket = accept(server,(struct sockaddr*) &(client->address),&(client->address_length)); ///  accept()  server create a new socket for incoming tcp connection.

            if (!client->socket)
            {
                printf("accept() failed.");
                exit(1);
            }
            printf("New connection from %s.\n", get_client_address(client));
        }
        struct client_info *client = client_list;
        while(client) {
            struct client_info *next = client->next;

            if (FD_ISSET(client->socket, &reads)) {

                if (MAX_REQUEST_SIZE == client->received) {
                    send_400(&client_list, client);
                    client = next;
                    continue;
                }
                errno = 0;
                int r = recv(client->socket,client->request + client->received,MAX_REQUEST_SIZE - client->received, 0);
                // int r = read(client->socket, client->request + client->received, MAX_REQUEST_SIZE - client->received);
                perror("eroo");
                printf("=>>>>%d\n",r);
                std::string s(client->request, r);
                std::cout << "acctual"  << s.size() << std::endl;
                size_t a = s.find("Content-Length: ");
                size_t i;
                for(i = a; i <= a + 50; i++)
                    if (s[a] == '\n')
                        break ;
                int len = stoi(s.substr(a + strlen("Content-Length: "), i));
                // std::cout << len << std::endl;
                std::ofstream file;
                file.open("blabla.png");
                for (std::string::iterator it = s.begin() + s.find("\r\n\r\n") + 4; len; len--)
                    file << *it++;
                file.close();
                if (r < 1) {
                    printf("Unexpected disconnect from %s.\n",
                            get_client_address(client));
                    drop_client(&client_list, client);

                } else {
                    client->received += r;
                    client->request[client->received] = 0;

                    char *q = strstr(client->request, "\r\n\r\n");
                    if (q) {
                        *q = 0;

                        if (strncmp("GET /", client->request, 5)) {
                            send_400(&client_list, client);
                        } else {
                            char *path = client->request + 4;
                            char *end_path = strstr(path, " ");
                            if (!end_path) {
                                send_400(&client_list, client);
                            } else {
                                *end_path = 0;
                                serve_resource(&client_list, client, path);
                            }
                        }
                    } //if (q)
                }
            }
            client = next;
        }

    } //while(1)


    printf("\nClosing socket...\n");
    close(server);


    printf("Finished.\n");
    return 0;
}
