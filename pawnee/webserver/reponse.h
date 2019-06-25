#include <stdio.h>
#include <string.h>

void send_status(FILE *client, int code, const char *reason_phrase);
void send_response(FILE * client, int code, const char *reson_phrase,const char *message_body);
