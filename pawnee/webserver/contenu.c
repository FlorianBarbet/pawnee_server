#include "contenu.h"

char * rewrite_target(char * target)
{
  char *tmp = strdup(target);
  tmp = strtok(tmp, "?");
  printf("tmp : %s",tmp);
  
  if(strlen(tmp)==1 || tmp[strlen(tmp)-1]=='/'){
    tmp = strcat(tmp,"index.html");
  }
  
  return tmp;
}

int check_and_open(const char * target , const char * document_root)
{
  
  char * path = strcat(strdup(document_root),target);
  struct stat file;
  //  printf("path : %s",path);
  if(stat(path,&file)<0)
    {
      // perror("ERROR from check_and_open: "); si activé provoque un message d'erreur
      return -1;
    }
  
  if(S_ISREG(file.st_mode))
    {
      return open(path,O_RDONLY);
    }
  return -1;
    
}
    

int get_file_size(int fd){
  struct stat file;
  fstat(fd,&file);
  return file.st_size;
}

int copy(int in, int out){
  char buffer[512];
  int size,retour=-1;
  while((size=read(in,buffer,512)) != 0){
    retour += size;
    if( write(out,buffer,size)<0 ){
      perror("ERROR from copy(in,out) :");
    }
  }
  if(strlen(buffer)<512){
    buffer[strlen(buffer)]='\0';
  }

  return retour;
}

  
