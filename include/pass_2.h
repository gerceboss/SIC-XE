#ifndef PASS_2
#define PASS_2

#include <iostream>
#include <vector>
#include <map>
#include "utils.h"
#include "tables.h"
#include "structs.h"
using namespace std;

bool createObjCodeForWord(parsedLine &line) // create object code for WORD
{
    ObjCode obj;
    if (line.op1 == "")
    {
        line.err = "WORD cannot be empty";
        return true;
    }
    if (isNumeric(line.op1))
    {
        obj.isWord = stoi(line.op1);
        line.objCode = obj;
        return false;
    }
    line.err = "Use BYTE for strings, integers for WORD";
    return true;
}

bool createObjectCodeForData(parsedLine &pl, ll subStrIndex) // create object code for BYTE
{
    ObjCode obj;
    obj.isData = true;
    string str = pl.op1.substr(2 + subStrIndex, pl.op1.length() - 1); // op1 will be of format ="<hex>"
    if (pl.op1[subStrIndex] == 'X')
    {
        obj.data = str; // we already have hex
    }
    else if (pl.op1[subStrIndex] == 'C')
    {

        obj.data = string_to_hex(str);
    }
    else
    {
        return true;
    }
    pl.objCode = obj;
    return false;
}

bool createObjectCodeWithRegisters(parsedLine &pl, map<string, OpCode> &opTab, map<string, ll> &regs) // format 2
{
    ObjCode obj;
    obj.ni = 3;
    ll op = opTab.find(pl.opcode)->second.opcode;
    if (pl.opcode == "CLEAR" || pl.opcode == "TIXR")
    {
        if (pl.op2 != "") // format 2 instructions should have only 1 operand
        {
            pl.err = "Too many arguments for this opcode";
            return true;
        }
        if (regs.find(pl.op1) == regs.end()) // invalid register
        {
            pl.err = "Not a valid register " + pl.op1;
            return true;
        }
        obj.hasReg = true;
        obj.reg1 = regs.find(pl.op1)->second;
        obj.reg2 = -1;
        obj.opcode = op;
        pl.objCode = obj;
        return false;
    }
    if (regs.find(pl.op1) == regs.end() || regs.find(pl.op2) == regs.end())
    {
        pl.err = "Not a valid register " + pl.op1 + " or " + pl.op2;
        return true;
    }
    obj.hasReg = true;
    obj.reg1 = regs.find(pl.op1)->second; //
    obj.reg2 = regs.find(pl.op2)->second;
    obj.opcode = op;
    pl.objCode = obj;
    return false;
}

bool createObjectCodeWithOnlyOpcode(parsedLine &pl, map<string, OpCode> &opTab) // format 1
{
    ObjCode obj;
    ll op = opTab.find(pl.opcode)->second.opcode;
    obj.opcode = op;
    pl.objCode = obj; // handle operands should not come with zero operand instructions
    return false;
}

ModificationRecord createModificationRecord(ll start, ll modify)
{
    ModificationRecord m;
    m.start = start;
    m.modified = modify;
    return m;
}

Symbol getSymbol(map<string, Symbol> &symTab, parsedLine &pl, ll substrIndex)
{
    string str = pl.op1.substr(substrIndex);
    auto symbol = symTab.find(str);
    if (symbol == symTab.end())
    {
        pl.err = "Undefined Label";
        throw pl.err;
    }
    return symbol->second;
}

