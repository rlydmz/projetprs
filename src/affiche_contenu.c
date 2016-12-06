#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "error.h"


int main(int argc, char *argv[]){
  int fd = open(argv[1], O_RDONLY);
  int donneeCourante = 0;
  while(read(fd, &donneeCourante, sizeof(int))!=0){
      printf("%d\n", donneeCourante);
  }
  close(fd);
}
