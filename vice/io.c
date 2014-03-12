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

#include "defs.h"

char *PrSq(const int sq) {
	static char SqStr[3];

	int file = FilesBrd[sq];
	int rank = RanksBrd[sq];

	sprintf(SqStr, "%c%c", ('a' + file), ('1' + rank));
	return SqStr;
}

char *PrMove(const int move) {
	static char MvStr[6];
	char pchar;

	int ff = FilesBrd[FROMSQ(move)];
	int rf = RanksBrd[FROMSQ(move)];
	int ft = FilesBrd[TOSQ(move)];
	int rt = RanksBrd[TOSQ(move)];

	int promoted = PROMOTED(move);

	if(promoted) {
		pchar = 'q';
		if(IsKn(promoted))
			pchar = 'n';
		else if(IsRQ(promoted) && !IsBQ(promoted))
			pchar = 'r';
		else if(!IsRQ(promoted) && IsBQ(promoted))
			pchar = 'b';

		sprintf(MvStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pchar);
	} else {
		sprintf(MvStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
	}

	return MvStr;
}

int ParseMove(char *ptrChar, S_BOARD *pos) {
	int from, to;
	int MoveNum = 0, Move = 0, PromPce = EMPTY;
	S_MOVELIST list[1];

	if(ptrChar[1] > '8' || ptrChar[1] < '1') return NOMOVE;
	if(ptrChar[3] > '8' || ptrChar[3] < '1') return NOMOVE;
	if(ptrChar[0] > 'h' || ptrChar[0] < 'a') return NOMOVE;
	if(ptrChar[2] > 'h' || ptrChar[2] < 'a') return NOMOVE;

	from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
	to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

	ASSERT(SqOnBoard(from) && SqOnBoard(to));

	GenerateAllMoves(pos, list);

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		Move = list->moves[MoveNum].move;
		if(FROMSQ(Move) == from && TOSQ(Move) == to) {
			PromPce = PROMOTED(Move);
			if(PromPce != EMPTY) {
				if(IsRQ(PromPce) && !IsBQ(PromPce) && ptrChar[4] == 'r') {
					return Move;
				} else if(!IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'b') {
					return Move;
				} else if(IsRQ(PromPce) && IsBQ(PromPce) && ptrChar[4] == 'q') {
					return Move;
				} else if(IsKn(PromPce) && ptrChar[4] == 'n') {
					return Move;
				}
				continue;
			}
			return Move;
		}
	}

	return NOMOVE;
}

void PrintMoveList(const S_MOVELIST *list) {
	int index = 0;
	int score = 0;
	int move = 0;
	printf("MoveList:\n");

	for(index = 0; index < list->count; ++index) {
		move = list->moves[index].move;
		score = list->moves[index].score;

		printf("Move:%d > %s (score:%d)\n", index + 1, PrMove(move), score);
	}

	printf("MoveList Total %d Moves.\n\n", list->count);
}