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

void ProcessaAcaoC(int acao){
    if(acao == 0){
        printf("\nVocê escolheu: Nuclear Attack\n");
    }
    else if(acao == 1){
        printf("\nVocê escolheu: Intercept Attack\n");
    }
    else if(acao == 2){
        printf("\nVocê escolheu: Cyber Attack\n");
    }
    else if(acao == 3){
        printf("\nVocê escolheu: Drone Strike\n");
    }
    else{
        printf("\nVocê escolheu: Bio Attack\n");
    }
}

void ProcessaAcaoS(int acao){
    if(acao == 0){
        printf("Servidor escolheu: Nuclear Attack\n");
    }
    else if(acao == 1){
        printf("Servidor escolheu: Intercept Attack\n");
    }
    else if(acao == 2){
        printf("Servidor escolheu: Cyber Attack\n");
    }
    else if(acao == 3){
        printf("Servidor escolheu: Drone Strike\n");
    }
    else{
        printf("Servidor escolheu: Bio Attack\n");
    }
}

void ProcessaResultado(int resultado){
    if(resultado == 0){
        printf("Resultado: Derrota!\n\n");
    }
    else if(resultado == -1){
        printf("Resultado: Empate!\n\n");
    }
    else{
        printf("Resultado: Vitória!\n\n");
    }
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

    printf("Conectado ao servidor.\n");

    //inicializando buffer e mensagem de jogo
    char buffer[BUFSZ];
    memset(buffer, 0, BUFSZ);
    GameMessage msg;
    memset(&msg,0,sizeof(GameMessage));


    while (1)
    {
        
        //Recebendo mensagens
        size_t count = recv(s, buffer, sizeof(GameMessage), 0);
         if (count != sizeof(GameMessage)){
              logexit("recv"); //isso ta dando erro
         }

        
        memcpy(&msg, buffer, sizeof(GameMessage));

        if(msg.type == 0 || msg.type == 5) //Primeira mensagem, ou mensagem de erro
        {
            printf("%s",msg.message);
            scanf("%d",&msg.client_action);           
            msg.type = 1; 
        }
        else if(msg.type == 2){ //msg do resultado
            ProcessaAcaoC(msg.client_action); //printa a Acao que o cliente realizou
            ProcessaAcaoS(msg.server_action); //printa a Acao que o servidor realizou;
            ProcessaResultado(msg.result); //printa o resultado da partida
            }
        else if(msg.type == 3){ //Pergunta se quer jogar de novo
            printf("%s",msg.message);
            int jogar_novamente = 0;
            scanf("%d",&jogar_novamente);
            msg.client_action = jogar_novamente; 
            msg.type = 4; 
            }       
        else if(msg.type == 6){ //Quando o jogo termina
            printf("%s",msg.message);
            close(s);
            break;
        }
        

        //Enviando mensagens
        memcpy(buffer,&msg,sizeof(GameMessage));
        count = send(s, buffer, sizeof(GameMessage), 0); 
         if (count != sizeof(GameMessage))
         { 
         logexit("send");
    }
}


close(s);
exit(EXIT_SUCCESS);
}
