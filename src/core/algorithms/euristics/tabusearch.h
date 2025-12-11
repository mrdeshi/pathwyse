#ifndef TABU_S
#define TABU_S

#include "metaheuristic.h"

class TabuSearch : public MetaHeuristic {
     private:
    bool isSwitch_inTabu(Switch s);
    bool swap(std::vector<int> nodes, int i, int node) override;
    std::list<Switch> tabu;

   public:
    /** Algorithm management **/
    // Constructors and destructors
    TabuSearch(std::string name, Problem* problem);
    ~TabuSearch();
};
#endif  // TABU_S