ObjCode pcOrBaseRelativeAddressing(map<string, Symbol> &symTab, map<string, OpCode> &opTab, ll &locationCtr, parsedLine &pl, ll ni, ll subStrIndex, vector<ModificationRecord> &modi, bool &base, ll &baseRegister)
{
    ObjCode obj;
    auto symbol = getSymbol(symTab, pl, subStrIndex);
    bool isAbsolute = symbol.flags == "A";
    long long effectiveLoc = symbol.block.startingAddress + symbol.location;
    bool simple = false;
    ll isIndexed = 0;
    if (pl.op2 != "")
    {
        if (pl.op2 == "X")
        {
            isIndexed = 1;
        }
        else
        {
            throw "Index Register should be X";
        }
    }
    if (isAbsolute)
    {
        effectiveLoc = symbol.location;
    }
    if (pl.isFormat4)
    {
        if (validf4(effectiveLoc))
        {
            obj.format = 4;
            obj.displacement = effectiveLoc;
            obj.ni = ni;
            obj.xbpe = 1 | (isIndexed << 3);
            obj.opcode = opTab[pl.opcode].opcode;
            if (!isAbsolute)
            {
                modi.push_back(createModificationRecord(pl.location + 1, 5));
            }
            return obj;
        }
        else
        {
            pl.err = "Symbol " + pl.op1 + "displacement overflows";
        }
    }
    else
    {
        effectiveLoc -= locationCtr;
        if (validf3(effectiveLoc))
        {
            obj.format = 3;
            obj.displacement = effectiveLoc;
            obj.ni = ni;
            obj.xbpe = 2 | (isIndexed << 3);
            obj.opcode = opTab[pl.opcode].opcode;
            return obj;
        }
        else if (base)
        {
            effectiveLoc += locationCtr - baseRegister;
            if (validf3(effectiveLoc))
            {
                obj.format = 3;
                obj.displacement = effectiveLoc;
                obj.ni = ni;
                obj.xbpe = 4 | (isIndexed << 3);
                obj.opcode = opTab[pl.opcode].opcode;
                return obj;
            }
            simple = true;
        }
        else if (simple)
        {
            long long simpleLoc = symbol.location;
            if (validf3(simpleLoc))
            {
                obj.format = 3;
                obj.displacement = simpleLoc;
                obj.ni = ni;
                obj.xbpe = 0 | (isIndexed << 3);
                obj.opcode = opTab[pl.opcode].opcode;
                return obj;
            }
            else
            {
                throw "Invalid Displacement";
            }
        }
        throw "Invalid Displacement";
    }
}

