#include "Hmi.h"
#include "ControlChain.h"

#include <thread>
#include <mutex>


Hmi           hmi;

void DisplayHelp(void)
{
  printf("Console commands (need newline, n=int, f=float):\n");
  printf("  h      : Display help.\n");
  printf("  hv     : HMI verbose switch (unsupported commands and pings).\n");
  printf("  hl     : HMI List controls.\n");
  printf("  hs n f : HMI Set control n to f.\n\n");
  printf("  cl     : CC  List controls.\n");
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

      if(sCommand[0] == 'h')
      {
        if(sCommand.length() == 2)
          DisplayHelp();
        else if (sCommand.length() == 3)
        {
          // Hmi commands
          switch (sCommand[1])
          {
            // list controls
            case 'l' :
            {
              hmi.ListControls();
              break;
            }

            // set Control
            case 's' :
            {
              if(uTokens == 3)
              {
                int   nHwId  = tokens[1];
                float fValue = tokens[2];
                hmi.SetControlValue(nHwId, fValue);
              }
              break;
            }

            // verbose
            case 'v' :
            {
              hmi.ToggleVerbose();
              break;
            }
          }
        }
      }
      else if (sCommand[0] == 'c')
      {
        // CC commands
        switch (sCommand[1])
        {
          // list controls
          case 'l' :
          {
            ControlChain::ListControls();
            break;
          }

          // set Control
          case 's' :
          {
            // if(uTokens == 3)
            // {
            //   int   nHwId  = tokens[1];
            //   float fValue = tokens[2];
            //   hmi.SetControlValue(nHwId, fValue);
            // }
            break;
          }
        }
      }
    }
  }
}


int main(int, char**)
{
  printf("HMI CC Emulator\n");
  printf("===============\n\n");
  DisplayHelp();

  if(!hmi.IsInitialised())
  {
    printf("HMI failed to initialise\n");
    return 1;
  }

  ControlChain::Init();
  if(!ControlChain::IsInitialised())
  {
    printf("Control Chain failed to initialise\n");
    return 1;
  }

  //std::thread hmiThread(hmi.HandleHMISerial);
  std::thread uiThread(HandleUI);

  //hmiThread.join();
  uiThread.join();
}

