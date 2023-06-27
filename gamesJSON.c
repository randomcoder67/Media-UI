#include "gamesJSON.h"

// Necessary for qsort
static int doCompare(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}

// Get number of keys in a json object
int getLength(json_object* root) {
	int length = 0;
	json_object_object_foreach(root, key, val) {
		length++;
	}
	return length;
}

// Get the length of each game genre
void getGenreLengths(json_object* root, int lengths[]) {
	int index = 0;
	json_object_object_foreach(root, key, val) {
		int lengthA = 0;
		json_object* genreA = json_object_object_get(root, key); // Get specific genre
		json_object_object_foreach(genreA, keyA, valA) { // Count number of games
			lengthA++;
		}
		lengths[index] = lengthA; // Add to array
		index++;
	}
}

// Get names of genres 
void getGenres(json_object* root, char** arrayA) {
	int i = 0;
	json_object_object_foreach(root, key, val) {
		arrayA[i] = key;
		i++;
	}
}

// Get the games from a specific genre
void getGamesFromGenre(json_object* root, char* arrayA[], char* genreName, int genreLength) {
	printf("%s\n", genreName);
	json_object* genre = json_object_object_get(root, genreName);
	int i = 0;
	
	// First make temp array with only the needed length
	char* tempArray[genreLength];
	json_object_object_foreach(genre, key, val) {
		if (i == genreLength) {
			break;
		}
		tempArray[i] = key;
		i++;
	}
	// Sort it
	qsort(tempArray, genreLength, sizeof(const char*), doCompare);
	
	// Then copy the values to the given array
	for (int i=0; i<genreLength; i++) {
		printf("%s\n", tempArray[i]);
		arrayA[i] = tempArray[i];
	}
	printf("\n\n");
}
