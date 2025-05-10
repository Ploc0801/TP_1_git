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
    addrtostr(addr, addrstr, BUFSZ);

    printf("connected to %s\n", addrstr);

    char buf[BUFSZ];
    memset(buf, 0, BUFSZ); // argumentos (ponteiro do valor inicial, valor de setagem, qts bytes devem ser setados com o valor)
    printf("mensagem> ");
    fgets(buf, BUFSZ - 1, stdin);                    // le input do usuario(stdin), armazena em buf. a funcao para quando encontra \n ou o tamanho pre estabelecido
    size_t count = send(s, buf, strlen(buf) + 1, 0); // send retorna o numero de bytes que foram enviados na rede
    if (count != strlen(buf) + 1)
    { // send(socket, variavel, tamanho(o +1 é pro \0), 0(n vai mandar funcoes pelo send))
        logexit("send");
    }

    memset(buf, 0, BUFSZ);
    unsigned total = 0; // total de bytes recebidos
    while (1)
    {
        count = recv(s, buf + total, BUFSZ - total, 0); // a medida que o dado vai chegando, coloca o dado na posicao buf+total, e diminui o tamanho
        if (count == 0)
        { // so retorna 0 se a conexao esta fechada
            break;
        }
        total += count;
    }

    close(s);

    printf("received %d bytes\n", total);
    puts(buf); // printa a string ate que encontre o \0
    exit(EXIT_SUCCESS);
}
