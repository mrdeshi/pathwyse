#ifndef TABU_S
#define TABU_S

#include "metaheuristic.h"

class TabuSearch : public MetaHeuristic {
    struct Switch {
        int a;
        int b;
        bool operator==(const Switch& other) const {
            // Compare values
            return (a == other.a && b == other.b) || (a == other.b && b == other.a);
        }
    };

   private:
    bool is_switch_equal(Switch a, Switch b);
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
