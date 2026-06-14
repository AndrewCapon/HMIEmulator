#pragma once

#include "Command.h"
#include "Control.h"
#include "ControlCollection.h"

class SetControlValueCommand : public Command
{
public:
  SetControlValueCommand(ISerial &serial, ControlCollection &controlCollection)
  : Command("s", serial),
    m_controlCollection(controlCollection)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    int nHwId    = static_cast<int>(tokens[1]);
    float fValue = static_cast<float>(tokens[2]);

    printf(" SetControlValueCommand [%d] = %f\n", nHwId, fValue);
    Control *pControl = m_controlCollection.Find(nHwId);
    if(pControl)
    {
      pControl->SetValue(fValue);
      printf("  Success updated '%s'\n", pControl->GetLabel().c_str());
    }
    else
      printf("  Failed to find control\n");

    SendResponse();
    return true;
  }

private:
  ControlCollection &m_controlCollection;
};