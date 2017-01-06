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

//Sous fonction de la fonction setWidth()
//Se charge de d'augmenter la taille de la carte
void increaseWidth(char *filename, int width, int widthDiff){

    int delimitor = DELIMITOR;
    int mapHeight, nbObjects = 0;
    int mapWidth = width;
    int oldWidth = 0;
    int buffer, textureChainLength, propTexture = 0;
    int x, y, idObj = 0;
    int widthTmp = widthDiff;

    int srcFile = open(filename, O_RDONLY);
    int tmpFile = open("saved2.map", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    read(srcFile, &mapHeight, sizeof(int));
    read(srcFile, &oldWidth, sizeof(int));
    read(srcFile, &nbObjects, sizeof(int));

    write(tmpFile, &mapHeight, sizeof(int));
    write(tmpFile, &mapWidth, sizeof(int));
    write(tmpFile, &nbObjects, sizeof(int));

    //On ne réécrit que les coordonnées qui ne correspondent pas au mur de droite
    while(read(srcFile, &y, sizeof(int))!=0){
        if(y == delimitor){
          break;
        }
        else{
          read(srcFile, &x, sizeof(int));
          read(srcFile, &idObj, sizeof(int));
          if(y>=0 && y<mapHeight-1 && x==oldWidth-1){
            continue;
          }
          else{
            write(tmpFile,&y, sizeof(int));
            write(tmpFile,&x, sizeof(int));
            write(tmpFile,&idObj, sizeof(int));
          }

        }
    }

    //Puis on ajoute les nouveaux sols
    idObj = 0;
    for(int i=oldWidth; i<mapWidth; i++){
      y = mapHeight-1;
      x = i;
      write(tmpFile,&y, sizeof(int));
      write(tmpFile,&x, sizeof(int));
      write(tmpFile,&idObj, sizeof(int));
    }

    //Et les nouveaux murs
    idObj = 1;
    for(int i=0; i<mapHeight-1; i++){
      y = i;
      x = mapWidth-1;
      write(tmpFile,&y, sizeof(int));
      write(tmpFile,&x, sizeof(int));
      write(tmpFile,&idObj, sizeof(int));
    }

    //On ecrit de nouveau le délimiteur
    write(tmpFile, &delimitor, sizeof(int));

    //Et on réecrit tout le reste du fichier, a savoir les propriétés des textures
    char currentCar = 0;

    while(read(srcFile, &textureChainLength, sizeof(int))){

      write(tmpFile, &textureChainLength, sizeof(int));

      for(int i=0; i<textureChainLength; i++){
        read(srcFile, &currentCar, sizeof(char));
        write(tmpFile, &currentCar, sizeof(char));
      }

      for(int j=0; j<5; j++){
        read(srcFile, &propTexture, sizeof(int));
        write(tmpFile, &propTexture, sizeof(int));
      }

    }

    close(srcFile);
    close(tmpFile);

    char command[1024];

    remove(filename);
    strcpy(command, "mv saved2.map ");
    strcat(command, filename);
    printf("%s\n", command);
    system(command);

    printf("%s\n", "Augmentation de la largeur de la carte effectuée avec succès");

}

//Sous fonction de la fonction setWidth()
//Se charge de reduire la taille de la carte
void reduceWidth(char *filename, int width, int widthDiff){

  int delimitor = DELIMITOR;
  int mapHeight, nbObjects = 0;
  int mapWidth = width;
  int oldWidth = 0;
  int buffer, textureChainLength, propTexture = 0;
  int x, y, idObj = 0;
  int widthTmp = widthDiff;
  int ok = 1;

  int srcFile = open(filename, O_RDONLY);
  int tmpFile = open("saved2.map", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  read(srcFile, &mapHeight, sizeof(int));
  read(srcFile, &oldWidth, sizeof(int));
  read(srcFile, &nbObjects, sizeof(int));

  write(tmpFile, &mapHeight, sizeof(int));
  write(tmpFile, &mapWidth, sizeof(int));
  write(tmpFile, &nbObjects, sizeof(int));

  //On ne réécrit que les coordonnées qui ne correspondent pas au mur de droite
  while(read(srcFile, &y, sizeof(int))!=0){
      if(y == delimitor){
        break;
      }
      else{
        read(srcFile, &x, sizeof(int));
        read(srcFile, &idObj, sizeof(int));
        if(y>=0 && y<mapHeight && x<mapWidth){
          write(tmpFile,&y, sizeof(int));
          write(tmpFile,&x, sizeof(int));
          write(tmpFile,&idObj, sizeof(int));
        }
      }
  }

  //Puis on ajoute les nouveaux murs
  idObj = 1;
  for(int i=0; i<mapHeight-1; i++){
    y = i;
    x = mapWidth-1;
    write(tmpFile,&y, sizeof(int));
    write(tmpFile,&x, sizeof(int));
    write(tmpFile,&idObj, sizeof(int));
  }

  //On ecrit de nouveau le délimiteur
  write(tmpFile, &delimitor, sizeof(int));

  //Et on réecrit tout le reste du fichier, a savoir les propriétés des textures
  char currentCar = 0;

  while(read(srcFile, &textureChainLength, sizeof(int))){

    write(tmpFile, &textureChainLength, sizeof(int));

    for(int i=0; i<textureChainLength; i++){
      read(srcFile, &currentCar, sizeof(char));
      write(tmpFile, &currentCar, sizeof(char));
    }

    for(int j=0; j<5; j++){
      read(srcFile, &propTexture, sizeof(int));
      write(tmpFile, &propTexture, sizeof(int));
    }

  }

  close(srcFile);
  close(tmpFile);

  char command[1024];

  remove(filename);
  strcpy(command, "mv saved2.map ");
  strcat(command, filename);
  printf("%s\n", command);
  system(command);

  printf("%s\n", "Reduction de la largeur de la carte effectuée avec succès");

}

void setWidth(char *filename, int newWidth){

  int initialWidth, widthDiff = 0;

  int fd = open(filename, O_RDONLY);
  lseek(fd, 4, SEEK_SET);

  if(newWidth<16 || newWidth>1024){
    exit_with_error("Echec lors du changement de la largeur de la carte. [16-1024]");
  }

  if(read(fd, &initialWidth, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture de la largeur initiale de la carte dans le fichier");
  }

  close(fd);

  widthDiff = initialWidth - newWidth;

  if(widthDiff == 0){
    exit_with_error("Hauteur inchangée \n");
  }
  else if(widthDiff < 0){
    widthDiff *= -1;
    increaseWidth(filename, newWidth, widthDiff);
  }
  else{
    reduceWidth(filename, newWidth, widthDiff);
  }

}

void increaseHeight(char *filename, int height, int heightDiff){

  int delimitor = DELIMITOR;
  int mapWidth, nbObjects = 0;
  int mapHeight = height;
  int oldHeight = 0;
  int buffer, textureChainLength, propTexture = 0;
  int x, y, idObj = 0;
  int heightTmp = heightDiff;
  int ok = 1;

  int srcFile = open(filename, O_RDONLY);
  int tmpFile = open("saved2.map", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  read(srcFile, &oldHeight, sizeof(int));
  read(srcFile, &mapWidth, sizeof(int));
  read(srcFile, &nbObjects, sizeof(int));

  write(tmpFile, &mapHeight, sizeof(int));
  write(tmpFile, &mapWidth, sizeof(int));
  write(tmpFile, &nbObjects, sizeof(int));

  //Puis on ajoute les nouveaux murs
  idObj = 1;
  for(int i=0; i<heightTmp; i++){

    y = i;

    x = mapWidth-1;
    write(tmpFile,&y, sizeof(int));
    write(tmpFile,&x, sizeof(int));
    write(tmpFile,&idObj, sizeof(int));

    x=0;
    write(tmpFile,&y, sizeof(int));
    write(tmpFile,&x, sizeof(int));
    write(tmpFile,&idObj, sizeof(int));

  }

  //On ne réécrit que les coordonnées qui ne correspondent pas au mur de droite
  while(read(srcFile, &y, sizeof(int))!=0){
      if(y == delimitor){
        break;
      }
      else{
        read(srcFile, &x, sizeof(int));
        read(srcFile, &idObj, sizeof(int));
        y += heightTmp;
        write(tmpFile,&y, sizeof(int));
        write(tmpFile,&x, sizeof(int));
        write(tmpFile,&idObj, sizeof(int));
        }
      }

  //On ecrit de nouveau le délimiteur
  write(tmpFile, &delimitor, sizeof(int));

  //Et on réecrit tout le reste du fichier, a savoir les propriétés des textures
  char currentCar = 0;

  while(read(srcFile, &textureChainLength, sizeof(int))){

    write(tmpFile, &textureChainLength, sizeof(int));

    for(int i=0; i<textureChainLength; i++){
      read(srcFile, &currentCar, sizeof(char));
      write(tmpFile, &currentCar, sizeof(char));
    }

    for(int j=0; j<5; j++){
      read(srcFile, &propTexture, sizeof(int));
      write(tmpFile, &propTexture, sizeof(int));
    }

  }

  close(srcFile);
  close(tmpFile);

  char command[1024];

  remove(filename);
  strcpy(command, "mv saved2.map ");
  strcat(command, filename);
  printf("%s\n", command);
  system(command);

  printf("%s\n", "Augmentation de la hauteur de la carte effectuée avec succès");


}

void reduceHeight(char *filename, int height, int heightDiff){

  int delimitor = DELIMITOR;
  int mapWidth, nbObjects = 0;
  int mapHeight = height;
  int oldHeight = 0;
  int buffer, textureChainLength, propTexture = 0;
  int x, y, idObj = 0;
  int heightTmp = heightDiff;
  int ok = 1;

  int srcFile = open(filename, O_RDONLY);
  int tmpFile = open("saved2.map", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  read(srcFile, &oldHeight, sizeof(int));
  read(srcFile, &mapWidth, sizeof(int));
  read(srcFile, &nbObjects, sizeof(int));

  write(tmpFile, &mapHeight, sizeof(int));
  write(tmpFile, &mapWidth, sizeof(int));
  write(tmpFile, &nbObjects, sizeof(int));

  //On ne réécrit que les coordonnées qui ne correspondent pas
  while(read(srcFile, &y, sizeof(int))!=0){
      if(y == delimitor){
        break;
      }
      else if(y >= heightTmp){
        y -= heightTmp;
        read(srcFile, &x, sizeof(int));
        read(srcFile, &idObj, sizeof(int));
        write(tmpFile,&y, sizeof(int));
        write(tmpFile,&x, sizeof(int));
        write(tmpFile,&idObj, sizeof(int));
      }
      else{
        lseek(srcFile, 8, SEEK_CUR);
      }
  }

  //On ecrit de nouveau le délimiteur
  write(tmpFile, &delimitor, sizeof(int));

  //Et on réecrit tout le reste du fichier, a savoir les propriétés des textures
  char currentCar = 0;

  while(read(srcFile, &textureChainLength, sizeof(int))){

    write(tmpFile, &textureChainLength, sizeof(int));

    for(int i=0; i<textureChainLength; i++){
      read(srcFile, &currentCar, sizeof(char));
      write(tmpFile, &currentCar, sizeof(char));
    }

    for(int j=0; j<5; j++){
      read(srcFile, &propTexture, sizeof(int));
      write(tmpFile, &propTexture, sizeof(int));
    }

  }

  close(srcFile);
  close(tmpFile);

  char command[1024];

  remove(filename);
  strcpy(command, "mv saved2.map ");
  strcat(command, filename);
  printf("%s\n", command);
  system(command);

  printf("%s\n", "Reduction de la hauteur de la carte effectuée avec succès");


}

void setHeight(char *filename, int newHeight){

  int initialHeight, heightDiff = 0;

  if(newHeight<12 || newHeight>20){
    exit_with_error("Echec lors du changement de la largeur de la carte. [16-1024]");
  }

  int fd = open(filename, O_RDONLY);

  if(read(fd, &initialHeight, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture de la largeur initiale de la carte dans le fichier");
  }

  close(fd);

  heightDiff = initialHeight - newHeight;

  if(heightDiff == 0){
    exit_with_error("Hauteur inchangée \n");
  }
  else if(heightDiff < 0){
    heightDiff *= -1;
    increaseHeight(filename, newHeight, heightDiff);
  }
  else{
    reduceHeight(filename, newHeight, heightDiff);
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
  else if(strcmp(argv[2],"--setwidth") == 0){
    setWidth(argv[1], atoi(argv[3]));
  }
  else if(strcmp(argv[2],"--setheight") == 0){
    setHeight(argv[1], atoi(argv[3]));
  }
  else{
    exit_with_error("Mauvais paramètres :(\n");
  }

}
