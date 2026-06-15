#pragma once

// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <string.h>
// #include <fcntl.h> 
// #include <errno.h> 
// #include <termios.h> 
// #include <sys/ioctl.h>
// #include <stdint.h>

#include <thread>
#include <mutex>

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

#include <assert.h>
#include "control_chain.h"
#include "SerialLinuxDev.h"


namespace ControlChain
{
  SerialLinuxDev          m_serial               = SerialLinuxDev("/dev/tnt1");
  std::mutex              m_mutex;
  bool                    m_bVerbose = false;
  std::thread             m_thread;

    // Actuators
  #define buttonPortsCount 8
  #define variablePortsCount 8

  struct Actuator
  {
      cc_actuator_t       *pActuator = nullptr;
      cc_assignment_t     *pAssignment = nullptr;
      float               fSetValue=0.0f;
  };

  float     buttonValues[buttonPortsCount];
  float     variableValues[variablePortsCount];
  Actuator  actuators[buttonPortsCount+variablePortsCount];
  int       nActuatorCount = 0;

  char *str16ToCstr(str16_t str16)
  {
    // simple stupid
    static char buffer[32];
    memcpy((void *)buffer, str16.text, sizeof(str16.text));
    buffer[str16.size+1]=0;
    return buffer;
  }

  void DisplayAssignment(const char *pszLabel, cc_assignment_t *pAssignment)
  {
    printf("[CC] %s\n", pszLabel);
    printf("[CC]   id          : %d\n", pAssignment->id);
    printf("[CC]   actuator_id : %d\n", pAssignment->actuator_id);
    printf("[CC]   value       : %f\n", pAssignment->value);
    printf("[CC]   min         : %f\n", pAssignment->min);
    printf("[CC]   max         : %f\n", pAssignment->max);
    printf("[CC]   def         : %f\n", pAssignment->def);
    printf("[CC]   mode        : %x\n", pAssignment->mode);
    printf("[CC]   steps       : %u\n", pAssignment->steps);
    printf("[CC]   list_count  : %u\n", pAssignment->steps);
  #ifndef CC_STRING_NOT_SUPPORTED
    printf("[CC]   label       : %s\n", str16ToCstr(pAssignment->label));
    printf("[CC]   unit        : %s\n", str16ToCstr(pAssignment->unit));
    
    // TODO
      // uint8_t list_index;
      // option_t **list_items;
      // str16_t label, unit;
  #endif
  }

  float SetActuatorFromAssignmentValue(int nActuatorId, float fAssignmentValue)
  {
    Actuator &act = actuators[nActuatorId];

    float fAssignmentRange = (act.pAssignment->max - act.pAssignment->min);
    float fAssignmentNormal = (fAssignmentValue - act.pAssignment->min) / fAssignmentRange;

    float fActuatorRange = (act.pActuator->max - act.pActuator->min);

    float fValue = act.pActuator->min + (fActuatorRange*fAssignmentNormal);

    act.fSetValue = act.pActuator->min + (fActuatorRange*fAssignmentNormal);

    return act.fSetValue;
  } 


  // Callbacks
  void AssignmentCB(cc_assignment_t *pAssignment)
  {
    DisplayAssignment("AssignmentCB", pAssignment);

    actuators[pAssignment->actuator_id].pAssignment = pAssignment;
    float fValue = SetActuatorFromAssignmentValue(pAssignment->actuator_id, pAssignment->value);
    printf("[CC]   fValue = %f\n", fValue);
  }

  void UnassignmentCB(int nActuatorId)
  {
    actuators[nActuatorId].pAssignment = nullptr;
  }

  void UpdateCB(cc_assignment_t *pAssignment)
  {
    DisplayAssignment("UpdateCB", pAssignment);
  }

  void SetValueCB(cc_set_value_t *pValue)
  {
    float fValue = SetActuatorFromAssignmentValue(pValue->actuator_id, pValue->value);
    printf("[CC] SetValueCB (%d, %d, %f) = %f\n", pValue->assignment_id, pValue->actuator_id, pValue->value, fValue);
  }


  void responseCB(void *arg) 
  {
    cc_data_t *response = (cc_data_t *) arg;
    int nBytesWritten = m_serial.Write(reinterpret_cast<char *>(response->data), response->size);
    if(nBytesWritten != response->size)
      printf("Error Should have written %d bytes to serial, only wrote %d\n", response->size, nBytesWritten);
  }

