#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* TODO (swim): implement per the lesson description. */


#define MAX_NODE_COUNT 100

typedef struct{
    int id;
    int block_list[MAX_NODE_COUNT];
    int block_count;
    int is_up;
    char status[10];
} Node;

Node global_nodes[MAX_NODE_COUNT + 1];
int global_node_count = 0;

int direct_ping(const Node *a, const Node *b){
    if (!a->is_up || !b->is_up) {
        return 0;
    }

    for (int i = 0; i < a->block_count; i++){
        if (a->block_list[i] == b->id){
            return 0;
        }
    }
    return 1;
}

int indirect_or_direct_ping(int a, int b, int* helper_nodes, int helper_count){
    if (direct_ping(&global_nodes[a], &global_nodes[b])){
        return 1;
    }

    for (int i = 0; i < helper_count; i++){
        if (direct_ping(&global_nodes[helper_nodes[i]], &global_nodes[b])){
            return 1;
        }
    }

    strcpy(global_nodes[b].status, "suspect");
    return 0;
}

void down_node(int node_id){
    global_nodes[node_id].is_up = 0;
}

void block_path(int a, int b){
    // insert b in a's block list
    global_nodes[a].block_list[global_nodes[a].block_count++] = b;
    // insert a in b's block list
    global_nodes[b].block_list[global_nodes[b].block_count++] = a;
    return;
}

void print_nodes_status(void){
    for (int i = 1; i <= global_node_count; i++){
        printf("%d %s\n", global_nodes[i].id, global_nodes[i].status);
    }
}

void activate_nodes(int range){
    for (int i = 1; i <= range; i++){
        strcpy(global_nodes[i].status, "alive");
        global_nodes[i].block_count = 0;
        global_nodes[i].is_up = 1;
        global_nodes[i].id = i;
    }
    global_node_count = range;
    return;
}


int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        if(strncmp(line, "NODES", 5) == 0){
            char* cursor = line + 5;
            while(isspace((unsigned char)*cursor)) cursor++;
            int node_count = atoi(cursor);
            activate_nodes(node_count);
        }
        else if (strncmp(line, "DOWN", 4) == 0){
            char* cursor = line + 4;
            while(isspace((unsigned char)*cursor)) cursor++;
            int node_id = atoi(cursor);
            down_node(node_id);
        }
        else if (strncmp(line, "PING", 4) == 0){
            char* cursor = line + 4;
            while(isspace((unsigned char)*cursor)) cursor++;
            int a = strtol(cursor, &cursor, 10);
            while(isspace((unsigned char)*cursor)) cursor++;
            int b = strtol(cursor, &cursor, 10);
            while(isspace((unsigned char)*cursor)) cursor++;
            int helper_nodes[MAX_NODE_COUNT];
            int helper_count = 0;
            char* node_id_str = strtok(cursor, ",");
            while(node_id_str != NULL){
                helper_nodes[helper_count++] = atoi(node_id_str);
                node_id_str = strtok(NULL, ",");
            }
            if(indirect_or_direct_ping(a, b, helper_nodes, helper_count)){
                printf("alive\n");
            }
            else{
                printf("suspect\n");
            }
        }
        else if (strncmp(line, "BLOCK", 5) == 0){
            char* cursor = line + 5;
            while(isspace((unsigned char)*cursor)) cursor++;
            int a = strtol(cursor, &cursor, 10);
            while(isspace((unsigned char)*cursor)) cursor++;
            int b = strtol(cursor, &cursor, 10);
            block_path(a, b);
        }
        else if (strncmp(line, "STATUS", 6) == 0){
            print_nodes_status();
        }
    }
    return 0;
}
