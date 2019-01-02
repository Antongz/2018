#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>


void readline(char *buf, int buflen, int s) {
  struct pollfd pfd = {s, POLLIN, 0};
  int i;
  
  do {
    for (i = 0; i < buflen - 1; i++) {
      switch (poll(&pfd, 1, 20000)) {
	case -1:
	  perror("Poll");
	  exit(1);
	case 0:
	  close(s);
	  buf[i] = '\0';
	  return;
	case 1:
	  break;
	default:
	  fprintf(stderr, "Poll???\n");
	  exit(1);
      }
      switch (read(s, buf+i, 1)) {
	case 0:
	  buf[i] = '\0';
	  return;
	case 1: 
	  break;
	default:
	  fprintf(stderr, "Read???\n");
	  exit(1);
      }
      if (isspace(buf[i]))
	break;
    }
    buf[i] = '\0';
  } while (i == 0);
}


static char *password= "password";

#define MLEN 2
uint8_t key[MLEN] = {
  '1', '2'
};
uint8_t message[MLEN] = {
  '0', '9'
};

int isPositiveNumber(char *s) {
  while (*s != '\0') {
    if (!isdigit(*s))
      return 0;
    s++;
  }
  return 1;
}
void serve(int ns) {
  char inbuf[100];
  char outbuf[100];

  for (;;) {
    readline(inbuf, sizeof(inbuf), ns);
    if (*inbuf == '\0') {
      close(ns);
      return;
    }
    if (isPositiveNumber(inbuf)) {
      int i = atoi(inbuf);
      if (i < MLEN) {
	sprintf(outbuf, "Byte at %d 0x%02x\n", i, message[i]);
	write(ns, outbuf, strlen(outbuf));
      } else {
	sprintf(outbuf, "Index %d out of bounds\n", i);
	write(ns, outbuf, strlen(outbuf));
      }
    } else {
      if (!strcmp(inbuf, password)) {
	for (int i = 0; i < MLEN; i++) {
	  uint8_t b = message[i]^key[i];
	  write(ns, &b, 1);
	}
      } else {
	write(ns, "Wrong password!\n", strlen("Wrong password!\n"));
      }
    }
  }
}



int initServer(int port) {
  signal(SIGCHLD, SIG_IGN);
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("socket");
    exit(1);
  }

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = INADDR_ANY;
  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("bind");
    close(s);
    exit(1);
  }

  if (listen(s, 10) < 0) {
    perror("Listen");
    exit(1);
  }

  return s;
}


void server(int s) {
  for (;;) {
    int ns = accept(s, NULL, NULL);
    if (ns < 0) {
      perror("accept");
      exit(1);
    }

    //serve(ns);
    switch (fork()) {
      case -1:
	perror("fork");
	exit(1);
      default:
	close(ns);
	break;
      case 0:
	close(s);
	serve(ns);
	exit(0);
    }
  }
}

int main() {
  int sock = initServer(8000);
  server(sock);
}
