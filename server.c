#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define BUFSZ 1024 // tamanho da mensagem

int ProcessaJogada(int acao){  //retorna 1 se o cliente envia uma acao invalida
    if(acao < 0 || acao >= 5){
        return 1;
    }
    else{
        return 0;
    }
}

int JogadaServidor(){  //escolhe um numero aleatorio de 0 a 4 para a jogada do servidor
    srand(time(NULL)); 
    int resultado = rand() % (5);
    return resultado;
}

int JogarPartida(int cliente, int servidor){ //processa a partida do servidor e do cliente(1 vitoria,0 derrota, -1 empate)
    int resultado;
    //Nuclear Attack
    if(cliente == 0){
        if(servidor == 0){
            resultado = -1;
            return resultado;
        }
        else if(servidor == 1){
            resultado = 0;
            return resultado;
        }
         else if(servidor == 2){
            resultado = 1;
            return resultado;
        }
         else if(servidor == 3){
            resultado = 1;
            return resultado;
        }
         else if(servidor == 4){
            resultado = 0;
            return resultado;
        }
    }

    //Intercept Attack
    else if(cliente == 1){
        if(servidor == 0){
            resultado = 1;
            return resultado;
        }
        else if(servidor == 1){
            resultado = -1;
            return resultado;
        }
         else if(servidor == 2){
            resultado = 0;
            return resultado;
        }
         else if(servidor == 3){
            resultado = 0;
            return resultado;
        }
         else if(servidor == 4){
            resultado = 1;
            return resultado;
        }
    }

    //Cyber Attack
    else if(cliente == 2){
        if(servidor == 0){
            resultado = 0;
            return resultado;
        }
        else if(servidor == 1){
            resultado = 1;
            return resultado;
        }
         else if(servidor == 2){
            resultado = -1;
            return resultado;
        }
         else if(servidor == 3){
            resultado = 1;
            return resultado;
        }
         else if(servidor == 4){
            resultado = 0;
            return resultado;
        }
    }

    //Drone Strike
    else if(cliente == 3){
        if(servidor == 0){
            resultado = 0;
            return resultado;
        }
        else if(servidor == 1){
            resultado = 1;
            return resultado;
        }
         else if(servidor == 2){
            resultado = 0;
            return resultado;
        }
         else if(servidor == 3){
            resultado = -1;
            return resultado;
        }
         else if(servidor == 4){
            resultado = 1;
            return resultado;
        }
    }

    //Bio Attack
    else {
        if(servidor == 0){
            resultado = 1;
            return resultado;
        }
        else if(servidor == 1){
            resultado = 0;
            return resultado;
        }
         else if(servidor == 2){
            resultado = 1;
            return resultado;
        }
         else if(servidor == 3){
            resultado = 0;
            return resultado;
        }
         else if(servidor == 4){
            resultado = -1;
            return resultado;
        }
    }
return 0;
}

