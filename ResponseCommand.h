#pragma once

#include "Command.h"

class ResponseCommand : public Command
{
public:
  ResponseCommand(ISerial &serial)
  : Command("r", serial)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf(" PingCommand %d\n", static_cast<int>(tokens[1]));
    return true;
  }
};