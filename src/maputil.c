#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DELIMITOR -666

#include "../include/error.h"

//Récupère et affiche sur la sortie standard la largeur
//Du fichier passé en paramètre
void getWidth(char *filename){

  int mapWidth = 0;

  int fd = open(filename, O_RDONLY);
  lseek(fd, 4, SEEK_SET);

  if(read(fd, &mapWidth, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture de la largeur de la carte dans le fichier");
  }

  close(fd);

  printf("Largeur de la carte du fichier %s : %d\n", filename, mapWidth);

}

//Récupère et affiche sur la sortie standard la hauteur
//Du fichier passé en paramètre
void getHeight(char *filename){

  int mapHeight = 0;

  int fd = open(filename, O_RDONLY);

  if(read(fd, &mapHeight, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture de la hauteur de la carte dans le fichier");
  }

  close(fd);

  printf("Hauteur de la carte du fichier %s : %d\n", filename, mapHeight);

}

//Récupère et affiche sur la sortie standard le nombre d'objets
//Du fichier passé en paramètre
void getObjects(char *filename){

  int mapNbObjects = 0;

  int fd = open(filename, O_RDONLY);
  lseek(fd, 8, SEEK_SET);

  if(read(fd, &mapNbObjects, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture du nombre d'objets de la carte dans le fichier");
  }

  close(fd);

  printf("Nombre d'objets de la carte du fichier %s : %d\n", filename, mapNbObjects);

}

//Utilise les 3 fonctions permettant de récupérer et d'afficher
//La largeur, la hauteur et le nombre d'objets de la carte du fichier passé
//En paramètre et les affiche sur la sortie standard
void getInfos(char *filename){

  getWidth(filename);
  getHeight(filename);
  getObjects(filename);

}

void increaseWidth(char *filename, int width){

    int fd = open(filename, O_RDONLY);
    int fdTmp = open("tmp", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    int data = 0;
    int newWidth = width;

    //int delimitor = DELIMITOR;

    read(fd, &data, sizeof(int));
    write(fdTmp, &data, sizeof(int));

    write(fdTmp, &newWidth, sizeof(int));

    lseek(fd, 8, SEEK_SET);
    read(fd, &data, sizeof(int));
    write(fdTmp, &data, sizeof(int));

    close(fd);
    close(fdTmp);

    while(read(fd, &data, sizeof(int))!=0){
        if(buffer == delimiteur){
          write(fdTmp, &delimiteur, sizeof(int));
          break;
        }
        else{
          read(fd, &largeurBuffer, sizeof(int));
          if(largeurBuffer < newWidth-1){
            write(fdTmp,&buffer, sizeof(int));
            write(fdTmp,&largeurBuffer, sizeof(int));
            read(fd, &buffer, sizeof(int));
            write(fdTmp,&buffer, sizeof(int));
          }
        }
        else{
          lseek(fd, 8, SEEK_CUR);
        }
    }

    fd = open("tmp", O_RDONLY);
    for(int i=0; i<3; i++){
      read(fd, &data, sizeof(int));
      printf("%d\n", data);
    }

}

void reduceWidth(){



}

void setWidth(char *filename, int newWidth){

  int initialWidth, widthDiff = 0;

  int fd = open(filename, O_RDONLY);
  lseek(fd, 4, SEEK_SET);

  if(read(fd, &initialWidth, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture de la largeur initiale de la carte dans le fichier");
  }

  close(fd);

  widthDiff = initialWidth - newWidth;

  if(widthDiff == 0){
    exit_with_error("Hauteur inchangée");
  }
  else if(widthDiff < 0){
    widthDiff *= -1;
    increaseWidth(filename, newWidth);
  }
  else{
    reduceWidth();
  }

}

int main(int argc, char *argv[]){

  if(strcmp(argv[2],"--getwidth") == 0){
    getWidth(argv[1]);
  }
  else if(strcmp(argv[2],"--getheight") == 0){
    getHeight(argv[1]);
  }
  else if(strcmp(argv[2],"--getobjects") == 0){
    getObjects(argv[1]);
  }
  else if(strcmp(argv[2],"--getinfos") == 0){
    getInfos(argv[1]);
  }
  else if(strcmp(argv[2],"-setwidth") == 0){
    setWidth(argv[1], atoi(argv[3]));
  }
  /*
  else if(strcmp(argv[2],"-setheight") == 0){
    modifyHeight(argv[1], atoi(argv[3]));
  }
  */
  else{
    exit_with_error("Mauvais paramètres :(\n");
  }

}
