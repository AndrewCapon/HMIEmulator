#pragma once

#include <thread>
#include <mutex>


#include "control_chain.h"
#include "SerialLinuxDev.h"


namespace ControlChain
{
  SerialLinuxDev          m_serial               = SerialLinuxDev("/dev/tnt1");
  std::mutex              m_mutex;
  bool                    m_bVerbose = false;
  std::thread             m_thread;

  // Actuators 
  // looks like switches and discrete have never been implemented properly look at update_assignment_value()
  #define continuousActuatorCount  4
  #define discreteActuatorCount    0
  #define momentaryActuatorCount   4
  #define switchActuatorCount      0
  #define TotalActuatorCount       (continuousActuatorCount + discreteActuatorCount + momentaryActuatorCount + switchActuatorCount)

  struct Actuator
  {
      cc_actuator_t       *pActuator = nullptr;
      cc_assignment_t     *pAssignment = nullptr;
      float               fSetValue=0.0f;
  };

  float     continuousValues[continuousActuatorCount];
  float     discreteValues[discreteActuatorCount];
  float     momentaryValues[momentaryActuatorCount];
  float     switchValues[switchActuatorCount];

  Actuator  actuators[TotalActuatorCount];
  int       nActuatorCount = 0;

  char *str16ToCstr(str16_t str16)
  {
    // simple stupid
    static char buffer[32];
    memcpy((void *)buffer, str16.text, sizeof(str16.text));
    buffer[str16.size+1]=0;
    return buffer;
  }

  char *modeToCstr(uint32_t uMode)
  {
    static char buffer [256];
    buffer[0]=0;
    if(uMode & CC_MODE_TOGGLE)
      strcat(buffer, "TOGGLE ");
    
    if(uMode & CC_MODE_TRIGGER)
      strcat(buffer, "TRIGGER ");
    
    if(uMode & CC_MODE_OPTIONS)
      strcat(buffer, "OPTIONS ");
    
    if(uMode & CC_MODE_TAP_TEMPO)
      strcat(buffer, "TAP_TEMPO ");

    if(uMode & CC_MODE_REAL)
      strcat(buffer, "REAL ");

    if(uMode & CC_MODE_INTEGER)
      strcat(buffer, "INTEGER ");

    if(uMode & CC_MODE_LOGARITHMIC)
      strcat(buffer, "LOGARITHMIC ");

    if(uMode & CC_MODE_COLOURED)
      strcat(buffer, "COLOURED ");

    if(uMode & CC_MODE_MOMENTARY)
      strcat(buffer, "MOMENTARY ");

    return buffer;
  }

  void DisplayAssignment(const char *pszLabel, cc_assignment_t *pAssignment, bool bFull)
  {
    if(bFull)
    {
      printf("[CC]  %s\n", pszLabel);
      printf("[CC]    id          : %d\n", pAssignment->id);
      printf("[CC]    actuator_id : %d\n", pAssignment->actuator_id);
      printf("[CC]    value       : %f\n", pAssignment->value);
      printf("[CC]    min         : %f\n", pAssignment->min);
      printf("[CC]    max         : %f\n", pAssignment->max);
      printf("[CC]    def         : %f\n", pAssignment->def);
      printf("[CC]    mode        : 0x%x %s\n", pAssignment->mode, modeToCstr(pAssignment->mode));
      printf("[CC]    steps       : %u\n", pAssignment->steps);
      printf("[CC]    label       : %s\n", str16ToCstr(pAssignment->label));
      printf("[CC]    unit        : %s\n", str16ToCstr(pAssignment->unit));
      printf("[CC]    list_count  : %u\n", pAssignment->list_count);
      for(int n=0; n < pAssignment->list_count; n++)
        printf("[CC]      list[%.2d]  : %s = %f\n", n, str16ToCstr(pAssignment->list_items[n]->label), pAssignment->list_items[n]->value);
      // TODO
      // uint8_t list_index;
      // option_t **list_items;
    }
    else
      printf("[CC]  %s = %d, %d, %f, %f, %f\n", pszLabel, pAssignment->id, pAssignment->actuator_id, pAssignment->min, pAssignment->max, pAssignment->value);
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
    DisplayAssignment("AssignmentCB", pAssignment, true);

    actuators[pAssignment->actuator_id].pAssignment = pAssignment;
    float fValue = SetActuatorFromAssignmentValue(pAssignment->actuator_id, pAssignment->value);
    printf("[CC]   fValue = %f\n", fValue);
  }

