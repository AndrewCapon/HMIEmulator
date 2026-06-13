#pragma once

#include "Command.h"

class PingCommand : public Command
{
public:
  PingCommand(ISerial &serial)
  : Command("pi", serial)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf(" PingCommand\n");
    SendResponse();

    return true;
  }
};