#include "variables.h"

#include <cctype>


namespace
{
    int readVariable(std::istream& in, const char* var)
    {
        in >> std::ws;
        int r = 0;
        int i = 0;

        while (var[i] && var[i] == in.get())
        {
            ++i;
            ++r;
        }

        if (var[i])
        {
            r = 0;
        }
        return r;
    }
}

void Variables::add(const char* var)
{
    size_t n = strlen(var);
    char* name = new char[n + 1];

    size_t k = 0;
    while (k < n && isspace(var[k]))
    {
        ++k;
    }

    size_t i = 0;
    while (k < n && !isspace(var[k]))
    {
        name[i] = var[k];
        ++i;
        ++k;
    }

    name[i] = '\0';
    if (find(name) >= 0)
    {
        IERROR("find(name) >= 0");
    }
    list_.push_back(name);
}

//if there exists the same word as var in list_, return larger than 1. If not, return -1
int Variables::find(const char* var) const
{
    int r = 0;
    ConstIterator i(list_.begin());

    while (i != list_.end() && strcmp(*i, var) != 0)
    {
        ++i;
        ++r;
    }

    if (i == list_.end())
    {
        r = -1;
    }
    return r;
}

//read if the first element of in contains the variable in the list_
int Variables::read(std::istream& in) const
{
    std::streampos posbeg = in.tellg(), posend;
    int varCurrent = 0, var = -1;
    int lenCurrent = 0, len = 0;

    ConstIterator i = list_.begin();
    while (i != list_.end())
    {
        in.seekg(posbeg); //read from the beginning
        lenCurrent = readVariable(in, *i); //check if i is the first element of in stream
        if (lenCurrent > 0)
        {
            var = varCurrent; // the index of the matching variable in the list_
            len = lenCurrent; // the length of the matching variable
            posend = in.tellg(); // position of the end of the matching variable
            break;
        }
        ++varCurrent;
        ++i;
    }

    // you need this in case the in stream contains u12, and there are variables u1 and u12. 
    // the first element of in stream can satisfy "readVariable(in, u1)" but is not right
    // this is on the premise that there won't be u121; 
    // the variable won't have the same first two digits and a varying third digit
    while (i != list_.end())
    {
        in.seekg(posbeg); //read from the beginning
        lenCurrent = readVariable(in, *i); //check if i is the first element of in stream
        if (lenCurrent > len) //u12 len is bigger than u1 
        {
            var = varCurrent;
            len = lenCurrent;
            posend = in.tellg();
        }
        ++varCurrent;
        ++i;
    }

    if (var >= 0) //if found, set read point where the variable ended
    {
        in.seekg(posend);
    }
    else //if not found, set read point where it was
    {
        in.seekg(posbeg);
    }

    return var;
}
