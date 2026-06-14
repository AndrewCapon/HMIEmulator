#pragma once

#include "HmiCommand.h"

class HmiCommandCollection
{
public:  
  HmiCommandCollection(void)
  {

  }

  void Add(HmiCommand *pCommand)
  {
    m_commands[pCommand->CommandName()] = pCommand;
  }

  HmiCommand *Find(std::string sCommandName)
  {
    HmiCommand::CommandMapIter i = m_commands.find(sCommandName);

    if(i != m_commands.end())
      return i->second;
    else
      return nullptr;
  }

private:
  HmiCommand::CommandMap  m_commands;
};