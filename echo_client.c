/* A simple echo client using TCP */
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>



#define SERVER_TCP_PORT 3000	/* well-known port */
#define BUFLEN		100	/* buffer length */

struct PDU{
	char type;
	unsigned int length;
        char data[BUFLEN];
        char fileName[100];
} rpdu,spdu;

int main(int argc, char **argv)
{
FILE * fp;	
int option;
	int 	n, i, bytes_to_read, fd;
	int 	sd, port;
	struct	hostent		*hp;
	struct	sockaddr_in server;
	char	*host, *bp, rbuf[BUFLEN], sbuf[BUFLEN];
        struct PDU rpdu,spdu;

	switch(argc){
	case 2:
		host = argv[1];
		port = SERVER_TCP_PORT;
		break;
	case 3:
		host = argv[1];
		port = atoi(argv[2]);
		break;
	default:
		fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
		exit(1);
	}

	/* Create a stream socket	*/	
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (hp = gethostbyname(host)) 
	  bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	else if ( inet_aton(host, (struct in_addr *) &server.sin_addr) ){
	  fprintf(stderr, "Can't get server's address\n");
	  exit(1);
	}

	/* Connecting to the server */
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
	  fprintf(stderr, "Can't connect \n");
	  exit(1);
	}
	
	printf("\nChoose an option: 1. List files in directory\n2.Download file\n3. Upload file\n4. Change directory\n5. Exit \n");
	scanf("%d", &option);
	while(1) {
		if(option == 1) {
			printf("Enter a directory path: \n");
			n = read(0, spdu.data, BUFLEN);
			spdu.type = 'L';
			spdu.length = n;
	
			write(sd, &spdu, sizeof(spdu));

			n = read(sd, &rpdu, sizeof(rpdu));
  			if(rpdu.type = 'I') {
				printf("Listing files: \n");
				while((n = read(sd, &rpdu, sizeof(rpdu))) != 0) {
					write(1, rpdu.data, rpdu.length);
					printf("\n");
				}
			}
			
		} else if (option == 2) {
  		  	printf("Enter the name of the file you want to download: \n");
scanf("%s", &spdu.fileName);
printf("Enter it once again: \n");
			n = read(0, spdu.data, BUFLEN);
 			spdu.type = 'D';
			spdu.length = n;

			write(sd, &spdu, sizeof(spdu));
			
			printf("Downloaded file contains: \n");
			while((n = read(sd, &rpdu, sizeof(rpdu))) != 0) {
				rpdu.data[rpdu.length - 1] = '\0';
				write(1, rpdu.data, rpdu.length);
fp = fopen (rpdu.fileName,"w");
fprintf(fp, rpdu.data);
fclose(fp);
}
		} else if (option == 3) {
			printf("Enter the name of the filer you want to upload: \n");
scanf("%s", &spdu.fileName);
printf("Enter it once again: \n");
  			n = read(0, spdu.data, BUFLEN);
   			spdu.type = 'U';
  			spdu.length = n;

  			write(sd, &spdu, sizeof(spdu));
  			spdu.data[spdu.length - 1] = '\0';
//fd = open(sbuf, O_RDONLY);

//	if(fd >= 0) {
//		while((n = read(fd, argv, BUFLEN)) != 0) 
//			write(sd, argv, n);
//	}
  			n= read(sd, &rpdu, sizeof(rpdu));
  			if(rpdu.type ='R'){
	  			printf("Uploading file. \n");
	  			fd = open(spdu.data, O_RDONLY);
		
		      		if(fd >= 0) {
		      			spdu.type = 'F';
		      			while((n = read(fd, &spdu.data, BUFLEN)) != 0) {
		      				spdu.length = n;
		      				write(sd, &spdu, sizeof(spdu));
		      			}
		      		} else {
					write(1, "Error: File not found\n", 22);
				}
  			}
		} else if (option == 4) {
			printf("Enter the directory path you want to change to: \n");
			n = read(0, spdu.data, BUFLEN);
			spdu.type = 'P';
			spdu.length = n;
	
			write(sd, &spdu, sizeof(spdu));

			n = read(sd, &rpdu, sizeof(rpdu));
  			if(rpdu.type = 'R') {
				printf("Directory has changed \n");
			}
		} else if (option == 5) {
			close(sd);
			exit(0);
		}
		printf("\nChoose an option: 1. List files in directory\n2.Download file\n3. Upload file\n4. Change directory\n5. Exit \n");
		scanf("%d", &option);
		if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			fprintf(stderr, "Can't creat a socket\n");
			exit(1);
		}

		bzero((char *)&server, sizeof(struct sockaddr_in));
		server.sin_family = AF_INET;
		server.sin_port = htons(port);
		if (hp = gethostbyname(host)) 
		  bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
		else if ( inet_aton(host, (struct in_addr *) &server.sin_addr) ){
		  fprintf(stderr, "Can't get server's address\n");
		  exit(1);
		}

		/* Connecting to the server */
		if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		  fprintf(stderr, "Can't connect \n");
		  exit(1);
		}
	}
	close(sd);
	return(0);
}
