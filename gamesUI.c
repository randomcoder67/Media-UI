#include "gamesUI.h"

static int gridWidth = 4;
static GtkWindow *window; // The window
static char* currentGames[MAX_CURRENT_GAMES]; // The array of current games (max 50) (need to add handling for if it goes over 50)
static GtkWidget* currentScrolled; // The scrolled widget for the "Current" tab
static GtkWidget* theWidgets[NUM_GENRES]; // The other scrolled widgets
static GtkWidget* theImages[NUM_GENRES*MAX_GAMES_PER_GENRE]; // The images
static GtkWidget* currentImages[MAX_CURRENT_GAMES]; // The images

// Function to check the size of the window and resize if necessary (i.e. if there is room for more/not enough room for the grids)
void getSize(GtkApplication *app, gpointer user_data) {
	int h, w;
	gtk_window_get_size(window, &w, &h);
	int stackWidth = w - 176; // The sidebar takes up 176 pixels
	
	// If the correct grid width is already inplace, return
	if (stackWidth/220 == gridWidth) {
		return;	
	}
	// Otherwise set the new grid width and remake the widgets
	else {
		gridWidth = stackWidth/220;
	}

	fullData* toUse = (fullData*)user_data;
	//int numGenres = toUse->genres; // Number of genres
	
	for (int i=0; i<NUM_GENRES; i++) {
		addGames(theWidgets[i], toUse->allGames, toUse->maxGenre, i, toUse->genreLengths, addGameToCurrent, theImages);
		gtk_widget_show_all(theWidgets[i]);			
	}
	
	int lenCurrent = 0;
	for (int i=0; i<50; i++) {	
		if (strcmp(currentGames[i], "END") == 0) {
			break;
		}
		lenCurrent++;
	}
	int genreLengths[] = {lenCurrent};
	gtk_container_remove (GTK_CONTAINER (currentScrolled), gtk_bin_get_child (GTK_BIN (currentScrolled)));
	makeImages(currentGames, lenCurrent, 0, genreLengths, currentImages);
	addGames(currentScrolled, currentGames, lenCurrent, 0, genreLengths, removeGameFromCurrent, currentImages);
	gtk_widget_show_all(currentScrolled);
}


// Needed for qsort
static int myCompare(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}

// Checks if a game is in the currentGames array. 
bool isGameInCurrent(char* game) {
	for (int i=0; i<50; i++) {
		// If the end is reached without finding a match, no match present
		if (strcmp(currentGames[i], "END") == 0) {
			break;
		}
		if (strcmp(currentGames[i], game) == 0) {
			return true;
		}
	}
	return false;
}

// Sorts the current games array
void sortCurrentGames(int length) {
	// Make a new arrray and add the games to it 
	char* currentTemp[length];
	for (int i=0; i<length; i++) {
		currentTemp[i] = currentGames[i];
	}
	// Sort it
	qsort(currentTemp, length, sizeof(const char*), myCompare);
	// Add the items from the sorted array to currentGames
	for (int i=0; i<length; i++) {
		currentGames[i] = currentTemp[i];
	}
	// Add END tag
	currentGames[length] = "END";
}

// Save the currentGames array to the current.json file
void saveJSONToFile() {
	printf("Saving JSON\n");
	// Create the parts for the JSON
	json_object* newRoot = json_object_new_object();
	json_object* currentArray = json_object_new_array();
	// Add games from currentGames to currentArray
	for (int i=0; i<50; i++) {
		if (strcmp(currentGames[i], "END") == 0) {
			break;
		}
		json_object_array_put_idx(currentArray, i, json_object_new_string(currentGames[i]));
	}
	// Add currentArray to newRoot
	json_object_object_add(newRoot, "current", currentArray);
	
	// Get filename
	int lengthHome = strlen(getenv("HOME"));
	char home[lengthHome-1];
	strcpy(home, getenv("HOME"));
	int lengthCurrent = strlen(CURRENT_SAVE_PATH);
	char fileNameCurrentGames[lengthHome+lengthCurrent+1];
	strcpy(fileNameCurrentGames, home);
	strcat(fileNameCurrentGames, CURRENT_SAVE_PATH);
	
	// Save to file and free root
	if (json_object_to_file(fileNameCurrentGames, newRoot)) {
		printf("Error, could not save JSON\n");
	}
	else {
		printf("Saved JSON\n");
	}
	json_object_put(newRoot);
}

