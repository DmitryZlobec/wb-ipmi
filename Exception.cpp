#include <cstdlib>
#include <vector>
#include <memory>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include "Exception.h"


const char* Exception::what () const throw ()
{
                return message.c_str();
}

void Exception::fatal () const
{
                cerr << "FATAL: " << what() << endl;
                exit(1);
}
