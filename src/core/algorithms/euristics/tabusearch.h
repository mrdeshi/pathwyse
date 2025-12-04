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
    std::vector<int> randomSolution();
    bool verifier(std::vector<int> nodes);
    int s;
    int t;
    Resource *consumption;
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
