/**
 * Tokenize raw input
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

#include "tokenize.h"

#define PIPE_STR "|"
#define SPACE_CHR ' '

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "joblist.h"

/**
 * Remove an existing newline in a given string.
 * @param str A string to strip.
 */

void remove_newline(char *str) {
	char *nl = NULL;
	char termchar = '\0';
	nl = strrchr(str, '\n');
	if(nl != NULL) {
		*nl = termchar;
	}
	return;
}

/**
 * Remove whitespace (before and end of line; strip).
 * @param str A string to strip.
 */

char *remove_whitespace(char *str) {
	char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

/**
 * Fill a given job with the parsed cmd data.
 * 
 * This function can be used to fill a job when a cmd string is given.
 * An example is the string "ls -a -lh", which will result in a job with the
 * cmd "ls", and argv[] = { "ls", "-a", "-lh" }
 * 
 * @param job The job to prepare
 * @param cmd Raw cmd string
 */

void fill_job(JOB *job, char *cmd) {
	char *strtok_buf = NULL;
	char *tknpart = NULL;
	int it = 0;

	// Split on spaces
	tknpart = strtok_r(cmd, " ", &strtok_buf);

	// Prepare argv
	job->argv = malloc(sizeof(char*) * JOB_MAX_ARGV_CNT);
	for(it = 0; it < JOB_MAX_ARGV_CNT; it++) {
		job->argv[it] = NULL;
	}

	// Copy binary name
	if(tknpart == NULL) {
		job->prg = malloc(sizeof(char) * (strlen("") + 1));
		strcpy(job->prg, "");
		job->empty = 1;
	} else {
		job->prg = malloc(sizeof(char) * (strlen(tknpart) + 1));
		strcpy(job->prg, tknpart);
	}
	// Set the first item of argv as well.
	it = 0;
	job->argv[it] = malloc(sizeof(char) * (strlen(job->prg) + 1));
	strcpy(job->argv[it], job->prg);

	// Set any possible other arguments as well
 	it = 1;
	tknpart = strtok_r(NULL, " ", &strtok_buf);
	if(tknpart != NULL) {
		do {
			job->argv[it] = malloc(sizeof(char) * strlen(tknpart));
			strcpy(job->argv[it], tknpart);

			tknpart = strtok_r(NULL, " ", &strtok_buf);
			it++;
		} while (tknpart != NULL);
	}
	return;
}

/**
 * Get a newly malloc'd joblist. 
 * @return JOBLIST
 */

JOBLIST *get_joblist() {
	JOBLIST *jl = malloc(sizeof(JOBLIST));
	jl->no_jobs = 0;
	jl->first = NULL;
	return jl;
}

/**
 * Get a newly malloc'd job. 
 * Free it yourself, @see free_job(JOB *);
 * @return JOB
 */

JOB *get_job() {
	JOB *j = malloc(sizeof(JOB));
	j->next = NULL;
	j->empty = 0;
	return j;
}

/**
 * Free a job, and its inner data.
 * Use this to clean up a job.
 * @return JOB if it has a next job, NULL otherwise.
 */

JOB *free_job(JOB *job) {
	int i = 0;
	JOB *ret_next = NULL;

	free(job->prg);

	if(job->argv != NULL) {
		// There are arguments, free them as well
		while(job->argv[i] != NULL) {
			free(job->argv[i++]);
		}
		free(job->argv);		
	}
	ret_next = job->next;

	free(job);

	return ret_next;
}

/**
 * Get a joblist from a raw input string.
 * The joblist contains all jobs.
 * @param raw_str The raw cmd string from the user.
 * @return JOBLIST with the job(s).
 */

JOBLIST *tokenize_get_joblist(char *raw_str) {
	JOBLIST *jl = get_joblist();
	JOB *cur = NULL;
	char *cmd_part = NULL;
	char *strtok_buf = NULL;

	// Iterate over all parts of the string, separated by PIPE_STR
	for(cmd_part = strtok_r(raw_str, PIPE_STR, &strtok_buf);
			cmd_part;
			cmd_part = strtok_r(NULL, PIPE_STR, &strtok_buf)) {

		cmd_part = remove_whitespace(cmd_part);
		remove_newline(cmd_part);

		JOB *j = get_job();
		fill_job(j, cmd_part);

		if(jl->first == NULL) {
			// Add the job to the joblist...
			jl->first = j;
			cur = j;
		} else {
			// Or at the end of the existing joblist.
			cur->next = j;
			cur = j;
		}
		jl->no_jobs++;
	}

	return jl;
}

/**
 * Free a given joblist.
 * Completely clear up a joblist, including it's child jobs.
 * @param JOBLIST The joblist to free.
 */

void tokenize_free_joblist(JOBLIST *jl) {
	JOB *it;
	if(jl->first != NULL) {
		it = jl->first;
		while(it != NULL) {
			it = free_job(it);
		}
	}
	free(jl);
	return;
}
