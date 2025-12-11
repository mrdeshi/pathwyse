#include "tabusearch.h"

#define TABU_SIZE 50
#define TRIES 1000000000
#include <algorithm>

// UTILS
// collectSoluton(id)
//

TabuSearch::TabuSearch(std::string name, Problem* problem)
    : MetaHeuristic(name, problem) {
}

TabuSearch::~TabuSearch() {
}

// tabu
bool TabuSearch::isSwitch_inTabu(Switch s) {
    return (std::find(tabu.begin(), tabu.end(), s) != tabu.end());
}
// simply impute i is to be swapped with node -> swap if new path is better
bool TabuSearch::swap(std::vector<int> nodes, int i, int node) {
    // cannot switch s or t obv
    if (nodes[i] == s || nodes[i] == t) {
        return false;
    }

    // first node to swap isn't in the path todo ask prof if it is too slow
    // if (!isIn(i, nodes))
    //{
    //   return false;
    //}

    int before_swap_cost = cost->getArcCost(nodes[i - 1], nodes[i]) + cost->getArcCost(nodes[i], nodes[i + 1]) + cost->getNodeCost(nodes[i]);
    int after_swap_cost = cost->getArcCost(nodes[i - 1], node) + cost->getArcCost(node, nodes[i - 1]) + cost->getNodeCost(node);

    int swap_cost = consumption->getNodeCost(nodes[i]);
    int node_cost = consumption->getNodeCost(node);

    int delta = before_swap_cost - after_swap_cost;

    // printf("before: %d after: %d -> delta=%d\n", r, a, r - a);

    // good choice
    if (delta > 0 && node_cost < swap_cost) {
        Switch change;
        change.a = nodes[i];
        change.b = node;

        // tabu rule, cannot apply change
        if (isSwitch_inTabu(change)) {
            printf("swap: tabu found\n");
            return false;
        }

        tabu.push_back(change);
        if (tabu.size() == TABU_SIZE + 1) {
            // printf("TABU MAX\n");
            tabu.pop_front();
        }
        return true;
    }
    // bad change
    else {
        return false;
    }
}

// tabu
