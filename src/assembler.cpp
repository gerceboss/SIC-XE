#include <iostream>
#include "tables.h"
#include "utils.h"
#include "parser.h"
#include "pass_1.h"
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
    vector<parsedLine> pls; // parsed lines
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
        parsedLine parsedLine = parseLine(s);
        if (parsedLine.err != "")
        {
            cout << "ERROR on line " << parsedLine.location << " : " << parsedLine.err << endl;
        }
        else if (!parsedLine.isEmpty && !parsedLine.isComment && !(parsedLine.label == "" && parsedLine.opcode == "" && parsedLine.op1 == "" && parsedLine.op2 == ""))
            pls.push_back(parsedLine);
    }
    // execute pass1 and pass2 inside a try-catch block

    try
    {
        cout << "-------------------------------------------------------------------\n"
             << "|                          INTERMEDIATE FILE                      |\n"
             << "-------------------------------------------------------------------\n\n";
        bool err = Pass1(pls, opCodeTable, symbolTable, blockTable, literalTable, programName, startingAddress);
        if (!err)
        {
            // execute pass_2
        }
    }
    catch (char *err)
    {
        cout << "Error in the pass :" << err << endl;
        return 1;
    }
    // if there is no error then write in the output file
    return 0;
}