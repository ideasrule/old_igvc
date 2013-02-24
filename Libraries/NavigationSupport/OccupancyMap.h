#pragma once

namespace Pave_Libraries_Navigation {

typedef double Probability;
typedef double LogOdds;

LogOdds probabilityToLogOdds(Probability p);
Probability logOddsToProbability(LogOdds lgo);

class OccupancyMap
{
public:
	virtual ~OccupancyMap();

    // Update cell (x, y) according to Table 9.1, Probabilistic Robotics, 2006, Thrun, Burgard, and Fox.
    // The inverse sensor model is p(m_i | z_t, x_t), that is, the probability
    //   that cell m_i in the map is occupied given the state and observation
    //   at time t.
    virtual void update(int x, int y, Probability inverseSensorModelValue) = 0;
    
    // Get p(m_i | z_1:t, x_1:t), that is, the probability cell i (specified
    //   by x, y) is occupied given all of the previous states and observations.
    virtual Probability pOccupied(int x, int y) const = 0;

    // LogOdds form of pOccupied. This doesn't involve any conversion from the internal
    //   data structure so it may be more accurate.
    virtual LogOdds lgoOccupied(int x, int y) const = 0;

    virtual int minX() const = 0;
    virtual int maxX() const = 0;
    virtual int minY() const = 0;
    virtual int maxY() const = 0;
};

} // namespace Pave_Libraries_Navigation