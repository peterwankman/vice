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

#include <string.h>
#include "defs.h"

int SqOnBoard(const int sq) {
	return (FilesBrd[sq] == OFFBOARD) ? 0 : 1;
}

int SideValid(const int side) {
	return ((side == WHITE) || (side == BLACK)) ? 1 : 0;
}

int FileRankValid(const int fr) {
	return (fr >= 0 && fr <= 7) ? 1 : 0;
}

int PieceValidEmpty(const int pce) {
	return (pce >= EMPTY && pce <= bK) ? 1 : 0;
}

int PieceValid(const int pce) {
	return (pce >= wP && pce <= bK) ? 1 : 0;
}

void MirrorEvalTest(S_BOARD *pos) {
	FILE *file;
	char lineIn [1024];
	int ev1 = 0; int ev2 = 0;
	int positions = 0;

	file = fopen("mirror.epd","r");

	if(file == NULL) {
		printf("File Not Found\n");
		return;
	} else {
		while(fgets (lineIn, 1024, file) != NULL) {
			ParseFen(lineIn, pos);
			positions++;
			ev1 = EvalPosition(pos);
			MirrorBoard(pos);
			ev2 = EvalPosition(pos);

			if(ev1 != ev2) {
				printf("\n\n\n");
				ParseFen(lineIn, pos);
				PrintBoard(pos);
				MirrorBoard(pos);
				PrintBoard(pos);
				printf("\n\nMirror Fail:\n%s\n", lineIn);
				getchar();
				return;
			}

			if((positions % 1000) == 0) {
				printf("position %d\n", positions);
			}

			memset(&lineIn[0], 0, sizeof(lineIn));
		}
	}
}
