/**
 * Job(list) dispather.
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

#include "dispatch.h"

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "debug.h"
#include "joblist.h"

/**
 * Execute a complete list of jobs.
 * This means piping the commands together.
 * @param JOBLIST The joblist with all jobs.
 * @param STATE The state struct.
 */

void exec_job_list(JOBLIST *jl, STATE *state) {
	JOB *j = jl->first;
	int pids[jl->no_jobs];
	int j_cnt = 0;

	// Duplicates of the original pipes
	int stdin = dup(0);
	int stdout = dup(1);

	int cur_out;
	int fds[2];

	// As long as there are jobs:
	while(j != NULL) {
		// is there a prev cmd?
		if(j_cnt > 0) {
			dup2(cur_out, 0);
			close(cur_out);
		}

		// Is there a next cmd?
		if(j->next != NULL) {
			pipe(fds);

			cur_out = fds[0];
			dup2(fds[1], 1);
			close(fds[1]);
		}

		// Last cmd?
		if(j->next == NULL) {
			dup2(stdout, 1);
			close(stdout);
		}

		// Do the actual forking
		pids[j_cnt] = fork();

		if(pids[j_cnt] < 0) {
			state->retcode = -1;
		} else if(pids[j_cnt] == 0) {
			// Child
			// Not the last command; close pipes
			if(j->next != NULL) {
				close(stdin);
				close(stdout);
				close(fds[0]);
			}
			execvp(j->prg, j->argv);
			// In case of an error:
			exit(1);
		}

		// Increment the number of jobs and set the following job.
		j_cnt++;
		j = j->next;
	}

	// Wait for all the childs to finish. Retrieve returncodes as well.
	for(j_cnt = 0; j_cnt < jl->no_jobs; j_cnt++) {
		int t;
		waitpid(pids[j_cnt], &t, 0);
		state->retcode |= t;
	}

	// Restore stdin on exit:
	dup2(stdin, 0);
	close(stdin);

	return;
}

/**
 * Execute a single job.
 * This means a standard command.
 * @param JOB The joblist
 * @param STATE The state struct.
 */

void exec_single_job(JOB *job, STATE *state) {
	if(is_builtin(job)) {
		exec_builtin(job, state);
	} else {
		int pid = fork();

		if(pid < 0) {
			state->retcode = -1;
			return;
		} else if(pid == 0) {
			// Child
			execvp(job->prg, job->argv);
			// In case of an error:
			exit(1);
		} else {
			// Parent
			waitpid(pid, &state->retcode, 0);
		}
	}
	return;
}

/**
 * Execute a given joblist.
 * The state of the execution can be stored in STATE.
 * @param JOBLIST The joblist
 * @param STATE The state struct
 */

void dispatch_exec(JOBLIST *jl, STATE *state) {
	JOB *head = jl->first;

	// Empty command (so no input is given)?
	if(head->empty) {
		state->retcode = 0;
		return;
	} else {
		if(head->next == NULL) {
			exec_single_job(head, state);
		} else {
			exec_job_list(jl, state);
		}
	}
	return;
}