// Adds a game to currentGames when it is empty, called from main
void initCurrentGames(const char* gameToAdd, int index) {
	// At the start, add END tag to make the following code work
	if (index == 0) {
		currentGames[0] = "END";
	}
	for (int i=0; i<50; i++) {
		if (strcmp(currentGames[i], "END") == 0) { // If the current index is END, add game here
			currentGames[i] = gameToAdd;
			currentGames[i+1] = "END"; // Mark next index as END
			break;
		}
	}
}

// Removes a game from currentGames
void removeGameFromCurrent(GtkApplication* app, gpointer data) {
	int lenCurrent = 0; // Used to get number of games present
	for (int i=0; i<50; i++) {
		// Once you find the game to remove, set current index to data at next index, then set next index to the game to find. This causes this process to repeat, moving all the games down one index so no gaps are left. 
		if (strcmp(currentGames[i], (char*)data) == 0) {
			currentGames[i] = currentGames[i+1];
			currentGames[i+1] = (char*)data;
		}
		if (strcmp(currentGames[i], "END") == 0) {
			lenCurrent = i;
			break;
		}
	}
	// Sort the array
	sortCurrentGames(lenCurrent);
	
	// Update the widget
	int genreLengths[] = {lenCurrent};
	gtk_container_remove (GTK_CONTAINER (currentScrolled), gtk_bin_get_child (GTK_BIN (currentScrolled)));
	makeImages(currentGames, lenCurrent, 0, genreLengths, currentImages);
	addGames(currentScrolled, currentGames, lenCurrent, 0, genreLengths, removeGameFromCurrent, currentImages);
	gtk_widget_show_all(currentScrolled);
}

// Adds a game from currentGames
void addGameToCurrent(GtkApplication* app, gpointer data) {
	// Don't allow duplicates 
	if (isGameInCurrent((char*)data)) {
		return;
	}
	int lenCurrent = 0; // Used to get number of games present
	for (int i=0; i<50; i++) {
		// Once you find the first END, add the game to that index, and set the next index to END
		if (strcmp(currentGames[i], "END") == 0) {
			currentGames[i] = (char*)data;
			currentGames[i+1] = "END";
			lenCurrent = i+1;
			break;
		}
	}
	// Sort the array
	sortCurrentGames(lenCurrent);
	
	// Update the widget
	int genreLengths[] = {lenCurrent};
	gtk_container_remove (GTK_CONTAINER (currentScrolled), gtk_bin_get_child (GTK_BIN (currentScrolled)));
	makeImages(currentGames, lenCurrent, 0, genreLengths, currentImages);
	addGames(currentScrolled, currentGames, lenCurrent, 0, genreLengths, removeGameFromCurrent, currentImages);
	gtk_widget_show_all(currentScrolled);
}

// q to quit function
static gboolean check_escape(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	if (event->keyval == GDK_KEY_q) {
		gtk_main_quit();
		return TRUE;
	}
	return FALSE;
}

// Adds a scrolled widget to the stack
GtkWidget* addPanel(GtkStack* stack, char* name) {
	GtkWidget *scrolled1; // Initialise widget pointer

	scrolled1 = gtk_scrolled_window_new(NULL, NULL); // Create widget
	gtk_widget_set_vexpand(scrolled1, true); // Allow expanding
	gtk_widget_set_hexpand(scrolled1, true);
	// Add it to the stack with the given name
	gtk_stack_add_titled(stack, GTK_WIDGET(scrolled1), name, name);
	return scrolled1;
}	

