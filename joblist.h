/**
 * Joblist and job structure definitions
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

#ifndef JOBLIST_H
#define JOBLIST_H

#define JOB_MAX_ARGV_CNT 64

typedef struct job JOB;

struct job {
	char *prg;
	char **argv;
	int empty;
	JOB *next;
};

typedef struct joblist {
	JOB *first;
	int no_jobs;
} JOBLIST;

#endif
