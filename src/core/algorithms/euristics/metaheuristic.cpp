#include "metaheuristic.h"

#include <unistd.h>

static unsigned int TRIES;
static unsigned int MAX_INSERT;
static unsigned int LAZY_ITERATIONS_SWAP_RATIO;
static unsigned int EXECUTION_TIME;
static unsigned int SLEEP;
static unsigned int CANDIDATE_SKIP;
static unsigned int LAZY_TRIES;

// UTILS
//

static unsigned int lazy_counter = 0;
static unsigned int dyn_total_resource = 0;

// Neighbourhood
bool MetaHeuristic::is_switch_equal(Switch a, Switch b) {
    return (a.a == b.a && a.b == b.b) || (a.a == b.b && a.b == b.a);
}

MetaHeuristic::MetaHeuristic(std::string name, Problem* problem)
    : Algorithm(name, problem) {
    TRIES = Parameters::getTries();
    MAX_INSERT = Parameters::maxInsert();
    LAZY_ITERATIONS_SWAP_RATIO = Parameters::lazyIterationsSwapRatio();
    EXECUTION_TIME = Parameters::executionTime();
    SLEEP = Parameters::getSleep();
    CANDIDATE_SKIP = Parameters::candidateSkip();
    LAZY_TRIES = Parameters::lazyTries();

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

bool MetaHeuristic::insert(std::vector<int> nodes, int i, int node, int total_consumption) {
    if (node == s || node == t) {
        return false;
    }

    int before_insert_cost = cost->getArcCost(nodes[i - 1], nodes[i]);
    int after_insert_cost = cost->getArcCost(nodes[i - 1], node) + cost->getArcCost(node, nodes[i]) + cost->getNodeCost(node);

    int node_consumption = consumption->getNodeCost(node);

    int delta = before_insert_cost - after_insert_cost;

    // printf("before: %d after: %d -> delta=%d\n", r, a, r - a);

    // good choice
    if (delta > 0 && (node_consumption + total_consumption) < consumption->getUB()) {
        return true;
    }
    // bad insert
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
    std::vector<int> rNodes;
    std::srand(std::time(NULL));

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
    int c = 0;
    for (size_t i = 0; i < dyn.size(); i++) {
        c = c + consumption->getNodeCost(dyn[i]);
    }
    dyn_total_resource = c;

    addSolution(firstRandomPath);
    updateBestSolution(0);
    printf("START WITH OBJ=%d\n", firstRandomPath.getObjective());
}

void MetaHeuristic::resetAlgorithm(int reset_level) {
    // Initializes bounds
    setStatus(ALGO_READY);

    solutions.clear();
    lazy_counter = 0;
    dyn_total_resource = 0;

    // Resets data collection and extra parameters
    collector.resetTimesCumulative();
    termination = false;
    initAlgorithm();
}

bool MetaHeuristic::checkTermination() {
    if (collector.getGlobalTimeNow() > EXECUTION_TIME) {
        printf("termination by time\n");
        return true;
    }
    if (lazy_counter >= LAZY_TRIES) {
        printf("termination by lazy\n");
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
    std::srand(std::time(NULL));
    int swaps = 1;
    int lenght = dyn.size();
    do {
        sleep(SLEEP);
        // candidate selection
        size_t i = rand() % dyn.size();
        int candidate;

        std::vector<int> candidates = problem->getNeighbors(dyn[i], true);

        do {
            candidate = candidates[rand() % candidates.size()];

            for (size_t j = 0; j < candidates.size(); j++) {
                int a = cost->getArcCost(dyn[i], candidates[j]) + cost->getNodeCost(candidates[j]);
                int b = cost->getArcCost(dyn[i], candidate) + cost->getNodeCost(candidate);
                if (a < b && !isIn(candidates[j], dyn)) {
                    candidate = candidates[j];
                }
            }

            if (rand() % CANDIDATE_SKIP > 1) {
                candidate = candidates[rand() % candidates.size()];  // restart
                continue;
            }

        } while (isIn(candidate, dyn) && !verifier(dyn));

        if (dyn[i] == candidate || (isIn(candidate, dyn))) {
            continue;
        }

        // end candidate selection
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
            dyn_total_resource = dyn_total_resource - consumption->getNodeCost(dyn[i]) + consumption->getNodeCost(candidate);
            dyn[i] = candidate;

            Path r = construct(dyn);
            printf("SWAPPED, r=%d\n", r.getObjective());
            if (r.getObjective() < getBestSolution()->getObjective()) {
                if (Parameters::getVerbosity() >= 3) {
                    printf("new solution:= %d\n", r.getObjective());
                }
                addSolution(r);
            }
        } else {
            lazy_counter++;
        }

        if (lazy_counter % LAZY_ITERATIONS_SWAP_RATIO == 0) {
            printf("try insert\n");
            int insert_candidate;
            int c = 0;
            do {
                i = rand() % dyn.size();
                insert_candidate = rand() % (problem->getNumNodes());
                c++;
            } while (!insert(dyn, i, insert_candidate, dyn_total_resource) && c < MAX_INSERT && !verifier(dyn));

            if (insert(dyn, i, insert_candidate, dyn_total_resource)) {
                // insert in array and shift
                dyn_total_resource = dyn_total_resource + consumption->getNodeCost(insert_candidate);
                std::vector<int> tmp_v;

                for (size_t j = 0; j < i; j++) {  // to i-1
                    tmp_v.push_back(dyn[j]);
                }
                tmp_v.push_back(insert_candidate);
                for (size_t k = i; k < dyn.size(); k++) {
                    tmp_v.push_back(dyn[k]);
                }
                dyn = tmp_v;

                Path r = construct(dyn);
                printf("insertion, r=%d\n", r.getObjective());
                if (r.getObjective() < getBestSolution()->getObjective()) {
                    if (Parameters::getVerbosity() >= 3) {
                        printf("new solution:= %d\n", r.getObjective());
                    }
                    solutions.push_back(r);
                    updateBestSolution(solutions.size() - 1);
                }
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
