#ifndef OBJGEN
#define OBJGEN

#include <iostream>
#include <map>
#include "structs.h"
#include "utils.h"
#include "tables.h"
#include <string>
#include <iomanip>

TextRecord tex;
bool writeToNew;

// write the header
void writeHeaderRecord(string name, ll startingAddress, ll programLength)
{
    cout << 'H' << getProgramName(name) << setfill('0') << setw(6) << right << hex << startingAddress << programLength << '\n';
}

void initTextRecord(ll start)
{
    tex.text = "";
    tex.startingAddress = start;
    tex.label = 'T';
    tex.size = 0;
    writeToNew = false;
}

// useful for printing the whole code
void printRecord(TextRecord t)
{
    cout << uppercase << t.label << setfill('0') << setw(6) << hex << t.startingAddress << setfill('0') << setw(2) << hex << (t.size / 2) << t.text << "\n";
}

// to start a new text record on a new line
void breakRecord()
{
    if (tex.size != 0)
        printRecord(tex);
    tex.size = 0;
}

void writeTextEntity(pair<ll, ll> p, ll &locationCtr)
{
    std::ostringstream ss;
    ss << uppercase << setfill('0') << setw(2 * p.second) << std::hex << p.first;
    std::string result = ss.str();
    // check if the generated has a length less than 60 columns else start a new one on the other line
    if ((tex.size + result.length()) <= 60)
    {
        tex.text += result;
        tex.size += result.length();
    }
    else
    {
        breakRecord();
        initTextRecord(locationCtr);
    }
}

// writing the generated text record, using if-else to check if any strings are like WORD, RESB etc...
void writeTextRecord(map<string, BlockTable> &blkTab, vector<parsedLine> &pls)
{
    ll startingAddress = 0;
    ll locationCtr = startingAddress;
    BlockTable current = blkTab["DEFAULT"];
    initTextRecord(startingAddress);
    ll newRel = current.startingAddress + locationCtr;

    for (auto &pl : pls)
    {
        newRel = current.startingAddress + locationCtr;
        pair<ll, ll> p;
        if (pl.opcode == "END")
        {
            breakRecord();
            continue;
        }
        if (pl.opcode == "LTORG" || pl.opcode[0] == '.' || pl.opcode == "BASE" || pl.opcode == "NOBASE" || pl.opcode == "START" || pl.opcode == "EQU")
        {
            continue;
        }
        if (pl.opcode == "USE")
        {
            // new bolck or default one
            if (pl.op1 == "")
            {
                pl.op1 = "DEFAULT";
            }
            blkTab[current.name].locationCtr = locationCtr;
            current = blkTab[pl.op1];
            locationCtr = current.locationCtr;
            newRel = current.startingAddress + locationCtr;
            breakRecord();
            initTextRecord(newRel);
        }
        else if (pl.opcode == "WORD")
        {
            if (writeToNew)
            {
                initTextRecord(newRel);
            }
            p = genObjcode(pl.objCode, pl);
            writeTextEntity(p, newRel);
            locationCtr += 3;
        }
        else if (pl.opcode == "BYTE")
        {
            p = genObjcode(pl.objCode, pl);
            if (writeToNew)
            {
                initTextRecord(newRel);
            }
            writeTextEntity(p, newRel);
            if (pl.op1[0] == 'X')
            {
                locationCtr += (pl.op1.length() - 3) / 2;
            }
            else
            {
                locationCtr += pl.op1.length() - 3;
            }
        }
        else if (pl.opcode == "RESB")
        {
            breakRecord();
            locationCtr += stoi(pl.op1);
            newRel = current.startingAddress + locationCtr;
            initTextRecord(newRel);
        }
        else if (pl.opcode == "RESW")
        {
            breakRecord();
            locationCtr += 3 * stoi(pl.op1);
            newRel = current.startingAddress + locationCtr;
            initTextRecord(newRel);
        }
        else if (pl.opcode == "*")
        {
            p = genObjcode(pl.objCode, pl);
            if (writeToNew)
            {
                initTextRecord(newRel);
            }
            writeTextEntity(p, newRel);
            if (pl.op1[1] == 'X')
            {
                locationCtr += (pl.op1.length() - 4) / 2;
            }
            else
            {
                locationCtr += (pl.op1.length() - 4);
            };
        }
        else
        {
            p = genObjcode(pl.objCode, pl);
            if (writeToNew)
            {
                initTextRecord(newRel);
            }
            writeTextEntity(p, newRel);

            if (pl.isFormat4)
            {
                locationCtr += 4;
            }
            else if (pl.objCode.format == 2)
            {
                // cout << "*******xbpe : " <<  pl.objCode.ni << endl;
                locationCtr += 2;
            }
            else
            {
                locationCtr += 3;
            };
        }
    }
}

void writeEndRecord(ll programLength)
{
    cout << 'E' << setfill('0') << setw(6) << hex << programLength << "\n";
}
void writeModificationRecord(vector<ModificationRecord> &modi)
{
    for (auto &x : modi)
    {
        cout << x.label << setfill('0') << setw(6) << x.start << setfill('0') << setw(2) << x.modified << "\n";
    }
}

#endif