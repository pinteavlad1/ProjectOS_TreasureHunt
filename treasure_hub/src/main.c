#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include "hub_commands.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *choices[] = {
            "Start Monitor",
            "Stop Monitor",
            "List Hunts",
            "List Treasures",
			"View Treasure",
			"Calculate Score",
			"Exit"
};
char *nums[] = {
            "1.",
            "2.",
            "3.",
            "4.",
			"5.",
			"6.",
			"7."
};

char *message;
Monitor monitor = {};

ITEM **my_items;	
MENU *my_menu;
int n_choices;

void close_ncurses() {
	unpost_menu(my_menu);
	for(int i = 0; i < n_choices; ++i)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();
}

void func(char *name);

void clear_message() {
	for (int i = 0; i < LINES; i++) {
		move(i, 40);
		clrtoeol();
	}
}

void print_message(char *message) {
	clrtoeol();
	char *buffer = (char *)malloc(1024);
	strcpy(buffer, message);
	char *line = strtok(buffer, "\n");
	mvprintw(0, 40, "Message:");
	int line_count = 1;
	while (line != NULL) {
		mvprintw(line_count++, 40, "%s", line);
		line = strtok(NULL, "\n");
	}
	refresh();
}

void init() {
    /* Initialize curses */	
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

    /* Initialize items */
    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM*));
    for(int i = 0; i < n_choices; ++i) {
              
        my_items[i] = new_item(nums[i], choices[i]);
        set_item_userptr(my_items[i], func);
    }
    my_items[n_choices] = (ITEM *)NULL;

    /* Create menu */
    my_menu = new_menu((ITEM **)my_items);

    /* Post the menu */

    //Put message to the right of the menu
    print_message(message);

    mvprintw(LINES - 7, 0, "Monitor status: Stopped");
    mvprintw(LINES - 3, 0, "Press <ENTER> to select");
    mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviagate");
    mvprintw(LINES - 1, 0, "F1 to exit");
    post_menu(my_menu);
    refresh();
}

int main(){	
	
	if (chdir("../treasure_hub") != 0) {
		perror("Failed to change directory to treasure_hub");
		return 1;
	}

	monitor.pid = -1;
	monitor.status = 0;
	monitor.pipefd[0] = -1;
	monitor.pipefd[1] = -1;

	message = (char*) malloc(1024);
	set_message(message);

	init();

	int c;

	while(1)
	{       

		if (monitor.pid != 0) {

			// Check monitor status and display it
			move(LINES - 7, 0);
			clrtoeol();
			if (is_monitor_stopping()) {
				attron(COLOR_PAIR(3));
				mvprintw(LINES - 7, 0, "Monitor status: Stopping...");
				attroff(COLOR_PAIR(3));
			} else if (monitor.status == 1) {
				attron(COLOR_PAIR(2));
				mvprintw(LINES - 7, 0, "Monitor status: Running");
				attroff(COLOR_PAIR(2));
			} else {
				attron(COLOR_PAIR(1));
				mvprintw(LINES - 7, 0, "Monitor status: Stopped");
				attroff(COLOR_PAIR(1));
			}

			move(LINES - 6, 0);
			clrtoeol();
			move(LINES - 5, 0);
			clrtoeol();

			clear_message();
			print_message(message);

			c = getch();
			if (c == KEY_F(1)) {
				break;
			}

			switch(c)
				{	case KEY_DOWN:
					menu_driver(my_menu, REQ_DOWN_ITEM);
					break;
				case KEY_UP:
					menu_driver(my_menu, REQ_UP_ITEM);
					break;
				case 10: /* Enter */
				{	ITEM *cur;
					void (*p)(char *);

					cur = current_item(my_menu);
					p = item_userptr(cur);
					p((char *)item_name(cur));
					pos_menu_cursor(my_menu);
					break;
				}
				break;
			}
		}
	}

	if (monitor.pid != 0)
		close_ncurses();

	free(message);
}

void func(char *name) {

	assert(monitor.pid != 0);

	clrtoeol();
	if (strcmp(name, "1.") == 0) {
		close_ncurses();
		if (is_monitor_stopping()) {
			strcpy(message, "Monitor is stopping. Please wait.");
		}
		else if (monitor.status == 1) {
			strcpy(message, "Monitor is already running.");
		} 
		else {
			start_monitor(message, &monitor);
		}
		if (monitor.pid != 0) {
			init();
		}
	}
	else if (strcmp(name, "2.") == 0) {
		if (is_monitor_stopping()) {
			strcpy(message, "Monitor is stopping. Please wait.");
		}
		else if (monitor.status == 0) {
			strcpy(message, "Monitor is not running.");
		} 
		else {
			stop_monitor(message, &monitor);
		}
		refresh();
	}
	else if (strcmp(name, "3.") == 0) {
		if (is_monitor_stopping()) {
			strcpy(message, "Monitor is stopping. Please wait.");
		}
		else if (monitor.status == 0) {
			strcpy(message, "Monitor is not running.");
		} 
		else {
			list_all_hunts(message, &monitor);
		}
		refresh();
	}
	else if (strcmp(name, "4.") == 0) {
        if (is_monitor_stopping()) {
            strcpy(message, "Monitor is stopping. Please wait.");
        }
        else if (monitor.status == 0) {
            strcpy(message, "Monitor is not running.");
        } 
        else {
            char hunt_id[21];
            echo();
            move(LINES - 6, 0);
            clrtoeol();
            mvprintw(LINES - 6, 0, "Enter hunt ID: ");
            scanw("%20s", hunt_id);
            noecho();
            list_treasures(message, &monitor, hunt_id);
        }
        refresh();	
    }
    else if (strcmp(name, "5.") == 0) {
        if (is_monitor_stopping()) {
            strcpy(message, "Monitor is stopping. Please wait.");
        }
        else if (monitor.status == 0) {
            strcpy(message, "Monitor is not running.");
        } 
        else {
            char hunt_id[21], treasure_id[21];
            echo();
            move(LINES - 6, 0);
            clrtoeol();
            mvprintw(LINES - 6, 0, "Enter hunt ID: ");
            scanw("%20s", hunt_id);
            move(LINES - 5, 0);
            clrtoeol();
            mvprintw(LINES - 5, 0, "Enter treasure ID: ");
            scanw("%20s", treasure_id);
            noecho();
            view_treasure(message, &monitor, hunt_id, treasure_id);
        }
        refresh();
    }
	else if (strcmp(name, "6.") == 0) {
		if (is_monitor_stopping()) {
			strcpy(message, "Monitor is stopping. Please wait.");
		}
		else if (monitor.status == 0) {
			strcpy(message, "Monitor is not running.");
		} 
		else {
			calculate_score(message, &monitor);
		}
		refresh();
	}
	else if (strcmp(name, "7.") == 0) {
		close_ncurses();
		free(message);
		exit(0);
	}
}