#ifndef PARSER
#define PARSER

#include <iostream>
#include "utils.h"
using namespace std;
#define ll long long

bool format_1(string s)
{
    // checks if the provided line corresponds to format 1 or not
    s = toUpperCase(s);
    bool check = s == ("RSUB") || s == ("NOBASE") || s == ("HIO") || s == ("FIX") || s == ("NORM") || s == ("TIO") || s == ("START") || s == ("SIO") || s == ("FLOAT") || s == ("CSECT");
    return check;
}

/*
    parses the read line as needed to process and generate tables and object code
    1.Iterate and skip the blank spaces.
    2..Create array of 5 strings to store the parsed line.

    According to the array sizes:
      size  => empty line
      size == 1 => only one opcode
      size == 2 => [label, opcode] or [opcode, operand]
      size == 3 => [label, opcode, operand] or [opcode, operand1, operand2]
    size == 4 => [label, opcode, operand1, operand2] or [opcode, operand1, operand2, operand3]
*/
parsedLine parseLine(string line)
{
    parsedLine pl;

    ll i = 0;
    string s[5] = {"", "", "", "", ""};

    for (ll j = 0; j < 5 && i < line.length(); j++)
    {
        // skip all the blank space
        for (; i < line.length() && (line[i] == ' ' || line[i] == '\t'); i++)
            ;

        // store the value
        for (; i < line.length() && (!(line[i] == ' ' || line[i] == '\t')); i++)
        {
            s[j] += line[i];
        }
    }

    if (s[0] == "")
    {
        return pl;
    }
    else if (s[1] == "")
    {
        // format 1
        pl.opcode = s[0];
    }
    else if (s[2] == "")
    {
        // format 1
        if (format_1(s[1]))
        {
            pl.label = s[0];
            pl.opcode = s[1];
        }
        // format 2
        else
        {
            pl.opcode = s[0];
            pl.op1 = s[1];
        }
    }
    else if (s[3] == "")
    {
        // format 2
        if (s[1][s[1].length() - 1] == ',' || s[2][0] == ',')
        {
            pl.opcode = s[0];
            pl.op1 = s[1] + s[2];
        }
        // format 3
        else
        {
            pl.label = s[0];
            pl.opcode = s[1];
            pl.op1 = s[2];
        }
    }
    else if (s[4] == "")
    {
        if (s[2] == ",")
        {
            pl.opcode = s[0];
            pl.op1 = s[1] + s[2] + s[3];
        }
        else
        {
            pl.label = s[0];
            pl.opcode = s[1];
            pl.op1 = s[2] + s[3];
        }
    }
    else
    {
        if (s[3] == ",")
        {
            pl.err = "Too many arguments";
            return pl;
        }
        // line containing too many arguments
        pl.label = s[0];
        pl.opcode = s[1];
        pl.op1 = s[2] + s[3] + s[4];
    }

    s[0] = "";
    s[1] = "";
    for (i = 0; i < pl.op1.length() && pl.op1[i] != ','; i++)
    {
        s[0] += pl.op1[i];
    }
    if (i == pl.op1.length() - 1 && pl.op1[i] == ',')
    {
        s[1] = ",";
    }
    else
    {
        for (i++; i < pl.op1.length(); i++)
        {
            s[1] += pl.op1[i];
        }
    }
    pl.op1 = s[0];
    pl.op2 = s[1];
    return pl;
}

#endif
