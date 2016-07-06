#include <cstdlib>
#include <vector>
#include <memory>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fstream>
using namespace std;

class Exception: public exception
{
public:
        Exception(string _message): message("LIRC error: " + _message){}
        const char* what () const throw ();
        void fatal () const;
private:
        string message;
};
