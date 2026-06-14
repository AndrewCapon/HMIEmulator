#pragma once

#include "HmiCommand.h"

class PingCommand : public HmiCommand
{
public:
  PingCommand(ISerial &serial)
  : HmiCommand("pi", serial)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    if(!m_bSilent)
      printf("[HMI] PingCommand\n");
      
    SendResponse();

    return true;
  }
};