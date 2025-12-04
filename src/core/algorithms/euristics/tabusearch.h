#ifndef SIMULATED_ANNEALING
#define SIMULATED_ANNEALING

#include "algorithms/algorithm.h"

class TabuSearch : Algorithm
{
public:
    /** Algorithm management **/
    // Constructors and destructors
    TabuSearch(std::string name, Problem *problem);
    ~TabuSearch();

    // Init and reset
    void initAlgorithm();
    void readConfiguration();

    void resetAlgorithm(int reset_level) override;

    // Solve
    void solve() override;
};

#endif // SIMULATED_ANNEALING