void makeImages(char** allGames, int numGames, int curGenre, int* genreLengths, GtkWidget** toAddTo) {
	for (int i=0; i<numGames; i++) {
		// Initalise widget pointers
		GdkPixbuf* pixbufA;
		GtkWidget* imageA;
		if (i == genreLengths[curGenre]) { // If end of the games reached, break
			break;
		}
		
		// Get image filename 
		int lengthHome = strlen(getenv("HOME"));
		char home[lengthHome-1];
		strcpy(home, getenv("HOME"));
		int lengthCurrent = strlen(COVER_ART_PATH);
		char partFilename[lengthHome+lengthCurrent+1];
		strcpy(partFilename, home);
		strcat(partFilename, COVER_ART_PATH);
		
		int lengthTemp = strlen(partFilename) + 2 + strlen(allGames[i+curGenre*numGames]);
		
		char* extension = COVER_ART_EXTENSION;
		char tempFilename[lengthTemp];
		strcpy(tempFilename, partFilename);
		strcat(tempFilename, allGames[i+curGenre*numGames]);
		
		int lengthFinal = strlen(tempFilename) + 2 + strlen(extension);
		char finalFilename[lengthFinal];
		strcpy(finalFilename, tempFilename);
		strcat(finalFilename, extension);
		
		
		// Create image widget
		pixbufA = gdk_pixbuf_new_from_file(finalFilename, NULL);
		pixbufA = gdk_pixbuf_scale_simple(pixbufA, 200, 300, GDK_INTERP_BILINEAR);
		imageA = gtk_image_new_from_pixbuf(pixbufA);
		gtk_widget_set_margin_start(imageA, 10);
		gtk_widget_set_margin_end(imageA, 10);
		
		// Attach to master grid
		int posY = i/gridWidth;
		int posX = i-gridWidth-(posY*gridWidth);
		
		toAddTo[i+curGenre*numGames] = imageA;
	}
}

// Adds the games from a specific genre to the given scrolled widget
void addGames(GtkWidget* scrolled, char** allGames, int numGames, int curGenre, int* genreLengths, void (*func)(GtkApplication *, void *), GtkWidget** toTakeFrom) {
	// Create the main grid all the games/buttons will be added to
	GtkWidget* gridMaster;
	gridMaster = gtk_grid_new();
	
	for (int i=0; i<numGames; i++) {
		// Initalise widget pointers
		GtkWidget* gridA;
		GtkWidget* buttonA;
		if (i == genreLengths[curGenre]) { // If end of the games reached, break
			break;
		}
		// Create button widget
		int gameNameLen = strlen(allGames[i+curGenre*numGames]);
		char newName[100];
		strcpy(newName, allGames[i+curGenre*numGames]);
		if (gameNameLen > 24) {	
			newName[21] = '.';
			newName[22] = '.';
			newName[23] = '.';
			newName[24] = '\0';
		}
		buttonA = gtk_button_new_with_label(newName);
		g_signal_connect(buttonA, "clicked", G_CALLBACK ((*func)), allGames[i+curGenre*numGames]);
		gtk_widget_set_margin_bottom(buttonA, 10);
		gtk_widget_set_margin_start(buttonA, 10);
		gtk_widget_set_margin_end(buttonA, 10);
		
		// Create grid widget and attach image and button 
		gridA = gtk_grid_new();
		// Get old grid and destroy it
		GtkWidget* parent = gtk_widget_get_parent(toTakeFrom[i+curGenre*numGames]);
		if (parent) {
			g_object_ref(toTakeFrom[i+curGenre*numGames]);	
			gtk_container_remove(GTK_CONTAINER(parent), toTakeFrom[i+curGenre*numGames]);
		}
		// Attach new image and button
		gtk_grid_attach(GTK_GRID(gridA), toTakeFrom[i+curGenre*numGames], 0, 0, 1, 1);
		gtk_grid_attach(GTK_GRID(gridA), buttonA, 0, 1, 1, 1);
		if (parent) {
			g_object_unref(toTakeFrom[i+curGenre*numGames]);
		}

		// Attach to master grid
		int posY = i/gridWidth;
		int posX = i-gridWidth-(posY*gridWidth);
		gtk_grid_attach(GTK_GRID(gridMaster), gridA, posX, posY, 1, 1);
	}
	// Add master grid to given scrolled widget
	if (gtk_bin_get_child (GTK_BIN (scrolled))) {
		gtk_container_remove (GTK_CONTAINER (scrolled), gtk_bin_get_child (GTK_BIN (scrolled)));
	}
	gtk_container_add(GTK_CONTAINER(scrolled), gridMaster);
}
	
