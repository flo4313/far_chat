#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

#define PORT 30200
#define SERVER_ADDR "127.0.0.1"

int main(int argc,char* argv[]){

		int dSock = socket(PF_INET,SOCK_STREAM,0);
		struct sockaddr_in adServ;
		adServ.sin_family = AF_INET;
		adServ.sin_port = htons(PORT);
		int res = inet_pton(AF_INET, SERVER_ADDR, &(adServ.sin_addr));
		if(res < 0){
			perror("Erreur de connexion.");
		}

		socklen_t IgA = sizeof(struct sockaddr_in);
		res = connect(dSock,(struct sockaddr *) &adServ, IgA);
		if(res < 0){
			perror("Pas reussi ");
		}else{
			printf("Reussi\n");
		}
		char r[16];
		recv(dSock,r,16,0);
		
		printf("Réponse serveur: %s\n",r);
		sleep(1);
		char* c1 = "Bonjour client 1";
		char* c2 = "Bonjour client 2";
		char* fin = "-1";
		char message[200];
		if (strcmp(c1,r)==0){
			printf("Bonjour client 1\n");
			while(1){
				printf("Moi: ");
				fgets(message,200,stdin);
				res = send(dSock,message,strlen(message)+1,0);
				if(res <0){
					perror("Le message n'a pas été envoyé");
				}else{
					printf("Message envoyé!\n");
				}
				recv(dSock,message,200,0);
				if (strcmp(fin,message)==0){
					recv(dSock,message,200,0);
					printf("Serveur: %s\n",r);
					close(dSock);
					return 0;
				}
				printf("Client 2: ");
				puts(message);
			}
		}
		else if (strcmp(c2,r)==0){
			printf("Je suis le client 2\n");
			while(1){
				recv(dSock,message,200,0);
				if (strcmp(fin,message)==0){
					recv(dSock,message,200,0);
					printf("Serveur: %s\n",message);
					close(dSock);
					return 0;
				}
				printf("Client 1: ");
				puts(message);
				printf("Moi: ");
				fgets(message,200,stdin);
				res = send(dSock,message,strlen(message)+1,0);
				if(res <0){
					perror("Le message n'a pas été envoyé");
				}else{
					printf("Message envoyé!\n");
				}
			}
		}
		else{
			printf("Connection saturé\n");
		}
	
		close(dSock);

	return 0;
}