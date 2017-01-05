#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

void map_new (unsigned width, unsigned height){
  map_allocate (width, height);

  for (int x = 0; x < width; x++)
    map_set (x, height - 1, 0); // Ground

  for (int y = 0; y < height - 1; y++) {
    map_set (0, y, 1); // Wall
    map_set (width - 1, y, 1); // Wall
  }

  map_object_begin (7);

  // Texture pour le sol
  map_object_add ("images/ground.png", 1, MAP_OBJECT_SOLID);
  // Mur
  map_object_add ("images/wall.png", 1, MAP_OBJECT_SOLID);
  // Gazon
  map_object_add ("images/grass.png", 1, MAP_OBJECT_SEMI_SOLID);
  // Marbre
  map_object_add ("images/marble.png", 1, MAP_OBJECT_SOLID | MAP_OBJECT_DESTRUCTIBLE);
  // Fleur1
  map_object_add ("images/flower.png", 1, MAP_OBJECT_AIR);
  // Fleur2
  map_object_add ("images/flower2.png", 1, MAP_OBJECT_AIR);
  // Piece
  map_object_add ("images/coin.png", 20, MAP_OBJECT_AIR | MAP_OBJECT_COLLECTIBLE);

  map_object_end ();

}

void map_save (char *filename){

  int textureChainLength, hTemp, wTemp, currentObject = 0;
  int frame, solidity, destructible, collectible, generator = 0;
  int delimiteur = -666;

  //Récupération de la hauteur de la map
  int mapHeight = map_height();
  //Récupération de la largeur de la map
  int mapWidth = map_width();
  //Récupération du nombre d'objets présents dans la map
  int nbObjects = map_objects();

  //Ouverture du fichier en mode écriture seulement
  int saveFile = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

  //Ecriture de la hauteur, de la largeur et du nombre d'objets de la map
  if(write(saveFile, &mapHeight, sizeof(int)) == -1){
    exit_with_error("Echec de l'ecriture de la hauteur dans le fichier");
  }
  if(write(saveFile, &mapWidth, sizeof(int)) == -1){
    exit_with_error("Echec de l'ecriture de la largeur dans le fichier");
  }
  if(write(saveFile, &nbObjects, sizeof(int)) == -1){
    exit_with_error("Echec de l'ecriture du nombre d'objets dans le fichier");
  }

  //Ecriture des coordonnées et du numéro de la texture pour chaque
  //Coordonnée de la map. S'il n'y a pas de textures à une coordonnée
  //On n'écrit rien dans le fichier
  for(int i=0; i<mapWidth; i++){
    for(int j=0; j<mapHeight; j++){

      hTemp = j;
      wTemp = i;

      currentObject = map_get(i,j);

      if(currentObject != MAP_OBJECT_NONE){
        if(write(saveFile, &hTemp, sizeof(int)) == -1){
          exit_with_error("Echec de l'ecriture de la coordonnées y de l'objet dans le fichier");
        }
        if(write(saveFile, &wTemp, sizeof(int)) == -1){
          exit_with_error("Echec de l'ecriture de la coordonnées x de l'objet dans le fichier");
        }
        if(write(saveFile, &currentObject, sizeof(int)) == -1){
          exit_with_error("Echec de l'ecriture de l'ID de l'objet dans le fichier");
        }
      }
    }
  }

  //Ecriture de l'entier délimiteur qui indique à partir de quand on arrête
  //D'écrire (ou de lire) les coordonnées des objets et où on commence à écriture
  //Les propriétés des objets
  if(write(saveFile, &delimiteur, sizeof(int)) == -1){
    exit_with_error("Echec de l'ecriture du delimiteur dans le fichier");
  }

  char* textureName;

  //Ecriture des noms des textures de la map dans le fichier
  //Ainsi que de leurs propriétés
  for(int i = 0; i<nbObjects; i++){
    textureChainLength = strlen(map_get_name(i));

    if(write(saveFile, &textureChainLength, sizeof(int)) == -1){
      exit_with_error("Echec de l'ecriture de la longueur du nom de la texture dans le fichier");
    }

    textureName = (char *)malloc(strlen(map_get_name(i))+1);
    strcpy(textureName, map_get_name(i));
    if(write(saveFile, textureName, strlen(textureName)) == -1){
      exit_with_error("Echec de l'ecriture du delimiteur dans le fichier");
    }
    free(textureName);

    frame = map_get_frames(i);
    solidity = map_get_solidity(i);
    destructible = map_is_destructible(i);
    collectible = map_is_collectible(i);
    generator = map_is_generator(i);

    if(write(saveFile, &frame, sizeof(int)) == -1){
      exit_with_error("Echec de l'ecriture du nombre de frame de la texture dans le fichier");
    }
    if(write(saveFile, &solidity, sizeof(int)) == -1){
      exit_with_error("Echec de l'ecriture de la solidité de la texture dans le fichier");
    }
    if(write(saveFile, &destructible, sizeof(int)) == -1){
      exit_with_error("Echec de l'ecriture de la destructibilité de la texture dans le fichier");
    }
    if(write(saveFile, &collectible, sizeof(int)) == -1){
      exit_with_error("Echec de l'ecriture de la collectibilité de la texture dans le fichier");
    }
    if(write(saveFile, &generator, sizeof(int)) == -1){
      exit_with_error("Echec de l'ecriture de la capacité de génération de la texture dans le fichier");
    }

  }

  //Fermeture du fichier
  close(saveFile);

  printf("Sauvegarde réussie\n");

}

