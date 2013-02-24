#pragma once

#include "IObstacleDetector.h"
#include "Manduchi.h"
#include "Ground.h"

class ObstacleDetectorFactory
{
public:
    static shared_ptr<IObstacleDetector> createManduchi() {
        return shared_ptr<IObstacleDetector>(new Manduchi());
    }

    static shared_ptr<IObstacleDetector> createGround() {
        return shared_ptr<IObstacleDetector>(new Ground());
    }

private:
    ObstacleDetectorFactory(void) {}
    ~ObstacleDetectorFactory(void) {}
};

