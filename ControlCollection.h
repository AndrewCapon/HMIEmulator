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

  void Remove(int nHwId)
  {
   Control::ControlMapIter i = m_controls.find(nHwId);

    if(i != m_controls.end())
    {
      m_controls.erase(i);
      delete i->second;
    }
  }

  Control *Find(int nHwId)
  {
    Control::ControlMapIter i = m_controls.find(nHwId);

    if(i != m_controls.end())
      return i->second;
    else
      return nullptr;
  }

  size_t Count(void)
  {
    return m_controls.size();
  }

  void DebugDump(void)
  {
    for(Control::ControlMapIter i = m_controls.begin(); i != m_controls.end(); i++)
    {
      i->second->DebugDump();
    }
  }
private:
  Control::ControlMap  m_controls;
};