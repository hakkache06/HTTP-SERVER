#include <iostream>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

/*

    CONTENT_TYPE =  The data type of the content , used when the client is sending attached content to the server 
    Uploadfile 
    CONTENT_LENGTH = the length of the query information . if (request == POST) *********
    HTTP_COOKIE = retuen  the set cookies in the form of key & value pair
    HTTP_USER_AGENT = Name of the web browser
    PATH_INFO = the path for cgi script
    QUERY_STRING = URL_encoded information if (method == GET)  ***********


    GATEWAY_INTERFACE =
    HTTP_ACCEPT
    HTTP_ACCEPT_ENCODING
    HTTP_ACCEPT_LANGUAGE
    HTTP_CONNECTION =
    HTTP_HOST =

    REMOTE_ADDR = the ip adress of the remote 
    REMOTE_HOST = fully   qualified
    REMOTE_PORT =

    REQUEST_METHOD = GET || POST
    REQUEST_URI = 

    SCRIPT_FILENAME = the full path to cgi script
    SCRIPT_NAME = The server's hostname or ip adress


    SERVER_ADDR =
    SERVER_ADMIN =
    SERVER_NAME = 
    SERVER_SOFTWARE = The name and version of server is runing
    SERVER_PORT = port
    SERVER_PROTOCOL = 
    SERVER_SIGNATURE = 


    HTTP_COOKIE=
    GATEWAY_INTERFACE=
    PATH_INFO=
    PATH_TRANSLATED=
    REMOTE_ADDR=
    REMOTE_HOST=
    SERVER_NAME=
    SERVER_PROTOCOL=
    SERVER_SOFTWARE=



 */

// void    php_cgi()
// {
//     pid_t   pid;
//     int     status;

//     pid = fork();
//     if (pid == 0)
//     {
//         exec_php_cgi();
//     }
//     else
//     {
//         waitpid(pid, &status, 0);
//     }
// }

//  void    exec_cmd_pipe(int *r)
//  {
//     int     fd[2];
//      pid_t   pid;
//      int     status;


//     if (pipe(fd) == -1)
//         perror("pipe");
//     if ((pid = fork()) == -1)
//          perror("fork");
//      else if (pid == 0)
//      {
//          close(fd[0]);
//          dup2(fd[1], 1);
//         close(fd[1]);
//          exec_php_cgi();
//      }
//      else
//      {
//         close(fd[1]);
//         dup2(fd[0], 0);
//          close(fd[0]);
//          exec_php_cgi();
//      }
//  }


void    exec_php_cgi(int fd) //struct client_info *client)
{
    char    *argv[3];
    char    *envp[13];

    pid_t   pid;
    int     status;
    
    argv[0] = strdup("/Users/yhakkach/Desktop/webserv/webserv/cgi-bin/php-cgi");
    argv[1] = strdup("index.php");
    argv[2] = NULL;
    envp[0] = strdup("SCRIPT_FILENAME=/Users/yhakkach/Desktop/webserv/index.php");
    envp[1] = strdup("REQUEST_METHOD=GET");
    envp[2] = strdup("SERVER_SOFTWARE=Webserver1");
    envp[3] = strdup("GATEWAY_INTERFACE=CGI/1.1");
    envp[4] = strdup("SERVER_PROTOCOL=HTTP/1.1");
    envp[5] = strdup("SERVER_PORT=3030");
    envp[6] = strdup("REDIRECT_STATUS=ture");
    // envp[7] = strdup("PATH_INFO=");
    // envp[8] = strdup("QUERY_STRING="); /// GET
    // envp[9] = strdup("CONTENT_TYPE=");
    // envp[10] = strdup("CONTENT_LENGTH="); // POST
    // envp[11] = strdup("HTTP_USER_AGENT=");
    // envp[12] = strdup("HTTP_COOKIE=");

    if ((pid = fork()) == -1)
         perror("fork");
     else if (pid == 0)
     {
        dup2(fd,1);
        dup2(fd,2);
        close(fd);
        execve(argv[0], argv, envp);
     }
    
}
 /// $ REDIRECT_STATUS=200 REQUEST_METHOD=GET SCRIPT_FILENAME=htdocs/index.php 
 //SCRIPT_NAME=/index.php PATH_INFO=/ 
 //SERVER_NAME=site.tld 
 //SERVER_PROTOCOL=HTTP/1.1
 // REQUEST_URI=/nl/page HTTP_HOST=site.tld /usr/bin/php-cgi 

int main()
{

    int fd = open("forks.html",O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    exec_php_cgi(fd);
    return 0;


}