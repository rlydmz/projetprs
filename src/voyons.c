#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"


int main(int argc, char *argv[]){

  char * t;
  t = (char *)malloc(3);
  strcpy(t, "ok");
  for(int i=0; i<5; i++){
  t = (char *)realloc(t, 5);
    t[0] = 'o';
    t[1] = 'k';
    t[2] = 'o';
    t[3] = i+49;
    //t[4] = '\0';
    printf("%s\n", t);
  }
  free(t);

}
