#ifndef SIM_AN
#define SIM_AN

#include "metaheuristic.h"

class SimulatedAnnealing : public MetaHeuristic {
   private:
    bool swap(std::vector<int> nodes, int i, int node) override;
    float temperature;

   public:
    /** Algorithm management **/
    // Constructors and destructors
    SimulatedAnnealing(std::string name, Problem* problem);
    ~SimulatedAnnealing();
};
#endif  // SIM_AN
