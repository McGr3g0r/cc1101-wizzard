# cc1101-wizzard
CC1101 serial console tool

This is my reimplementation of cc1101-tool.
It has handlers for  commands that can be registered in the code dynamically.

Currently working on monitor mode - monitoring for registered radio protocols (like EV1527, HCS200)

Just enter help for commands list.

To apply default radio profile:

radio proa 0

To start monitor:
monitor start

and you will get an output when received data:

monitor freq: 433920 Khz, press ctrl-c...
{ "prot":"ev1527", "code":"aa331", "btn": 1, "ts": 400352  }
{ "prot":"hcs200", "code":"54f4517d", "serial":"01b846d", "btn": 0002, "battery_low": "false", "rpt": "false", "ts": 403683  }
