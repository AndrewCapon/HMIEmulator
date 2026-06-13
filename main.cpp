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

#include "mod-protocol.h"

#include "SerialTNT3.h"
#include "Command.h"
#include "CommandCollection.h"
#include "ControlCollection.h"
#include "AddCommand.h"
#include "NotSupportedCommand.h"
#include "PingCommand.h"

SerialTNT3 serial;

CommandCollection     commandCollection;
ControlCollection     controlCollection;

NotSupportedCommand   notSupportedCommand(serial);
AddCommand            addCommand(serial, controlCollection);
PingCommand           pingCommand(serial);

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
  commandCollection.Add(&addCommand);
  commandCollection.Add(&pingCommand);

  while(true)
  {
    // Handle serial input
    char buffer[2048];
    int n = serial.Read(buffer, 2048);
    if(n>0)
      HandleMessage(buffer);
  }
}


// // This struct is used on callbacks argument
// typedef struct PROTO_T {
//     char **list;
//     uint32_t list_count;
//     char *response;
//     uint32_t response_size;
// } proto_t;

// // This struct must be used to pass a message to protocol parser
// typedef struct MSG_T {
//     int sender_id;
//     char *data;
//     uint32_t data_size;
// } msg_t;

// 
// uint8_t copy_command(char *buffer, const char *command)
// {
//     uint8_t i = 0;
//     const char *cmd = command;

//     while (*cmd && (*cmd != '%' && *cmd != '.'))
//     {
//         buffer[i++] = *cmd;
//         cmd++;
//     }

//     return i;
// }

// static char* reverse(char* str, uint32_t str_len)
// {
//     char *end = str + (str_len - 1);
//     char *start = str, tmp;

//     while (start < end)
//     {
//         tmp = *end;
//         *end = *start;
//         *start = tmp;

//         start++;
//         end--;
//     }

//     return str;
// }

// uint32_t int_to_str(int32_t num, char *string, uint32_t string_size, uint8_t zero_leading)
// {
//     char *pstr = string;
//     uint8_t signal = 0;
//     uint32_t str_len;

//     if (!string) return 0;

//     // exception case: number is zero
//     if (num == 0)
//     {
//         *pstr++ = '0';
//         if (zero_leading) zero_leading--;
//     }

//     // need minus signal?
//     if (num < 0)
//     {
//         num = -num;
//         signal = 1;
//         string_size--;
//     }

//     // composes the string
//     while (num)
//     {
//         *pstr++ = (num % 10) + '0';
//         num /= 10;

//         if (--string_size == 0) break;
//         if (zero_leading) zero_leading--;
//     }

//     // checks buffer size
//     if (string_size == 0)
//     {
//         *string = 0;
//         return 0;
//     }

//     // fills the zeros leading
//     while (zero_leading--) *pstr++ = '0';

//     // put the minus if necessary
//     if (signal) *pstr++ = '-';
//     *pstr = 0;

//     // invert the string characters
//     str_len = (pstr - string);
//     reverse(string, str_len);

//     return str_len;
// }

// void protocol_response(const char *response, proto_t *proto)
// {
//     static char response_buffer[32];

//     proto->response = response_buffer;

//     proto->response_size = strlen(response);
//     if (proto->response_size >= sizeof(response_buffer))
//         proto->response_size = sizeof(response_buffer) - 1;

//     strncpy(response_buffer, response, sizeof(response_buffer)-1);
//     response_buffer[proto->response_size] = 0;
// }

// void protocol_send_response(const char *response, const uint8_t value ,proto_t *proto)
// {
//     char buffer[20];
//     uint8_t i = 0;
//     memset(buffer, 0, sizeof buffer);

//     i = copy_command(buffer, response); 
    
//     // insert the value on buffer
//     int_to_str(value, &buffer[i], sizeof(buffer) - i, 0);

//     protocol_response(&buffer[0], proto);
//     printf("  Response: %s\n", buffer);
//     write(nSerialPort, buffer, 20);
// }
// char** strarr_split(char *str, const char token)
// {
//     uint32_t count;
//     char *pstr, **list = NULL;
//     uint8_t quote = 0;

//     if (!str) return list;

//     // count the tokens
//     pstr = str;
//     count = 1;
//     while (*pstr)
//     {
//         if (*pstr == token && quote == 0)
//         {
//             count++;
//         }
// #ifdef ENABLE_QUOTATION_MARKS
//         if (*pstr == '"')
//         {
//             if (quote == 0) quote = 1;
//             else
//             {
//                 if (*(pstr+1) == '"') pstr++;
//                 else quote = 0;
//             }
//         }
// #endif
//         pstr++;
//     }

//     // allocates memory to list
//     size_t size = (count + 1) * sizeof(char *);
//     list = (char **)malloc(size);
//     if (!list) return NULL;

//     // fill the list pointers
//     pstr = str;
//     list[0] = pstr;
//     count = 0;
//     while (*pstr)
//     {
//         if (*pstr == token && quote == 0)
//         {
//             *pstr = '\0';
//             list[++count] = pstr + 1;
//         }
// #ifdef ENABLE_QUOTATION_MARKS
//         if (*pstr == '"')
//         {
//             if (quote == 0) quote = 1;
//             else
//             {
//                 if (*(pstr+1) == '"') pstr++;
//                 else quote = 0;
//             }
//         }
// #endif
//         pstr++;
//     }

//     list[++count] = NULL;

// #ifdef ENABLE_QUOTATION_MARKS
//     count = 0;
//     while (list[count]) parse_quote(list[count++]);
// #endif

//     return list;
// }

// uint32_t strarr_length(char** const str_array)
// {
//     uint32_t count = 0;

//     if (str_array) while (str_array[count]) count++;
//     return count;
// }

// void HandleMessage(uint8_t *pszMessage)
// {
//   printf("Msg: %s\n", pszMessage);

//   proto_t proto;

//   proto.list = strarr_split((char *)pszMessage, ' ');
//   proto.list_count = strarr_length(proto.list);
//   proto.response = NULL;

//   // TODO: check invalid argumets (wildcards)

//   if (proto.list_count == 0) 
//   {
//       free(proto.list);
//       return;
//   }

//   protocol_send_response(CMD_RESPONSE, 0, &proto);
// }

