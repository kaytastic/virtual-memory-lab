#include "oslabs.h"

int fifo_victim(struct PTE page_table[TABLEMAX], int table_cnt) {
    int v = -1;
    for (int i = 0; i < table_cnt; i++) {
        if (page_table[i].is_valid) {
            if (v == -1 || page_table[i].arrival_timestamp < page_table[v].arrival_timestamp)
                v = i;
        }
    }
    return v;
}


int lru_victim(struct PTE page_table[TABLEMAX], int table_cnt) {
    int v = -1;
    for (int i = 0; i < table_cnt; i++) {
        if (page_table[i].is_valid) {
            if (v == -1 || page_table[i].last_access_timestamp < page_table[v].last_access_timestamp)
                v = i;
        }
    }
    return v;
}

int lfu_victim(struct PTE page_table[TABLEMAX], int table_cnt) {
    int v = -1;
    for (int i = 0; i < table_cnt; i++) {
        if (page_table[i].is_valid) {
            if (v == -1 ||
                page_table[i].reference_count < page_table[v].reference_count ||
               (page_table[i].reference_count == page_table[v].reference_count &&
                page_table[i].arrival_timestamp < page_table[v].arrival_timestamp))
                v = i;
        }
    }
    return v;
}


int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt,
                             int page_number, int frame_pool[POOLMAX],
                             int *frame_cnt, int current_timestamp)
{
    struct PTE *p = &page_table[page_number];

    if (p->is_valid) { 
        p->last_access_timestamp = current_timestamp;
        p->reference_count++;
        return p->frame_number;
    }

    if (*frame_cnt > 0) { 
        int f = frame_pool[--(*frame_cnt)];
        p->is_valid = 1;
        p->frame_number = f;
        p->arrival_timestamp = current_timestamp;
        p->last_access_timestamp = current_timestamp;
        p->reference_count = 1;
        return f;
    }


    int v = fifo_victim(page_table, *table_cnt);
    int f = page_table[v].frame_number;

    page_table[v].is_valid = 0;
    page_table[v].frame_number = -1;
    page_table[v].arrival_timestamp = 0;
    page_table[v].last_access_timestamp = 0;
    page_table[v].reference_count = 0;

    p->is_valid = 1;
    p->frame_number = f;
    p->arrival_timestamp = current_timestamp;
    p->last_access_timestamp = current_timestamp;
    p->reference_count = 1;

    return f;
}


int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt,
                           int reference_string[REFERENCEMAX], int reference_cnt,
                           int frame_pool[POOLMAX], int frame_cnt)
{
    int faults = 0, t = 1;

    for (int i = 0; i < reference_cnt; i++, t++) {
        int page = reference_string[i];
        struct PTE *p = &page_table[page];

        if (p->is_valid) { 
            p->last_access_timestamp = t;
            p->reference_count++;
            continue;
        }

        faults++;

        if (frame_cnt > 0) { 
            int f = frame_pool[--frame_cnt];
            p->is_valid = 1;
            p->frame_number = f;
            p->arrival_timestamp = t;
            p->last_access_timestamp = t;
            p->reference_count = 1;
            continue;
        }

       
        int v = fifo_victim(page_table, table_cnt);
        int f = page_table[v].frame_number;

        page_table[v].is_valid = 0;
        page_table[v].frame_number = -1;
        page_table[v].arrival_timestamp = 0;
        page_table[v].last_access_timestamp = 0;
        page_table[v].reference_count = 0;

        p->is_valid = 1;
        p->frame_number = f;
        p->arrival_timestamp = t;
        p->last_access_timestamp = t;
        p->reference_count = 1;
    }

    return faults;
}

