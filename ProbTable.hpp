#ifndef PROBTABLE_H
#define PROBTABLE_H
#include <iostream>
#include <string>

using namespace std;

class ProbTable {

    private:
        string featureName;
        // TT TF FT FF
        long double probParams[4];
    public:
        ProbTable();
        ProbTable(string feature);
        void calcParams(int numWith, int total, int location);
        void setParam(long double percent, int location);
        long double getParam(int paramLoc);
};

#endif
