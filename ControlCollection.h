#pragma once

#include "Control.h"

class ControlCollection
{
public:  
  ControlCollection(void)
  {

  }

  void Add(Control *pControl)
  {
    m_controls[pControl->GetHwID()] = pControl;
  }

  Control *Find(int nHwId)
  {
    Control::ControlMapIter i = m_controls.find(nHwId);

    if(i != m_controls.end())
      return i->second;
    else
      return nullptr;
  }

private:
  Control::ControlMap  m_controls;
};