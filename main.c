#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INTERVAL_COUNT 200
#define MAX_NODE_COUNT 64
#define DEFAULT_MEAN 1000.0
#define DEFAULT_STDDEV 200.0

typedef struct {
    char name;
    int intervals[MAX_INTERVAL_COUNT];
    int interval_count;
    int last_arrival_time;
    int has_arrival;
    double mean;
    double stddev;
} Node;

static Node nodes[MAX_NODE_COUNT];
static int node_count = 0;

static int find_node_index(char name) {
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].name == name) {
            return i;
        }
    }
    return -1;
}

static int find_or_create_node_index(char name) {
    int index = find_node_index(name);
    if (index >= 0) {
        return index;
    }

    if (node_count == MAX_NODE_COUNT) {
        return -1;
    }

    nodes[node_count] = (Node){
        .name = name,
        .mean = DEFAULT_MEAN,
        .stddev = DEFAULT_STDDEV,
    };
    return node_count++;
}

static void update_interval_stats(Node *node) {
    if (node->interval_count == 0) {
        node->mean = DEFAULT_MEAN;
        node->stddev = DEFAULT_STDDEV;
        return;
    }

    double sum = 0.0;
    for (int i = 0; i < node->interval_count; i++) {
        sum += node->intervals[i];
    }
    node->mean = sum / node->interval_count;

    double squared_difference_sum = 0.0;
    for (int i = 0; i < node->interval_count; i++) {
        double difference = node->intervals[i] - node->mean;
        squared_difference_sum += difference * difference;
    }
    node->stddev = sqrt(squared_difference_sum / node->interval_count);
}

static void add_heartbeat(char name, int arrival_time) {
    int index = find_or_create_node_index(name);
    if (index < 0) {
        return;
    }

    Node *node = &nodes[index];
    if (node->has_arrival) {
        int interval = arrival_time - node->last_arrival_time;
        if (node->interval_count < MAX_INTERVAL_COUNT) {
            node->intervals[node->interval_count++] = interval;
        }
        update_interval_stats(node);
    }

    node->last_arrival_time = arrival_time;
    node->has_arrival = 1;
}

static double calculate_phi(char name, int current_time) {
    int index = find_node_index(name);
    if (index < 0 || !nodes[index].has_arrival) {
        return 0.0;
    }

    Node *node = &nodes[index];
    double elapsed = current_time - node->last_arrival_time;
    if (node->stddev == 0.0) {
        return 0.0;
    }
    return (elapsed - node->mean) / node->stddev;
}

int main(void) {
    char line[1024];

    while (fgets(line, sizeof line, stdin)) {
        char command[16];
        char name;
        int timestamp;

        if (sscanf(line, "%15s %c %d", command, &name, &timestamp) != 3) {
            continue;
        }

        if (strcmp(command, "HEARTBEAT") == 0) {
            add_heartbeat(name, timestamp);
        } else if (strcmp(command, "PHI") == 0) {
            printf("phi=%.2f\n", calculate_phi(name, timestamp));
        }
    }

    return 0;
}
