#pragma once

#include "HmiCommand.h"

class ResponseCommand : public HmiCommand
{
public:
  ResponseCommand(ISerial &serial)
  : HmiCommand("r", serial)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    if(!m_bSilent)
      printf("[HMI] ResponseCommand %d\n", static_cast<int>(tokens[1]));

    return true;
  }
};