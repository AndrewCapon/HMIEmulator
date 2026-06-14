#pragma once

#include "HmiCommand.h"
#include "Control.h"
#include "ControlCollection.h"

class SetControlValueCommand : public HmiCommand
{
public:
  SetControlValueCommand(ISerial &serial, ControlCollection &controlCollection)
  : HmiCommand("s", serial),
    m_controlCollection(controlCollection)
  {

  }

  bool Process(Tokenizer::TokenVector tokens)
  {
    bool bResult = true;

    int   nHwId  = static_cast<int>(tokens[1]);
    float fValue = static_cast<float>(tokens[2]);

    printf("[HMI] SetControlValueCommand [%d] = %f\n", nHwId, fValue);
    Control *pControl = m_controlCollection.Find(nHwId);
    if(pControl)
    {
      pControl->SetValue(fValue);
      printf("[HMI]  Success updated '%s'\n", pControl->GetLabel().c_str());
    }
    else
    {
      printf("[HMI]  Failed to find control\n");
      bResult = false;
    }

    SendResponse();

    return bResult;
  }

  bool Send(int nHwId, float fValue)
  {
    char buffer[128];
    sprintf(buffer, "s %d %f", nHwId, fValue);

    printf("[HMI] Sending SetControlValueCommand [%d] = %f\n", nHwId, fValue);
    return SendCommand(buffer);
  }

private:
  ControlCollection &m_controlCollection;
};