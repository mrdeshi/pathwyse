#ifndef META_HEUR
#define META_HEUR

#include "algorithms/algorithm.h"

class MetaHeuristic : public Algorithm {
   private:
    std::vector<int> results;
    bool termination;
    bool checkTermination();
    std::vector<int> randomSolution(int lenght);
    bool verifier(std::vector<int> nodes);
    virtual bool swap(std::vector<int> nodes, int i, int node) = 0;
    Path construct(std::vector<int> nodes);

   public:
    /** Algorithm management **/
    // Constructors and destructors
    MetaHeuristic(std::string name, Problem* problem);
    ~MetaHeuristic();

    // Init and reset
    void initAlgorithm();

    void resetAlgorithm(int reset_level) override;

    // Solve
    void solve() override;

    // eredit
    std::vector<int> dyn;

    int s;
    int t;
    Resource* consumption;
    Resource* resource;
    int maxConsumption;
    int computeResult(std::vector<int> nodes);
};

#endif  // META_HEUR
