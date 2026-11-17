#include "ranking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RankingInit(RankList *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void RankingFree(RankList *list)
{
    RankEntry *entry = list->head;
    while (entry)
    {
        RankEntry *next = entry->next;
        free(entry);
        entry = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

RankEntry *RankingInsert(RankList *list, const char *name, int score, int *position)
{
    RankEntry *entry = malloc(sizeof *entry);
    strncpy(entry->name, name, NAME_MAX_LENGTH);
    entry->name[NAME_MAX_LENGTH] = '\0';
    entry->score = score;

    RankEntry *node = list->tail;
    while (node && node->score < score)
        node = node->prev;

    if (node)
    {
        entry->prev = node;
        entry->next = node->next;
        if (node->next) node->next->prev = entry;
        else list->tail = entry;
        node->next = entry;
    }
    else
    {
        entry->prev = NULL;
        entry->next = list->head;
        if (list->head) list->head->prev = entry;
        else list->tail = entry;
        list->head = entry;
    }

    list->count++;

    if (position)
    {
        int pos = 1;
        for (RankEntry *walk = entry->prev; walk; walk = walk->prev) pos++;
        *position = pos;
    }

    return entry;
}

void RankingLoad(RankList *list, const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file) return;

    char line[128];
    while (fgets(line, sizeof line, file))
    {
        char name[NAME_MAX_LENGTH + 1];
        int score;
        if (sscanf(line, "%10s %d", name, &score) == 2)
            RankingInsert(list, name, score, NULL);
    }

    fclose(file);
}

void RankingSave(const RankList *list, const char *path)
{
    FILE *file = fopen(path, "w");
    if (!file) return;

    for (RankEntry *entry = list->head; entry; entry = entry->next)
        fprintf(file, "%s %d\n", entry->name, entry->score);

    fclose(file);
}

void RankingGetPath(char *buffer, int size)
{
    snprintf(buffer, size, "%sranking.txt", GetApplicationDirectory());
}
