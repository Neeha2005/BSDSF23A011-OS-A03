#include "../include/shell.h"

static char *history_buf[HISTORY_SIZE];
static int history_count = 0;

void add_to_history_custom(const char *cmd) {
    if (!cmd || strlen(cmd) == 0) return;
    if (history_count < HISTORY_SIZE) {
        history_buf[history_count++] = strdup(cmd);
    } else {
        free(history_buf[0]);
        memmove(history_buf, history_buf + 1, sizeof(char*) * (HISTORY_SIZE - 1));
        history_buf[HISTORY_SIZE - 1] = strdup(cmd);
    }
}

void show_history(void) {
    int start = (history_count > HISTORY_SIZE) ? history_count - HISTORY_SIZE : 0;
    int len = (history_count > HISTORY_SIZE) ? HISTORY_SIZE : history_count;
    printf("---- Command History ----\n");
    for (int i = 0; i < len; ++i) {
        int idx = start + i;
        printf("%d: %s\n", i + 1, history_buf[idx]);
    }
}
