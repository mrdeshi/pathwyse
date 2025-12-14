#include "metaheuristic.h"

#define TRIES 1000000000
#define MAX_INSERT 2000000
#define SWAP_INSERT_RATIO 1000
#define EXECUTION_TIME 5

#define LAZY_TRIES 2000000000000

// UTILS
// collectSoluton(id)
//

static unsigned int lazy_counter = 0;

// Neighbourhood
bool MetaHeuristic::is_switch_equal(Switch a, Switch b) {
    return (a.a == b.a && a.b == b.b) || (a.a == b.b && a.b == b.a);
}

MetaHeuristic::MetaHeuristic(std::string name, Problem* problem)
    : Algorithm(name, problem) {
    termination = false;
    s = problem->getOrigin();
    t = problem->getDestination();
    consumption = problem->getRes(0);  // consumption will decrease in node visit
    maxConsumption = consumption->getUB();

    cost = problem->getObj();
    initDataCollection();
}

MetaHeuristic::~MetaHeuristic() {
    collector.writeData();
    collector_sol.writeData();
}

static int isIn(int node, std::vector<int> nodes) {
    int c = 0;
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i] == node) {
            c++;
        }
    }
    return c;
}

static bool noCommons(std::vector<int> a, std::vector<int> b) {
    for (size_t i = 0; i < a.size(); i++) {
        for (size_t j = 0; j < b.size(); j++) {
            if (a[i] == b[j]) {
                return false;
            }
        }
    }
    return true;
}

bool MetaHeuristic::insert(std::vector<int> nodes, int i, int node) {
    if (node == s || node == t) {
        return false;
    }

    int before_swap_cost = cost->getArcCost(nodes[i - 1], nodes[i]);
    int after_swap_cost = cost->getArcCost(nodes[i - 1], node) + cost->getArcCost(node, nodes[i]) + cost->getNodeCost(node);

    int total_consumption = 0;
    for (size_t i = 0; i < nodes.size(); i++) {
        total_consumption = total_consumption + consumption->getNodeCost(nodes[i]);
    }

    int node_consumption = consumption->getNodeCost(node);

    int delta = before_swap_cost - after_swap_cost;

    // printf("before: %d after: %d -> delta=%d\n", r, a, r - a);

    // good choice
    if (delta > 0 && (node_consumption + total_consumption) < consumption->getUB()) {
        return true;
    }
    // bad change
    else {
        return false;
    }
};

bool MetaHeuristic::verifier(std::vector<int> nodes) {
    int size = nodes.size();

    if (nodes.front() != s || nodes.back() != t) {
        return false;
    }

    for (size_t n = 0; n < size; n++) {
        int node = nodes[n];

        // path not elementary
        if (isIn(node, nodes) > 1) {
            // ask prof
            return false;
        }
    }

    int cap = 0;

    for (size_t i = 0; i < nodes.size(); i++) {
        cap = cap + consumption->getNodeCost(nodes[i]);
    }

    if (Parameters::getVerbosity() >= 4) {
        printf("MAX CAPACITY %d \n calculated capacity: %d result=%d\n", maxConsumption, cap, computeResult(nodes));
        Path tmp;

        std::list<int> list(nodes.begin(), nodes.end());
        tmp.setTour(list);
        std::cout << (tmp.getTourAsString()) << std::endl;
    }

    // path not feasible
    if (cap > maxConsumption) {
        return false;
    }

    return true;
}

static std::vector<int> hardSolution() {
    // 0 6 32 14 27 12 2 24 51 41 34 54
    std::vector<int> v;
    v.push_back(0);
    v.push_back(6);
    v.push_back(32);
    v.push_back(14);
    v.push_back(27);
    v.push_back(12);
    v.push_back(2);
    v.push_back(24);
    v.push_back(51);
    v.push_back(41);
    v.push_back(34);
    v.push_back(54);
    return v;
}

