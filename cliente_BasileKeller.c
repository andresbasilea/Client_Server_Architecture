/////////////////////////////////////////////////////////////////////////////////////

//                          UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
//                                  FACULTAD DE INGENIERIA
//                              ARQUITECTURA CLIENTE-SERVIDOR



//                                     PROYECTO FINAL
//                                    PROGRAMA CLIENTE 




//                          ALUMNOS:
//                          - BASILE ALVAREZ ANDRES JOSE
//                          - KELLER ASCENCIO RODOLFO ANDRES

/////////////////////////////////////////////////////////////////////////////////////






// Se importan las bibliotecas a utilizar para el cliente
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <errno.h>
 #include <string.h>
 #include <netdb.h>
 #include <sys/types.h>
 #include <netinet/in.h>
 #include <sys/socket.h>
//Indica el tamaño de caracteres del comando recibido, se quedo en 100
 #define MAXDATASIZE 100      
//Indica el tamaño de caracteres de la respuesta en la terminal, pasa de 20000 a 60000
 #define MAXDATASIZE_RESP 60000 

 int main(int argc, char *argv[]){
  
  // Variables para el comando ingresado por parte del cliente
  char comando[MAXDATASIZE];      //Almacena el comando ingresado por el usuario
  int len_comando;                //Indica la longitud del tamaño
  
  // Variables para la respuesta recibida por parte del servidor
  char buf[MAXDATASIZE_RESP];     //Almacena el buffer con la respuesta recibida del servidor
  int numbytes;                   //Indica el numero de bytes recibidos como respuesta
  
  int sockfd;                     //Entero para el descritor del socket
  struct hostent *he;             //Almacena informacion del servidor
  struct sockaddr_in cliente;     //Información de la dirección de destino del cliente

  //Se verifica que se hayan recibido tres argumentos al momento de ejecutar el programa, para tener el host y el puerto
  if (argc != 3) {                //Si no se recibieron tres argumentos se manda un mensaje y se sale del programa        
    fprintf(stderr,"usage: client hostname puerto\n");
    exit(1);                      
  }

  //Se almacena la informacion del servidor, en caso de que sea NULL, se muestra error y se sale del programa
  if ((he=gethostbyname(argv[1])) == NULL) {  // obtener información de host servidor 
   perror("gethostbyname");
   exit(1);
  }

  //Se crea un socket que se almacena en sockfd, si hay un error en su creacion se sale del programa
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
   perror("socket");
   exit(1);
  }

  //Se configura la direccion y puerto del servidor al que se conectara el cliente
  cliente.sin_family = AF_INET;               // Ordenación de bytes de la máquina 
  cliente.sin_port = htons( atoi(argv[2]) );  // short, Ordenación de bytes de la red. Puerto a conectarse
  cliente.sin_addr = *((struct in_addr *)he->h_addr);   //Servidor a conectarse
  memset(&(cliente.sin_zero), '\0',8);        // poner a cero el resto de la estructura 


  //Se establece la conexion con el servidor mediante el descriptor y la informacion del servidor.
  if (connect(sockfd, (struct sockaddr *)&cliente, sizeof(struct sockaddr)) == -1) {
   perror("connect");
   exit(1);
  }

  ///////////////////////////////////////////////////////////////////////////////////
  //MODIFICACIONES DEL CODIGO PROPORCIONADO

  //CICLO PARA QUE SE SIGA EJECUTANDO TRAS VARIOS COMANDOS HASTA RECIBIR "EXIT"
  while(1){

    //Se lee el comando como entrada del teclado y se almacena en la variable comando
    fgets(comando,MAXDATASIZE-1,stdin);
    len_comando = strlen(comando) - 1;    //No tomamos en cuenta el salto de linea
    comando[len_comando] = '\0';          //Agrega un caracter nulo al final del comando para trabajar con una cadena de caracteres valida
    //Se imprime el comando que ingreso el cliente
    printf("Comando: %s\n",comando);

    //Mandamos el comando al servidor usando la funcion send() y el descriptor del socket
    if(send(sockfd,comando, len_comando, 0) == -1) {
     perror("send()");
     exit(1);                             //Si hay un error en el envio sale del programa
    } else printf("Comando enviado...\n");

    // SI EL COMANDO DE ENTRADA ES LA PALABRA "EXIT", CERRAMOS LA SESIÓN DEL CLIENTE.
    if(strcmp(comando,"exit")==0)
      break;     

    //Si el send no devuelve error el programa continua y lee la respuesta del servidor
    //mediante el uso de recv y lo almacena en buf
    if ((numbytes=recv(sockfd, buf, MAXDATASIZE_RESP-1, 0)) == -1) {
     perror("recv");
     exit(1);
    }
    buf[numbytes] = '\0';                 //Agrega un caracter nulo al final de la respuesta para trabajar con una cadena de caracteres valida
    printf("Recibido:\n%s\n",buf);        //Imprime la respuesta recibida del servidor
  }

  ///////////////////////////////////////////////////////////////////////////////////

  //Tras haber trabajado con todos los comandos y haber obtenido un mensaje "exit" o un error,
  //cerramos el file descriptor del cliente
  close(sockfd);

  return 0;
} 
