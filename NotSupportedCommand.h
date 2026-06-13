#pragma once

#include "Command.h"

class NotSupportedCommand : public Command
{
public:
  NotSupportedCommand(void)
  : Command("NotSupported")
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf(" NotSupportedCommand\n");
    SendResponse();

    return true;
  }
};