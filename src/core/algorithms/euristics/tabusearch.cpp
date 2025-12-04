#include "tabusearch.h"
#include <cstdlib>
#include <unistd.h>

#define TABU_SIZE 50

// UTILS
// collectSoluton(id)
//

TabuSearch::TabuSearch(std::string name, Problem *problem)
    : Algorithm(name, problem)
{
    termination = false;
    s = problem->getOrigin();
    t = problem->getDestination();
    consumption = problem->getRes(0); // consumption will decrease in node visit
    maxConsumption = consumption->getUB();
    initDataCollection();
}

TabuSearch::~TabuSearch()
{
    collector.writeData();
    collector_sol.writeData();
}

static int isIn(int node, std::vector<int> nodes)
{
    int c = 0;
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (nodes[i] == node)
        {
            c++;
        }
    }
    return c;
}

static bool noCommons(std::vector<int> a, std::vector<int> b)
{
    for (size_t i = 0; i < a.size(); i++)
    {
        for (size_t j = 0; j < b.size(); j++)
        {
            if (a[i] == b[j])
            {
                return false;
            }
        }
    }
    return true;
}

bool TabuSearch::verifier(std::vector<int> nodes)
{

    int size = nodes.size();

    if (nodes.front() != s || nodes.back() != t)
    {
        return false;
    }

    for (size_t n = 0; n < size; n++)
    {
        int node = nodes[n];

        // path not elementary
        if (isIn(node, nodes) > 1)
        {
            // ask prof
            return false;
        }
    }

    int cap = 0;

    for (size_t i = 0; i < nodes.size(); i++)
    {
        cap = cap + consumption->getNodeCost(nodes[i]);
    }

    if (Parameters::getVerbosity() >= 4)
    {
        printf("MAX CAPACITY %d \n calculated capacity: %d result=%d\n", maxConsumption, cap, computeResult(nodes));
        Path tmp;

        std::list<int> list(nodes.begin(), nodes.end());
        tmp.setTour(list);
        std::cout << (tmp.getTourAsString()) << std::endl;
    }

    // path not feasible
    if (cap > maxConsumption)
    {
        return false;
    }

    return true;
}

std::vector<int> TabuSearch::randomSolution()
{
    std::srand(std::time({}));
    std::vector<int> rNodes;

    // check feasible & elementary
    do
    {

        rNodes.clear();
        rNodes.push_back(s);

        // paga arco guadagna nodo problem->getObj();

        // check feasible & elementarity
        // constraint: resource 0 -> only one

        // choose next candidate node
        int cap = 0;

        while (!(rNodes.back() == t))
        {
            int candidate;
            if (cap > maxConsumption)
            {
                printf("cap: %d, max: %d\n\n\n\n", cap, maxConsumption);
                rNodes.pop_back();
                candidate = t;
            }
            else
            {

                std::vector<int> candidates = problem->getNeighbors(rNodes.back(), true);

                if (Parameters::getVerbosity() >= 4)
                {

                    printf("candidates next to %d: ", rNodes.back());
                    for (size_t i = 0; i < candidates.size(); i++)
                    {
                        printf("%d - ", candidates[i]);
                    }
                    printf("\n");
                }

                do
                {
                    candidate = candidates[std::rand() % candidates.size()];
                } while (isIn(candidate, rNodes));

                cap = cap + consumption->getNodeCost(candidate);
            }

            rNodes.push_back(candidate);
        }
    } while (!verifier(rNodes));

    // conversion from vector to list

    return rNodes;
}

int TabuSearch::computeResult(std::vector<int> nodes)
{

    int result = 0;

    for (size_t i = 0; i < nodes.size() - 1; i++)
    {
        result = result - problem->getObj()->getNodeCost(nodes[i]) + problem->getObj()->getArcCost(nodes[i], nodes[i + 1]);
    }
    return result;
}

void TabuSearch::initAlgorithm()
{
    Path firstRandomPath;

    std::vector<int> randomNodes = randomSolution();
    std::list<int> list(randomNodes.begin(), randomNodes.end());

    firstRandomPath.setTour(list);
    firstRandomPath.setStatus(PATH_SUPEROPTIMAL);
    firstRandomPath.setObjective(computeResult(randomNodes));
    addSolution(firstRandomPath);
    updateBestSolution(0);
}

void TabuSearch::resetAlgorithm(int reset_level)
{
    // Initializes bounds
    Algorithm::initAlgorithm();
    setStatus(ALGO_READY);

    // Resets solutions
    best_solution_id = -1;
    solutions.clear();

    // Resets data collection and extra parameters
    collector.resetTimesCumulative();
    termination = false;
}

bool TabuSearch::checkTermination()
{
    if (results[best_solution_id - 1] - results[best_solution_id] < 2)
    {
        return true;
    }
    return false;
}

void TabuSearch::solve()
{

    if (Parameters::getVerbosity() >= 4)
        std::cout << "Solving..." << std::endl;

    setStatus(ALGO_OPTIMIZING);
    initAlgorithm();

    while (not termination)
    {
        collector.startGlobalTime();

        // Check Termination
        // termination = checkTermination();

        termination = true;

        collector.stopGlobalTime();
        // s collectData();
    }

    if (Parameters::getVerbosity() >= 3)
        std::cout << "Solving complete" << std::endl;

    setStatus(ALGO_DONE);

    collector.print();
}
