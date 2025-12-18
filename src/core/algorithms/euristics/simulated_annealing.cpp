#include "simulated_annealing.h"

#define TRIES 1000000000
#define PRECISION 1000

SimulatedAnnealing::SimulatedAnnealing(std::string name, Problem* problem)
    : MetaHeuristic(name, problem) {
    temperature = 500.0f;
}

SimulatedAnnealing::~SimulatedAnnealing() {
}

///////////////................... UTILS///////////
//
float getRandomStandard() {
    float r = (float)(rand() % PRECISION) / PRECISION;
    if (Parameters::getVerbosity() > 4) {
        printf("called getRandom %f\n", r);
    }

    return r;
}

static int min(int a, int b) {
    if (a < b) {
        return a;
    } else
        return b;
}

// simply impute i is to be swapped with node -> swap if new path is better
bool SimulatedAnnealing::swap(std::vector<int> nodes, int i, int node) {
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

    float delta = before_swap_cost - after_swap_cost;

    // int r = computeResult(nodes);
    // nodes[i] = node;
    // int a = computeResult(nodes);
    //  printf("before: %d after: %d -> delta=%d\n", r, a, r - a);
    if (node_cost < swap_cost) {
        // temperature choice
        //
        if (min(1, exp(-delta / temperature)) >= getRandomStandard()) {
            temperature--;
            return true;

        } else {
            //
            if (delta > 0) {
                //
                return true;
            }
        }
    }
    // bad change
    else {
        return false;
    }
}
