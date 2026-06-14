#pragma once

#include "HmiCommand.h"

class NotSupportedCommand : public HmiCommand
{
public:
  NotSupportedCommand(ISerial &serial)
  : HmiCommand("NotSupported", serial)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    if(!m_bSilent)
      printf("[HMI] NotSupportedCommand\n");

    SendResponse();

    return true;
  }
};