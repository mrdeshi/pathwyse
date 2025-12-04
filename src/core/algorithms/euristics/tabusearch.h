#ifndef SIMULATED_ANNEALING
#define SIMULATED_ANNEALING

#include "algorithms/algorithm.h"

class TabuSearch : Algorithm
{
private:
    std::vector<int> results;
    bool termination;
    bool checkTermination();
    int computeResult(std::vector<int> nodes);
    std::list<int> randomSolution();
    bool verifier(std::vector<int> nodes);

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
