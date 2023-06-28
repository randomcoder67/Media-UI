#include <gtk/gtk.h>
#include <stdbool.h>
#include "pathsAndStuff.h"

typedef struct fullData {
	char** allGames; // All the games 
	int maxGenre; // The largest genre (which is the size to "delimit" when navigating the array)
	int genres; // Number of genres 
	char** allGenres; // All the genres 
	int* genreLengths; // The genre lengths 
} fullData;

void remakeCurrent(int lenCurrent);
void getSize(GtkApplication *app, gpointer user_data);
bool isGameInCurrent(char* game);
void sortCurrentGames(int length);
void saveJSONToFile();
void initCurrentGames(const char* gamesToAdd, int index);
void removeGameFromCurrent(GtkApplication* app, gpointer data);
void addGameToCurrent(GtkApplication* app, gpointer data);
GtkWidget* addPanel(GtkStack* stack, char* name);
void makeImages(char** allGames, int numGames, int curGenre, int* genreLengths, GtkWidget** toAddTo);
void addGames(GtkWidget* scrolled, char** allGames, int numGames, int curGenre, int* genreLengths, void (*func)(GtkApplication *, void *), GtkWidget** toTakeFrom);
int initUI(fullData toUse);
