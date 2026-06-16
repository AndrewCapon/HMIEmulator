# HMI and Control Chain emulator for linux

Emulator for testing mod-ui and mod-host with Control Chain and HMI.

The code has been knocked together quickly, there is very little error checking.

Basically allows you to run the Mod web interface and make it think it as a HMI conncted and a control chain device.

The functionality is very simple:

1. Allows you to start mod-ui on linux and make it think it has a HMI an CC dvice attached.
2. Allows you to see the messages sent to HMI/CC concerning controls.
3. Allows you to set control values as if they were coming from the HMI or CC.

The Control Chain device device is currently is set up for 4 momentary switches and 4 continuous controls.

I'm using the Mod Dwarf device description for the HMI.

## Prerequisites

You need tty0tty installed: https://github.com/freemed/tty0tty

It uses two pairs of serial devices: `/dev/tnt0 <-> /dev/tnt1` for ControlChain and `/dev/tnt2 <-> /dev/tnt3` for HMI.


## Setup for Control chain

The code listens on /dev/tnt1 for Control Change serial messages.

`controlchaind` should be started use `/dev/tnt0` for example : `LIBCONTROLCHAIN_DEBUG=2 ./controlchaind /dev/tnt0 -f` 


## Setup for HMI

the code listens on /dev/tnt3 for HMI serial messages.

`mod-ui` needs two env vars set:
`MOD_HMI_SERIAL_PORT=/dev/tnt2`
`MOD_HARDWARE_DESC_FILE=./mod-hardware-descriptor.json`

`MOD_HARDWARE_DESC_FILE` is the location for the json file to use for HMI setup to emulate, one is included here for the Mod Dwarf.

## Usage

Console commands (need newline, n=int, f=float):
  <empty> : Display help.
  hv      : HMI verbose switch (unsupported commands and pings).
  hl      : HMI List controls.
  hs n f  : HMI Set control n to f.

  cl      : CC  List controls.
  cs n f  : CC  Set actuator n to f.

  ct n    : CC  Toggle actuator n from 1 to 0.

## cc-slave

Using version 0.6 which is the same as the mod footswitch.

You need to edit `utils.c` to fix what looks to be pretty dodgy code to me, I could be wrong though!
The code is still quite dodgy but won't crash till you get to OPTIONS_MAX_ITEMS where it was crashing if you had more than one list based control mapped.

```
option_t **options_list_create(uint8_t items_count)
{
    if (items_count == 0)
        return 0;

    static option_t all_items[OPTIONS_MAX_ITEMS];
    option_t **list = malloc((items_count + 1) * sizeof(option_t *));

    if (list)
    {
        for (int i = 0, j = 0; (i < OPTIONS_MAX_ITEMS) && (j < items_count); i++) 
        {
            if (all_items[i].label.size == 0)
            {
                all_items[i].label.size = 0xFF;
                list[j++] = &all_items[i];
            }
        }

        list[items_count] = 0;
    }

    return list;
}
```


