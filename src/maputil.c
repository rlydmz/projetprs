#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DELIMITEUR -666

#include "../include/error.h"

void diminuerHauteur(char* filename, int height, int diffHauteur){

  int fd = open(filename, O_RDONLY);
  int fdTmp = open("tmp", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  int hauteurMap;
  int hauteurTmp;
  int largeurMap;
  int nbObjects;

  int delimiteur = DELIMITEUR;

  read(fd, &hauteurMap, sizeof(int));
  read(fd, &largeurMap, sizeof(int));
  read(fd, &nbObjects, sizeof(int));
  hauteurTmp = diffHauteur;


  write(fdTmp, &height,sizeof(int));
  write(fdTmp, &largeurMap,sizeof(int));
  write(fdTmp, &nbObjects,sizeof(int));

  int buffer;

  //On lit le fichier tant qu'on ne rencontre pas le délimiteur
  //On vérifie si les coordonnées du fichier source doivent être exportées
  //Dans le fichier de destination
  //SI oui on écrit la coordonnée avec sa Texture
  //SINON on saute à la paire (x,y,texture) suivante
  while(read(fd, &buffer, sizeof(int))!=0){
      if(buffer == delimiteur){
        write(fdTmp, &delimiteur, sizeof(int));
        break;
      }
      else if(buffer>= hauteurTmp){
        buffer = buffer - hauteurTmp;
        write(fdTmp,&buffer, sizeof(int));
        read(fd, &buffer, sizeof(int));
        write(fdTmp,&buffer, sizeof(int));
        read(fd, &buffer, sizeof(int));
        write(fdTmp,&buffer, sizeof(int));

      }
      else{
        lseek(fd, 8, SEEK_CUR);
      }
  }

  //On le reste du fichier source dans le fichier de destination
  //C'est à dire les informations des textures
  while(read(fd, &buffer, sizeof(int))!=0){
        write(fdTmp,&buffer, sizeof(int));
  }


  close(fd);
  close(fdTmp);

  remove(filename);


  fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  fdTmp = open("tmp", O_RDONLY);

  while(read(fdTmp, &buffer, sizeof(int))!=0){
        write(fd,&buffer, sizeof(int));
  }

  close(fd);
  close(fdTmp);

  remove("tmp");

}

void augmenterHauteur(char* filename, int height, int diffHauteur){

  int fd = open(filename, O_RDONLY);
  int fdTmp = open("tmp", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  int hauteurMap;
  int hauteurTmp;
  int largeurMap;
  int nbObjects;

  int delimiteur = DELIMITEUR;

  read(fd, &hauteurMap, sizeof(int));
  read(fd, &largeurMap, sizeof(int));
  read(fd, &nbObjects, sizeof(int));
  hauteurTmp = diffHauteur;


  write(fdTmp, &height,sizeof(int));
  write(fdTmp, &largeurMap,sizeof(int));
  write(fdTmp, &nbObjects,sizeof(int));

  int buffer;

  int x, y = 0;
  int text = 1;

  for(int i=0; i<diffHauteur; i++){

    y = i;

    x = 0;
    write(fdTmp, &y, sizeof(int));
    write(fdTmp, &x, sizeof(int));
    write(fdTmp, &text, sizeof(int));

    x = largeurMap-1;
    write(fdTmp, &y, sizeof(int));
    write(fdTmp, &x, sizeof(int));
    write(fdTmp, &text, sizeof(int));

  }

  //On lit le fichier tant qu'on ne rencontre pas le délimiteur
  //On vérifie si les coordonnées du fichier source doivent être exportées
  //Dans le fichier de destination
  //SI oui on écrit la coordonnée avec sa Texture
  //SINON on saute à la paire (x,y,texture) suivante
  while(read(fd, &buffer, sizeof(int))!=0){
      if(buffer == delimiteur){
        write(fdTmp, &delimiteur, sizeof(int));
        break;
      }
      else{
        buffer = buffer + hauteurTmp;
        write(fdTmp,&buffer, sizeof(int));
        read(fd, &buffer, sizeof(int));
        write(fdTmp,&buffer, sizeof(int));
        read(fd, &buffer, sizeof(int));
        write(fdTmp,&buffer, sizeof(int));
      }
  }

  //On le reste du fichier source dans le fichier de destination
  //C'est à dire les informations des textures
  while(read(fd, &buffer, sizeof(int))!=0){
        write(fdTmp,&buffer, sizeof(int));
  }


  close(fd);
  close(fdTmp);

  remove(filename);


  fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  fdTmp = open("tmp", O_RDONLY);

  while(read(fdTmp, &buffer, sizeof(int))!=0){
        write(fd,&buffer, sizeof(int));
  }

  close(fd);
  close(fdTmp);

  remove("tmp");

}

void diminuerLargeur(char* filename, int width, int diffLargeur){

  int fd = open(filename, O_RDONLY);
  int fdTmp = open("tmp", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  int height;
  int nbObjects;

  int delimiteur = DELIMITEUR;

  read(fd, &height, sizeof(int));
  lseek(fd, 4, SEEK_CUR);
  read(fd, &nbObjects, sizeof(int));

  int newWidth = width;

  write(fdTmp, &height,sizeof(int));
  write(fdTmp, &newWidth,sizeof(int));
  write(fdTmp, &nbObjects,sizeof(int));

  int buffer;
  int largeurBuffer;

  //On lit le fichier tant qu'on ne rencontre pas le délimiteur
  //On vérifie si les coordonnées du fichier source doivent être exportées
  //Dans le fichier de destination
  //SI oui on écrit la coordonnée avec sa Texture
  //SINON on saute à la paire (x,y,texture) suivante

  while(read(fd, &buffer, sizeof(int))!=0){
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

  //On le reste du fichier source dans le fichier de destination
  //C'est à dire les informations des textures
  while(read(fd, &buffer, sizeof(int))!=0){
        write(fdTmp,&buffer, sizeof(int));
  }


  close(fd);
  close(fdTmp);

  remove(filename);


  fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  fdTmp = open("tmp", O_RDONLY);

  while(read(fdTmp, &buffer, sizeof(int))!=0){
        write(fd,&buffer, sizeof(int));
  }

  close(fd);
  close(fdTmp);

  remove("tmp");

}


void modifyHeight(char* filename, int height){

  int fd = open(filename, O_RDONLY);
  int hauteurInitiale, diffHauteur = 0;
  read(fd, &hauteurInitiale, sizeof(int));
  close(fd);

  diffHauteur = hauteurInitiale - height;

  if(diffHauteur == 0){
    exit_with_error("Hauteur inchangee \n");
  }
  else if(diffHauteur<0){
    //Augmente la hauteur
    diffHauteur *= -1;
    augmenterHauteur(filename, height, diffHauteur);
  }
  else{
    //Diminuer hauteur
    diminuerHauteur(filename, height, diffHauteur);
  }

}

void modifyWidth(char* filename, int width){

  int fd = open(filename, O_RDONLY);
  int largeurInitiale, diffLargeur = 0;
  lseek(fd, 4, SEEK_SET);
  read(fd, &largeurInitiale, sizeof(int));
  close(fd);

  diffLargeur = largeurInitiale - width;

  if(diffLargeur == 0){
    exit_with_error("Hauteur inchangee \n");
  }
  else if(diffLargeur<0){
    //Augmente la hauteur
    diffLargeur *= -1;
    augmenterLargeur(filename, width, diffLargeur);
  }
  else{
    //Diminuer hauteur
    diminuerLargeur(filename, width, diffLargeur);
  }

}

int main(int argc, char *argv[]){

  int donnee = 0;
  char *message;

  int fd = open(argv[1], O_RDONLY);

  if(strcmp(argv[2],"-getwidth") == 0){
    lseek(fd, 4, SEEK_SET);
    read(fd, &donnee, sizeof(int));
  }
  else if(strcmp(argv[2],"-getheight") == 0){
    lseek(fd, 0, SEEK_SET);
    read(fd, &donnee, sizeof(int));
  }
  else if(strcmp(argv[2],"-getobjects") == 0){
    lseek(fd, 8, SEEK_SET);
    read(fd, &donnee, sizeof(int));
  }
  else if(strcmp(argv[2],"-setwidth") == 0){
    modifyWidth(argv[1], atoi(argv[3]));
  }
  else if(strcmp(argv[2],"-setheight") == 0){
    modifyHeight(argv[1], atoi(argv[3]));
  }
  else{
    exit_with_error("Mauvais paramètres :(\n");
  }

  close(fd);

  printf("%d\n", donnee);

}
