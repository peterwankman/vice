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

#include "defs.h"

const int KnDir[8] = { -8, -19, -21, -12, 8, 19, 21, 12 };
const int RkDir[4] = { -1, -10, 1, 10 };
const int BiDir[4] = { -9, -11, 11, 9 };
const int KiDir[8] = { -1, -10, 1, 10, -9, -11, 11, 9 };

int SqAttacked(const int sq, const int side, const S_BOARD *pos) {
	int pce, index, t_sq, dir;

	ASSERT(SqOnBoard(sq));
	ASSERT(SideValid(side));
	ASSERT(CheckBoard(pos));

	if(side == WHITE) {
		if(pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
			return TRUE;
		}
	} else {
		if(pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
			return TRUE;
		}
	}

	for(index = 0; index < 8; index++) {
		pce = pos->pieces[sq + KnDir[index]];
		if(pce != OFFBOARD && IsKn(pce) && PieceCol[pce] == side)
			return TRUE;
	}

	for(index = 0; index < 4; index++) {
		dir = RkDir[index];
		t_sq = sq + dir;
		pce = pos->pieces[t_sq];
		while(pce != OFFBOARD) {
			if(pce != EMPTY) {
				if(IsRQ(pce) && PieceCol[pce] == side) {
					return TRUE;
				}
				break;
			}
			t_sq += dir;
			pce = pos->pieces[t_sq];
		}
	}

	for(index = 0; index < 4; index++) {
		dir = BiDir[index];
		t_sq = sq + dir;
		pce = pos->pieces[t_sq];		
		while(pce != OFFBOARD) {
			if(pce != EMPTY) {
				if(IsBQ(pce) && PieceCol[pce] == side) {
					return TRUE;
				}
				break;
			}
			t_sq += dir;
			pce = pos->pieces[t_sq];
		}
	}

	for(index = 0; index < 8; index++) {
		pce = pos->pieces[sq + KiDir[index]];
		if(pce != OFFBOARD && IsKi(pce) && PieceCol[pce] == side) {
			return TRUE;
		}
	}

	return FALSE;
}