#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

char * rewrite_target ( char * target );
int check_and_open ( const char * target , const char * document_root );
int get_file_size(int fd);
int copy(int in,int out);
