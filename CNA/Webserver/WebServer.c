/* http_server.c - http 1.0 server  */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <fcntl.h>
#include "config.h"
#include "helpers.h"

/*------------------------------------------------------------------------
 * Program:   http server
 *
 * Purpose:   allocate a socket and then repeatedly execute the following:
 *              (1) wait for the next connection from a client
 *              (2) read http request, reply to http request
 *              (3) close the connection
 *              (4) go back to step (1)
 *
 * Syntax:    http_server [ port ]
 *
 *               port  - protocol port number to use
 *
 * Note:      The port argument is optional.  If no port is specified,
 *            the server uses the port specified in config.h
 *
 *------------------------------------------------------------------------
 */




int main(int argc, char *argv[])
{
    struct  sockaddr_in serv_addr; /* structure to hold server's address  */
    int     listen_socket, connection_socket;
    int c;
    int     port;
    pid_t   pid;  /* id of child process to handle request */
    char    response_buffer[MAX_HTTP_RESP_SIZE];      /*a variable to hold the response that your
                                                      server will send back to the client*/
    int     status_code;            // will be used to hold the HTTP response code and phase that
    char *  status_phrase;          // should be sent back to the client


    /* 1) Create a socket */
    listen_socket = socket(AF_INET,SOCK_STREAM,0);
    if(listen_socket==-1)
        perror("Could not create socket");

    /* Check command-line argument for port and extract    */
    /* port number if one is specified.  Otherwise, use default  */

    if (argc > 1) {                 /* if argument specified        */
        port = atoi(argv[1]);   /* convert from string to integer   */
    } else {
        port = DEFAULT_PORT;
    }
    if (port <= 0) {             /* test for legal value       */
        fprintf(stderr, "bad port number %d", port);
        exit(EXIT_FAILURE);
    }

    /* 2) Set the values for the server  address structure:  serv_addr */
    memset(&serv_addr,0,sizeof(serv_addr)); /* clear sockaddr structure */

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    /* 3) Bind the socket to the address information set in serv_addr */
    if (bind(listen_socket,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
        perror("bind failed: ");


    /* 4) Start listening for connections */
    listen(listen_socket,QLEN);

    /* Main server loop - accept and handle requests */

    c = sizeof(struct sockaddr_in);
    while (true) {
        memset((char*)&response_buffer,0,MAX_HTTP_RESP_SIZE);
        /* 5) Accept a connection */
        struct sockaddr_in clientAddress;
        socklen_t clientSocketLength;
        connection_socket = accept(listen_socket,(struct sockaddr *)&clientAddress,(socklen_t*)&clientSocketLength);
        //connection_socket = accept(listen_socket,NULL,NULL);
        if(connection_socket<0)
            perror("Error on accept");
        /* Fork a child process to handle this request */
        if ((pid = fork()) == 0) {

            /*----------START OF CHILD CODE----------------*/
            /* we are now in the child process */

            /* child does not need access to listen_socket */
            if ( close(listen_socket) < 0) {
                fprintf(stderr, "child couldn't close listen socket");
                exit(EXIT_FAILURE);
            }

            struct http_request new_request; // defined in httpreq.h
            /* 6) call helper function to read the request         *
            * this will fill in the struct new_request for you *
            * see helper.h and httpreq.h                       */
            bool temp = Parse_HTTP_Request(connection_socket,&new_request);

            /*** PART A:  PRINT OUT
            *   URI, METHOD and return value of  Parse_HTTP_Request()*/
            printf(new_request.URI);
            printf(new_request.method);
            printf("%B\n",temp);

            /****  PART B ONLY *******/

            /* 7) Decide which status_code and reason phrase to return to client */
            status_code = 400;
            status_phrase = "Bad Request";
            // set the reply to send
            if(temp==true){
                // if the resuest is HEAD
                if(strcmp(new_request.method,"HEAD")==0){
                    if(Is_Valid_Resource(new_request.URI)){
                        status_code = 200; status_phrase = "OK";
                    }else{
                        status_code = 404; status_phrase = "Not Found";
                    }
                    // if the request is GET
                }else if(strcmp(new_request.method,"GET")==0){
                    if(Is_Valid_Resource(new_request.URI)){
                        status_code = 200; status_phrase = "OK";
                    }else{
                        status_code = 404; status_phrase = "Not Found";
                    }
                    // if the request is DELETE
                }else if(strcmp(new_request.method,"DELETE")==0){
                    status_code = 501; status_phrase = "Not Implemented";
                }
            }else{
                status_code = 400;
                status_phrase = "Bad Request";
            }
            sprintf(response_buffer, "HTTP/1.0 %d %s\r\n", status_code,status_phrase);
            printf("Sending response line: %s\n", response_buffer);
            send(connection_socket,response_buffer,strlen(response_buffer),0);

            // send resource if requested, under what condition will the server send an
            // entity body?


            if((strcmp(new_request.method,"GET")==0&&status_code==200)){
                Send_Resource(connection_socket, new_request.URI);
            }else if(strcmp(new_request.method,"HEAD")==0){
                char * server_directory,  * resource;
                char * location;

                /* set the root server directory */

                if ( (server_directory = (char *) malloc(PATH_MAX)) != NULL)
                  getcwd(server_directory, PATH_MAX);

                /* remove http://domain/ from URI */
                resource = strstr(new_request.URI, "http://");
                if (resource == NULL) {
                  /* no http:// check if first character is /, if not add it */
                  if (new_request.URI[0] != '/')
                    resource = strcat("/", new_request.URI);
                  else
                    resource = new_request.URI;
                }
                else
                  /* if http:// resource must start with '/' */
                  resource = strchr(resource, '/');

                /* append root server directory *
                 * for example if request is for /images/myphoto.jpg          *
                 * and directory for server resources is /var/www/            *
                 * then the resource location is /var/www/images/myphoto.jpg  */

                strcat(server_directory, RESOURCE_PATH);
                location = strcat(server_directory, resource);
                /* open file and send contents on socket */

                FILE * file = fopen(location, "r");

                if (file < 0) {
                  fprintf(stderr, "Error opening file.\n");
                  exit(EXIT_FAILURE);
                }

                char c;
                long sz;
                char content_header[MAX_HEADER_LENGTH];

                /* get size of file for content_length header */
                fseek(file, 0L, SEEK_END);
                sz = ftell(file);
                rewind(file);

                sprintf(content_header, "Content-Length: %ld\r\n\r\n", sz);
                printf("Sending headers: %s\n", content_header);
                send(connection_socket, content_header, strlen(content_header), 0);

                printf("Sending file contents of %s\n", location);
                free(server_directory);
                puts("\nfinished reading file\n");
                fclose(file);
			}else{
                // don't need to send resource.  end HTTP headers
                send(connection_socket,"\r\n\r\n", strlen("\r\n\r\n"), 0);
            }



            /***** END PART B  ****/

            /* child's work is done, close remaining descriptors and exit */

            if ( close(connection_socket) < 0) {
                fprintf(stderr, "closing connected socket failed");
                exit(EXIT_FAILURE);
            }

            /* all done return to parent */
            exit(EXIT_SUCCESS);

        }
        /*----------END OF CHILD CODE----------------*/

        /* back in parent process  */
        /* close parent's reference to connection socket, */
        /* then back to top of loop waiting for next request */
        if ( close(connection_socket) < 0) {
            fprintf(stderr, "closing connected socket failed");
            exit(EXIT_FAILURE);
        }

        /* if child exited, wait for resources to be released */
        waitpid(-1, NULL, WNOHANG);

    } // end while(true)
}
