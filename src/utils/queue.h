#pragma once

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define QUEUE_OK     0
#define QUEUE_EMPTY (-1)
#define QUEUE_FULL  (-2)

typedef struct {
    size_t offset;
    size_t size;
    size_t alloc_size;
    char *data;
} Queue;

Queue *queue_with_size(size_t size);

int requeue(Queue *q);

int enqueue(Queue *q, char val);

int dequeue(Queue *q, char *out);

void destroy_queue(Queue *q);

void read_after_sequence(FILE *fp, const char *seq, char *buffer, size_t buffer_size);

#endif	// QUEUE_H
