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

#ifndef DEFS_H
#define DEFS_H
#include <stdio.h>
#include <stdlib.h>

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
	do {\
		if(!(n)) { \
			printf("%s ", __DATE__); \
			printf("%s: ", __TIME__); \
			printf("Assertion '%s' failed.\n", #n); \
			printf("File '%s'\n", __FILE__); \
			printf("Line %d\n", __LINE__); \
			getchar(); \
			exit(1); \
		} \
	} while(0);
#endif

typedef unsigned long long U64;

#define NAME "Vicemod 1.0"
#define BRD_SQ_NUM 120

#define MAXGAMEMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64
#define MAX_HASH 1024

#define INF 30000
#define ISMATE (INF - MAXDEPTH)

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum { WHITE, BLACK, BOTH };
enum { UCIMODE, XBOARDMODE, CONSOLEMODE };

enum {
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8,
	NO_SQ, OFFBOARD
};

enum { FALSE, TRUE };

enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

enum {  HFNONE, HFALPHA, HFBETA, HFEXACT};

typedef struct {
	int move;
	int score;
} S_MOVE;

typedef struct {
	S_MOVE moves[MAXPOSITIONMOVES];
	int count;
} S_MOVELIST;

typedef struct {
	U64 posKey;
	int move, score, depth, flags;
} S_HASHENTRY;

typedef struct {
	S_HASHENTRY *pTable;
	int numEntries, newWrite, overWrite, hit, cut;
} S_HASHTABLE;

typedef struct {
	int move, castlePerm, enPas, fiftyMoves;
	U64 posKey;
} S_UNDO;

typedef struct {
	int pieces[BRD_SQ_NUM];
	U64 pawns[3];

	int KingSq[2];
	int side, enPas, fiftyMove;
	int ply, hisPly, castlePerm;

	U64 posKey;

	int pceNum[13];
	int bigPce[2];
	int majPce[2];
	int minPce[2];
	int material[2];

	S_UNDO history[MAXGAMEMOVES];
	int pList[13][10];

	S_HASHTABLE HashTable[1];

	int PvArray[MAXDEPTH];
	int searchHistory[13][BRD_SQ_NUM];
	int searchKillers[2][MAXDEPTH];
} S_BOARD;

typedef struct {
	int starttime, stoptime, depth, timeset, movestogo;
	int quit, stopped, nullCut;
	int GAME_MODE, POST_THINKING;

	long nodes;
	float fh, fhf;
} S_SEARCHINFO;

/* MACROS */

#define FR2SQ(f, r) ((21 + (f)) + ((r) * 10))
#define SQ64(sq120) (Sq120ToSq64[(sq120)])
#define SQ120(sq64) (Sq64ToSq120[(sq64)])
#define POP(b) PopBit(b)
#define CNT(b) CountBits(b)
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])

#define IsBQ(p) (PieceBishopQueen[(p)])
#define IsRQ(p) (PieceRookQueen[(p)])
#define IsKn(p) (PieceKnight[(p)])
#define IsKi(p) (PieceKing[(p)])

#define MIRROR64(sq) (Mirror64[(sq)])

#define FROMSQ(m) ((m) & 0x7f)
#define TOSQ(m) (((m) >> 7) & 0x7f)
#define CAPTURED(m) (((m) >> 14) & 0xf)
#define PROMOTED(m) (((m) >> 20) & 0xf)

#define MFLAGEP   0x00040000
#define MFLAGPS   0x00080000
#define MFLAGCA   0x01000000
#define MFLAGCAP  0x0007C000
#define MFLAGPROM 0x00f00000

#define NOMOVE 0

/* GLOBALS */

extern int Sq120ToSq64[BRD_SQ_NUM];
extern int Sq64ToSq120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];
extern U64 PieceKeys[13][120];
extern U64 SideKey;
extern U64 CastleKeys[16];
extern char PceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

extern int PieceBig[13];
extern int PieceMaj[13];
extern int PieceMin[13];
extern int PieceVal[13];
extern int PieceCol[13];

extern int FilesBrd[BRD_SQ_NUM];
extern int RanksBrd[BRD_SQ_NUM];

extern int PiecePawn[13];
extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];
extern int PieceSlides[13];

extern int Mirror64[64];

extern U64 FileBBMask[8];
extern U64 RankBBMask[8];

extern U64 BlackPassedMask[64];
extern U64 WhitePassedMask[64];
extern U64 IsolatedMask[64];

/* FUNCTIONS */

/* init.c */
extern void AllInit(void);

/* bitboard.c */
extern int PopBit(U64 *bb);
extern int CountBits(U64 bb);
extern void PrintBitBoard(U64 bb);

/* hashkeys.c */
extern U64 GeneratePositionKey(const S_BOARD *pos);

/* board.c */
extern void ResetBoard(S_BOARD *pos);
extern int ParseFen(char *fen, S_BOARD *pos);
extern void PrintBoard(const S_BOARD *pos);
extern void UpdateListsMaterial(S_BOARD *pos);
extern int CheckBoard(const S_BOARD *pos);
extern void MirrorBoard(S_BOARD *pos);

/* attack.c */
extern int SqAttacked(const int sq, const int side, const S_BOARD *pos);

/* io.c */
extern char *PrMove(const int move);
extern char *PrSq(const int sq);
extern int ParseMove(char *ptrChar, S_BOARD *pos);
extern void PrintMoveList(const S_MOVELIST *list);

/* validate.c */
extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);
extern void MirrorEvalTest(S_BOARD *pos);

/* movegen.c */
extern void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list);
extern int MoveExists(S_BOARD *pos, const int move);
extern void InitMvvLva(void);
extern void GenerateAllCaps(const S_BOARD *pos, S_MOVELIST *list);

/* makemove.c */
extern int MakeMove(S_BOARD *pos, int move);
extern void TakeMove(S_BOARD *pos);
extern void MakeNullMove(S_BOARD *pos);
extern void TakeNullMove(S_BOARD *pos);

/* perft.c */
extern void PerftTest(int depth, S_BOARD *pos);

/* search.c */
extern void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info);

/* misc.c */
extern int GetTimeMs(void);
extern void ReadInput(S_SEARCHINFO *info);

/* pventry.c */
extern void InitHashTable(S_HASHTABLE *table, const int MB);
extern void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth);
extern int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth);
extern int ProbePvMove(const S_BOARD *pos);
extern int GetPvLine(const int depth, S_BOARD *pos);
extern void ClearHashTable(S_HASHTABLE *table);

/* evaluate.c */
extern int EvalPosition(const S_BOARD *pos);

/* uci.c */
extern void Uci_Loop(S_BOARD *pos, S_SEARCHINFO *info);

extern void XBoard_Loop(S_BOARD *pos, S_SEARCHINFO *info);
extern void Console_Loop(S_BOARD *pos, S_SEARCHINFO *info);
#endif