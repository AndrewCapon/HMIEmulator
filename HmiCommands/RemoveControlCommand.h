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
    int nHwId = static_cast<int>(tokens[1]);
    printf("[HMI] RemoveCommand (%d)\n", nHwId);
    m_controlCollection.Remove(nHwId);
    SendResponse();
    return true;
  }

private:
  ControlCollection &m_controlCollection;
};