bool createObjectCodeForInstruction(parsedLine &pl, map<string, OpCode> &opTab, map<string, Symbol> &symTab, map<string, Literal> &litTab, long long &locationCtr, vector<ModificationRecord> &modi, bool &base, ll &baseRegister)
{

    try
    {
        ObjCode obj;
        if (pl.opcode == "RSUB")
        {
            ll op = opTab.find(pl.opcode)->second.opcode;
            obj.displacement = 0;
            obj.ni = 3;
            obj.xbpe = 0;
            obj.opcode = op;
            pl.objCode = obj;
            obj.format = 3;
            return false;
        }

        if (pl.op1[0] == '@')
        {
            obj = pcOrBaseRelativeAddressing(symTab, opTab, locationCtr, pl, 2, 1, modi, base, baseRegister);
        }
        else if (pl.op1[0] == '#')
        {
            if (pl.op2 != "")
            {
                pl.err = "Too many arguments";
                return true;
            }

            string str = pl.op1.substr(1);

            if (isNumeric(str))
            {
                ll disp = stoi(str);
                bool simple = false;

                if (pl.isFormat4)
                {
                    if (validf4(disp))
                    {
                        obj.format = 4;
                        obj.displacement = disp;
                        obj.ni = 1;
                        obj.xbpe = 1;
                        obj.opcode = opTab[pl.opcode].opcode;
                    }
                    else
                    {
                        pl.err = "Immediate " + pl.op1 + "displacement overflows";
                    }
                }
                else
                {
                    if (validf3(disp))
                    {
                        obj.format = 3;
                        obj.displacement = disp;
                        obj.ni = 1;
                        obj.xbpe = 0;
                        obj.opcode = opTab[pl.opcode].opcode;
                    }
                    else
                    {
                        throw "Invalid Displacement";
                    }
                }
            }
            else
            {
                auto symbol = getSymbol(symTab, pl, 1);
                bool isAbsolute = symbol.flags == "A";
                long long effectiveLoc = symbol.block.startingAddress + symbol.location;
                bool simple = false;

                if (isAbsolute)
                {
                    effectiveLoc = symbol.location;
                }
                if (pl.isFormat4)
                {

                    if (validf4(effectiveLoc))
                    {
                        obj.format = 4;
                        obj.displacement = effectiveLoc;
                        obj.ni = 1;
                        obj.xbpe = 1;
                        obj.opcode = opTab[pl.opcode].opcode;
                        if (!isAbsolute)
                        {
                            modi.push_back(createModificationRecord(pl.location + 1, 5));
                        }
                    }
                    else
                    {
                        pl.err = "Symbol " + pl.op1 + "displacement overflows";
                    }
                }
                else
                {
                    if (!isAbsolute)
                    {
                        effectiveLoc -= locationCtr;
                    }
                    if (validf3(effectiveLoc))
                    {
                        obj.format = 3;
                        obj.displacement = effectiveLoc;
                        obj.ni = 1;
                        obj.xbpe = 2;
                        obj.opcode = opTab[pl.opcode].opcode;
                    }
                    else if (base && !isAbsolute)
                    {
                        effectiveLoc += locationCtr - baseRegister;
                        if (validf3(effectiveLoc))
                        {
                            obj.format = 3;
                            obj.displacement = effectiveLoc;
                            obj.ni = 1;
                            obj.xbpe = 4;
                            obj.opcode = opTab[pl.opcode].opcode;
                        }
                        else
                        {
                            simple = true;
                        }
                    }
                    else if (simple && !isAbsolute)
                    {
                        long long simpleLoc = symbol.location;
                        if (validf3(simpleLoc))
                        {
                            obj.format = 3;
                            obj.displacement = simpleLoc;
                            obj.ni = 1;
                            obj.xbpe = 0;
                            obj.opcode = opTab[pl.opcode].opcode;
                        }
                        else
                        {
                            throw "Invalid Displacement";
                        }
                    }
                    else
                    {
                        throw "Invalid Displacement";
                    }
                }
            }
        }
        else if (pl.op1[0] == '=')
        {
            auto literal = litTab.find(pl.op1)->second;
            bool simple = false;

            if (pl.isFormat4)
            {
                long long effectiveLoc = literal.block.startingAddress + literal.address;
                if (validf4(effectiveLoc))
                {
                    obj.format = 4;
                    obj.displacement = effectiveLoc;
                    obj.ni = 3;
                    obj.xbpe = 1;
                    obj.opcode = opTab[pl.opcode].opcode;
                    modi.push_back(createModificationRecord(pl.location + 1, 5));
                }
                else
                {
                    pl.err = "Literal " + pl.op1 + "displacement overflows";
                }
            }
            else
            {

                long long effectiveLoc = literal.block.startingAddress + literal.address - locationCtr;
                if (validf3(effectiveLoc))
                {
                    obj.format = 3;
                    obj.displacement = effectiveLoc;
                    obj.ni = 3;
                    obj.xbpe = 2;
                    obj.opcode = opTab[pl.opcode].opcode;
                }
                else if (base)
                {
                    effectiveLoc += locationCtr - baseRegister;
                    if (validf3(effectiveLoc))
                    {
                        obj.format = 3;
                        obj.displacement = effectiveLoc;
                        obj.ni = 3;
                        obj.xbpe = 4;
                        obj.opcode = opTab[pl.opcode].opcode;
                    }
                    else
                    {
                        simple = true;
                    }
                }
                else if (simple)
                {
                    long long simpleLoc = literal.address;
                    if (validf3(simpleLoc))
                    {
                        obj.format = 3;
                        obj.displacement = simpleLoc;
                        obj.ni = 3;
                        obj.xbpe = 0;
                        obj.opcode = opTab[pl.opcode].opcode;
                    }
                    else
                    {
                        throw "Invalid Displacement";
                    }
                }
                else
                {
                    throw "Invalid Displacement";
                }
            }
        }
        else
        {
            obj = pcOrBaseRelativeAddressing(symTab, opTab, locationCtr, pl, 3, 0, modi, base, baseRegister);
        }
        pl.objCode = obj;
        if (pl.opcode == "LDB")
        {
            baseRegister = obj.displacement;
        }
    }
    catch (string err)
    {
        return true;
    }
    return false;
}

