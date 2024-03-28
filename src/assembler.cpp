#include <iostream>
#include "tables.h"
#include "utils.h"
using namespace std;
#define ll long long
int main(int argc, char *argv[])
{
    // check the terminal command and hhow many arguments does it passes , while running this main functions
    if (argc != 3)
    {
        cout << "Invalid arguments provided for execution of the program" << endl;
        cout << "Usage : ./assembler <input file name> <output file name>" << endl;
        exit(1);
        // exit the process as there is no such defined command to run this program
    }

    // if everything is fine then read the file names
    string input_file = argv[1];
    string output_file = argv[2];
    freopen((input_file).c_str(), "r", stdin);
    freopen((output_file).c_str(), "w", stdout);

    // storing the parsed instructions
    vector<parsedLine> vec;
    map<string, OpCode> opCodeTable;
    map<string, Symbol> symbolTable;
    map<string, BlockTable> blockTable;
    map<string, Literal> literalTable;
    vector<ModificationRecord> modifications;

    // initiate the default block
    BlockTable b;
    blockTable["DEFAULT"] = b;

    // build the opCodeTable and registerTable
    buildOpTable(opCodeTable);
    buildRegMap();

    // read the input file
    string s;
    ll startingAddress = 0; // custom startingAddress
    ll locCtr = startingAddress;
    ll programLength;
    string programName = "TEST";
    while (true)
    {
        getline(cin, s);
        if (cin.fail())
        {
            break;
        }
        // make a parsing function in the parser.h file
    }
    // after reading try to do pass1 and pass2  inside a try-catch block

    // if there is no error then write in the output file
    return 0;
}