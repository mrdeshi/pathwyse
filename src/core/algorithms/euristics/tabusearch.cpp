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
    size = problem->getDestination();
    initDataCollection();
}

TabuSearch::~TabuSearch()
{
    collector.writeData();
    collector_sol.writeData();
}

bool TabuSearch::verifier(std::vector<int> nodes)
{

    if (nodes.size() != size || nodes.front() != s || nodes.back() != t)
    {
        return false;
    }

    for (size_t n = 0; n < size; n++)
    {
        int node = nodes[n];
        int see = 0;
        for (size_t i = 0; i < size; i++)
        {
            if (nodes[i] == node)
            {
                see++;
            }
        }
        // path not elementary
        if (see > 1)
        {
            return false;
        }
    }

    Resource *consumption = problem->getRes(0); // consumption will decrease in node visit
    int maxCapacity = consumption->getUB();

    int cap = 0;

    for (size_t i = 0; i < nodes.size(); i++)
    {
        cap = cap + consumption->getNodeCost(nodes[i]);
    }
    printf("MAX CAPACITY %d \n calculated: %d \n", maxCapacity, cap);
    Path tmp;

    std::list<int> list(nodes.begin(), nodes.end());
    tmp.setTour(list);
    std::cout << (tmp.getTourAsString()) << std::endl;

    // path not feasible
    if (cap > maxCapacity)
    {
        return false;
    }

    return true;
}

std::list<int> TabuSearch::randomSolution()
{

    printf("NUMBER OF NODES %d", size);
    std::vector<int> randomList;

    // check feasible & elementary
    while (!verifier(randomList))
    {

        randomList.clear();
        randomList.push_back(s);

        // paga arco guadagna nodo problem->getObj();

        // check feasible & elementarity
        // constraint: resource 0 -> only one

        // choose next candidate node

        for (size_t i = 0; i < size - 2; i++)
        {
            std::srand(std::time({}));
            std::vector<int> candidates = problem->getNeighbors(randomList.back(), true);
            int candidate = candidates[std::rand() % candidates.size()];
            randomList.push_back(candidate);
        }

        randomList.push_back(t);
        sleep(1);
    }

    // conversion from vector to list
    std::list<int> list(randomList.begin(), randomList.end());
    return list;
}

int TabuSearch::computeResult(std::vector<int> nodes)
{
    if (verifier(nodes))
    {
        int result;

        for (size_t i = 0; i < nodes.size() - 1; i++)
        {
            result = result + problem->getObj()->getNodeCost(nodes[i]) - problem->getObj()->getArcCost(nodes[i], nodes[i + 1]);
        }
        return result;
    }
    else
        return 99999999999999;
}

void TabuSearch::initAlgorithm()
{
    Path firstRandomPath;
    firstRandomPath.setTour(randomSolution());
    firstRandomPath.setStatus(PATH_SUPEROPTIMAL);
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

    if (algo_status == ALGO_OPTIMIZING)
        setStatus(ALGO_DONE);

    collector.print();
}