std::vector<int> MetaHeuristic::greedySolution() {
    std::srand(std::time({}));
    std::vector<int> rNodes;

    int maxNodes = problem->getNumNodes();
    int tries = 0;

    // check feasible & elementary
    do {
        rNodes.clear();
        rNodes.push_back(s);

        // paga arco guadagna nodo problem->getObj();

        // check feasible & elementarity
        // constraint: cost 0 -> only one

        // choose next candidate node
        int cap = 0;

        while (!(rNodes.back() == t)) {
            int candidate;
            if (cap > maxConsumption || rNodes.size() == maxNodes - 1) {
                rNodes.pop_back();
                candidate = t;
            } else {
                std::vector<int> candidates = problem->getNeighbors(rNodes.back(), true);

                if (Parameters::getVerbosity() >= 4) {
                    printf("candidates next to %d: ", rNodes.back());
                    for (size_t i = 0; i < candidates.size(); i++) {
                        printf("%d - ", candidates[i]);
                    }
                    printf("\n");
                }

                do {
                    candidate = candidates[std::rand() % candidates.size()];
                    for (size_t i = 0; i < candidates.size(); i++) {
                        int a = cost->getArcCost(rNodes.back(), candidates[i]) + cost->getNodeCost(candidates[i]);
                        int b = cost->getArcCost(rNodes.back(), candidate) + cost->getNodeCost(candidate);
                        if (a < b && !isIn(candidates[i], rNodes)) {
                            candidate = candidates[i];
                        }
                    }
                } while (isIn(candidate, rNodes));

                cap = cap + consumption->getNodeCost(candidate);
            }

            rNodes.push_back(candidate);
        }

    } while (!verifier(rNodes));

    printf("\n=================TRY================ lenght:%d\n", rNodes.size());

    for (size_t i = 0; i < rNodes.size(); i++) {
        printf("%d -> ", rNodes[i]);
    }

    return rNodes;
}

std::vector<int> MetaHeuristic::randomSolution(int lenght) {
    std::srand(std::time({}));
    std::vector<int> rNodes;

    int maxNodes = problem->getNumNodes();
    int tries = 0;

    // check feasible & elementary
    do {
        printf("tries=%d\n", tries);
        if (tries > TRIES && lenght != -1) {
            return randomSolution(lenght++);
        }

        tries++;
        rNodes.clear();
        rNodes.push_back(s);

        // paga arco guadagna nodo problem->getObj();

        // check feasible & elementarity
        // constraint: cost 0 -> only one

        // choose next candidate node
        int cap = 0;

        while (!(rNodes.back() == t)) {
            int candidate;
            if (cap > maxConsumption || rNodes.size() == maxNodes - 1) {
                rNodes.pop_back();
                candidate = t;
            } else {
                std::vector<int> candidates = problem->getNeighbors(rNodes.back(), true);

                if (Parameters::getVerbosity() >= 4) {
                    printf("candidates next to %d: ", rNodes.back());
                    for (size_t i = 0; i < candidates.size(); i++) {
                        printf("%d - ", candidates[i]);
                    }
                    printf("\n");
                }

                do {
                    candidate = candidates[std::rand() % candidates.size()];
                } while (isIn(candidate, rNodes));

                cap = cap + consumption->getNodeCost(candidate);
            }

            rNodes.push_back(candidate);
        }

    } while ((!verifier(rNodes) || rNodes.size() != lenght) && lenght != -1);

    printf("\n=================TRY================ lenght:%d\n", rNodes.size());

    for (size_t i = 0; i < rNodes.size(); i++) {
        printf("%d -> ", rNodes[i]);
    }

    return rNodes;
}

int MetaHeuristic::computeResult(std::vector<int> nodes) {
    int result = 0;

    for (size_t i = 0; i < nodes.size() - 1; i++) {
        result = result + cost->getNodeCost(nodes[i]) + cost->getArcCost(nodes[i], nodes[i + 1]);
    }
    result = result + cost->getNodeCost(nodes[nodes.size() - 1]);
    return result;
}

