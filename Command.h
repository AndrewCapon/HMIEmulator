#pragma once

#include <string>
#include <vector>
#include <map>

#include "Tokenizer.h"
#include "Serial.h"


class Command
{
public:
  typedef std::map<std::string, Command *>            CommandMap;
  typedef std::map<std::string, Command *>::iterator  CommandMapIter;
  
  Command(std::string sCommandName)
  : m_sCommandName(sCommandName)
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
    Serial::Write(buffer, 20);
    printf("  Response : %s\n", buffer);
  }

private:
  std::string m_sCommandName;  
};
