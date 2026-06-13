#pragma once

#include "Command.h"
#include "Control.h"

class AddCommand : public Command
{
public:
  AddCommand()
  : Command("a")
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf(" AddCommand\n");
    Control control(tokens);
    SendResponse();
    return true;
  }
};