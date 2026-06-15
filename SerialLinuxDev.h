#pragma once

#include <stdio.h>
#include <fcntl.h> 
#include <errno.h> 
#include <cstring>
#include <unistd.h>

#include "ISerial.h"

class SerialLinuxDev : public ISerial
{
public:
  SerialLinuxDev(const char *pszDeviceName)
  {
    m_nSerialPort= open(pszDeviceName, O_RDWR);

    // Check for errors
    if(IsInitialised())
    {
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
#endif // !NON_BLOCKING
    }
  }

  ~SerialLinuxDev(void)
  {
    if(IsInitialised())
      close(m_nSerialPort);
  }
   
  size_t Write(char *pBuffer, size_t uSize)
  {
    size_t count = write(m_nSerialPort, pBuffer, uSize);
    fsync(m_nSerialPort);

    return count;
  }

  size_t Read(char *pBuffer, size_t uSize)
  {
    return read(m_nSerialPort, pBuffer, uSize);
  }

  bool IsInitialised(void)
  {
    return m_nSerialPort != -1;
  }

private:
  int m_nSerialPort = -1;
};