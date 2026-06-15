# HMI and Control Chain emulator for linux

Emulator for testing mod-ui and mod-host with Control Chain and HMI.

You need tty0tty installed: https://github.com/freemed/tty0tty

It uses two pairs of serial devices: `/dev/tnt0 <-> /dev/tnt1` for ControlChain and `/dev/tnt2 <-> /dev/tnt3` for HMI.

## Setup for Control chain

code listens on /dev/tnt1 for Control Change serial

controlchaind should be started use `/dev/tnt0` for example : `LIBCONTROLCHAIN_DEBUG=2 ./controlchaind /dev/tnt0 -f` 
The code currently is set up for 4 momentary switches and variable controls.


## Setup for HMI

code listens on /dev/tnt3 for HMI serial

mod-ui needs two env vars set:
`MOD_HMI_SERIAL_PORT=/dev/tnt2`
`MOD_HARDWARE_DESC_FILE=./mod-hardware-descriptor.json`

`MOD_HARDWARE_DESC_FILE` is the location for the json file to use for HMI setup to emulate, one is included here for the Mod Dwarf.

## cc-slave

Using version 0.6 which is the same as the mod footswitch.

You need to edit `utils.c` to fix what looks to be pretty dodgy code to me, I could be wrong though!
The code is still quite dodgy but won't crash till you get to OPTIONS_MAX_ITEMS where it was crashing if you had more than on list based control mapped.

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


