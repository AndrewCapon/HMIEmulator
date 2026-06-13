#pragma once

#include <stdio.h>
#include <fcntl.h> 
#include <errno.h> 
#include <cstring>

namespace Serial
{
  int nSerialPort;

  bool Initialise(void)
  {
    nSerialPort= open("/dev/tnt3", O_RDWR);

    // Check for errors
    if (nSerialPort < 0) {
      printf("Error %i from open: %s\n", errno, strerror(errno));
      return false;
    }

  #if NON_BLOCKING
    // set non blocking
    struct termios tty;

    if(tcgetattr(nSerialPort, &tty) != 0) 
    {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      close(nSerialPort);
      return false;
    }

    tty.c_cc[VTIME] = 0; 
    tty.c_cc[VMIN] = 0;

    if (tcsetattr(nSerialPort, TCSANOW, &tty) != 0) 
    {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      close(nSerialPort);
      return false;
    }
  #endif // !USE_POSIX_TIMER

    return true;
  }
   
  size_t Write(char *pBuffer, size_t uSize)
  {
    return write(nSerialPort, pBuffer, uSize);
  }

  size_t Read(char *pBuffer, size_t uSize)
  {
    return read(nSerialPort, pBuffer, uSize);
  }

  void Close(void)
  {
    close(nSerialPort);
  }
}