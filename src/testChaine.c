#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"


int main(int argc, char *argv[]){

  //int fd = open("chaine", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  char* t = "Je suis Charlie";
  printf("%s\n",t);

  char *stringT = (char *)malloc((strlen(t)+1)*sizeof(char));
  strcpy(stringT, "Je suis Charlie");

  int fd = open("texte", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  write(fd, &stringT, strlen(stringT)*sizeof(char));
  //write(fd, &as, sizeof(char));
  close(fd);
  free(stringT);

}
