#pragma once

#include "Command.h"

class NotSupportedCommand : public Command
{
public:
  NotSupportedCommand(ISerial &serial)
  : Command("NotSupported", serial)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf(" NotSupportedCommand\n");
    SendResponse();

    return true;
  }
};