ll baseRegister = 0;
bool base = false;

bool Pass2(map<string, Symbol> &symTab, map<string, OpCode> &opTab, map<string, Literal> &litTab, map<string, BlockTable> &blkTab, map<string, ll> &regs, vector<parsedLine> &v, ll &programL, vector<ModificationRecord> &modi)
{
    ll startingAddress = 0, locationCtr = 0, programLength = 0; // by default start address is 0
    bool err = false;
    BlockTable active = blkTab["DEFAULT"];
    ll numLines = v.size(); // number of parsed lines
    // iterate through the vector of parsed lines
    for (ll i = 0; i < numLines; i++)
    {
        parsedLine line = v[i];
        if (line.opcode == "START")
        {
            startingAddress = stoi(line.op1);
            locationCtr = startingAddress;
        }
        else if (line.opcode == "END")
        {
            // Get and Set the program length
            computeProgramLength(blkTab, programLength);
            programL = programLength;
        }
        else if (line.opcode == "EQU" || line.opcode == "LTORG")
        {
            // Implemented in pass-1
        }
        else if (line.opcode == "ORG")
        {
            // TODO
        }
        else if (line.opcode == "USE")
        {
            blkTab[active.name].locationCtr = locationCtr;
            active = blkTab[line.op1]; // block error
            locationCtr = active.locationCtr;
        }
        else if (line.opcode == "BASE")
        {
            auto sym = symTab.find(line.op1);
            if (sym == symTab.end())
            {
                line.err = "Symbol Undefined";
                err = true;
                continue;
            }
            base = true;
        }
        else if (line.opcode == "RESW")
        {
            locationCtr += 3 * stoi(line.op1);
        }
        else if (line.opcode == "RESB")
        {
            locationCtr += stoi(line.op1);
        }
        else if (line.opcode == "WORD")
        {
            locationCtr += 3;
            err = createObjCodeForWord(line);
        }
        else if (line.opcode == "*")
        {
            createObjectCodeForData(line, 1);
            if (line.op1[0] == 'C')
            {
                locationCtr += line.op1.length() - 4;
            }
            else if (line.op1[0] == 'X')
            {
                locationCtr += (line.op1.length() - 4) / 2; // raise error if not a valid prepended hex
            }
        }
        else if (line.opcode == "BYTE" || line.opcode == "*")
        {
            createObjectCodeForData(line, 0);
            if (line.op1[0] == 'C')
            {
                locationCtr += line.op1.length() - 3;
            }
            else if (line.op1[0] == 'X')
            {
                locationCtr += (line.op1.length() - 3) / 2; // raise error if not a valid prepended hex
            }
        }
        else if (line.opcode == "NOBASE")
        {
            base = false;
        }
        else // no assembler directive, mnemonic is present
        {
            OpCode op = opTab.find(line.opcode)->second;
            if (line.isFormat4)
            {
                locationCtr += 4;
            }
            else
            {
                locationCtr += op.possibleFormat;
            }
            ll pcRel = active.startingAddress + locationCtr;
            if (op.possibleFormat == FORMAT_2)
            {
                err = createObjectCodeWithRegisters(line, opTab, regs);
                line.objCode.format = 2;
                line.objCode.ni = 0;
                line.objCode.xbpe = 0;
            }
            else if (op.possibleFormat == FORMAT_1)
            {
                err = createObjectCodeWithOnlyOpcode(line, opTab);
                line.objCode.format = 1;
            }
            else
            {
                err = createObjectCodeForInstruction(line, opTab, symTab, litTab, pcRel, modi, base, baseRegister);
            }
        }
        v[i] = line;
    }
    return err;
}

#endif