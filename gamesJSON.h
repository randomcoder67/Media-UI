#include <stdbool.h>
#include <stdio.h>
#include <json-c/json.h>
#include <string.h>
#include "pathsAndStuff.h"

#ifndef _GAMESJSONH_
#define _GAMESJSONH_

int getLength(json_object* root);
void getGenres(json_object* root, char** arrayA);
void getGenreLengths(json_object* root, int lengths[]);
void getGamesFromGenre(json_object* root, char** arrayA, char* genreName, int genreLength);

#endif
