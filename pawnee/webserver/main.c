#include <dirent.h>
#include "socket.h"
#include "analyse.h"
#include "reponse.h"
#include "contenu.h"
#include <limits.h>
#include "stats.h"

const char * ROOT="content_web/";

char * get_mime_type(char * name){
  if(strstr(name,".pdf")!=NULL)
    {
      return "application/pdf ";
    }
  else if(strstr(name,".html")!=NULL)
    {
      return "text/html ";
    }
  else if(strstr(name,".jpg")!=NULL)
    {
      return "image/jpeg ";
    }
  else if(strstr(name,".css")!=NULL)
    {
      return "text/css ";
    }
  else
    {
      return "text/plain ";
    }
}


void traitement_signal(int sig)
{
  printf("Signal %d recu\n",sig);
  waitpid(0, NULL, WNOHANG);
}

void initialiser_signaux(void)
{
  struct sigaction sa;

  if ( signal ( SIGPIPE , SIG_IGN ) == SIG_ERR )
    {
      perror ( " signal " );
    }

  sa.sa_handler = traitement_signal;

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if(sigaction(SIGCHLD,&sa,NULL) == -1)
    perror("sigaction(SIGCHLD)");

}


/**
 * Externalisation des tests d'entetes.
 * Stockage du protocole utilise.
 * Retour :
 * -1 si erreur 400
 * -2 si erreur 404
 *  0 si tout se passe bien
 **/
int verification_Header(char * message_client,char * cp_protocole){
  
  char *requete,*chemin,*protocole;
  
  requete = strtok(message_client, " ");
  if (strcmp(requete,"GET") == 0){
    if(! ((chemin=strtok(NULL, " ")) != NULL && (protocole=strtok(NULL, " ")) != NULL && strtok(NULL, " ") == NULL) )
      return -1;
  }

  strcpy(cp_protocole,protocole);
  
  if(!(strcmp(chemin,"/")==0))
    return -2 ;
  
  if(!(strcmp(protocole,"HTTP/1.0\r\n") == 0 || strcmp(protocole,"HTTP/1.1\r\n") == 0 ))
    return -1;
  
  return 0;
  
}

/**
 * Externalisation du traitement de chaque ligne après l'entete.
 * Retour :
 * -1 si la chaine ne se fini pas par \r\n
 *  0 si tout va bien
 **/
int verification_FinChaine(char * message_client){
  if(message_client[strlen(message_client)-1]=='\n'&&message_client[strlen(message_client)-2]=='\r')
    return 0;
  return -1;
}

/**
 * Externalisation du traitement des reponse d'erreur+vidage de la socket
 * aucun retour.
 **/
void traitement_err(FILE * fp, http_request *request){
  fprintf(stdout, "\ntarget: %s\n", request->target);

  web_stats * stats = get_stats();
  
  if(request->method == HTTP_UNSUPPORTED)
    {     
      stats->ko_400++;
      send_response (fp,400,"Bad Request","400 Bad Request (Method Unsupported)\r\n");       
    }
  else if (request->major_version != 1 || request->minor_version < 0 || request->minor_version > 1)
    {            
      stats->ko_400++;
      send_response (fp,400,"Bad Request","400 Bad request (Protocol Version Unsupported)\r\n");
    }
  else {
    stats->ko_404++;
    send_response (fp,404,"Not Found-","<h1> 404 Not Found</h1>\r\n");              
  }
}

/**
 *Il faudrat tester le tag 09 à l'iut, normalement il fonctionne
 *
 **/
void traitement_GET(char * message_client,FILE * fp, http_request *request, char *root,int socket){
  int succes,fd=0,size=-1;

  web_stats * stats = get_stats();
  
  char size_S [20];
  char type[512];
  fgets(message_client,512,fp); 
  // printf("\n --< %s\n",pwd);
  succes=parse_http_request(message_client, request);    
  request->target=rewrite_target(request->target);

  
  stats->served_requests++;

  if(strcmp(request->target,"/stats")!=0)
    {
      fd = check_and_open(request->target,root);      
    }


  skip_headers(fp);
  printf("\nsucces: %d ; message_client: %s; fd : %d \n",succes,message_client,fd);
  
  if(succes==0)
    {   
      traitement_err(fp, request);

    }
  else if(fd == -1)
    {
      traitement_err(fp, request);  
    }
  else if(strstr(request->target,"/../")!=NULL)
    {
      stats->ko_403++;
      send_response(fp, 403, "Forbidden", "403 Forbidden\r\n");
    }
  else if(strcmp(request->target,"/stats")==0)
    {
      send_stats(fp);
      fflush(fp);
    }
  else
    {
      get_stats()->ok_200++;
      
      size = get_file_size(fd);
      sprintf(type,"Content-Type: %s; charset=UTF-8\r\n",get_mime_type(request->target));
      sprintf(size_S,"Content-Length: %d\r\n\r\n",size);
    
      char * tmp = strcat(type,size_S);
      
      printf("%s -->tmp\n", tmp);
      
      
      send_response (fp, 200, "OK",tmp);
      copy(fd,socket);
      fflush(fp);      
    }

  fclose(fp);  
}



void traitement_client(int socket_client, http_request *request) 
{
  char message_client [512];
  char absolute[PATH_MAX+1];
    
  while(socket_client >= 0)
    {
      FILE *fp = fdopen(socket_client, "w+");
      
      //      	printf("\n --> %s - %s\n",pwd,ROOT);
      printf("\nrp : %s -- %s\n",realpath("content_web/",absolute),absolute);
	
      if(opendir(realpath(ROOT,absolute)) == NULL){
	traitement_err(fp, request);   
	perror("ERROR: Folder not found!");
      }

      traitement_GET(message_client,fp, request,absolute,socket_client);
      sleep(1);
      printf("Message client :\n");
    }
      
  exit(EXIT_SUCCESS);
}
  

int main(int argc, char **argv)
{
  int port = 8080;
  int pid = -1;
  int socket_client=-1;
  int socket_serveur=-1;
  char pwd[PATH_MAX+1];
  http_request request;
    
  if(argc > 1)
    {
      port = atoi(argv[1]);
    }

  getcwd(pwd,PATH_MAX+1);
  printf("chemin : %s\n",pwd);
  if(strstr(pwd,"content_web")!=NULL
     || strstr(pwd,"/webserver")==NULL ){
    printf("Veuillez vous placer dans le dossier pawnee/webserver\n");
    exit(EXIT_FAILURE);
  }
  

  printf("Creation du serveur sur le port %d.\n", port);
  socket_serveur = creer_serveur(port);

  memset(&request, '\0', sizeof(request));

  init_stats();
  
  while (1)
    {
      socket_client = accept(socket_serveur, NULL, NULL);
      if (socket_client == -1)
	{
	  perror(" accept ");
	}
      initialiser_signaux();
    

      pid = fork();
      if (pid == 0)
	{
	  traitement_client(socket_client, &request);
	}
      else
	{
	  get_stats()->served_connections++;
	  
	  if (close(socket_client) < 0)
	    {
	      perror(" close ");
	    }
	}
    }



  return 0;
}
