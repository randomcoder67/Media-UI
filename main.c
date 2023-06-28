#include "gamesUI.h"
#include "gamesJSON.h"

int main() {
	// Init filenames
	int lengthAllFilename = strlen(getenv("HOME")) + strlen(ALL_GAMES_JSON_PATH) + 1;
	int lengthCurrentFilename = strlen(getenv("HOME")) + strlen(CURRENT_SAVE_PATH) + 1;
	char fileNameAllGames[lengthAllFilename];
	char fileNameCurrentGames[lengthCurrentFilename];
	strcpy(fileNameAllGames, getenv("HOME")); strcat(fileNameAllGames, ALL_GAMES_JSON_PATH);
	strcpy(fileNameCurrentGames, getenv("HOME")); strcat(fileNameCurrentGames, CURRENT_SAVE_PATH);
	
	// Get current.json
	json_object* current_root = json_object_from_file(fileNameCurrentGames);
	if (!current_root) {
		printf("JSON file not found\n");
		exit(1);
	}
	
	// Init currentGames array in gamesUI.c
	json_object* current = json_object_object_get(current_root, "current");
	int lengthOfCurrent = json_object_array_length(current);
	for (int i=0; i<lengthOfCurrent; i++) {
		initCurrentGames(json_object_get_string(json_object_array_get_idx(current, i)), i);
	}
	sortCurrentGames(lengthOfCurrent);	

	// Get root object 
	json_object* root = json_object_from_file(fileNameAllGames);
	if (!root) {
		printf("JSON file not found\n");
		exit(1);
	}
	
	// Get array of genres 
	//int numGenres = getLength(root);
	printf("Number of Genres: %d\n", NUM_GENRES);
	char* genres[NUM_GENRES];
	getGenres(root, genres);
	
	// Find the largest genre 
	int genreLengths[NUM_GENRES];
	getGenreLengths(root, genreLengths);
	int biggestGenre = 0;
	for (unsigned int i=0; i<sizeof(genreLengths)/sizeof(int); i++) {
		if (genreLengths[i] > biggestGenre) {
			biggestGenre = genreLengths[i];
		}
	}
	printf("Largest Genre: %d\n", biggestGenre);
	
	// Get array of arrays of games (each array is a genre)
	char* allGames[NUM_GENRES][biggestGenre];
	for (int i=0; i<NUM_GENRES; i++) {
		getGamesFromGenre(root, allGames[i], genres[i], genreLengths[i]);
	}
	
	// Pack everything into a struct
	fullData toUse;
	toUse.allGames = allGames[0];
	toUse.maxGenre = biggestGenre;
	toUse.genres = NUM_GENRES;
	toUse.allGenres = genres;
	toUse.genreLengths = genreLengths;
	
	// Make UI
	initUI(toUse);
	
	// Save currentGames to JSON file
	saveJSONToFile();
	
	// Free JSON objects
	json_object_put(root);
	json_object_put(current_root);
	
	return 0;
}
