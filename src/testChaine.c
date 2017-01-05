#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"


int main(int argc, char *argv[]){

  int fd = open("chaine", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  char c=0;
  int tailleChaine;
  int nbAl = 50;

  char* t = (char *)malloc(16);
  char* tt = (char *)malloc(18);
  char* ttt = (char *)malloc(18);
  strcpy(t, "Je suis Charlie");
  strcpy(tt, "Je suis Charlie!!");
  strcpy(ttt, "/texture/wall.png");
  printf("%s\n",t);
  tailleChaine = strlen(t);
  write(fd, &tailleChaine, sizeof(int));
  write(fd, t, strlen(t));
  tailleChaine = strlen(tt);
  write(fd, &tailleChaine, sizeof(int));
  write(fd, tt, strlen(tt));
  tailleChaine = strlen(ttt);
  write(fd, &tailleChaine, sizeof(int));
  write(fd, ttt, strlen(ttt));
  write(fd, &nbAl, sizeof(int));
  close(fd);




  fd = open("chaine", O_RDONLY);
  read(fd, &tailleChaine, sizeof(int));
  for(int i=0; i<tailleChaine; i++){
  read(fd, &c, sizeof(char));
  t[i] = c;
  printf("%c\n", c);
  }
  read(fd, &tailleChaine, sizeof(int));
  for(int i=0; i<tailleChaine; i++){
  read(fd, &c, sizeof(char));
  tt[i] = c;
  printf("%c\n", c);
  }
  read(fd, &tailleChaine, sizeof(int));
  for(int i=0; i<tailleChaine; i++){
  read(fd, &c, sizeof(char));
  ttt[i] = c;
  printf("%c\n", c);
  }
  close(fd);

  printf("%d\n", tailleChaine);
  printf("%s\n", t);
  printf("%s\n", tt);
  printf("%s\n", ttt);

  free(t);

}
