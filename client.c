#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024 // tamanho da mensagem

// agrc é a variavel que armazena o numero de parametros no terminal
// agrv é um vetor de strings que armazena os parametros
void usage(int argc, char **argv)
{
    printf("usage: %s <server IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage; // pode receber um parametro de ipv4 ou ipv6
    if (0 != addrparse(argv[1], argv[2], &storage))
    {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0); // sock stream significa que esta usando tcp, AF_INET(adress family internet)
    if (s == -1)
    {
        logexit("socket");
    }
    struct sockaddr *addr = (struct sockaddr *)(&storage); // trata os dois casos de endereco, como se fosse uma especie de superclasse

    if (0 != connect(s, addr, sizeof(storage)))
    { // o conect bem sucedido retorna 0. Os paramtreos da funcao sao o socket, endereco e o tamanho
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ); //essa funcao converte o endereco em string
    printf("connected to %s\n", addrstr);

    

    // //Respondendo o jokempo
    // memset(&msg,0,sizeof(GameMessage));
    // memset(buffer, 0, BUFSZ); 
    // msg.type = 1;
    // printf("Sua escolha: ");
    // fgets(buffer, BUFSZ - 1, stdin);                    // le input do usuario(stdin), armazena em buf. a funcao para quando encontra \n ou o tamanho pre estabelecido
    // count = send(s, buffer, strlen(buffer) + 1, 0); // send retorna o numero de bytes que foram enviados na rede
    // if (count != strlen(buffer) + 1)
    // { // send(socket, variavel, tamanho(o +1 é pro \0), 0(n vai mandar funcoes pelo send))
    //     logexit("send");
    // }

    unsigned total = 0; // total de bytes recebidos
    while (1)
    {
        
        //Recebendo a primeira mensagem
        char buffer[BUFSZ];
        memset(buffer, 0, BUFSZ);
        size_t count = recv(s, buffer, sizeof(GameMessage), 0);
        if (count != sizeof(GameMessage)){
            logexit("recv");
        }

        GameMessage msg;
        memcpy(&msg, buffer, sizeof(GameMessage));
        printf("%s",msg.message);
        scanf("%d",&msg.client_action);
        printf("Você escolheu: %d", msg.client_action);
         break;
        //Respondendo o jokempo
        
        // memset(buffer, 0, BUFSZ);
        // memset(msg, 0, sizeof(GameMessage));
        // printf("Sua escolha: ");

        // fgets(buffer, BUFSZ, stdin);   
        // printf("%s\n", buffer);
        // int resposta_cliente;
        // sscanf(buffer, "%d" ,&resposta_cliente);
        // printf("%d",resposta_cliente);

        // msg->client_action = resposta_cliente; //atribuindo a reposta para o GameMessage
        // msg->type = 1; //MSG_RESPONSE
        // memset(buffer, 0, BUFSZ);
        // memcpy(buffer,msg,sizeof(GameMessage));
        // count = send(s, buffer, sizeof(GameMessage), 0); 
        // if (count != sizeof(GameMessage))   
        // { 
        //     logexit("send");
        // }

        // count = recv(s, buffer + total, BUFSZ - total, 0); // a medida que o dado vai chegando, coloca o dado na posicao buf+total, e diminui o tamanho
        // if (count == 0)
        // { // so retorna 0 se a conexao esta fechada
        //     break;
        // }
        // total += count;

        
    }

    close(s);

    printf("received %d bytes\n", total);
    //puts(buffer); // printa a string ate que encontre o \0
    exit(EXIT_SUCCESS);
}