  void UnassignmentCB(int nActuatorId)
  {
    printf("[CC]  UnassignmentCB(%d)\n", nActuatorId); 
    actuators[nActuatorId].pAssignment = nullptr;
  }

  void UpdateCB(cc_assignment_t *pAssignment)
  {
    DisplayAssignment("UpdateCB", pAssignment, false);
  }

  void SetValueCB(cc_set_value_t *pValue)
  {
    float fValue = SetActuatorFromAssignmentValue(pValue->actuator_id, pValue->value);
    printf("[CC]  SetValueCB(%d, %d, %f) = %f\n", pValue->assignment_id, pValue->actuator_id, pValue->value, fValue);
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
        printf("[CC]  Handshake failed\n");
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
        printf("[CC]  Device disabled\n");
        break;
      }

      case CC_EV_MASTER_RESETED :
      {
        printf("[CC]  Device reset\n");
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
        printf("[CC]  Warning unhandled event %d\n", event->id);
        break;
      }
    }
  }

  void CreateActuator(cc_device_t *pDevice, char *pszName, int type, float fMax, float *pValue, uint32_t modes)
  {
    cc_actuator_config_t actuator_config;
    actuator_config.type = type;
    actuator_config.name = pszName;
    actuator_config.value = pValue;
    actuator_config.min = 0.0;
    actuator_config.max = fMax;
    actuator_config.supported_modes = modes;
    actuator_config.max_assignments = 1;

    // create and add actuator to device
    cc_actuator_t *pActuator = cc_actuator_new(&actuator_config);
    cc_device_actuator_add(pDevice, pActuator);
    actuators[nActuatorCount++].pActuator = pActuator;
  }

  void CreateActuators(void)
  {
    // create device
    const char *uri = "https://github.com/moddevices/cc-arduino-lib/tree/master/examples/TestDevice"; // TODO
    cc_device_t *pDevice = cc_device_new("TestDevice", uri);

    for (int i = 0; i < continuousActuatorCount; i++) 
    {
      char sName[40];
      sprintf(sName, "[CC]  Continuous %d", i+1);
      CreateActuator(pDevice, sName, CC_ACTUATOR_CONTINUOUS, 1000.0f, &momentaryValues[i], CC_MODE_INTEGER | CC_MODE_REAL);
    }

    for (int i = 0; i < discreteActuatorCount; i++) 
    {
      char sName[40];
      sprintf(sName, "[CC]  Discrete %d", i+1);
      CreateActuator(pDevice, sName, CC_ACTUATOR_DISCRETE, 1000.0f, &momentaryValues[i], CC_MODE_INTEGER | CC_MODE_REAL || CC_MODE_OPTIONS || CC_MODE_COLOURED);
    }

    for (int i = 0; i < momentaryActuatorCount; i++) 
    {
      char sName[40];
      sprintf(sName, "[CC]  Momentary %d", i+1);
      CreateActuator(pDevice, sName, CC_ACTUATOR_MOMENTARY, 1.0f, &momentaryValues[i], CC_MODE_TOGGLE | CC_MODE_TRIGGER | CC_MODE_OPTIONS | CC_MODE_TAP_TEMPO | CC_MODE_COLOURED | CC_MODE_MOMENTARY);
    }

    for (int i = 0; i < switchActuatorCount; i++) 
    {
      char sName[40];
      sprintf(sName, "[CC]  Switch %d", i+1);
      CreateActuator(pDevice, sName, CC_ACTUATOR_SWITCH, 1.0f, &momentaryValues[i], CC_MODE_TOGGLE | CC_MODE_TRIGGER | CC_MODE_OPTIONS | CC_MODE_TAP_TEMPO | CC_MODE_COLOURED | CC_MODE_MOMENTARY);
    }

  }

  bool IsInitialised(void)
  {
    return m_serial.IsInitialised();
  }

  void ListControls(void)
  {
    printf("[CC]  ListControls\n");
    std::lock_guard<std::mutex> lock(m_mutex);
    for(uint32_t uActuator = 0; uActuator < (TotalActuatorCount); uActuator++)
    {
      Actuator &act = actuators[uActuator];
      if(act.pAssignment)
      {
        DisplayAssignment(str16ToCstr(act.pActuator->name), act.pAssignment, true);
      }
    }
  }

  void SetControlValue(int nActuatorId, float fValue)
  {
    printf("[CC]  SetControlValue(%d, %f)\n", nActuatorId, fValue);
    std::lock_guard<std::mutex> lock(m_mutex);
    *(actuators[nActuatorId].pActuator->value) = fValue;
    cc_process();
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

