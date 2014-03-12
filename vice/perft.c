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

long leafNodes;

void Perft(int depth, S_BOARD *pos) {
	S_MOVELIST list[1];
	int MoveNum = 0;

	ASSERT(CheckBoard(pos));

	if(depth == 0) {
		leafNodes++;
		return;
	}

	GenerateAllMoves(pos, list);

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		if(!MakeMove(pos, list->moves[MoveNum].move)) {
			continue;
		}

		Perft(depth - 1, pos);
		TakeMove(pos);
	}

	return;
}

void PerftTest(int depth, S_BOARD *pos) {
	S_MOVELIST list[1];
	int move;
	int MoveNum = 0;
	int start = GetTimeMs();
	long cumnodes, oldnodes;

	ASSERT(CheckBoard(pos));

	PrintBoard(pos);
	printf("\nStarting Test To Depth:%d\n", depth);
	leafNodes = 0;
	
	GenerateAllMoves(pos, list);

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		move = list->moves[MoveNum].move;
		if(!MakeMove(pos, move))
			continue;

		cumnodes = leafNodes;
		Perft(depth - 1, pos);
		TakeMove(pos);
		oldnodes = leafNodes - cumnodes;
		printf("move %d : %s : %ld\n", MoveNum + 1, PrMove(move), oldnodes);
	}

	printf("\nTest Complete : %ld nodes visited in %dms\n", leafNodes, GetTimeMs() - start);
}