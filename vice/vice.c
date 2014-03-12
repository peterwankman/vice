/*
 * VICE -- Video Instructions Chess Engine
 * (C) 2013 Bluefever Software
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"

int main(void) {
	S_BOARD pos[1];
	S_SEARCHINFO info[1];
	char line[256];

	pos->HashTable->pTable = NULL;

	AllInit();
	InitHashTable(pos->HashTable, 64);
	memset(pos->searchKillers, 0, 2 * MAXDEPTH * sizeof(int));

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	
	printf("Welcome to Vice! Type 'vice' for console mode...\n");

	while (TRUE) {
		memset(&line[0], 0, sizeof(line));

		fflush(stdout);
		if (!fgets(line, 256, stdin))
			continue;
		if (line[0] == '\n')
			continue;
		if (!strncmp(line, "uci",3)) {			
			Uci_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "xboard",6))	{
			XBoard_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if (!strncmp(line, "vice",4))	{
			Console_Loop(pos, info);
			if(info->quit == TRUE) break;
			continue;
		} else if(!strncmp(line, "quit",4))	{
			break;
		}
	}
	
	free(pos->HashTable->pTable);
	return 0;
}
