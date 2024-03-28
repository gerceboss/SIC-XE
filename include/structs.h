#ifndef STRUCT
#define STRUCT

#include <iostream>
using namespace std;

#define ll long long

const unsigned ll FORMAT_1 = 1;
const unsigned ll FORMAT_2 = 2;
const unsigned ll FORMAT_3_4 = 3;

struct OpCode
{
    string mnemonic;
    ll opcode;
    ll possibleFormat;
};
struct Symbol
{
};
struct Literal
{
};
struct ObjCode
{
};

struct BlockTable
{
};
struct parsedLine
{
};
struct TextRecord
{
};
struct ModificationRecord
{
};

#endif // MACRO
