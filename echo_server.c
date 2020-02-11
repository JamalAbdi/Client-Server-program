/* A simple echo server using TCP */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		256	/* buffer length */

struct PDU{
	char type;
	unsigned int length;
        char data[100];
        char fileName[100];
} rpdu,spdu;

int echod(int);
void reaper(int);

int main(int argc, char **argv)
{
	int 	sd, new_sd, client_len, port;
	struct	sockaddr_in server, client;

	switch(argc){
	case 1:
		port = SERVER_TCP_PORT;
		break;
	case 2:
		port = atoi(argv[1]);
		break;
	default:
		fprintf(stderr, "Usage: %d [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket	*/	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	/* Bind an address to the socket	*/
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		fprintf(stderr, "Can't bind name to socket\n");
		exit(1);
	}

	/* queue up to 5 connect requests  */
	listen(sd, 5);

	(void) signal(SIGCHLD, reaper);

	while(1) {
	  client_len = sizeof(client);
	  new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
	  if(new_sd < 0){
	    fprintf(stderr, "Can't accept client \n");
	    exit(1);
	  }
	  switch (fork()){
	  case 0:		/* child */
		(void) close(sd);
		exit(echod(new_sd));
	  default:		/* parent */
		(void) close(new_sd);
		break;
	  case -1:
		fprintf(stderr, "fork: error\n");
	  }
	}
}

/*	echod program	*/
int echod(int sd)
{
FILE * fp;	
DIR *dir;
	struct dirent *ent;
	char cwd[1024];
	char	*bp, buf[BUFLEN];
	int 	n, bytes_to_read, fd, i;
	struct PDU rpdu,spdu;
        
	n = read(sd, &rpdu, sizeof(rpdu));
	rpdu.data[rpdu.length - 1] = '\0';

             if(rpdu.type == 'L') {
		if ((dir = opendir (rpdu.data)) != NULL) {
		  spdu.type = 'I';
		  while ((ent = readdir (dir)) != NULL) {
		  	memcpy(spdu.data, ent->d_name, strlen(ent->d_name) + 1);
			spdu.length = strlen(ent->d_name) + 1;
			spdu.data[spdu.length - 1] = '\0';
			write(sd, &spdu, sizeof(spdu));
		  }
		  closedir (dir);
		}
	}   
else if(rpdu.type == 'D') {
		fd = open(rpdu.data, O_RDONLY);
		
		if(fd >= 0) {
			spdu.type = 'F';
for (i=0; i<100; i++)
spdu.fileName[i]=rpdu.fileName[i];
			while((n = read(fd, &spdu.data, BUFLEN)) != 0) {
				spdu.length = n;
				write(sd, &spdu, sizeof(spdu));
			}
		} else {
			spdu.type = 'E';
			spdu.length = 22;
			memcpy(spdu.data, "Error: File not found\n", 22);
			write(sd, &spdu, sizeof(spdu));
		} 
                } else if(rpdu.type == 'U'){
	    spdu.type = 'R';
	    spdu.length = 0;
	    write(sd, &spdu, sizeof(spdu));
	    
	    printf("Requested file contains: \n");
	    while((n = read(sd, &rpdu, sizeof(rpdu))) != 0) {
		rpdu.data[rpdu.length - 1] = '\0';
		write(1, rpdu.data, rpdu.length);
		printf("\n");
//fd = rpdu.length;
		//while((n = read(sd, buf, BUFLEN)) != 0) 
		//	write(sd, buf, n);
fp = fopen(rpdu.fileName, "w");
fprintf(fp, rpdu.data);
fclose(fp);
	    }
	} else if(rpdu.type == 'P') {
		chdir(rpdu.data);
	   	if (strcmp(getcwd(cwd, sizeof(cwd)), rpdu.data) == 0) {
			printf("Current working directory is: %s\n", cwd);
			spdu.type = 'R';
	    		spdu.length = 0;
	    		write(sd, &spdu, sizeof(spdu));
		}
	} 
	printf("\n");
	close(sd);
	return(0);
}

/*	reaper		*/
void	reaper(int sig)
{
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}
