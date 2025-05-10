#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void logexit(const char *msg)
{
    perror(msg); // imprime o erro que ocorreu na msg
    exit(EXIT_FAILURE);
}

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage)
{
    if (addrstr == NULL || portstr == NULL)
    {
        return -1; // o main ira fechar se retorna -1
    }

    uint16_t port = (uint16_t)atoi(portstr); // unsigned short , a funcao atoi ASCII to integer
    if (port == 0)
    {
        return -1;
    }
    port = htons(port); // host to network short -> numero de porta é sempre big endian, deve-se converter do dispostivo p o da rede

    // tentar em ipv4
    struct in_addr inaddr4; // 32bit ip addr
    if (inet_pton(AF_INET, addrstr, &inaddr4))
    { // inet_pton retorna 1 se o endereco foi convertido com sucesso p binario e 0 se nao(ou se n existe) presentation to network
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    // tentar em ipv6
    struct in6_addr inaddr6; // 128bit ip addr
    if (inet_pton(AF_INET6, addrstr, &inaddr6))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        // addr6->sin6_addr = inaddr6; -> nao se consegue fazer a atribuicao de 16bytes diretamente
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1; // nao deu em nenhum dos dois protocolos
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize)
{
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = ""; // constante q usa o tamanho do inet
    uint16_t port;

    if (addr->sa_family == AF_INET)
    { // endereco ipv4
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        version = 4;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN + 1))
        { // network to presentation
            logexit("ntop");
        }
        port = ntohs(addr4->sin_port); // network to host short
    }
    else if (addr->sa_family == AF_INET6)
    { // endereco ipv6
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        version = 6;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1))
        { // network to presentation
            logexit("ntop");
        }
        port = ntohs(addr6->sin6_port); // network to host short
    }
    else
    {
        logexit("unknown protocol family.");
    }
    if (str)
    {
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port); // printa esses bytes no buffer
    }
}

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage)
{
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short , a funcao atoi ASCII to integer
    if (port == 0)
    {
        return -1;
    }
    port = htons(port); // host to network short -> numero de porta é sempre big endian, deve-se converter do dispostivo p o da rede

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4"))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY; // qualquer endereco da interface de rede
        return 0;
    }
    else if (0 == strcmp(proto, "v6"))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        return 0;
    }
    else
    {
        return -1;
    }
}