void map_load (char *filename)
{

  int fileLoad = open(filename, O_RDONLY);

  char c = 0;
  int indexBackslash = 0;
  int nbObjects, mapHeight, mapWidth = 0;
  int xObj, yObj, idObj;
  int nbFrames, solidity, destructible, collectible, generator;
  int tagTexture;
  int textureChainLength = 0;

  char *t = malloc(3); strcpy(t, "ok");

  //Récupération des dimensions de la map ainsi que
  //Du nombre d'objets présents dans celle-ci
  if(read(fileLoad, &mapHeight, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture de la hauteur de la carte dans le fichier");
  }
  if(read(fileLoad, &mapWidth, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture de la largeur de la carte dans le fichier");
  }
  if(read(fileLoad, &nbObjects, sizeof(int)) == -1){
    exit_with_error("Echec de la lecture du nombre d'objets de la carte dans le fichier");
  }

  map_allocate(mapWidth, mapHeight);

  //Récupération des coordonnés de la map où il y a une texture.
  //Puis on l'ajoute à la map crée
  while(read(fileLoad, &yObj, sizeof(int))!=-0){
      if(yObj == -666){
        break;
      }
      else{
        if(read(fileLoad, &xObj, sizeof(int)) == -1){
          exit_with_error("Echec de la lecture de la coordonnée x de l'objet dans le fichier");
        }
        if(read(fileLoad, &idObj, sizeof(int)) == -1){
          exit_with_error("Echec de la lecture de l'ID l'objet dans le fichier");
        }
      }
      map_set(xObj,yObj,idObj);
  }

  map_object_begin (nbObjects);

  //Récupération des propriétés de chacuns des objets
  //Tout d'abord on reconstitue la chaine de caractère correspondant
  //Au chemin menant à la texture
  for(int i=0; i<nbObjects; i++){
    if(read(fileLoad, &textureChainLength, sizeof(int)) == -1){
      exit_with_error("Echec de la lecture de la longueur du nom de la texture dans le fichier");
    }
    t = (char *)realloc(t, textureChainLength+1);
    for(int j=0; j<textureChainLength; j++){
      if(read(fileLoad, &c, sizeof(char)) == -1){
        exit_with_error("Echec de la lecture d'un caractère dans le nom de la texture dans le fichier");
      }
      t[j] = c;
      indexBackslash = j;
    }
    indexBackslash++;
    t[indexBackslash] = '\0';

    //Puis on récupère l'ensenble de ses propriétés
    if(read(fileLoad, &nbFrames, sizeof(int)) == -1){
      exit_with_error("Echec de la lecture du nombre de frames de la texture dans le fichier");
    }
    if(read(fileLoad, &solidity, sizeof(int)) == -1){
      exit_with_error("Echec de la lecture de la solidité de la texture dans le fichier");
    }
    if(read(fileLoad, &destructible, sizeof(int)) == -1){
      exit_with_error("Echec de la lecture de la destructibilité de la texture dans le fichier");
    }
    if(read(fileLoad, &collectible, sizeof(int)) == -1){
      exit_with_error("Echec de la lecture de la collectibilité de la texture dans le fichier");
    }
    if(read(fileLoad, &generator, sizeof(int)) == -1){
      exit_with_error("Echec de la lecture de la capacité de génération de la texture dans le fichier");
    }

    tagTexture = solidity;

    //Pour construire le tag final
    if(destructible == 1){
      tagTexture = tagTexture | MAP_OBJECT_DESTRUCTIBLE;
    }
    if(collectible == 1){
      tagTexture = tagTexture | MAP_OBJECT_COLLECTIBLE;
    }
    if(generator == 1){
      tagTexture = tagTexture | MAP_OBJECT_GENERATOR;
    }

    map_object_add(t, nbFrames, tagTexture);

  }

  free(t);

  map_object_end();

  close(fileLoad);

  printf("Chargement réussi !\n");

}

#endif
