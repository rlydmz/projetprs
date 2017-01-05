#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"


int main(int argc, char *argv[]){

  int fd = open(argv[1], O_RDONLY);

  char c=0;
  char *t = (char *)malloc(3);
  strcpy(t, "ok");
  int donneeInteger, nbObjects = 0;
  int tailleChaine = 0;

  read(fd, &donneeInteger, sizeof(int));
  printf("%d\n", donneeInteger);
  read(fd, &donneeInteger, sizeof(int));
  printf("%d\n", donneeInteger);
  read(fd, &nbObjects, sizeof(int));
  printf("%d\n", nbObjects);

  while (read(fd, &donneeInteger, sizeof(int))) {
    if(donneeInteger == -666){
      break;
    }
    else{
      printf("y : %d\n", donneeInteger);
      read(fd, &donneeInteger, sizeof(int));
      printf("x : %d\n", donneeInteger);
      read(fd, &donneeInteger, sizeof(int));
      printf("id t : %d\n", donneeInteger);
    }
  }
  char as = 0;
  for(int i=0; i<nbObjects; i++){
    read(fd, &tailleChaine, sizeof(int));
    printf("%d\n", tailleChaine);
    t = (char *)realloc(t,tailleChaine+1);
    for(int j=0; j<tailleChaine; j++){
      read(fd, &c, sizeof(char));
      t[j] = c;
      as = j;
    }
    as++;
    t[as] = '\0';

    printf("%s\n", t);

    read(fd, &donneeInteger, sizeof(int));
    printf("frames : %d\n", donneeInteger);
    read(fd, &donneeInteger, sizeof(int));
    printf("solidite : %d\n", donneeInteger);
    read(fd, &donneeInteger, sizeof(int));
    printf("destructible : %d\n", donneeInteger);
    read(fd, &donneeInteger, sizeof(int));
    printf("collectible : %d\n", donneeInteger);
    read(fd, &donneeInteger, sizeof(int));
    printf("generateur : %d\n", donneeInteger);

  }
  free(t);
  close(fd);



}
