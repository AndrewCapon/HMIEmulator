#pragma once

#include <stdio.h>
#include <fcntl.h> 
#include <errno.h> 
#include <cstring>

class ISerial
{
public:  
  virtual size_t  Write(char *pBuffer, size_t uSize) = 0;
  virtual size_t  Read(char *pBuffer, size_t uSize) = 0;
  virtual bool    IsInitialised(void) = 0;
};