#include "Matrix3dFactory.hpp"

namespace Utilities{

    Data::Matrix3d Matrix3dFactory::Create(int numOUDStates, int numInterventions, int numDemographics){
        std::array<long int, 3> order = {0,0,0};
        order[Data::OUD] = numOUDStates;
        order[Data::INTERVENTION] = numInterventions;
        order[Data::DEMOGRAPHIC_COMBO] = numDemographics; 
        Data::Matrix3d empty(order);
        return empty;
    }
    
}
