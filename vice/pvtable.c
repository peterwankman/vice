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

const int PvSize = 0x100000 * 2;

int GetPvLine(const int depth, S_BOARD *pos) {
	int move = ProbePvMove(pos);
	int count = 0;
	
	ASSERT(depth < MAXDEPTH);

	while(move != NOMOVE && count < depth) {
		ASSERT(count < MAXDEPTH);

		if(MoveExists(pos, move)) {
			MakeMove(pos, move);
			pos->PvArray[count++] = move;
		} else {
			break;
		}
		move = ProbePvMove(pos);
	}

	while(pos->ply > 0) {
		TakeMove(pos);
	}

	return count;
}

void ClearHashTable(S_HASHTABLE *table) {

  S_HASHENTRY *tableEntry;
  
  for (tableEntry = table->pTable; tableEntry < table->pTable + table->numEntries; tableEntry++) {
    tableEntry->posKey = 0ULL;
    tableEntry->move = NOMOVE;
    tableEntry->depth = 0;
    tableEntry->score = 0;
    tableEntry->flags = 0;
  }
  table->newWrite=0;
}

void InitHashTable(S_HASHTABLE *table, const int MB) {  
	
	int HashSize = 0x100000 * MB;
    table->numEntries = HashSize / sizeof(S_HASHENTRY);
    table->numEntries -= 2;
	
	if(table->pTable!=NULL) {
		free(table->pTable);
	}
		
    table->pTable = (S_HASHENTRY *) malloc(table->numEntries * sizeof(S_HASHENTRY));
	if(table->pTable == NULL) {
		printf("Hash Allocation Failed, trying %dMB...\n",MB/2);
		InitHashTable(table,MB/2);
	} else {
		ClearHashTable(table);
		printf("HashTable init complete with %d entries\n",table->numEntries);
	}
	
}

/*
void StorePvMove(const S_BOARD *pos, const int move) {
	int index = pos->posKey % pos->HashTable->numEntries;

	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);

	pos->HashTable->pTable[index].move = move;
	pos->HashTable->pTable[index].posKey = pos->posKey;
}
*/

void StoreHashEntry(S_BOARD *pos, const int move, int score, const int flags, const int depth) {

	int index = pos->posKey % pos->HashTable->numEntries;
	
	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);
	ASSERT(depth>=1&&depth<MAXDEPTH);
    ASSERT(flags>=HFALPHA&&flags<=HFEXACT);
    ASSERT(score>=-INF&&score<=INF);
    ASSERT(pos->ply>=0&&pos->ply<MAXDEPTH);
	
	if( pos->HashTable->pTable[index].posKey == 0) {
		pos->HashTable->newWrite++;
	} else {
		pos->HashTable->overWrite++;
	}
	
	if(score > ISMATE) score += pos->ply;
    else if(score < -ISMATE) score -= pos->ply;
	
	pos->HashTable->pTable[index].move = move;
    pos->HashTable->pTable[index].posKey = pos->posKey;
	pos->HashTable->pTable[index].flags = flags;
	pos->HashTable->pTable[index].score = score;
	pos->HashTable->pTable[index].depth = depth;
}


int ProbeHashEntry(S_BOARD *pos, int *move, int *score, int alpha, int beta, int depth) {

	int index = pos->posKey % pos->HashTable->numEntries;
	
	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);
    ASSERT(depth>=1&&depth<MAXDEPTH);
    ASSERT(alpha<beta);
    ASSERT(alpha>=-INF&&alpha<=INF);
    ASSERT(beta>=-INF&&beta<=INF);
    ASSERT(pos->ply>=0&&pos->ply<MAXDEPTH);
	
	if( pos->HashTable->pTable[index].posKey == pos->posKey ) {
		*move = pos->HashTable->pTable[index].move;
		if(pos->HashTable->pTable[index].depth >= depth){
			pos->HashTable->hit++;
			
			ASSERT(pos->HashTable->pTable[index].depth>=1&&pos->HashTable->pTable[index].depth<MAXDEPTH);
            ASSERT(pos->HashTable->pTable[index].flags>=HFALPHA&&pos->HashTable->pTable[index].flags<=HFEXACT);
			
			*score = pos->HashTable->pTable[index].score;
			if(*score > ISMATE) *score -= pos->ply;
            else if(*score < -ISMATE) *score += pos->ply;
			
			switch(pos->HashTable->pTable[index].flags) {
				
                ASSERT(*score>=-INF&&*score<=INF);

                case HFALPHA: if(*score<=alpha) {
                    *score=alpha;
                    return TRUE;
                    }
                    break;
                case HFBETA: if(*score>=beta) {
                    *score=beta;
                    return TRUE;
                    }
                    break;
                case HFEXACT:
                    return TRUE;
                    break;
                default: ASSERT(FALSE); break;
            }
		}
	}
	
	return FALSE;
}

int ProbePvMove(const S_BOARD *pos) {

	int index = pos->posKey % pos->HashTable->numEntries;
	ASSERT(index >= 0 && index <= pos->HashTable->numEntries - 1);
	
	if( pos->HashTable->pTable[index].posKey == pos->posKey ) {
		return pos->HashTable->pTable[index].move;
	}
	
	return NOMOVE;
}