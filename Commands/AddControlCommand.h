#pragma once

#include "Command.h"
#include "Control.h"
#include "ControlCollection.h"

class AddControlCommand : public Command
{
public:
  AddControlCommand(ISerial &serial, ControlCollection &controlCollection)
  : Command("a", serial),
    m_controlCollection(controlCollection)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf(" AddCommand\n");
    m_controlCollection.Add(new Control(tokens));
    SendResponse();
    return true;
  }

private:
  ControlCollection &m_controlCollection;
};