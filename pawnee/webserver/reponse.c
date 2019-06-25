#include "reponse.h"
// Il faut trouver un moyen d'afficher le protocole utilisé par l'utilisateur...
void send_status(FILE *client, int code, const char *reason_phrase) {
  fprintf(client, "HTTP/1.1 %d %s\r\n", code, reason_phrase);
  if(code != 200) {
    fprintf(client, "Connection: close\r\n");
  }
}

void send_response(FILE *client, int code, const char *reason_phrase, const char *message_body) {
  send_status(client,code, reason_phrase);
  if(code != 200){
    fprintf(client, "Content-Length: %zd\r\n\r\n%s", strlen(message_body), message_body);
    // Ci dessous c'est pour le debugging
    printf("Content-Length: %zd\r\n\r\n%s", strlen(message_body), message_body);
  }else{
    fprintf(client, "%s", message_body);
  }
  
  fflush(client);
}
