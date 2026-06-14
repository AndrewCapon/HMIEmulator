#pragma once

#include "Command.h"
#include "Control.h"
#include "ControlCollection.h"

class RemoveControlCommand : public Command
{
public:
  RemoveControlCommand(ISerial &serial, ControlCollection &controlCollection)
  : Command("d", serial),
    m_controlCollection(controlCollection)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf(" AddCommand\n");
    m_controlCollection.Remove(static_cast<int>(tokens[1]));
    SendResponse();
    return true;
  }

private:
  ControlCollection &m_controlCollection;
};