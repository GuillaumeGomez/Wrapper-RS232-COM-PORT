#include "Wrapper.hpp"

using namespace std;

Wrapper::Wrapper()
{
#if defined (WIN32)
  this->InitCOM();
#else
  fd = -1;
#endif
}

Wrapper::~Wrapper()
{
  this->CloseCOM();
}

#if defined (WIN32)
void Wrapper::InitCOM()
{
  g_hCOM = NULL;
  COMMTIMEOUTS t_g_cto =
  {
    //MAX_WAIT_READ,  /* ReadIntervalTimeOut          */
    0,              /* ReadTotalTimeOutMultiplier   */
    //MAX_WAIT_READ,  /* ReadTotalTimeOutConstant     */
    0,              /* WriteTotalTimeOutMultiplier  */
    0               /* WriteTotalTimeOutConstant    */
  };

  memcpy(&g_cto, &t_g_cto, sizeof(t_g_cto));

  // COM port configuration
  DCB t_g_dcb =
  {
    sizeof(DCB),        /* DCBlength            */
    19200,               /* BaudRate             */
    TRUE,               /* fBinary              */
    FALSE,              /* fParity              */
    FALSE,              /* fOutxCtsFlow         */
    FALSE,              /* fOutxDsrFlow         */
    DTR_CONTROL_ENABLE, /* fDtrControl          */
    FALSE,              /* fDsrSensitivity      */
    FALSE,              /* fTXContinueOnXoff    */
    FALSE,              /* fOutX                */
    FALSE,              /* fInX                 */
    FALSE,              /* fErrorChar           */
    FALSE,              /* fNull                */
    RTS_CONTROL_ENABLE, /* fRtsControl          */
    FALSE,              /* fAbortOnError        */
    0,                  /* fDummy2              */
    0,                  /* wReserved            */
    0x100,              /* XonLim               */
    0x100,              /* XoffLim              */
    8,                  /* ByteSize             */
    NOPARITY,           /* Parity               */
    ONESTOPBIT,         /* StopBits             */
    0x11,               /* XonChar              */
    0x13,               /* XoffChar             */
    '?',                /* ErrorChar            */
    0x1A,               /* EofChar              */
    0x10                /* EvtChar              */
  };

  memcpy(&g_dcb, &t_g_dcb, sizeof(t_g_dcb));
}

#else
bool Wrapper::Configure(const char *pPort, int speed, int NbBits, int parity, int NbStop)
{
  // opening COM port
  if ((fd = open(pPort, O_RDWR)) == -1){
      return false;
    }

  Config.c_iflag = IGNBRK | IGNPAR;
  Config.c_oflag = 0;

  Config.c_cflag = CREAD | CLOCAL;
  Config.c_cflag = Config.c_cflag & ~CRTSCTS;
  // speed
  switch (speed)
    {
    case BPS9600:
      Config.c_cflag |= B9600;
      break;
    case BPS4800:
      Config.c_cflag |= B4800;
      break;
    case BPS2400:
      Config.c_cflag |= B2400;
      break;
    case BPS19200:
      Config.c_cflag |= B19200;
      break;
    default: //invalid speed
      return false;
    }
  // data size
  switch (NbBits)
    {
    case BIT8:
      Config.c_cflag |= CS8;
      break;
    case BIT7:
      Config.c_cflag |= CS7;
      break;
    case BIT6:
      Config.c_cflag |= CS6;
      break;
    case BIT5:
      Config.c_cflag |= CS5;
      break;
    default: // invalid data size
      return false;
    }
  // parity
  switch (parity)
    {
    case PAIR_PARITY:
      Config.c_cflag |=  PARENB;
      break;
    case ODD_PARITY:
      Config.c_cflag |= (PARODD | PARENB);
      break;
    default:
      break;
    }

  switch (NbStop)
    {
    case STOP2:
      Config.c_cflag |= CSTOPB;
      break;
    }

  // apply configuration
  if (tcsetattr(fd, TCSANOW, &Config) == -1){
      return false;
    }
  tcflush(fd, TCIFLUSH);
  tcflush(fd, TCOFLUSH);

  return true;
}
#endif

bool Wrapper::OpenCOM(std::string nId)
{
#if defined (WIN32)

  nId = "\\\\.\\COM" + nId;

  g_hCOM = CreateFile(nId.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
  if (g_hCOM == INVALID_HANDLE_VALUE)
    return false;

  SetupComm(g_hCOM, RX_SIZE, TX_SIZE);

  if (!SetCommTimeouts(g_hCOM, &g_cto) || !SetCommState(g_hCOM, &g_dcb))
    {
      CloseHandle(g_hCOM);
      return false;
    }

  PurgeComm(g_hCOM, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);
  EscapeCommFunction(g_hCOM, SETDTR);
  return true;
#else
  return Configure(nId.c_str(), BPS19200, BIT8, NO_PARITY, STOP1);
#endif
}

bool Wrapper::CloseCOM()
{
#if defined (WIN32)
  return CloseHandle(g_hCOM);
#else
  if (fd >= 0)
    close(fd);
  fd = -1;
  return true;
#endif
}

bool Wrapper::ReadCOM(char *buffer, unsigned int nBytesToRead, unsigned int *readBytes)
{
  int tmp(0);

  *readBytes = 0;
  while (*readBytes < nBytesToRead)
    {
#if defined (WIN32)
      if (!ReadFile(g_hCOM, buffer + *readBytes, nBytesToRead - *readBytes, (DWORD*)&tmp, NULL))
        return false;
#else
      if ((tmp = read(fd, buffer + *readBytes, nBytesToRead - *readBytes, 1)) < 1)
        return false;
#endif
      *readBytes += tmp;
    }
  return true;
}

bool Wrapper::WriteCOM(const char *buffer, unsigned int nBytesToWrite, unsigned int *pBytesWritten)
{
  int tmp(0);

  *pBytesWritten = 0;
  while (*pBytesWritten < nBytesToWrite)
    {
#if defined (WIN32)
      if (!WriteFile(g_hCOM, buffer + *pBytesWritten, nBytesToWrite - *pBytesWritten, (DWORD*)&tmp, NULL))
        return false;
#else
      if ((tmp = write(fd, buffer + *pBytesWritten, nBytesToWrite - *pBytesWritten)) < 1)
        return false;
#endif
      *pBytesWritten += tmp;
    }
  return true;
}