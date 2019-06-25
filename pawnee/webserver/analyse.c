#include "analyse.h"
#include "reponse.h"

char *fgets_or_exit(char *buffer, int size, FILE *stream) {
  char * tmp_buff = buffer;
  char * ret = fgets(buffer, size, stream);
  if(ret == NULL) {
    if(tmp_buff[0]!='\r' || tmp_buff[1]!='\n')
      send_response(stream,400,"Bad Request","Bad Request (Line Sepparator Not Found)\r\n");
    exit(EXIT_SUCCESS);
  }
  return ret;
}

int parse_http_request ( const char * request_line , http_request * request ){
  char *method,*version,*target;
  int major_version_i,minor_version_i;

  int success = 1;
  char *copy;
  copy = strdup(request_line);
  method = strtok(copy, " ");
  target = strtok(NULL, " ");
  version = strtok(NULL, "/");
  major_version_i = version[5] - 48;

  minor_version_i = version[7] - 48;

  if (strcmp(method, "GET") == 0)
    {
      request -> method=HTTP_GET;
    }
  else
    {
      request -> method=HTTP_UNSUPPORTED;
      success = 0;
      fprintf(stdout, "method");
      free(copy);
    }

  request -> major_version=major_version_i;  
  request -> minor_version=minor_version_i;

  
  
  if (major_version_i != 1 || minor_version_i < 0 || minor_version_i > 1)
    {
      success = 0;
      // fprintf(stdout, "version %i.%i", major_version_i, minor_version_i);
    }
  
  request -> target=target;
  if (request->target[0]!='/')
    {
      success = 0;
      //  fprintf(stdout, "target");
    }
  
  return success;
}

void skip_headers(FILE *client) {
  char buffer[1024];
  while(fgets_or_exit(buffer, sizeof(buffer), client)!=NULL&&(strncmp(buffer, "\r\n", 2) != 0)) {

  }

}
