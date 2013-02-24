#pragma once

#include <iostream>
#include <string>
#include <PerformanceTimer.h>

// ----------------------------------------------------------------------------
// TIMING
// ----------------------------------------------------------------------------

class PerfTimer {
    std::ostream& os;
    std::string desc;
    CPerformanceTimer timer;
public:
    PerfTimer(const std::string& description, std::ostream& os = std::cout)
        : desc(description), os(os) { timer.Start(); }
    ~PerfTimer() { timer.Stop(); os << timer.Interval_mS() << " ms: " << desc << std::endl; }
};
#define Timing(desc, a) do { PerfTimer _(desc); a } while(false)