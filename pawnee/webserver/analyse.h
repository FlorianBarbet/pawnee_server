
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum http_method {
	HTTP_GET,
	HTTP_UNSUPPORTED,
};

typedef struct {
	enum http_method method;
	int major_version;
	int minor_version;
	char *target;
} http_request;

char *fgets_or_exit(char *buffer, int size, FILE *stream);
int parse_http_request ( const char * request_line , http_request * request );
void skip_headers(FILE *client);
