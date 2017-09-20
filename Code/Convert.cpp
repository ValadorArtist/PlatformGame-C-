#include <sstream>
#include "Convert.h"

std::string IntToStr(int number) 
{
    std::ostringstream ss;
    ss << number;
    return ss.str();
}


int StrToInt(const std::string& number) 
{
    std::istringstream ss(number);;
    int n;
    ss >> n;
    return n;
}
