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
#include "polykeys.h"

typedef struct {
	U64 key;
	unsigned short move;
	unsigned short weight;
	unsigned int learn;
} S_POLY_BOOK_ENTRY;

long NumEntries = 0;

S_POLY_BOOK_ENTRY *entries = NULL;

const int PolyKindOfPiece[13] = {
	-1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10
};

void InitPolyBook(void) {
	FILE *pFile = fopen("performance.bin", "rb");
	long position, returnValue;

	EngineOptions->UseBook = FALSE;

	if(pFile == NULL) {
		printf("Book File Not Read\n");
	} else {
		fseek(pFile, 0, SEEK_END);
		position = ftell(pFile);

		if(position < sizeof(S_POLY_BOOK_ENTRY)) {
			printf("No Entries Found\n");
			return;
		}

		NumEntries = position / sizeof(S_POLY_BOOK_ENTRY);
		printf("%ld Entries Found In File\n", NumEntries);

		entries = malloc(NumEntries * sizeof(S_POLY_BOOK_ENTRY)); /* NULL! */

		rewind(pFile);
		returnValue = fread(entries, sizeof(S_POLY_BOOK_ENTRY), NumEntries, pFile);
		printf("fread() %ld Entries Read in from File\n", returnValue);

		if(NumEntries > 0) {
			EngineOptions->UseBook = TRUE;
		}
	}

	fclose(pFile);
}

void CleanPolyBook(void) {
	free(entries);
}

int HasPawnForCapture(const S_BOARD *board) {
	int sqWithPawn = 0;
	int targetPce = (board->side == WHITE) ? wP : bP;
	if(board->enPas != NO_SQ) {
		if(board->side == WHITE) {
			sqWithPawn = board->enPas - 10;
		} else {
			sqWithPawn = board->enPas + 10;
		}

		if(board->pieces[sqWithPawn + 1] == targetPce) {
			return TRUE;
		} else if(board->pieces[sqWithPawn - 1] == targetPce) {
			return TRUE;
		}
	}
	return FALSE;
}

U64 PolyKeyFromBoard(const S_BOARD *board) {
	int sq = 0, rank = 0, file = 0;
	U64 finalKey = 0;
	int piece = EMPTY;
	int polyPiece = 0;
	int offset = 0;

	for(sq = 0; sq < BRD_SQ_NUM; ++sq) {
		piece = board->pieces[sq];
		if(piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
			ASSERT(piece >= wP && piece <= bK);
			polyPiece = PolyKindOfPiece[piece];
			rank = RanksBrd[sq];
			file = FilesBrd[sq];

			finalKey ^= Random64Poly[(64 * polyPiece) + (8 * rank) + file];
		}
	}

	offset = 768;
	if(board->castlePerm & WKCA) finalKey ^= Random64Poly[offset + 0];
	if(board->castlePerm & WQCA) finalKey ^= Random64Poly[offset + 1];
	if(board->castlePerm & BKCA) finalKey ^= Random64Poly[offset + 2];
	if(board->castlePerm & BQCA) finalKey ^= Random64Poly[offset + 3];

	offset = 772;
	if(HasPawnForCapture(board) == TRUE) {
		file = FilesBrd[board->enPas];
		finalKey ^= Random64Poly[offset + file];
	}

	if(board->side == WHITE)
		finalKey ^= Random64Poly[780];

	return finalKey;
}

unsigned short endian_swap_u16(unsigned short x) {
	x = (x >> 8) |
		(x << 8);
	return x;
}

unsigned int endian_swap_u32(unsigned int x) {
	x = (x >> 24) |
		((x << 8) & 0x00ff0000) |
		((x >> 8) & 0x0000ff00) |
		(x << 24);
	return x;
}

U64 endian_swap_u64(U64 x) {
	x = (x >> 56) |
		((x << 40) & 0x00ff000000000000ULL) |
		((x << 24) & 0x0000ff0000000000ULL) |
		((x << 8)  & 0x000000ff00000000ULL) |
		((x >> 8)  & 0x00000000ff000000ULL) |
		((x >> 24) & 0x0000000000ff0000ULL) |
		((x >> 40) & 0x000000000000ff00ULL) |
		(x << 56);
	return x;
}

int ConvertPolyMoveToInternalMove(unsigned short polyMove, S_BOARD *board) {
	int ff = (polyMove >> 6) & 7;
	int fr = (polyMove >> 9) & 7;
	int tf = (polyMove >> 0) & 7;
	int tr = (polyMove >> 3) & 7;
	int pp = (polyMove >> 12) & 7;
	char promChar = 'q';

	char moveString[6];
	if(pp == 0) {
		sprintf(moveString, "%c%c%c%c",
			FileChar[ff],
			RankChar[fr],
			FileChar[tf],
			RankChar[tr]);
	} else {
		switch(pp) {
			case 1: promChar = 'n';
			case 2: promChar = 'b';
			case 3: promChar = 'r';
		}

		sprintf(moveString, "%c%c%c%c%c",
			FileChar[ff],
			RankChar[fr],
			FileChar[tf],
			RankChar[tr],
			promChar);
	}

	return ParseMove(moveString, board);
}

int GetBookMove(S_BOARD *board) {
	S_POLY_BOOK_ENTRY *entry;
	unsigned short move;
	const int MAXBOOKMOVES = 32;
	int bookMoves[32]; /* !! */
	int tempMove = NOMOVE;
	int count = 0;
	int randMove;

	U64 polyKey = PolyKeyFromBoard(board);

	for(entry = entries; entry < entries + NumEntries; entry++) {
		if(polyKey == endian_swap_u64(entry->key)) {
			move = endian_swap_u16(entry->move);
			tempMove = ConvertPolyMoveToInternalMove(move, board);
			if(tempMove != NOMOVE) {
				bookMoves[count++] = tempMove;
				if(count > MAXBOOKMOVES)
					break;
			}
		}
	}

	if(count != 0) {
		randMove = rand() % count;
		return bookMoves[randMove];
	} else {
		return NOMOVE;
	}
}
