#pragma once

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define QUEUE_OK    0
#define QUEUE_EMPTY (-1)
#define QUEUE_FULL  (-2)

typedef struct {
    size_t offset;
    size_t size;
    size_t alloc_size;
    char *data;
} Queue;

Queue *queueWithSIze(size_t size);

int requeue(Queue *q);

int enqueue(Queue *q, char val);

int dequeue(Queue *q, char *out);

void destroyQueue(Queue *q);

void readAfterSequence(FILE *fp, const char *seq, char *buffer, size_t buffer_size);
