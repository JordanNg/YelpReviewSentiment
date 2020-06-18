#include <math.h>
#include <cmath>
#include "ProbTable.hpp"

ProbTable::ProbTable() { }

ProbTable::ProbTable(string feature) {
    featureName = feature;
}

void ProbTable::calcParams(int numWith, int total, int location) {
    // long double percentage = (long double) numWith/(long double)total;
    // Also use Dirichlet Priors
    long double percentage = log((((long double) numWith) + 1.0)/(((long double) total) + 2.0));
    //long double percentage = ((long double) numWith + 1)/((long double) total + 2);

    // Set the prob in the param array
    setParam(percentage, location);
}

void ProbTable::setParam(long double percent, int location) {
    probParams[location] = percent;
}

long double ProbTable::getParam(int paramLoc) {
    return probParams[paramLoc];
}
