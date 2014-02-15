#include "Wrapper.hpp"
#include <iostream>
#include <cstdlib>

using namespace std;

int main()
{
  Wrapper     	wrap;
  char  		str[] = "Hello !";
  unsigned int 	bytes;

#if defined(WIN32)
  if (!wrap.OpenCOM("1")) // check Windows config panel to see COM port list
#else
  if (!wrap.OpenCOM("/dev/usb0")) // depends on linux version
#endif
    {
      cerr << "COM port opening failed" << endl;
      return EXIT_FAILURE;
    }
  if (!wrap.WriteCOM(str, strlen(str), &bytes)){
      cerr << "Write error" << endl;
    }
  cout << "Bytes wrote : " << bytes << endl;

  if (!wrap.ReadCOM(str, 4, &bytes)){
      cerr << "Read error" << endl;
    }
  cout << "Bytes read : " << bytes << endl;
  // at Wrapper's deletion, file description will be destroyed
  // so no need to do it !
  return EXIT_SUCCESS;
}