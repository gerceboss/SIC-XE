#ifndef STRUCT
#define STRUCT

#include <iostream>
using namespace std;

#define ll long long

const unsigned ll FORMAT_1 = 1;
const unsigned ll FORMAT_2 = 2;
const unsigned ll FORMAT_3_4 = 3;

struct BlockTable
{
    string name = "DEFAULT"; // block with no name is named "DEFAULT"s
    ll number = 0;
    ll locationCtr = 0;
    ll startingAddress = 0; // default as of now
    ll blockLength;
};

struct OpCode
{
    string mnemonic;
    ll opcode;
    ll possibleFormat;
};

struct Symbol
{
    string label;
    ll location = -1;
    BlockTable block;
    string flags = "R";
};

struct Literal
{
    string value;
    ll address;
    BlockTable block;
    ll size;
    bool dumped = false;
};

struct ObjCode
{
    bool isData = false;       // has some operand
    bool hasReg = false;       // 2 byte instruction
    bool isOnlyOpcode = false; // 1 byte instruction
    ll isWord = -10;
    ll opcode;
    ll displacement;
    ll reg1, reg2;
    ll ni, xbpe;
    ll format;
    string data;
};

struct parsedLine
{
    string label;
    string opcode;
    string op1;
    string op2;
    string err; // error of try-catch block to be displayed if any
    ll location;
    bool isComment = false;
    bool isFormat4 = false;
    bool isEmpty = false;
    ObjCode objCode;
};

struct TextRecord
{
    ll startingAddress;
    ll size;
    string text;
    string label;
};

struct ModificationRecord
{
    char label = 'M';
    ll start;
    ll modified;
};

#endif // MACRO
