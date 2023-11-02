#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

//#define SERVER "localhost"
//#define PORT 4897
#define BLEN 128

void main(int argc, char *argv[])
{
  struct sockaddr_in socket_cliente;
  struct hostent *h;
  int fd;
  int n;
  //char *host;
  char buf[BLEN];
  
  // Nombre
  char* nombre = "Basile Alvarez Andrés José";

  printf( "1. Se llama a la funcion socket(...) y devuelve el file descriptor propio del cliente fd\n" );
  printf( "fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);\n\n" );
  fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  printf( "2. Se inicializan los atributos de la variable struct sockaddr_in llamada socket_cliente\n\n" );
  printf( "Pero se inicializan con datos del servidor, como son la IP y el puerto\n\n" );

  // memset((char *) &socket_cliente, 0, sizeof(struct sockaddr_in));
  socket_cliente.sin_family = AF_INET;
  //socket_cliente.sin_port = htons((u_short) PORT);
  socket_cliente.sin_port = htons(atoi(argv[2]));
  socket_cliente.sin_addr.s_addr = inet_addr(argv[1]);
  
  
  //h = gethostbyname(SERVER);
  //memcpy(&socket_cliente.sin_addr, h->h_addr, h->h_length);

  printf( "3. La funcion connect() es la que intenta conectarse al servidor\n" );
  printf( "Esta funcion se relaciona con la funcion accept(...) del servidor\n\n" );
  connect(fd, (struct sockaddr *) &socket_cliente, sizeof(socket_cliente));

  printf( "4. El cliente envía el nombre al servidor con send(fd, nombre, strlen(nombre), 0)\n" );
  n = send(fd, nombre, strlen(nombre), 0);


  printf( "5. Como el servidor hace send(...), entonces el cliente hace recv(...)\n" );
  printf( "n = recv(fd, buf, sizeof(buf), 0);\n" );
  
  n = recv(fd, buf, sizeof(buf), 0);
  printf("\n\nnumero real de bytes leidos=%d\n",n);
  while (n > 0) {
    printf("respuesta del servidor: \n");
    write(1, buf, n);
    n = recv(fd, buf, sizeof(buf), 0);
  }
  printf( "\n" );

  printf( "\n\n6. Por último, se cierra el file descriptor\n" );
  printf( "close(fd);\n" );
  close(fd);
  exit(0);
}