int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt,
                            int page_number, int frame_pool[POOLMAX],
                            int *frame_cnt, int current_timestamp)
{
    struct PTE *p = &page_table[page_number];

    if (p->is_valid) {
        p->last_access_timestamp = current_timestamp;
        p->reference_count++;
        return p->frame_number;
    }

    if (*frame_cnt > 0) {
        int f = frame_pool[--(*frame_cnt)];
        p->is_valid = 1;
        p->frame_number = f;
        p->arrival_timestamp = current_timestamp;
        p->last_access_timestamp = current_timestamp;
        p->reference_count = 1;
        return f;
    }

    int v = lru_victim(page_table, *table_cnt);
    int f = page_table[v].frame_number;

    page_table[v].is_valid = 0;
    page_table[v].frame_number = -1;
    page_table[v].arrival_timestamp = 0;
    page_table[v].last_access_timestamp = 0;
    page_table[v].reference_count = 0;

    p->is_valid = 1;
    p->frame_number = f;
    p->arrival_timestamp = current_timestamp;
    p->last_access_timestamp = current_timestamp;
    p->reference_count = 1;

    return f;
}


int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt,
                          int reference_string[REFERENCEMAX], int reference_cnt,
                          int frame_pool[POOLMAX], int frame_cnt)
{
    int faults = 0, t = 1;

    for (int i = 0; i < reference_cnt; i++, t++) {
        int page = reference_string[i];
        struct PTE *p = &page_table[page];

        if (p->is_valid) {
            p->last_access_timestamp = t;
            p->reference_count++;
            continue;
        }

        faults++;

        if (frame_cnt > 0) {
            int f = frame_pool[--frame_cnt];
            p->is_valid = 1;
            p->frame_number = f;
            p->arrival_timestamp = t;
            p->last_access_timestamp = t;
            p->reference_count = 1;
            continue;
        }

        int v = lru_victim(page_table, table_cnt);
        int f = page_table[v].frame_number;

        page_table[v].is_valid = 0;
        page_table[v].frame_number = -1;
        page_table[v].arrival_timestamp = 0;
        page_table[v].last_access_timestamp = 0;
        page_table[v].reference_count = 0;

        p->is_valid = 1;
        p->frame_number = f;
        p->arrival_timestamp = t;
        p->last_access_timestamp = t;
        p->reference_count = 1;
    }

    return faults;
}


int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt,
                            int page_number, int frame_pool[POOLMAX],
                            int *frame_cnt, int current_timestamp)
{
    struct PTE *p = &page_table[page_number];

    if (p->is_valid) {
        p->last_access_timestamp = current_timestamp;
        p->reference_count++;
        return p->frame_number;
    }

    if (*frame_cnt > 0) {
        int f = frame_pool[--(*frame_cnt)];
        p->is_valid = 1;
        p->frame_number = f;
        p->arrival_timestamp = current_timestamp;
        p->last_access_timestamp = current_timestamp;
        p->reference_count = 1;
        return f;
    }

    int v = lfu_victim(page_table, *table_cnt);
    int f = page_table[v].frame_number;

    page_table[v].is_valid = 0;
    page_table[v].frame_number = -1;
    page_table[v].arrival_timestamp = 0;
    page_table[v].last_access_timestamp = 0;
    page_table[v].reference_count = 0;

    p->is_valid = 1;
    p->frame_number = f;
    p->arrival_timestamp = current_timestamp;
    p->last_access_timestamp = current_timestamp;
    p->reference_count = 1;

    return f;
}


int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt,
                          int reference_string[REFERENCEMAX], int reference_cnt,
                          int frame_pool[POOLMAX], int frame_cnt)
{
    int faults = 0, t = 1;

    for (int i = 0; i < reference_cnt; i++, t++) {
        int page = reference_string[i];
        struct PTE *p = &page_table[page];

        if (p->is_valid) {
            p->last_access_timestamp = t;
            p->reference_count++;
            continue;
        }

        faults++;

        if (frame_cnt > 0) {
            int f = frame_pool[--frame_cnt];
            p->is_valid = 1;
            p->frame_number = f;
            p->arrival_timestamp = t;
            p->last_access_timestamp = t;
            p->reference_count = 1;
            continue;
        }

        int v = lfu_victim(page_table, table_cnt);
        int f = page_table[v].frame_number;

        page_table[v].is_valid = 0;
        page_table[v].arrival_timestamp = 0;
        page_table[v].last_access_timestamp = 0;
        page_table[v].reference_count = 0;
        page_table[v].frame_number = -1;

        p->is_valid = 1;
        p->frame_number = f;
        p->arrival_timestamp = t;
        p->last_access_timestamp = t;
        p->reference_count = 1;
    }

    return faults;
}
