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

#include "SerialTNT3.h"
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

#define SILENT 1

std::mutex mutex;
SerialTNT3 serial;

HmiCommandCollection    hmiCommandCollection;
ControlCollection       controlCollection;

NotSupportedCommand     notSupportedCommand(serial);
PingCommand             pingCommand(serial);
ResponseCommand         responseCommand(serial);
AddControlCommand       addControlCommand(serial, controlCollection);
RemoveControlCommand    removeControlCommand(serial, controlCollection);
SetControlValueCommand  setControlValueCommand(serial, controlCollection);

void HandleMessage(const char *pszMessage)
{

  Tokenizer::TokenVector tokens = Tokenizer::tokenise(pszMessage, ' ');

  if(tokens.size())
  {
    HmiCommand *pFoundCommand = hmiCommandCollection.Find(tokens[0]);
    if(pFoundCommand)
    {
      if(!pFoundCommand->IsSilent())
        printf("[HMI] Msg: %s\n", pszMessage);

      pFoundCommand->Process(tokens);
    }
    else
    {
      if(!notSupportedCommand.IsSilent())
        printf("[HMI] Msg: %s\n", pszMessage);

      notSupportedCommand.Process(tokens);
    }
  }
}

void HandleHMISerial(void)
{
  printf("Processing HMI Commands...\n");
  while(true)
  {
    // Handle serial input
    char buffer[2048];
    int n = serial.Read(buffer, 2048);
    if(n>0)
    {
      std::lock_guard<std::mutex> lock(mutex);
      HandleMessage(buffer);
    }
  }
}

void HandleUI(void)
{
  char  *line = nullptr;
  size_t len = 0;

  while(true)
  {
    getline(&line, &len, stdin);
    Tokenizer::TokenVector tokens = Tokenizer::tokenise(line, ' ');
    size_t uTokens = tokens.size();
    if(uTokens)
    {
      std::string sCommand = tokens[0];

      std::lock_guard<std::mutex> lock(mutex);
      switch (sCommand.front())
      {
        // list controls
        case 'c' :
        {
          printf("[UI] Control count = %lu\n", controlCollection.Count());
          controlCollection.DebugDump();
        }

        // set Control
        case 's' :
        {
          if(uTokens == 3)
          {
            int   nHwId  = tokens[1];
            float fValue = tokens[2];
            setControlValueCommand.Send(nHwId, fValue);
          }
        }
      }
    }
  }
}

int main(int, char**)
{
  printf("HMIEmulator\n");
  printf("===========\n\n");
  printf("Console commands (need newline):\n");
  printf("  c     : List controls.\n");
  printf("  s n f : Set control n to f.\n");

  printf("Initialising serial\n");
  if(!serial.IsInitialised())
  {
    printf("Serial port failed to initialise\n");
    return 1;
  }

  hmiCommandCollection.Add(&notSupportedCommand);
  hmiCommandCollection.Add(&pingCommand);
  hmiCommandCollection.Add(&responseCommand);
  hmiCommandCollection.Add(&addControlCommand);
  hmiCommandCollection.Add(&removeControlCommand);
  hmiCommandCollection.Add(&setControlValueCommand);

#if SILENT
  pingCommand.SetSilent(true);
  notSupportedCommand.SetSilent(true);
#endif

  std::thread hmiThread(HandleHMISerial);
  std::thread uiThread(HandleUI);

  hmiThread.join();
  uiThread.join();
}

