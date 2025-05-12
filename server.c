#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024 // tamanho da mensagem

void usage(int argc, char **argv)
{
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage; // pode receber um parametro de ipv4 ou ipv6
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage))
    {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0); // sock stream significa que esta usando tcp, AF_INET(adress family internet)
    if (s == -1)
    {
        logexit("socket");
    }

    int enable =1;
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0)
    {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage); // trata os dois casos de endereco, como se fosse uma especie de superclasse

    if (bind(s, addr, sizeof(storage)) != 0)
    {
        logexit("bind");
    }

    if (0 != listen(s, 10))
    { // listen(socket, qt de conexoes pendentes)
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);


    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage); // socket q conversa c o cliente
        socklen_t caddrlen = sizeof(cstorage);
        //Aceitando a conexao do cliente
        int csock = accept(s, caddr, &caddrlen);
                if (csock == -1)
                {
                    logexit("accept");
                }

        //Enviando a primeira mensagem (MSG_REQUEST)
        GameMessage msg;
        memset(&msg,0,sizeof(GameMessage));
        msg.type = 3;
        strcpy(msg.message,"Escolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n"
        "2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n\n");

        char buffer[BUFSZ];
        memset(buffer, 0, BUFSZ);
        memcpy(buffer,&msg,sizeof(GameMessage));
        size_t count = send(csock, buffer, sizeof(GameMessage), 0); // send retorna o numero de bytes que foram enviados na rede
        if (count != sizeof(GameMessage))// send(socket, *variavel, tamanho, 0(n vai mandar funcoes pelo send))
        { 
        logexit("send");
        }

        //Recebendo a resposta do cliente
        memset(buffer, 0, BUFSZ);
        count = recv(csock, buffer, BUFSZ, 0);
        // if (count != sizeof(GameMessage)){
        //     logexit("recv");
        // }
        GameMessage *msg_pointer = (GameMessage*)buffer; //trata o buffer como um ponteiro para GameMessage
        printf("%d\n",msg_pointer->client_action);


        
        char caddrstr[BUFSZ];
        addrtostr(addr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        count = recv(csock, buf, BUFSZ, 0);
        printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, buf);

        sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
        count = send(csock, buf, strlen(buf) + 1, 0);
        if (count != strlen(buf) + 1)
        {
            logexit("send");
        }
        close(csock);
    }

    exit(EXIT_SUCCESS);
}