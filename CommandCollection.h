#pragma once

#include "Command.h"

class CommandCollection
{
public:  
  CommandCollection(void)
  {

  }

  void Add(Command *pCommand)
  {
    m_commands[pCommand->CommandName()] = pCommand;
  }

  Command *Find(std::string sCommandName)
  {
    Command::CommandMapIter i = m_commands.find(sCommandName);

    if(i != m_commands.end())
      return i->second;
    else
      return nullptr;
  }

private:
  Command::CommandMap  m_commands;
};