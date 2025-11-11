#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* history[HISTORY_SIZE] = {0};
static int history_count = 0;

void add_to_history(const char* cmd) {
    if (!cmd || strlen(cmd) == 0)
        return;

    int index = history_count % HISTORY_SIZE;

    if (history[index])
        free(history[index]);

    history[index] = strdup(cmd);
    history_count++;
}

void print_history() {
    int start = (history_count > HISTORY_SIZE) ? history_count - HISTORY_SIZE : 0;
    int len = (history_count > HISTORY_SIZE) ? HISTORY_SIZE : history_count;
    for (int i = 0; i < len; i++) {
        int index = (start + i) % HISTORY_SIZE;
        printf("%d: %s\n", i + 1, history[index]);
    }
}

char* get_history_command(int n) {
    if (n < 1 || n > history_count) return NULL;
    int start = (history_count > HISTORY_SIZE) ? history_count - HISTORY_SIZE : 0;
    int index = (start + n - 1) % HISTORY_SIZE;
    return history[index];
}
