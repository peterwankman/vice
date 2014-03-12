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

#define INF 30000
#define MATE 29000

static void CheckUp(S_SEARCHINFO *info) {
	if(info->timeset == TRUE && GetTimeMs() > info->stoptime)
		info->stopped = TRUE;

	ReadInput(info);
}

static void PickNextMove(int moveNum, S_MOVELIST *list) {
	S_MOVE temp;
	int index = 0;
	int bestScore = 0;
	int bestNum = moveNum;

	for(index = moveNum; index < list->count; ++index) {
		if(list->moves[index].score > bestScore) {
			bestScore = list->moves[index].score;
			bestNum = index;
		}
	}

	temp = list->moves[moveNum];
	list->moves[moveNum] = list->moves[bestNum];
	list->moves[bestNum] = temp;
}

static int IsRepetition(const S_BOARD *pos) {
	int index = 0;

	for(index = pos->hisPly - pos->fiftyMove; index < pos->hisPly; ++index) {
		ASSERT(index >= 0 && index < MAXGAMEMOVES);
		if(pos->posKey == pos->history[index].posKey)
			return TRUE;
	}

	return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
	int index = 0;
	int index2 = 0;

	for(index = 0; index < 13; ++index)
		for(index2 = 0; index2 < BRD_SQ_NUM; ++index2)
			pos->searchHistory[index][index2] = 0;

	for(index = 0; index < 2; ++index)
		for(index2 = 0; index2 < MAXDEPTH; ++index2)
			pos->searchKillers[index][index2] = 0;

	ClearHashTable(pos->HashTable);
	pos->ply = 0;

	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	int MoveNum = 0;
	int Legal = 0;
	int Score;
	S_MOVELIST list[1];

	ASSERT(CheckBoard(pos));
	ASSERT(beta > alpha);

	if((info->nodes & 2047) == 0)
		CheckUp(info);

	info->nodes++;

	if(IsRepetition(pos) || pos->fiftyMove >= 100)
		return 0;

	if(pos->ply > MAXDEPTH - 1)
		return EvalPosition(pos);

	Score = EvalPosition(pos);

	ASSERT(Score > -INF && Score < INF);

	if(Score >= beta)
		return beta;

	if(Score > alpha)
		alpha = Score;

	GenerateAllCaps(pos, list);

	Score = -INF;

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		PickNextMove(MoveNum, list);

		if (!MakeMove(pos, list->moves[MoveNum].move))
			continue;

		Legal++;
		Score = -Quiescence(-beta, -alpha, pos, info);
		TakeMove(pos);

		if(info->stopped == TRUE)
			return 0;

		if(Score > alpha) {
			if(Score >= beta) {
				if(Legal == 1)
					info->fhf++;
				info->fh++;
				return beta;
			}
			alpha = Score;
		}
	}

	return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {
	S_MOVELIST list[1];
	int InCheck;
	int Score = -INF;
	int PvMove = NOMOVE;
	int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int BestScore = -INF;

	ASSERT(CheckBoard(pos));
	ASSERT(beta > alpha);
	ASSERT(depth >= 0);

	if(depth <= 0)
		return Quiescence(alpha, beta, pos, info);

	if((info->nodes & 2047) == 0)
		CheckUp(info);

	info->nodes++;

	if((IsRepetition(pos) || pos->fiftyMove >= 100) && pos->ply)
		return 0;

	if(pos->ply > MAXDEPTH - 1)
		return EvalPosition(pos);

	InCheck = SqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos);

	if(InCheck == TRUE)
		depth++;

	if(ProbeHashEntry(pos, &PvMove, &Score, alpha, beta, depth) == TRUE) {
		pos->HashTable->cut++;
		return Score;
	}

	if(DoNull && !InCheck && pos->ply && (pos->bigPce[pos->side] > 0) && depth >= 4) {
		MakeNullMove(pos);
		Score = -AlphaBeta(-beta, -beta + 1, depth - 4, pos, info, FALSE);
		TakeNullMove(pos);
		if(info->stopped == TRUE)
			return 0;

		if (Score >= beta && abs(Score) < ISMATE) {
			info->nullCut++;
			return beta;
		}
	}

	GenerateAllMoves(pos, list);

	Score = -INF;

	if(PvMove != NOMOVE) {
		for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
			if( list->moves[MoveNum].move == PvMove) {
				list->moves[MoveNum].score = 2000000;
				break;
			}
		}
	}

	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {
		PickNextMove(MoveNum, list);

		if(!MakeMove(pos,list->moves[MoveNum].move))
			continue;

		Legal++;
		Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);
		TakeMove(pos);

		if(info->stopped == TRUE)
			return 0;

		if(Score > BestScore) {
			BestScore = Score;
			BestMove = list->moves[MoveNum].move;
			if(Score > alpha) {
				if(Score >= beta) {
					if(Legal == 1)
						info->fhf++;
					info->fh++;

					if(!(list->moves[MoveNum].move & MFLAGCAP)) {
						pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
						pos->searchKillers[0][pos->ply] = list->moves[MoveNum].move;
					}

					StoreHashEntry(pos, BestMove, beta, HFBETA, depth);
					return beta;
				}
				alpha = Score;

				if(!(list->moves[MoveNum].move & MFLAGCAP))
					pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] += depth;
			}
		}
	}

	if(Legal == 0) {
		if(InCheck) {
			return -INF + pos->ply;
		} else {
			return 0;
		}
	}

	ASSERT(alpha >= OldAlpha);

	if(alpha != OldAlpha) {
		StoreHashEntry(pos, BestMove, BestScore, HFEXACT, depth);
	} else {
		StoreHashEntry(pos, BestMove, alpha, HFALPHA, depth);
	}

	return alpha;
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
	int bestMove = NOMOVE;
	int bestScore = -INF;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;

	ClearForSearch(pos, info);
	for(currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {
		bestScore = AlphaBeta(-INF, INF, currentDepth, pos, info, TRUE);

		if(info->stopped == TRUE)
			break;

		pvMoves = GetPvLine(currentDepth, pos);
		bestMove = pos->PvArray[0];

		if(info->GAME_MODE == UCIMODE) {
			printf("info score cp %d depth %d nodes %ld time %d ",
				bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime);
		} else if(info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
			printf("%d %d %d %ld ",
				currentDepth, bestScore, (GetTimeMs() - info->starttime) / 10, info->nodes);
		} else if(info->POST_THINKING == TRUE) {
			printf("score:%d depth:%d nodes:%ld time:%d(ms) ",
				bestScore, currentDepth, info->nodes, GetTimeMs() - info->starttime);
		}

		if(info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {
			pvMoves = GetPvLine(currentDepth, pos);
			printf("pv");
			for(pvNum = 0; pvNum < pvMoves; ++pvNum)
				printf(" %s", PrMove(pos->PvArray[pvNum]));
			printf("\n");
		}
	}

	if(info->GAME_MODE == UCIMODE) {
		printf("bestmove %s\n", PrMove(bestMove));
	} else if(info->GAME_MODE == XBOARDMODE) {	
		printf("move %s\n", PrMove(bestMove));
		MakeMove(pos, bestMove);
	} else {
		printf("\n\n***!! Vice makes move %s !!***\n\n", PrMove(bestMove));
		MakeMove(pos, bestMove);
		PrintBoard(pos);
	}
}
