#include "stats.h"
#include "reponse.h"

static web_stats stats ;

void send_stats ( FILE * client ){
  char message[2048];
  char temp[8];
  strcat(message, "<h1>Statistiques</h1><ul>");
  strcat(message, "<li>served_connections: ");
  sprintf(temp, "%d", stats.served_connections);
  strcat(message, temp);
  strcat(message, "</li><li>served_requests: ");
  sprintf(temp, "%d", stats.served_requests);
  strcat(message, temp);
  strcat(message, "</li><li>ok_200: ");
  sprintf(temp, "%d", stats.ok_200);
  strcat(message, temp);
  strcat(message, "</li><li>ko_400: ");
  sprintf(temp, "%d", stats.ko_400);
  strcat(message, temp);
  strcat(message, "</li><li>ko_403: ");
  sprintf(temp, "%d", stats.ko_403);
  strcat(message, temp);
  strcat(message, "</li><li>ko_404: ");
  sprintf(temp, "%d", stats.ko_404);
  strcat(message, temp);
  strcat(message, "</li></ul>");

  
  int size = strlen(message);
  char size_S[100];
  sprintf(size_S,"Content-Length: %d\r\n\r\n",size);
  
  
  send_response (client, 200, "OK",strcat(strdup(size_S),strdup(message)));
  fprintf(client,"%s1",message);
  
}

int init_stats ( void ){
  stats.served_connections = 0;
  stats.served_requests = 0;
  stats.ok_200 = 0;
  stats.ko_400 = 0;
  stats.ko_403 = 0;
  stats.ko_404 = 0;
  
  return 0;
}

web_stats * get_stats ( void ){
  return &stats;
}
