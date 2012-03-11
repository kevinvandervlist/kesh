/**
 * Shell builtins.
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

#include "builtin.h"

#include <string.h>
#include <unistd.h>

#include "debug.h"

/**
 * Is the given job an exit command?
 * @param JOB The job to check
 */

int is_exit(JOB *job) {
	return strcmp(job->prg, "exit") == 0;
}

/**
 * Is the given job an cd command?
 * @param JOB The job to check
 */

int is_cd(JOB *job) {
	return strcmp(job->prg, "cd") == 0;
}

/**
 * Is the given job a builtin?
 * TODO: Could be a bit more beautiful. Using hashtables or functionpointers for example.
 * @param JOB The job to check
 */

int is_builtin(JOB *job) {
	int ret = 0;

	if(is_cd(job)) {
		ret = 1;
	}	else if(is_exit(job)) {
		ret = 1;
	}

	return ret;
}

/**
 * Execute a given job as it's builtin command.
 * TODO: Could be a bit more beautiful. Using hashtables or functionpointers for example.
 * @param JOB The job to execute
 */

void exec_builtin(JOB *job, STATE *state) {
	state->retcode = 0;
	state->next_cmd = 1;

	if(is_cd(job)) {
		state->retcode = chdir(job->argv[1]);
	}	else if(is_exit(job)) {
		state->next_cmd = 0;
	}
	return;
}
