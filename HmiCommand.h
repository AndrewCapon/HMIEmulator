#pragma once

#include <string>
#include <vector>
#include <map>

#include "Tokenizer.h"
#include "ISerial.h"


class HmiCommand
{
public:
  typedef std::map<std::string, HmiCommand *>            CommandMap;
  typedef std::map<std::string, HmiCommand *>::iterator  CommandMapIter;
  
  HmiCommand(std::string sCommandName, ISerial &serial)
  : m_sCommandName(sCommandName),
    m_serial(serial)
  {

  }
  
  void SetSilent(bool bSilent)
  {
    m_bSilent = bSilent;
  }

  bool IsSilent(void)
  {
    return m_bSilent;
  }

  std::string CommandName(void)
  {
    return m_sCommandName;
  }

  virtual bool Process(Tokenizer::TokenVector tokens) = 0;
  
protected:
  bool        m_bSilent = false;

  void SendResponse(void)
  {
    char buffer[20] = "r 0";
    m_serial.Write(buffer, 20);
    if(!m_bSilent)
      printf("[HMI]  Response : %s\n", buffer);
  }

  bool SendCommand(char *buffer)
  {
    size_t len = strlen(buffer)+1;
    return len == m_serial.Write(buffer, len);

  }
private:
  std::string m_sCommandName;  
  ISerial     &m_serial;
};
