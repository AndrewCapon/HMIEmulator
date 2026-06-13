#pragma once

#include <string>
#include <vector>
#include <map>

#include "Tokenizer.h"
#include "ISerial.h"


class Command
{
public:
  typedef std::map<std::string, Command *>            CommandMap;
  typedef std::map<std::string, Command *>::iterator  CommandMapIter;
  
  Command(std::string sCommandName, ISerial &serial)
  : m_sCommandName(sCommandName),
    m_serial(serial)
  {

  }
  
  std::string CommandName(void)
  {
    return m_sCommandName;
  }

  virtual bool Process(Tokenizer::TokenVector tokens) = 0;
  
protected:
  void SendResponse(void)
  {
    char buffer[20] = "r 0";
    m_serial.Write(buffer, 20);
    printf("  Response : %s\n", buffer);
  }

private:
  std::string m_sCommandName;  
  ISerial     &m_serial;
};