// Main function to create the UI
static void activate(GtkApplication* app, gpointer user_data) {
	// Initialise pointers 
	GtkWidget *grid;
	GtkStack *stack;
	GtkStackSidebar *sidebar;
	
	// Get the struct that was passed in
	fullData* toUse = (fullData*)user_data;
	//int numGenres = toUse->genres; // Number of genres

	// Create window, grid, stack and sidebar
	window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	grid = gtk_grid_new();
	stack = GTK_STACK(gtk_stack_new());
	sidebar = GTK_STACK_SIDEBAR(gtk_stack_sidebar_new());

	// Attach parts together
	gtk_stack_sidebar_set_stack(sidebar, stack);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(sidebar), 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(stack), 1, 0, 1, 1);
	
	// Add the panels
	for (int i=0; i<NUM_GENRES+1; i++) {
		if (i < NUM_GENRES) { // If it is a genre 
			theWidgets[i] = addPanel(stack, toUse->allGenres[i]);
		}
		else { // If it is the "Current" panel
			// Create currentScrolled and set properties
			currentScrolled = gtk_scrolled_window_new(NULL, NULL);
			gtk_widget_set_vexpand(currentScrolled, true);
			gtk_widget_set_hexpand(currentScrolled, true);
			gtk_stack_add_titled(stack, GTK_WIDGET(currentScrolled), "Current", "Current");
			
			// Find number of games in currentGames
			int lenCurrent = 0;
			for (int i=0; i<50; i++) {
				if (strcmp(currentGames[i], "END") == 0) {
					break;
				}
				lenCurrent++;
			}
			
			// Sort currentGames and add the games to UI
			sortCurrentGames(lenCurrent);
			int genreLengths[] = {lenCurrent};
			makeImages(currentGames, lenCurrent, 0, genreLengths, currentImages);
			addGames(currentScrolled, currentGames, lenCurrent, 0, genreLengths, removeGameFromCurrent, currentImages);
		}
	}
	
	for (int i=0; i<NUM_GENRES; i++) {
		makeImages(toUse->allGames, toUse->maxGenre, i, toUse->genreLengths, theImages);
	}
	
	// Add the grids for genres (current already added)
	for (int i=0; i<NUM_GENRES; i++) {
		addGames(theWidgets[i], toUse->allGames, toUse->maxGenre, i, toUse->genreLengths, addGameToCurrent, theImages);
	}

	// Add window and set properties 
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(grid));
	gtk_window_set_title(GTK_WINDOW(window), "Media UI\n");
	gtk_window_set_default_size (GTK_WINDOW (window), 1200, 1000);
	g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);
	g_signal_connect(window, "key_press_event", G_CALLBACK(check_escape), NULL);

	// Create list of different icon sizes and add to window
	GList* iconList = NULL;
	GdkPixbuf* icon16;
	GdkPixbuf* icon32;
	GdkPixbuf* icon48;
	GdkPixbuf* icon64;
	GdkPixbuf* icon128;
	
	icon16 = gdk_pixbuf_new_from_file("/usr/share/icons/Papirus-Dark/16x16/apps/applications-games.svg", NULL);
	icon32 = gdk_pixbuf_new_from_file("/usr/share/icons/Papirus-Dark/32x32/apps/applications-games.svg", NULL);
	icon48 = gdk_pixbuf_new_from_file("/usr/share/icons/Papirus-Dark/48x48/apps/applications-games.svg", NULL);
	icon64 = gdk_pixbuf_new_from_file("/usr/share/icons/Papirus-Dark/64x64/apps/applications-games.svg", NULL);
	icon128 = gdk_pixbuf_new_from_file("/usr/share/icons/Papirus-Dark/128x128/apps/applications-games.svg", NULL);
	
	iconList = g_list_append(iconList, icon16);
	iconList = g_list_append(iconList, icon32);
	iconList = g_list_append(iconList, icon48);
	iconList = g_list_append(iconList, icon64);
	iconList = g_list_append(iconList, icon128);
	
	gtk_window_set_icon_list(GTK_WINDOW(window), iconList);
	g_signal_connect(G_OBJECT(window), "check-resize", G_CALLBACK(getSize), user_data);
	// Show window
	gtk_widget_show_all (GTK_WIDGET(window));
	gtk_main();
}

// Called to start building the UI
int initUI(fullData toUse) {
	GtkApplication *app;
	int status;
	status = 0;
	
	app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK (activate), &toUse);
	status = g_application_run (G_APPLICATION (app), 0, NULL);
	g_object_unref (app);

	return status;
}