  void eventsCB(void *arg) 
  {
    cc_event_t *event = (cc_event_t *) arg;

    switch(event->id)
    {
      case CC_EV_HANDSHAKE_FAILED :
      {
        printf("[CC] Handshake failed\n");
        break;
      }

      case CC_EV_ASSIGNMENT :
      {
        cc_assignment_t *pAssignment = (cc_assignment_t *) event->data;
        AssignmentCB(pAssignment);
        break;
      }

      case CC_EV_UNASSIGNMENT:
      {
        int *pActuatorId = (int *)event->data;
        UnassignmentCB(*pActuatorId);
        break;
      }

      case CC_EV_UPDATE :
      {
        cc_assignment_t *pAssignment = (cc_assignment_t *) event->data;
        UpdateCB(pAssignment);
        break;
      }

      case CC_EV_DEVICE_DISABLED :
      {
        printf("[CC] Device disabled\n");
        break;
      }

      case CC_EV_MASTER_RESETED :
      {
        printf("[CC] Device reset\n");
        break;
      }

      case CC_CMD_SET_VALUE :
      {
        cc_set_value_t *pValue = (cc_set_value_t *) event->data;
        SetValueCB(pValue);
        break;
      }

      default:
      {
        printf("[CC] Warning unhandled event %d\n", event->id);
        break;
      }
    }
  }

  void CreateActuators(void)
  {
    // create device
    const char *uri = "https://github.com/moddevices/cc-arduino-lib/tree/master/examples/TestDevice"; // TODO
    cc_device_t *device = cc_device_new("TestDevice", uri);

      // configure buttons 
    for (int i = 0; i < buttonPortsCount; i++) 
    {
      char sName[40];
      sprintf(sName, "Button %d", i+1);
      //printf("[CC]   Creating Actuator: %s\n", sName);
      cc_actuator_config_t actuator_config;
      actuator_config.type = CC_ACTUATOR_MOMENTARY;
      actuator_config.name = sName;
      actuator_config.value = &buttonValues[i];
      actuator_config.min = 0.0;
      actuator_config.max = 1.0;
      actuator_config.supported_modes = CC_MODE_TOGGLE | CC_MODE_TRIGGER;
      actuator_config.max_assignments = 1;

      // create and add actuator to device
      cc_actuator_t *actuator = cc_actuator_new(&actuator_config);
      cc_device_actuator_add(device, actuator);
      actuators[nActuatorCount++].pActuator = actuator;
    }

    for (int i = 0; i < variablePortsCount; i++) 
    {
      char sName[40];
      sprintf(sName, "Variable %d", i+1);
      //printf("[CC]   Creating Actuator: %s\n", sName);
      cc_actuator_config_t actuator_config;
      actuator_config.type = CC_ACTUATOR_CONTINUOUS;
      actuator_config.name = sName;
      actuator_config.value = &variableValues[i];
      actuator_config.min = 0.0;
      actuator_config.max = 1023.0;
      actuator_config.supported_modes = CC_MODE_REAL | CC_MODE_INTEGER;
      actuator_config.max_assignments = 1;

      // create and add actuator to device
      cc_actuator_t *actuator = cc_actuator_new(&actuator_config);
      cc_device_actuator_add(device, actuator);
      actuators[nActuatorCount++].pActuator = actuator;
    }
  }

  bool IsInitialised(void)
  {
    return m_serial.IsInitialised();
  }

  void ListControls(void)
  {
    for(uint32_t uActuator = 0; uActuator < (buttonPortsCount+variablePortsCount); uActuator++)
    {
      Actuator &act = actuators[uActuator];
      if(act.pAssignment)
      {
        DisplayAssignment(str16ToCstr(act.pActuator->name), act.pAssignment);
      }
    }
  }

  void ThreadHandler(void)
  {
    char buffer[2048];

    // discard any buffered serial data
    while(2048 == m_serial.Read(buffer, 2048))
      ;

    while(true)
    {
      // Handle serial input
      int n = m_serial.Read(buffer, 2048);
      if(n>0)
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        cc_data_t data = {reinterpret_cast<uint8_t *>(&buffer[0]), static_cast<uint32_t>(n)};
        if (cc_parse(&data) < 0)
          printf("[CC] error: cc_parse() failed\n");
      }
      cc_process();
    }
  }

  void Init(void)
  {
    m_thread = std::thread{&ThreadHandler};
    cc_init(ControlChain::responseCB, ControlChain::eventsCB);

    CreateActuators();
  }

};

