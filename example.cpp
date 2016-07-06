#include <iostream>
#include "execprocess.hpp"

using namespace std;

int main( int argc, char *argv[] )
  {
  if (argc > 1)
    {
    cout << "child: Hello world!" << endl;
    return 42;
    }
  else
    {
    cout << "parent: executing child" << endl;
    ExecProcess p( string( argv[ 0 ] ) +" child" );
    if (!p)
      cout << "fooey!" << endl;
    else
      cout << "parent: child exited with code " << p.exitcode() << endl;
    }
  return EXIT_SUCCESS;
  }
