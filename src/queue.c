#include "queue.h"

#include <stdio.h>
#include <string.h>

Queue *queue_with_size(size_t size) {
    Queue *q = malloc(sizeof(Queue));

    size_t byte_size = sizeof(char) * size;
    q->data = malloc(byte_size);
    memset(q->data, 0, byte_size);

    q->alloc_size = size;
    q->size = 0;
    q->offset = 0;

    return q;
}

int requeue(Queue *q) {
    // If there is no available space,
    // requeueing won't do anything.
    if(q->size == q->alloc_size)
        return -1;
    
    size_t used_byte_size = sizeof(char) * q->size;
    char buf[q->size];

    char *relevant_data_start = q->data + q->offset;
    memcpy(buf, relevant_data_start, used_byte_size);

    memcpy(q->data, buf, used_byte_size);
    q->offset = 0;

    return 0;
}

int enqueue(Queue *q, char val) {
    if(q->size >= q->alloc_size)
        return QUEUE_FULL;

    size_t write_index = q->size + q->offset;

    if(write_index >= q->alloc_size) {
        // We know that a requeue is possible because we check that the queue is not full.
        requeue(q); 
        write_index = q->size;
    }

    q->data[write_index] = val;
    q->size++;

    return QUEUE_OK;
}

int dequeue(Queue *q, char *out) {
    // If there is no elements in the array, returns the null character.
    if(q->size < 1)
        return QUEUE_EMPTY;

    // If the offset is greater than data,
    // it points to nothing.
    if(q->offset >= q->alloc_size)
        return QUEUE_EMPTY;

    char front = q->data[q->offset];

    q->offset++; // Increment the front.
    q->size--;   // Decrease the size since we removed a character.

    *out = front;
    
    return QUEUE_OK;
}

void destroy_queue(Queue *q) {
    free(q->data);
    free(q);
}

void read_after_sequence(FILE *fp, const char *seq, char *buffer, size_t buffer_size) {
    size_t seq_size = strlen(seq);
    Queue *q = queue_with_size(3 * seq_size);
    int ch;
    int error;
    int found = 0;
    char elem;

    while((ch = fgetc(fp)) != EOF) {
        if(q->size < seq_size) {
            enqueue(q, ch);
            continue;
        }

        assert(q->size == seq_size);   // Window is of correct width

        if(!strncmp((char *)q->data + q->offset, seq, seq_size)) {
            found = 1;
            break;
        }
        
        error = dequeue(q, &elem);
        assert(error != QUEUE_EMPTY);  // Queue should always have at least `seq_size` items

        error = enqueue(q, ch);
        assert(error != QUEUE_FULL);    // Queue should maintain same size, as 1 item is added and another is removed
    }

    destroy_queue(q);

    if(!found) {
        buffer[0] = 0;  // make buffer an empty string if the sequence is not found
    }

    // Actually read for the rest of the file, or the buffer.
    for(size_t i = 0; i < buffer_size; ++i) {
        if((ch = fgetc(fp)) == EOF)
            break;

        buffer[i] = ch;
    }
}