void ContabilizaPlacar(int resultado, int* cliente, int* servidor){
    if(resultado==0){
        (*servidor)++;
    }
    if(resultado == 1){
        (*cliente)++;
    }
}

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

    //inicializando buffer e mensagem de jogo
    char buffer[BUFSZ];
    memset(buffer, 0, BUFSZ);
    GameMessage msg;
    memset(&msg,0,sizeof(GameMessage));
    int placarServidor = 0;
    int placarCliente = 0;
    int jogando = 1;

    while (1)
    {   
        
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage); //socket q conversa c o cliente
        socklen_t caddrlen = sizeof(cstorage);
        printf("Servidor iniciado em modo IP%s na porta %s. Aguardando conexão...\n",argv[1],argv[2]);


        //Aceitando a conexao do cliente
        int csock = accept(s, caddr, &caddrlen);
                if (csock == -1)
                {
                    logexit("accept");
                }
        printf("Cliente conectado.\n");


        //Enviando a primeira mensagem (MSG_REQUEST)
                msg.type = 0;
                strcpy(msg.message,"Escolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n"
                "2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n\n");

                memcpy(buffer,&msg,sizeof(GameMessage));
                size_t count = send(csock, buffer, sizeof(GameMessage), 0); // send retorna o numero de bytes que foram enviados na rede
                if (count != sizeof(GameMessage))// send(socket, *variavel, tamanho, 0(n vai mandar funcoes pelo send))
                { 
                logexit("send");
                }
                printf("Apresentando as opções para o cliente.\n");


        while(jogando)
        {
       
        //Recebendo a resposta do cliente
        count = recv(csock, buffer, sizeof(GameMessage), 0);
        if (count != sizeof(GameMessage))
            { 
            logexit("send");
            }
        memcpy(&msg, buffer, sizeof(GameMessage));
        

        if (msg.type == 0){
            strcpy(msg.message,"Escolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n"
                "2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n\n");

                memcpy(buffer,&msg,sizeof(GameMessage));
                size_t count = send(csock, buffer, sizeof(GameMessage), 0); // send retorna o numero de bytes que foram enviados na rede
                if (count != sizeof(GameMessage))// send(socket, *variavel, tamanho, 0(n vai mandar funcoes pelo send))
                { 
                logexit("send");
                }
        }
        else if(msg.type == 1){
            if(ProcessaJogada(msg.client_action)) //retorna 1 se houver erro na jogada
            { 
            msg.type = 5; //error
            
            strcpy(msg.message,"Por favor, selecione um valor de 0 a 4\nEscolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n"
        "2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n\n");
            memcpy(buffer,&msg,sizeof(GameMessage));
            count = send(csock, buffer, sizeof(GameMessage), 0); 
            if (count != sizeof(GameMessage))
            { 
                logexit("send");
            }
             }else{
            
            msg.type = 2;
            msg.server_action = JogadaServidor(); //Processa a jogada do servidor
            printf("Cliente escolheu %d.\n",msg.client_action);
            printf("Servidor escolheu aleatoriamente %d.\n", msg.server_action);
            msg.result = JogarPartida(msg.client_action,msg.server_action); //Processa o resultado da partida
            ContabilizaPlacar(msg.result,&placarCliente, &placarServidor);
            if(msg.result==-1){
                printf("Jogo empatado.\nSolicitando ao cliente mais uma escolha.\n");
            }
            else{
                 printf("Placar atualizado: Cliente %d x %d Servidor.\n", placarCliente,placarServidor);
            }
           

            memcpy(buffer,&msg,sizeof(GameMessage));    //enviando o resultado
            count = send(csock, buffer, sizeof(GameMessage), 0); 
            if (count != sizeof(GameMessage))
            { 
                logexit("send");
            }
        
         } 
        } else if(msg.type == 2){ //ocorre logo apos determinar o resultado
            if(msg.result==-1){ //se der empate
                msg.type = 0;
                strcpy(msg.message,"Escolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n"
                "2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n\n");
            }
            else{
            msg.type = 3;
            strcpy(msg.message,"Deseja jogar novamente?\n1 - Sim\n0 - Não\n\n");
            printf("Perguntando se o cliente deseja jogar novamente.\n\n");
            }

            memcpy(buffer,&msg,sizeof(GameMessage));
            count = send(csock, buffer, sizeof(GameMessage), 0); 
            if (count != sizeof(GameMessage))
            { 
                logexit("send");
            }
         
        } else if(msg.type == 4){
            int jogar_novamente = msg.client_action;
            if(jogar_novamente != 0 && jogar_novamente != 1){
                printf("Erro: resposta inválida para jogar novamente.\n\n");
                msg.type = 3;
                strcpy(msg.message,"Deseja jogar novamente?\n1 - Sim\n0 - Não\n\n");
                printf("Perguntando se o cliente deseja jogar novamente.\n\n");
                memcpy(buffer,&msg,sizeof(GameMessage));
                size_t count = send(csock, buffer, sizeof(GameMessage), 0); // send retorna o numero de bytes que foram enviados na rede
                if (count != sizeof(GameMessage))// send(socket, *variavel, tamanho, 0(n vai mandar funcoes pelo send))
                { 
                logexit("send");
                }
                
            }
            else{
            if(jogar_novamente == 1){
                jogando = 1;
                msg.type = 0;
                strcpy(msg.message,"Escolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n"
                "2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n\n");
                 memcpy(buffer,&msg,sizeof(GameMessage));
                size_t count = send(csock, buffer, sizeof(GameMessage), 0); // send retorna o numero de bytes que foram enviados na rede
                if (count != sizeof(GameMessage))// send(socket, *variavel, tamanho, 0(n vai mandar funcoes pelo send))
                { 
                logexit("send");
                }
                printf("Apresentando as opções para o cliente.\n");

            }
            else{
                jogando = 0;
                msg.type = 6;
                printf("Cliente não deseja jogar novamente.\nEnviando Placar Final.\n");
                sprintf(msg.message,"Fim de jogo!\nPlacar Final: Você %d x %d Servidor\nObrigado por jogar!\n",placarCliente,placarServidor);
                memcpy(buffer,&msg,sizeof(GameMessage));
                size_t count = send(csock, buffer, sizeof(GameMessage), 0); // send retorna o numero de bytes que foram enviados na rede
                if (count != sizeof(GameMessage))// send(socket, *variavel, tamanho, 0(n vai mandar funcoes pelo send))
                { 
                logexit("send");
                }

            }
          }
        }
       

}
        
        memset(buffer, 0, BUFSZ);
        memset(&msg,0,sizeof(GameMessage));
        placarServidor = 0;
        placarCliente = 0;
        jogando = 1;
        printf("Encerrando conexão.\nCliente Desconectado.\n\n");
        close(csock);
    
    }

    exit(EXIT_SUCCESS);
}