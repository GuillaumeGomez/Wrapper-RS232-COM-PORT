#ifndef Wrapper_H
#define Wrapper_H

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <string.h>

#if defined (WIN32)
#include <windows.h>
#include <conio.h>

#define RX_SIZE         4096    // in buffer size
#define TX_SIZE         4096    // out buffer size
#define MAX_WAIT_READ   5000    // max waiting time (in ms)

#else
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BPS9600		0
#define BPS4800		1
#define BPS2400		2
#define BPS19200	3

#define BIT8	0
#define BIT7	1
#define BIT6	2
#define BIT5	3

#define STOP1	0
#define STOP2	1

#define NO_PARITY		0
#define PAIR_PARITY		1
#define ODD_PARITY		2

#define OK			0
#define ERREUR		-1
#define TIMEOUT		-2
#define NOTHING		1

#endif


class Wrapper
{
public :
  Wrapper();
  virtual ~Wrapper();

  bool OpenCOM(std::string s);
  bool ReadCOM(char* buffer, unsigned int nBytesToRead, unsigned int *pBytesRead);
  bool WriteCOM(const char *buffer, unsigned int nBytesToWrite, unsigned int *pBytesWritten);
  bool CloseCOM();

private :
#if defined (WIN32)
  HANDLE	  	g_hCOM;
  COMMTIMEOUTS  g_cto;
  DCB         	g_dcb;

  void InitCOM();
#else
  termios Config;
  int fd;

  bool  Configure(const char *pPort, int speed, int NbBits, int parity, int NbStop);
#endif
};

#endif