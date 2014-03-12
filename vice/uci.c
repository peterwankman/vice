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
#include <string.h>
#include "defs.h"

#define INPUTBUFFER 400 * 6

void ParseGo(char* line, S_SEARCHINFO *info, S_BOARD *pos) {

	int depth = -1, movestogo = 30,movetime = -1;
	int time = -1, inc = 0;
	char *ptr = NULL;
	info->timeset = FALSE;

	if((ptr = strstr(line,"INF")));

	if((ptr = strstr(line,"binc")) && pos->side == BLACK)
		inc = atoi(ptr + 5);
	if((ptr = strstr(line,"winc")) && pos->side == WHITE)
		inc = atoi(ptr + 5);
	if((ptr = strstr(line,"wtime")) && pos->side == WHITE)
		time = atoi(ptr + 6);
	if((ptr = strstr(line,"btime")) && pos->side == BLACK)
		time = atoi(ptr + 6);
	if((ptr = strstr(line,"movestogo")))
		movestogo = atoi(ptr + 10);
	if((ptr = strstr(line,"movetime")))
		movetime = atoi(ptr + 9);
	
	if((ptr = strstr(line,"depth")))
		depth = atoi(ptr + 6);

	if(movetime != -1) {
		time = movetime;
		movestogo = 1;
	}

	info->starttime = GetTimeMs();
	info->depth = depth;

	if(time != -1) {
		info->timeset = TRUE;
		time /= movestogo;
		time -= 50;	
		info->stoptime = info->starttime + time + inc;
	}

	if(depth == -1)
		info->depth = MAXDEPTH;

	printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
		time, info->starttime, info->stoptime, info->depth, info->timeset);
	SearchPosition(pos, info);
}

void ParsePosition(char *lineIn, S_BOARD *pos) {
	char *ptrChar = lineIn + 9;
	int move;

	lineIn = ptrChar;

	if(strncmp(lineIn, "startpos", 8) == 0) {
		ParseFen(START_FEN, pos);
	} else {
		ptrChar = strstr(lineIn, "fen");
		if(ptrChar == NULL) {
			ParseFen(START_FEN, pos);
		} else {
			ptrChar += 4;
			ParseFen(ptrChar, pos);
		}
	}

	ptrChar = strstr(lineIn, "moves");

	if(ptrChar != NULL) {
		ptrChar += 6;
		while(*ptrChar) {
			move = ParseMove(ptrChar, pos);
			if(move == NOMOVE)
				break;
			MakeMove(pos, move);
			pos->ply = 0;
			while(*ptrChar && *ptrChar != ' ')
				ptrChar++;
			ptrChar++;
		}
	}
	PrintBoard(pos);
}

void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info) {
	char line[INPUTBUFFER], *ptrTrue = NULL;
	int MB = 64;

	info->GAME_MODE = UCIMODE;
	info->quit = 0;

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	printf("id name %s\n",NAME);
	printf("id author Bluefever\n");
	printf("option name Hash type spin default 64 min 4 max %d\n", MAX_HASH);
	printf("option name Book type check default true\n", MAX_HASH);
	printf("uciok\n");


	while (TRUE) {
		memset(&line[0], 0, sizeof(line));
		fflush(stdout);
		if (!fgets(line, INPUTBUFFER, stdin))
			continue;

		if (line[0] == '\n')
			continue;

		if (!strncmp(line, "isready", 7)) {
			printf("readyok\n");
			continue;
		} else if (!strncmp(line, "position", 8)) {
			ParsePosition(line, pos);
		} else if (!strncmp(line, "ucinewgame", 10)) {
			ParsePosition("position startpos\n", pos);
		} else if (!strncmp(line, "go", 2)) {
			ParseGo(line, info, pos);
		} else if (!strncmp(line, "quit", 4)) {
			info->quit = TRUE;
			break;
		} else if (!strncmp(line, "uci", 3)) {
			printf("id name %s\n", NAME);
			printf("id author Bluefever\n");
			printf("uciok\n");
		} else if(!strncmp(line, "setoption name Hash value ", 26)) {
			sscanf(line, "%*s %*s %*s %*s %d", &MB);
			if(MB < 4) MB = 4;
			if(MB > MAX_HASH) MB = MAX_HASH;
			printf("Set Hash to %d MB\n", MB);
			InitHashTable(pos->HashTable, MB);
		} else if(!strncmp(line, "setoption name Book value ", 26)) {
			ptrTrue = strstr(line, "true");
			if(ptrTrue != NULL) {
				EngineOptions->UseBook = TRUE;
			} else {
				EngineOptions->UseBook = FALSE;
			}
		}
		if(info->quit) break;
	}
}
