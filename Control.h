#pragma once

#include "Tokenizer.h"
class Control
{
public:
  typedef std::map<int, Control *>            ControlMap;
  typedef std::map<int, Control *>::iterator  ControlMapIter;

  Control(Tokenizer::TokenVector &tokens)
  {
    m_nHwId       = static_cast<int>        (tokens[1]);
    m_sLabel      = static_cast<std::string>(tokens[2]);
    m_nProperties = static_cast<int>        (tokens[3]);
    m_sUnit       = static_cast<std::string>(tokens[4]);
    m_fValue      = static_cast<float>      (tokens[5]);
    m_fMax        = static_cast<float>      (tokens[6]);
    m_fMin        = static_cast<float>      (tokens[7]);
    m_fSteps      = static_cast<float>      (tokens[8]);
  }

  int GetHwID(void)
  {
    return m_nHwId;
  }

  void SetValue(float fValue)
  {
    m_fValue = fValue;
  }

  float GetValue(void)
  {
    return m_fValue;
  }

  std::string GetLabel(void)
  {
    return m_sLabel;
  }
  
  void DebugDump(void)
  {
    printf("[HMI] Control[%d] : %s, %d, %s, %f, %f, %f, %f\n", m_nHwId, m_sLabel.c_str(), m_nProperties, m_sUnit.c_str(), m_fValue, m_fMin, m_fMax, m_fSteps);
  }

private:
  int           m_nHwId;
  std::string   m_sLabel;
  int           m_nProperties;
  std::string   m_sUnit;
  float         m_fValue;
  float         m_fMin;
  float         m_fMax;
  float         m_fSteps;
  // Scale points ignored
};