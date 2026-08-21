#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TODO (applications): implement per the lesson description. */


# define MAX_DC_COUNT 64
int time_step = 0;
typedef struct {
    char dc_id;
    int total_nodes;
    int infected_nodes;
    char neighbors[MAX_DC_COUNT];
    int neighbor_count;
} DataCerter;

DataCerter global_DCs[MAX_DC_COUNT];
DataCerter history_global_DCs[MAX_DC_COUNT];
int global_DC_count = 0;

void snapshot(void) {
     // deep copy
    memcpy(history_global_DCs, global_DCs, sizeof(DataCerter) * global_DC_count);
}

int find_dc_idx(char dc_id) {
    for(int i = 0; i < global_DC_count; i++){
        if(global_DCs[i].dc_id == dc_id) return i;
    }
    return -1;
}

void infect_dc(char dc_id, int infected_nodes) {
    int dc_idx = find_dc_idx(dc_id);
    if(dc_idx == -1) return;
    DataCerter* dc = &global_DCs[dc_idx];
    dc->infected_nodes = infected_nodes + dc->infected_nodes > dc->total_nodes ? dc->total_nodes : infected_nodes + dc->infected_nodes;
    snapshot();
}

void link_dc(char dc_id1, char dc_id2) {
    int dc_idx1 = find_dc_idx(dc_id1);
    int dc_idx2 = find_dc_idx(dc_id2);
    if(dc_idx1 == -1 || dc_idx2 == -1) return;
    DataCerter* dc1 = &global_DCs[dc_idx1];
    DataCerter* dc2 = &global_DCs[dc_idx2];
    dc1->neighbors[dc1->neighbor_count] = dc_id2;
    dc1->neighbor_count++;
    dc2->neighbors[dc2->neighbor_count] = dc_id1;
    dc2->neighbor_count++;
    snapshot();
}


void init_dc(char dc_id, int total_nodes) {
    DataCerter* dc = &global_DCs[global_DC_count];
    dc->dc_id = dc_id;
    dc->total_nodes = total_nodes;
    dc->infected_nodes = 0;
    dc->neighbor_count = 0;
    global_DC_count++;
    snapshot();
}

void time_go_forward(int fan_out) {
    time_step++;
    // internal infection
    for(int i = 0; i < global_DC_count; i++){
        int newly_infected_nodes_delta = history_global_DCs[i].infected_nodes * fan_out;
        int newly_infected_nodes = newly_infected_nodes_delta + history_global_DCs[i].infected_nodes;
        global_DCs[i].infected_nodes = newly_infected_nodes > history_global_DCs[i].total_nodes ? history_global_DCs[i].total_nodes : newly_infected_nodes;
        global_DCs[i].total_nodes = history_global_DCs[i].total_nodes;
    }

    // cross - center infection
    if(time_step % 10 != 0) {snapshot(); return;}

    for(int i = 0; i < global_DC_count; i++){
        if(history_global_DCs[i].neighbor_count == 0) continue;
        if(history_global_DCs[i].infected_nodes == 0) continue;
        char first_neighbor = history_global_DCs[i].neighbors[0];
        int first_neighbor_idx = find_dc_idx(first_neighbor);
        DataCerter* dc = &global_DCs[first_neighbor_idx];
        if(dc->infected_nodes == dc->total_nodes) continue;
        dc->infected_nodes += 1;
    }
    snapshot();
    return;
}

void print_dc_stats(void) {
    // DC <name>: <infected>/<node_count> infected
    for(int i = 0; i < global_DC_count; i++) {
        printf("DC %c: %d/%d infected\n", global_DCs[i].dc_id, global_DCs[i].infected_nodes, global_DCs[i].total_nodes);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        if(strncmp(line, "INIT_DC", 7) == 0) {
            char dc_id;
            int total_nodes;
            sscanf(line + 7, " %c %d", &dc_id, &total_nodes);
            init_dc(dc_id, total_nodes);
        }
        else if (strncmp(line, "LINK_DC", 7) == 0) {
            char dc_id1;
            char dc_id2;
            sscanf(line + 7, " %c %c", &dc_id1, &dc_id2);
            link_dc(dc_id1, dc_id2);
        }
        else if (strncmp(line, "INFECT", 6) == 0) {
            char dc_id;
            int infected_node_count;
            sscanf(line + 6, " %c %d", &dc_id, &infected_node_count);
            infect_dc(dc_id, infected_node_count);
        }
        else if (strncmp(line, "ROUND", 5) == 0) {
            int time_interval;
            sscanf(line + 5, "%d", &time_interval);
            time_go_forward(time_interval);
        }
        else if (strncmp(line, "STATS", 5) == 0){
            print_dc_stats();
        }
    }
    return 0;
}