Path MetaHeuristic::construct(std::vector<int> nodes) {
    Path p;
    std::list<int> list(nodes.begin(), nodes.end());
    p.setTour(list);
    p.setStatus(PATH_SUPEROPTIMAL);
    p.setObjective(computeResult(nodes));
    return p;
}

void MetaHeuristic::initAlgorithm() {
    std::vector<int> randomNodes = greedySolution();  // randomSolution(-1);  // test others
    dyn = randomNodes;
    Path firstRandomPath = construct(randomNodes);
    addSolution(firstRandomPath);
    updateBestSolution(0);
}

void MetaHeuristic::resetAlgorithm(int reset_level) {
    // Initializes bounds
    setStatus(ALGO_READY);

    solutions.clear();

    // Resets data collection and extra parameters
    collector.resetTimesCumulative();
    termination = false;
    initAlgorithm();
}

bool MetaHeuristic::checkTermination() {
    //((results[best_solution_id - 1] - results[best_solution_id]) < 2) ||
    // printf("%f\n", collector.getGlobalTimeNow());
    if (collector.getGlobalTimeNow() > EXECUTION_TIME) {
        return true;
    }
    if (lazy_counter >= LAZY_TRIES) {
        return true;
    }

    return false;
}

void MetaHeuristic::solve() {
    if (Parameters::getVerbosity() >= 4)
        std::cout
            << "Solving..." << std::endl;

    setStatus(ALGO_OPTIMIZING);
    initAlgorithm();
    collector.startGlobalTime();
    std::srand(std::time({}));
    int swaps = 1;
    int lenght = dyn.size();
    do {
        // candidate selection

        int i = rand() % dyn.size();
        int candidate = rand() % (problem->getNumNodes());

        if (dyn[i] == candidate || (isIn(candidate, dyn))) {
            continue;
        }
        iterations++;

        if (Parameters::getVerbosity() >= 4) {
            printf("------------current dyn------------\n");
            for (size_t i = 0; i < dyn.size(); i++) {
                printf("%d -> ", dyn[i]);
            }

            printf("\niterations=%d i=%d candidate=%d swap=%d\n", iterations, i, candidate, swaps);
        }

        if (swap(dyn, i, candidate)) {
            lazy_counter = 0;
            swaps++;
            dyn[i] = candidate;

            Path r = construct(dyn);
            printf("SWAPPED, r=%d\n", r.getObjective());
            if (r.getObjective() < getBestSolution()->getObjective()) {
                if (Parameters::getVerbosity() >= 3) {
                    printf("new solution:= %d\n", r.getObjective());
                }
                solutions.push_back(r);
                updateBestSolution(solutions.size() - 1);
            }
        } else {
            lazy_counter++;
        }

        if (swaps % SWAP_INSERT_RATIO == 0) {
            i = rand() % dyn.size();
            int insert_candidate;
            int c = 0;
            do {
                insert_candidate = rand() % (problem->getNumNodes());
                c++;
            } while (!insert(dyn, i, insert_candidate) && c < MAX_INSERT);

            if (insert(dyn, i, insert_candidate)) {
                // insert in array and shift

                std::vector<int> tmp_v;

                for (size_t j = 0; j < i; j++) {  // to i-1
                    tmp_v.push_back(dyn[j]);
                }
                tmp_v.push_back(insert_candidate);
                for (size_t k = i; k < dyn.size(); k++) {
                    tmp_v.push_back(dyn[k]);
                }
                dyn = tmp_v;
                printf("INSERTION %d\n", dyn.size());
            }
        }

        // Check Termination
        termination = checkTermination();

    } while (not termination);

    collector.stopGlobalTime();
    // s collectData();

    if (Parameters::getVerbosity() >= 3)
        std::cout << "Solving complete" << std::endl;

    setStatus(ALGO_DONE);

    collector.print();
}
