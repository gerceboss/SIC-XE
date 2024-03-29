#ifndef PASS_1
#define PASS_1

#include <iostream>
#include <map>
#include "tables.h"
#include "utils.h"
#include <iomanip>
using namespace std;
/*
    Initial data:
        starting address = 0
        program length = 0
        error = false
        current block = DEFAULT
        locationCtr = locationCtr of current block
        id = number of current block
*/
bool Pass1(vector<parsedLine> &pls, map<string, OpCode> &opTab, map<string, Symbol> &symbolTable, map<string, BlockTable> &blockTable, map<string, Literal> &literalTable, string &programName, ll &startingAddress)
{
    // NOTE: every parsed line is already converted to uppercase to make it convinient to check
    ll programLength = 0;
    bool err = false;
    BlockTable current = blockTable["DEFAULT"];
    ll locationCtr = current.locationCtr;
    ll id = current.number;

    vector<pair<ll, parsedLine>> lits;

    for (ll i = 0; i < pls.size(); i++)
    {
        parsedLine line = pls[i];
        line.location = locationCtr;

        // classifying the instructions into different types using if-else statements
        if (line.opcode == "START")
        {
            programName = line.label;
            startingAddress = stoi(line.op1);
            locationCtr = startingAddress;
        }
        else if (line.opcode == "END")
        {

            // Perform operation performed while leaving program-block and manage the literalTable
            blockTable[current.name].locationCtr = locationCtr;
            manageBlocks(blockTable, symbolTable, programLength, startingAddress, literalTable);
            manageLitTab(literalTable, locationCtr, pls, lits, i + 1, blockTable["DEFAULT"]);
        }
        else if (line.opcode == "*")
        {
            // DONE
            continue;
        }
        else if (line.opcode == "USE")
        {
            // if : only USE -> move to default block
            // else : create new block, set the current block and continue
            if (line.op1 == "")
            {
                line.op1 = "DEFAULT";
            }
            blockTable[current.name].locationCtr = locationCtr;
            // Create new if not exist
            if (blockTable.find(line.op1) == blockTable.end())
            {
                blockTable[line.op1] = createBlock(line.op1, id + 1);
            }
            // set the current configs
            current = blockTable[line.op1];
            locationCtr = current.locationCtr;
        }
        else if (line.opcode == "EQU")
        {
            // make entry to the symbol table
            err = enterSymbolTab(line, symbolTable, current, locationCtr);
        }
        else if (line.opcode == "ORG") // doubt
        {
            // TODO: Implement whole
        }
        else if (line.opcode == "BASE")
        {
            // handelled in pass-2
        }
        else if (line.opcode == "LTORG")
        {
            // handlelitrals and update the locationCtr and required var accordingly
            manageLitTab(literalTable, locationCtr, pls, lits, i + 1, current);
        }
        else
        {
            // insert symbol if label present
            if (line.label != "")
            {
                symbolTable[line.label] = *createSymbol(line.label, locationCtr, current);
            }

            // memory allocation instructions
            if (line.opcode == "RESB")
            {
                locationCtr += stoi(line.op1); // set error if empty or not a valid integer
            }
            else if (line.opcode == "WORD")
            {
                locationCtr += 3;
            }
            else if (line.opcode == "RESW")
            {
                locationCtr += 3 * stoi(line.op1);
            }
            else if (line.opcode == "BYTE")
            {
                if (line.op1[0] == 'C')
                {
                    locationCtr += line.op1.length() - 3;
                }
                else if (line.op1[0] == 'X')
                {
                    locationCtr += (line.op1.length() - 3) / 2; // raise error if not a valid prepended hex
                }
            }
            else
            {
                // check Format-4
                bool isFormat4 = false;
                string opcode = line.opcode;
                if (opcode[0] == '+')
                {
                    opcode = line.opcode.substr(1);
                    isFormat4 = true;
                }
                if (opcode[0] == '.')
                {
                    // comment line
                    continue;
                }

                auto op = opTab.find(opcode);

                if (op == opTab.end())
                {
                    line.err = "Invalid Opcode " + opcode;
                    err = true;
                }
                else
                {
                    // check if literal used
                    if (line.op1[0] == '=')
                    {
                        auto literal = literalTable.find(line.op1);
                        // if already not present in the LitTab
                        if (literal == literalTable.end())
                        {
                            ll litSize = 0;
                            switch (line.op1[1])
                            {
                            case '*':
                                if (line.op1.length() != 2)
                                {
                                    line.err = "Invalid literal format";
                                    err = true;
                                }
                                litSize = 3; // means current PC and PC is of 3 bytes
                                break;
                            case 'C':
                                if (line.op1.length() <= 4)
                                {
                                    cout << "Bad literal\n";
                                    err = true;
                                    break;
                                }
                                litSize = line.op1.length() - 4; //=C"<literal-val>"
                                break;
                            case 'X':
                                if (line.op1.length() <= 4)
                                {
                                    cout << "Bad literal\n";
                                    err = true;
                                    break;
                                }
                                litSize = (line.op1.length() - 4) / 2; // set error if not a vaild prepended hex
                                break;

                            default:
                                line.err = "Invalid literal format";
                                err = true;
                                break;
                            }
                            literalTable[line.op1] = createLiteral(line.op1, current, litSize);
                        }
                    }

                    line.isFormat4 = isFormat4;
                    line.opcode = opcode;
                    if (isFormat4)
                    {
                        locationCtr += 4;
                    }
                    else
                    {
                        locationCtr += op->second.possibleFormat;
                    }
                }
            }
        }
        pls[i] = line;
        // helps  in printing the intermediate file
        // Use setfill to append the character and setw to change how many times it must be prepended, right for shifting the data to right
        if (line.opcode != "END" && line.opcode != "LTORG" && line.opcode != "USE" && line.opcode[0] != '.' && line.opcode != "BASE")
        {
            if (line.err != "")
            {
                cout << setfill('0') << setw(4) << right << hex << line.location << " " << line.label << " " << line.opcode << " " << line.op1 << " " << line.op2 << "\t\t <------- ERROR : " << line.err << "\n";
            }
            else
            {
                cout << setfill('0') << setw(4) << right << hex << line.location << " " << line.label << " " << line.opcode << " " << line.op1 << " " << line.op2 << "\n";
            }
        }
    }
    sort(lits.begin(), lits.end(), litsComparator);
    for (ll i = 0; i < lits.size(); i++)
    {
        pls.insert(pls.begin() + lits[i].first, lits[i].second);
    }
    return err;
}
#endif