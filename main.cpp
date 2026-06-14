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
#include "Command.h"
#include "CommandCollection.h"
#include "ControlCollection.h"
#include "AddControlCommand.h"
#include "RemoveControlCommand.h"
#include "NotSupportedCommand.h"
#include "PingCommand.h"
#include "ResponseCommand.h"
#include "SetControlValueCommand.h"

SerialTNT3 serial;

CommandCollection     commandCollection;
ControlCollection     controlCollection;

NotSupportedCommand     notSupportedCommand(serial);
PingCommand             pingCommand(serial);
ResponseCommand         responseCommand(serial);
AddControlCommand       addControlCommand(serial, controlCollection);
RemoveControlCommand    removeControlCommand(serial, controlCollection);
SetControlValueCommand  setControlValueCommand(serial, controlCollection);

void HandleMessage(const char *pszMessage)
{
  printf("Msg: %s\n", pszMessage);

  Tokenizer::TokenVector tokens = Tokenizer::tokenise(pszMessage, ' ');

  if(tokens.size())
  {
    Command *pFoundCommand = commandCollection.Find(tokens[0]);
    if(pFoundCommand)
      pFoundCommand->Process(tokens);
    else
      notSupportedCommand.Process(tokens);
  }
}

int main(int, char**)
{
  printf("HMIEmulator\n");
  printf("===========\n\n");

  printf("Initialising serial\n");
  if(!serial.IsInitialised())
  {
    printf("Serial port failed to initialise\n");
    return 1;
  }

  commandCollection.Add(&notSupportedCommand);
  commandCollection.Add(&pingCommand);
  commandCollection.Add(&responseCommand);
  commandCollection.Add(&addControlCommand);
  commandCollection.Add(&removeControlCommand);
  commandCollection.Add(&setControlValueCommand);

  printf("Processing Commands...\n");
  while(true)
  {
    // Handle serial input
    char buffer[2048];
    int n = serial.Read(buffer, 2048);
    if(n>0)
      HandleMessage(buffer);
  }
}

