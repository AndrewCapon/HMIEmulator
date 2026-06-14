#pragma once

#include "HmiCommand.h"
#include "Control.h"
#include "ControlCollection.h"

class AddControlCommand : public HmiCommand
{
public:
  AddControlCommand(ISerial &serial, ControlCollection &controlCollection)
  : HmiCommand("a", serial),
    m_controlCollection(controlCollection)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    printf("[HMI] AddCommand\n");
    m_controlCollection.Add(new Control(tokens));
    SendResponse();
    return true;
  }

private:
  ControlCollection &m_controlCollection;
};