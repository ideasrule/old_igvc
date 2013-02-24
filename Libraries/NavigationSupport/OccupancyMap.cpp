#include "OccupancyMap.h"

#include <cmath>

namespace Pave_Libraries_Navigation {

OccupancyMap::~OccupancyMap() {}

LogOdds probabilityToLogOdds(Probability p)
{
    return log(p / (1 - p));
}

Probability logOddsToProbability(LogOdds lgo)
{
    return 1 - 1 / (1 + exp(lgo));
}

} // namespace Pave_Libraries_Navigation
