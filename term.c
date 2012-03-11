/**
 * Functions related to manipulating the terminal
 * Copyright (C) 2012 Kevin van der Vlist
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Kevin van der Vlist - kevin@kevinvandervlist.nl
 */

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "term.h"

struct term_info {
	char *user;
	char *host;
	char *dir;
};

/**
 * Set user to current effective username.
 * @param char** The string to set the username to.
 */

void set_username(char **user) {
	struct passwd *pw;
	uid_t uid;

	uid = geteuid();
	pw = getpwuid(uid);

	*user = malloc(sizeof(char) * strlen(pw->pw_name));
	strcpy(*user, pw->pw_name);
}

/**
 * Set hostname to the current hostname.
 * @param char** The string to set the hostname to.
 */

void set_hostname(char **hostname) {
	*hostname = malloc(sizeof(char) * 100);
	gethostname(*hostname, 100);
}

/**
 * Set path to the current path.
 * @param char** The string to set the path to.
 */

void set_curpath(char **path) {
	long size = pathconf(".", _PC_PATH_MAX);
	*path = malloc(sizeof(char) * size);
  getcwd(*path, (size_t)size);
}

/**
 * Fill a term_info struct with the correct information.
 * @param term_info The struct containing information.
 */

void fill_term_info(struct term_info *ti) {
	set_username(&ti->user);
	set_hostname(&ti->host);
	set_curpath(&ti->dir);
	return;
}

/**
 * Free a term_info struct.
 * @param term_info The struct to free.
 */

void free_term_info(struct term_info *ti) {
	free(ti->user);
	free(ti->host);
	free(ti->dir);
	free(ti);
}

/**
 * Display the prompt on the terminal.
 * @param int The returncode of the last command.
 */

void term_display(int retcode) {
	struct term_info *inf = malloc(sizeof(struct term_info));
	fill_term_info(inf);

	printf("%d|%s@%s:%s> ", retcode, inf->user, inf->host, inf->dir);
	fflush(stdout);

	free_term_info(inf);
	return;
}

/**
 * Read a terminal's input for a line.
 * @param char** A pointer to a char* for the string.
 */

void term_read_line(char **raw_input) {
	size_t len = 0;

	if(getline(raw_input, &len, stdin) == -1) {
		*raw_input = malloc(sizeof(char) * 10);
		strcpy(*raw_input, "exit\n");
	}
	return;
}

/**
 * This function is called when the interactive shell is shut down.
 */

void term_exit() {
	printf("\n");
}
