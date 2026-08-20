#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* TODO (membership): implement per the lesson description. */
#define MAX_NODES_COUNT 64

typedef struct{
    char* name;
    int version;
    char* status;
}Node;

Node global_nodes[MAX_NODES_COUNT];
int global_nodes_count = 0;

char* copy_str(char* str){
    char* copy = malloc(strlen(str) + 1);
    strcpy(copy, str);
    copy[strlen(str)] = '\0';
    return copy;
}

int find_node(char* name){
    for(int i = 0; i < global_nodes_count; i++){
        if(strcmp(global_nodes[i].name, name) == 0){
            return i;
        }
    }

    return -1;
}

void add_or_update_node(char* line){
    char* cursor = line;
    while (isspace((unsigned char)*cursor)) cursor++;
    char* name = cursor;
    char* space = strchr(name, ' ');
    if (space == NULL) return;
    *space = '\0';
    cursor = space + 1;
    int version = (int)strtol(cursor, &cursor, 10);
    while (isspace((unsigned char)*cursor)) cursor++;
    char* status = cursor;
    status[strcspn(status, "\r\n")] = '\0';

    int node_idx = find_node(name);
    if (node_idx == -1) {
        node_idx = global_nodes_count++;
        global_nodes[node_idx].version = version;
        global_nodes[node_idx].status = copy_str(status);
        global_nodes[node_idx].name = copy_str(name);
        printf("APPLIED\n");
    } else if (version > global_nodes[node_idx].version) {
        global_nodes[node_idx].version = version;
        free(global_nodes[node_idx].status);
        global_nodes[node_idx].status = copy_str(status);
        printf("APPLIED\n");
    } else {
        printf("STALE\n");
    }
}

int compare_nodes(const void* a, const void* b){
    Node* node_a = (Node*)a;
    Node* node_b = (Node*)b;
    return strcmp(node_a->name, node_b->name);
}

void list_nodes(){
    qsort(global_nodes, global_nodes_count, sizeof(Node), compare_nodes);
    for(int i = 0; i < global_nodes_count; i++){
        printf("%s v=%d %s\n", global_nodes[i].name, global_nodes[i].version, global_nodes[i].status);
    }
}


int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        if (strncmp(line, "UPDATE", 6) == 0){
            char* cursor = line + 6;
            while(isspace(*cursor)) cursor++;
            add_or_update_node(cursor);
        }
        else if (strncmp(line, "LIST", 4) == 0){
            list_nodes();
        }
    }

    return 0;
}
