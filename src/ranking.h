#ifndef RANKING_H
#define RANKING_H

#include "game.h"

typedef struct RankEntry {
    char name[NAME_MAX_LENGTH + 1];
    int score;
    struct RankEntry *prev, *next;
} RankEntry;

typedef struct {
    RankEntry *head, *tail;
    int count;
} RankList;

void RankingInit(RankList *list);
void RankingFree(RankList *list);
RankEntry *RankingInsert(RankList *list, const char *name, int score, int *position);
void RankingLoad(RankList *list, const char *path);
void RankingSave(const RankList *list, const char *path);
void RankingGetPath(char *buffer, int size);

#endif
