#ifndef SIMULATED_ANNEALING
#define SIMULATED_ANNEALING

#include "algorithms/algorithm.h"

class TabuSearch : public Algorithm
{
private:
    std::vector<int> results;
    bool termination;
    bool checkTermination();
    int computeResult(std::vector<int> nodes);
    std::vector<int> randomSolution(int lenght);
    bool verifier(std::vector<int> nodes);
    bool swap(std::vector<int> nodes, int i, int node);
    Path construct(std::vector<int> nodes);

    int s;
    int t;
    Resource *consumption;
    Resource *resource;
    int maxConsumption;

public:
    /** Algorithm management **/
    // Constructors and destructors
    TabuSearch(std::string name, Problem *problem);
    ~TabuSearch();

    // Init and reset
    void initAlgorithm();

    void resetAlgorithm(int reset_level) override;

    // Solve
    void solve() override;
};

#endif // SIMULATED_ANNEALING
