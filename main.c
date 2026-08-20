#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int total_nodes_count = 0;
int infected_nodes_count = 0;
int global_round = 0;
int converged_global_round = -1;

void add_global_round(int fanout) {
    global_round++;
    int infected_count_before_this_global_round = infected_nodes_count;
    int uninfected_count_before_this_global_round = total_nodes_count - infected_nodes_count;
    int potential_newly_infected_nodes_count = fanout * infected_count_before_this_global_round;
    int newly_infected_nodes_count =
        potential_newly_infected_nodes_count < uninfected_count_before_this_global_round
            ? potential_newly_infected_nodes_count
            : uninfected_count_before_this_global_round;
    infected_nodes_count += newly_infected_nodes_count;
    if (infected_nodes_count >= total_nodes_count && converged_global_round == -1) {
        converged_global_round = global_round;
    }
}

void add_infected_node(int newly_infected_nodes_count) {
    infected_nodes_count += newly_infected_nodes_count;
    if (infected_nodes_count >= total_nodes_count && converged_global_round == -1) {
        converged_global_round = global_round;
    }
}

int get_converged_global_round(void) {
    return converged_global_round;
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) {
            continue;
        }

        if (strncmp(line, "CONVERGED?", 10) == 0) {
            int converged_global_round = get_converged_global_round();
            if (converged_global_round == -1) {
                printf("NO\n");
            } else {
                printf("YES %d\n", converged_global_round);
            }
        } else if (strncmp(line, "INIT ", 5) == 0) {
            total_nodes_count = atoi(line + 5);
            infected_nodes_count = 0;
            global_round = 0;
            converged_global_round = -1;
        } else if (strncmp(line, "INFECT ", 7) == 0) {
            add_infected_node(atoi(line + 7));
        } else if (strncmp(line, "ROUND ", 6) == 0) {
            int fanout = atoi(line + 6);
            add_global_round(fanout);
        }
    }
    return 0;
}
