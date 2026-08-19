#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#define MAX_MESSAGE_COUNT 32
#define MAX_NODE_COUNT 16
/* TODO (why-gossip): implement per the lesson description. */
typedef struct{
    char* key;
    char* val;
    int version;
} GossipMessage;

typedef struct{
    char node_id;
    GossipMessage messages[MAX_MESSAGE_COUNT];
    int message_count;
} GossipNode;

GossipNode global_nodes[MAX_NODE_COUNT];
int global_node_count = 0;

static char *copy_text(const char *text) {
    size_t length = strlen(text) + 1;
    char *copy = malloc(length);
    if (copy != NULL) {
        memcpy(copy, text, length);
    }
    return copy;
}

void add_node_info(char* node_info, char node_id) {
    if (global_node_count == MAX_NODE_COUNT) {
        return;
    }

    GossipNode* node = &global_nodes[global_node_count];
    node->node_id = node_id;
    node->message_count = 0;
    global_node_count++;

    char* msg = strtok(node_info, " \t\r\n");
    while (msg != NULL && node->message_count < MAX_MESSAGE_COUNT) {
        char* eq_mark = strchr(msg, '=');
        if (eq_mark != NULL) {
            char* v_mark;
            *eq_mark = '\0';
            v_mark = strrchr(eq_mark + 1, 'v');
            if (v_mark != NULL) {
                *v_mark = '\0';
                node->messages[node->message_count].key = copy_text(msg);
                node->messages[node->message_count].val = copy_text(eq_mark + 1);
                node->messages[node->message_count].version = atoi(v_mark + 1);
                if (node->messages[node->message_count].key != NULL &&
                    node->messages[node->message_count].val != NULL) {
                    node->message_count++;
                }
            }
        }
        msg = strtok(NULL, " \t\r\n");
    }
}

int find_node_index(char node_id) {
    for (int i = 0; i < global_node_count; i++) {
        if (node_id == global_nodes[i].node_id) {
            return i;
        }
    }
    return -1;
}

int find_message_index(const GossipNode *node, const char *key) {
    for (int i = 0; i < node->message_count; i++) {
        if (strcmp(node->messages[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void print_node(const GossipNode *node) {
    printf("%c:", node->node_id);
    for (int i = 0; i < node->message_count; i++) {
        const GossipMessage *msg = &node->messages[i];
        printf(" %s=%sv%d", msg->key, msg->val, msg->version);
    }
    printf("\n");
}

void evaluate_merge(char merge_nodes[2]) {
    char merge_node1 = merge_nodes[0];
    char merge_node2 = merge_nodes[1];
    int node1_index = find_node_index(merge_node1);
    int node2_index = find_node_index(merge_node2);
    if(node1_index == -1 || node2_index == -1){
        printf("Error: node %c or node %c not found\n", merge_node1, merge_node2);
        return;
    }
    GossipNode* node1 = &global_nodes[node1_index];
    GossipNode* node2 = &global_nodes[node2_index];

    for (int i = 0; i < node2->message_count; i++) {
        GossipMessage *incoming = &node2->messages[i];
        int message_index = find_message_index(node1, incoming->key);
        if (message_index == -1) {
            if (node1->message_count < MAX_MESSAGE_COUNT) {
                node1->messages[node1->message_count++] = *incoming;
            }
        } else if (incoming->version > node1->messages[message_index].version) {
            node1->messages[message_index] = *incoming;
        }
    }

    node2->message_count = 0;
    for (int i = 0; i < node1->message_count; i++) {
        node2->messages[node2->message_count++] = node1->messages[i];
    }

    print_node(node1);
    print_node(node2);
}



int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        if (strncmp(line, "STATE", 5) == 0) {
            char* node_info = line + 5;
            while (isspace((unsigned char)*node_info)) {
                node_info++;
            }
            char node_id = *node_info;
            node_info++;
            while (isspace((unsigned char)*node_info)) {
                node_info++;
            }
            add_node_info(node_info, node_id);

        } else if (strncmp(line, "MERGE", 5) == 0) {
            char* merge_info = line + 5;
            while (isspace((unsigned char)*merge_info)) {
                merge_info++;
            }
            char merge_nodes[2];
            merge_nodes[0] = *merge_info++;
            while (isspace((unsigned char)*merge_info)) {
                merge_info++;
            }
            merge_nodes[1] = *merge_info;
            evaluate_merge(merge_nodes);
        }
    }
    return 0;
}
