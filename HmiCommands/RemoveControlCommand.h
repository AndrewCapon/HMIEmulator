#pragma once

#include "HmiCommand.h"
#include "Control.h"
#include "ControlCollection.h"

class RemoveControlCommand : public HmiCommand
{
public:
  RemoveControlCommand(ISerial &serial, ControlCollection &controlCollection)
  : HmiCommand("d", serial),
    m_controlCollection(controlCollection)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf("[HMI] RemoveCommand\n");
    m_controlCollection.Remove(static_cast<int>(tokens[1]));
    SendResponse();
    return true;
  }

private:
  ControlCollection &m_controlCollection;
};