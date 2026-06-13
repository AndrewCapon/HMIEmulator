#pragma once

#include "Command.h"

class PingCommand : public Command
{
public:
  PingCommand()
  : Command("pi")
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf(" PingCommand\n");
    SendResponse();

    return true;
  }
};