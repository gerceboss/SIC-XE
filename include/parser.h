#include <iostream>
#include "utils.h"

bool format1()
{

    // checks iiif the provided line corresponds to format 1 or not
}

/*
    parses the read line as needed to process and generate tables and object code
    1.Iterate and skip the blank spaces.
    2..Create array of 5 strings to store the parsed line.

    According to the array sizes:
      size == 0 => empty line
      size == 1 => only one opcode
      size == 2 => [label, opcode] or [opcode, operand]
      size == 3 => [label, opcode, operand] or [opcode, operand1, operand2]
    size == 4 => [label, opcode, operand1, operand2] or [opcode, operand1, operand2, operand3]
*/
parsedLine parseLine()
{
}
