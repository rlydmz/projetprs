#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "map.h"
#include "error.h"

#ifdef PADAWAN

void map_new (unsigned width, unsigned height)
{

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

void map_save (char *filename)
{
  int hauteur = map_height();
  int largeur = map_width();
  int nbObjets = map_objects();
  int objetCourant = 0;
  int hTemp = 0;
  int lTemp = 0;
  int delimiteur = -666;

  //Overture du fichier en mode écriture seulement
  int saveFile = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

  //Sauvegarde de la hauteur de la carte
  write(saveFile, &hauteur, sizeof(hauteur));
  //Sauvegarde de la largeur de la carte
  write(saveFile, &largeur, sizeof(largeur));
  //Sauvegarde du nombre d'objets de la carte
  write(saveFile, &nbObjets, sizeof(nbObjets));

  for(int i=0; i<largeur; i++){
    for(int j=0; j<hauteur; j++){

      hTemp = j;
      lTemp = i;

      objetCourant = map_get(i,j);

      if(objetCourant != MAP_OBJECT_NONE){
        write(saveFile, &hTemp, sizeof(objetCourant));
        write(saveFile, &lTemp, sizeof(objetCourant));
        write(saveFile, &objetCourant, sizeof(objetCourant));
      }
    }
  }

write(saveFile, &delimiteur, sizeof(objetCourant));

//char * temp= map_get_name(0);
//write(saveFile, &temp, strlen(temp)*sizeof(char));

  int name = 0;
  int frame = 0;
  int solidity = 0;
  int destructible = 0;
  int collectible = 0;
  int generator = 0;

  for(int i=0; i<nbObjets; ++i){
    name = map_get_name(i);
    frame = map_get_frames(i);
    solidity = map_get_solidity(i);
    destructible = map_is_destructible(i);
    collectible = map_is_collectible(i);
    generator = map_is_generator(i);

    write(saveFile, &name, sizeof(int));
    write(saveFile, &frame, sizeof(int));
    write(saveFile, &solidity, sizeof(int));
    write(saveFile, &destructible, sizeof(int));
    write(saveFile, &collectible, sizeof(int));
    write(saveFile, &generator, sizeof(int));
  }

  close(saveFile);

  fprintf (stderr, "Okay morray\n");
}

void map_load (char *filename)
{

  int hauteur = 0;
  int largeur = 0;
  int nbObjets = 0;

  int y=0;
  int x=0;
  int texture = 0;


  int fileLoad = open(filename, O_RDONLY);

  read(fileLoad, &hauteur, sizeof(int));
  read(fileLoad, &largeur, sizeof(int));
  read(fileLoad, &nbObjets, sizeof(int));

  map_allocate(largeur, hauteur);


  while(read(fileLoad, &y, sizeof(int))!=0){
      if(y == -666){
        break;
      }
      else{
        read(fileLoad, &x, sizeof(int));
        read(fileLoad, &texture, sizeof(int));
      }
      map_set(x,y,texture);
  }

  close(fileLoad);

  map_object_begin (nbObjets);

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

#endif
