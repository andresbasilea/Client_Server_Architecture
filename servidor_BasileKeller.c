/////////////////////////////////////////////////////////////////////////////////////

//                          UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
//                                  FACULTAD DE INGENIERIA
//                              ARQUITECTURA CLIENTE-SERVIDOR



//                                     PROYECTO FINAL
//                                    PROGRAMA SERVIDOR 




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
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <sys/wait.h>
 #include <signal.h>
//Indica el tamaño de caracteres del comando recibido, se quedo en 100
 #define MAXDATASIZE 100   
//Indica el tamaño de caracteres de la respuesta en la terminal, pasa de 20000 a 60000
 #define LENGTH 60000

int main(int argc, char *argv[]){

  int numbytes;                   //Almacena el numero de bytes recibidos por parte del cliente como comando
  char buf[MAXDATASIZE];          //Almacena el comando recibido por parte del cliente

  // Tenemos dos estructuras sockaddr_in:
  // Una para el propio server y otra para la conexion cliente
  // Necesitamos dos file descriptor
  int server_fd, cliente_fd;

  // Estas son las dos estructuras, la primera llamada servidor, que se asociara a server_fd
  // y la segunda estructura llamada cliente que se asocia a cliente_fd
  struct sockaddr_in servidor;    //Información sobre direccion del servidor
  struct sockaddr_in cliente;     //Información sobre dirección del cliente

  // Variables que almacenan el tamaño de servidor y de cliente
  int sin_size_servidor;
  int sin_size_cliente;

  //Se crea un socket, donde el descriptor se almacena en server_fd
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
   perror("socket");
   exit(1);                       //Si hay un error salimos del programa
  }

  //Se establecen las opciones del socket
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)  {
    perror("Server-setsockopt() error!");
    exit(1);
  }else printf("Server-setsockopt is OK...\n");

  //Se configura la direccion y puerto del servidor
  servidor.sin_family = AF_INET;                  // Ordenación de bytes de la máquina
  servidor.sin_port = htons( atoi(argv[1]) );     // short, Ordenación de bytes de la red
  servidor.sin_addr.s_addr = INADDR_ANY;          // Establece mi dirección IP
  memset(&(servidor.sin_zero), '\0', 8);          // Poner a cero el resto de la estructura

  sin_size_servidor = sizeof( servidor );         //Calcula el tamaño del servidor y lo almacena
  
  //Enlaza el socket del servidor a su direccion y puerto
  if (bind(server_fd, (struct sockaddr *)&servidor, sin_size_servidor) == -1){
   perror("bind");
   exit(1);
  }

  //Se pone al servidor en modo de escucha para buscar conexiones, y establece la cola de conexiones a 1
  //espera una unica conexion
  if (listen(server_fd, 1) == -1){
   perror("listen");
   exit(1);
  }

  sin_size_cliente = sizeof( cliente );           //Se calcula el tamaño del cliente y lo almacena

  //Al establecer una conexion con un cliente, se crea un socket para la conexion y le añade la informacion del cliente
  if ((cliente_fd = accept(server_fd, (struct sockaddr *)&cliente, &sin_size_cliente)) == -1){
   perror("accept");
   exit(1);
  }

  //Se muestra la direccion IP del cliente que se conecto al servidor
  printf("server: conexion cliente desde %s\n", inet_ntoa(cliente.sin_addr));

  ///////////////////////////////////////////////////////////////////////////////////
  //MODIFICACIONES DEL CODIGO PROPORCIONADO

  //CICLO PARA QUE SE SIGA EJECUTANDO TRAS VARIOS COMANDOS HASTA RECIBIR "EXIT"

  while(1){
    //Recibe los datos enviados por el cliente a traves de su socket y los almacena en buf
    if ((numbytes=recv(cliente_fd, buf, 100-1, 0)) == -1) {
       perror("recv");
       exit(1);
    }

    //AÑADIMOS COMANDO PARA SALIR EN CASO DE QUE EL COMANDO INGRESADO SEA "EXIT"
    if (strcmp(buf,"exit") == 0) break;

    buf[numbytes] = '\0'; //Agrega un caracter nulo al final del comando para trabajar con una cadena de caracteres valida

    // Se abre el archivo "abk.txt" en modo escritura
    FILE* file = fopen("abk.txt", "w");
    if (file == NULL) {
      perror("fopen failed");
      exit(1);
    }

    // Ejecutamos el comando almacenado en buf y redirigimos la salida a pipe
    FILE* pipe = popen(buf, "r");
    if (pipe == NULL) {
      perror("popen failed");
      exit(1);
    }

    // Leemos la salida del comando desde el pipe y escribimos en el archivo
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), pipe)) > 0) {
      fwrite(buffer, 1, bytesRead, file);
    }

    // Cerramos el archivo y el pipe
    fclose(file);
    pclose(pipe);

    //considerar comando "ps -e -o pid,ppid,user,stat,command"


    // Se lee el archivo abk.txt y su informacion se envia al cliente
    char* fs_name="abk.txt";
    char sdbuf[LENGTH];             //Se prepara el envio de caracteres del tamaño definido 60000
    printf("[Server] Enviando salida al Cliente...\n");
    FILE *fs = fopen(fs_name, "r");
    if(fs == NULL){
     printf("ERROR: File %s not found on server.\n", fs_name);
     exit(1);
    }

    //Envia la salida del comando al cliente 
    bzero(sdbuf, LENGTH);           //Establece todos los bytes a utilizar del buffer a cero
    int fs_block_sz; 
    //Se lee la informacion del archivo que contiene la salida del comando ejecutado
    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0){
      //Se envia el contenido del bufer al cliente
      if(send(cliente_fd, sdbuf, fs_block_sz, 0) < 0){
        printf("ERROR: al enviar la salida del comando al cliente\n");
        exit(1);
      }
      bzero(sdbuf, LENGTH);
    }
    //Se cierra el archivo nuevamente
    fclose( fs );
    printf("Ok sent to client!\n");
  }

  ///////////////////////////////////////////////////////////////////////////////////


  //Cierra los descriptores del cliente y del servidor
  close(cliente_fd);
  close(server_fd);

  //Deshabilita la recepcion y el envio de datos a traves del socket del servidor
  //Se cierra tanto la lectura como la escritura del socket del servidor
  shutdown(server_fd, SHUT_RDWR);

  // Termina con exit(0) que significa terminacion exitosa
  exit(0);

  return 0;
}
