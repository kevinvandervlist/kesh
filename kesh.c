/**
 * Kesh simple shell entry point
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

#include <stdlib.h>

#include "debug.h"
#include "dispatch.h"
#include "term.h"
#include "tokenize.h"

#include "state.h"
#include "joblist.h"
               
int main() {
	STATE *state = malloc(sizeof(STATE));
	state->next_cmd = 1;
	state->retcode = 0;

	// While we want to handle next cmd's:
	while(state->next_cmd) {
		// Display the prompt of the terminal
		term_display(state->retcode);

		// Get a raw line
		char *raw_input = NULL;
		term_read_line(&raw_input);

		// Get a joblist of the raw line
		JOBLIST *jl = tokenize_get_joblist(raw_input);

		// Execute the joblist in the dispatcher
		dispatch_exec(jl, state);

		// Cleanup this round
		free(raw_input);
		tokenize_free_joblist(jl);
	}

	term_exit();

	// Done
	free(state);
	return 0;
}
