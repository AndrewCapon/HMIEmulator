#pragma once

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <errno.h> 
#include <termios.h> 
#include <sys/ioctl.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>

#include <vector>
#include <string>

#include "SerialLinuxDev.h"
#include "HmiCommand.h"
#include "HmiCommandCollection.h"
#include "ControlCollection.h"
#include "AddControlCommand.h"
#include "RemoveControlCommand.h"
#include "NotSupportedCommand.h"
#include "PingCommand.h"
#include "ResponseCommand.h"
#include "SetControlValueCommand.h"
#include <thread>
#include <mutex>

class Hmi
{
public:
  Hmi(void)
  {
    m_hmiCommandCollection.Add(&m_notSupportedCommand);
    m_hmiCommandCollection.Add(&m_pingCommand);
    m_hmiCommandCollection.Add(&m_responseCommand);
    m_hmiCommandCollection.Add(&m_addControlCommand);
    m_hmiCommandCollection.Add(&m_removeControlCommand);
    m_hmiCommandCollection.Add(&m_setControlValueCommand);

    m_pingCommand.SetSilent(!m_bVerbose);
    m_notSupportedCommand.SetSilent(!m_bVerbose);

    m_thread = std::thread{&Hmi::ThreadHandler, this};
  }
  
  bool IsInitialised(void)
  {
    return m_hmiSerial.IsInitialised();
  }

  void ListControls(void)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    printf("[HMI] Control count = %lu\n", m_controlCollection.Count());
    m_controlCollection.DebugDump();
  }

  void SetControlValue(int nHwId, float fValue)
  {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_setControlValueCommand.Send(nHwId, fValue);
  }

  void ToggleVerbose(void)
  {
    m_bVerbose = !m_bVerbose;
    m_pingCommand.SetSilent(!m_bVerbose);
    m_notSupportedCommand.SetSilent(!m_bVerbose);
    printf("[HMI] Verbose %s\n", m_bVerbose ? "On" : "Off");
  }

private:
  void HandleMessage(const char *pszMessage)
  {
    Tokenizer::TokenVector tokens = Tokenizer::tokenise(pszMessage, ' ');

    if(tokens.size())
    {
      HmiCommand *pFoundCommand = m_hmiCommandCollection.Find(tokens[0]);
      if(pFoundCommand)
      {
        if(!pFoundCommand->IsSilent())
          printf("[HMI] Msg: %s\n", pszMessage);

        pFoundCommand->Process(tokens);
      }
      else
      {
        if(!m_notSupportedCommand.IsSilent())
          printf("[HMI] Msg: %s\n", pszMessage);

        m_notSupportedCommand.Process(tokens);
      }
    }
  }

  void ThreadHandler(void)
  {
    while(true)
    {
      // Handle serial input
      char buffer[2048];
      int n = m_hmiSerial.Read(buffer, 2048);
      if(n>0)
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        HandleMessage(buffer);
      }
    }
  }

  SerialLinuxDev          m_hmiSerial               = SerialLinuxDev("/dev/tnt3");

  NotSupportedCommand     m_notSupportedCommand     = NotSupportedCommand(m_hmiSerial);
  PingCommand             m_pingCommand             = PingCommand(m_hmiSerial);
  ResponseCommand         m_responseCommand         = ResponseCommand(m_hmiSerial);
  AddControlCommand       m_addControlCommand       = AddControlCommand(m_hmiSerial, m_controlCollection);
  RemoveControlCommand    m_removeControlCommand    = RemoveControlCommand(m_hmiSerial, m_controlCollection);
  SetControlValueCommand  m_setControlValueCommand  = SetControlValueCommand(m_hmiSerial, m_controlCollection);
  
  HmiCommandCollection    m_hmiCommandCollection;
  ControlCollection       m_controlCollection;

  std::mutex              m_mutex;
  bool                    m_bVerbose = false;
  std::thread             m_thread;
};