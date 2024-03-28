#ifndef TABLE
#define TABLE

#include <iostream>
#include <map>
using namespace std;

#define ll long long

map<string, ll> registers;
void buildRegMap()
{
    registers["A"] = 0;
    registers["X"] = 1;
    registers["L"] = 2;
    registers["B"] = 3;
    registers["S"] = 4;
    registers["T"] = 5;
    registers["F"] = 6;
    registers["PC"] = 8;
    registers["SW"] = 9;
}

void buildOpTable(){
    
}

#endif