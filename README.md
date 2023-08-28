# cc1101-wizzard
CC1101 serial console tool

This is my reimplementation of cc1101-tool, and it is a work in progress.
It has handlers for commands that can be registered in the code dynamically.

Currently working on monitor mode - monitoring for registered radio protocols (like EV1527, HCS200, Somfy RTS Keytis)

Just enter help for commands list.

To apply default radio profile:
```
radio proa 0
```
To start monitor:
```
monitor start
```

and you will get an output when received data:
```
monitor freq: 433920 Khz, press ctrl-c...

{ "prot":"ev1527", "code":"aa331", "btn": 1, "ts": 400352  }

{ "prot":"hcs200", "code":"54xx51xd", "serial":"x1bx4xd", "btn": 0002, "battery_low": "false", "rpt": "false", "ts": 403683  }
```

Supported commands:

radio
```
- sfreq : set frequency, use Khz, for example 433920'
- smod : set modulation, 0 to 4 or 2fsk, gfsk, ook, 4fsk, msk
- sdev : set freqency deviation in Hz. Value from 158 to 380850. Default is 47600 Hz.
- schan : set the channel number from 0 to 255. Default channel is 0
- schsp : set channel spacing <hz>, it is multiplied by the channel number and added to the base frequency in Hz. Value from 25390 to 405450. Default is 199950 Hz.
- srxbw : set the receive bandwidth in Hz. Value from 58030 to 812500. Default is 812500 Hz
- sdrate : set the data rate <Baud>. Value from 20 to 1621830. Default is 99970 Baud
- spa : set rf transmission power. Values depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is 12
- ssm : set sync mode 0 - no preamble/sync,
          1 - 16bit sync word, 2-16/16bits sync word
          3 - 30/32bits sync word, 4-No preamble/sync, carrier-sense above threshold
          5 - 15/16bits + carrier-sense above threshold
          6 - 16/16bits + carrier-sense above threshold
          7 - 30/32 + carrier-sense above threshold.
- ssw : set sync word <high> <low>. Must be the same for the transmitter and receiver. Default is 211,145
- sac : set address check <chk>: 0 = No address check. 1 = Address check, no broadcast. 2 = Address check and 0 (0x00) broadcast. 3 = Address check and 0 (0x00) and 255 (0xFF) broadcas.
- sad : set address <hex address>, address used for packet filtration. Optional broadcast addresses are 0x00 0xFF
- swd : set white data <whitening>, Turn data whitening on / off. 0 = Whitening off. 1 = Whitening on.
- spf : set packet format, 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.        2 = Random TX mode; see
- splm : set packet length mode, 0 = Fixed packet length mode. 1 = Variable packet length mode. 2 = Infinite packet length mode. 3 = Reserved.
- spl : set packet length <len>, set packet length when fixed packed lenght mode is set. If variable packet length mode is used, this value indicates the maximum packet length allowed.
- sc : switches on/of CRC calculation and check. 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
- sca : Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.
- sdfo :  Disable digital DC blocking filter before demodulator. Only for data rates �≤ 250 kBaud The recommended IF frequency changes when the DC blocking is disabled. 1 = Disable (cu.
- sma :  Enables Manchester encoding/decoding. 0 = Disable. 1 = Enable.
- sfe :  Enables Forward Error Correction (FEC) with interleaving for packet payload (Only supported for fixed packet length mode. 0 = Disable. 1 = Enable.
- spr :  Sets the minimum number of preamble bytes to be transmitted. Values: 0 : 2, 1 : 3, 2 : 4, 3 : 6, 4 : 8, 5 : 12, 6 : 16, 7 : 24.
- spq :  Preamble quality estimator threshold.
- sas :  Set append status. When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.
- rssi : Display quality information about last received frames over RF.
- scan : Scan frequency range <start> <stop> Khz for the highest signal.
- prol : list predefined radio profiels
- proa : apply profile by idx or name
- pulc : get pulses count in buffer
- pulf : flush puses buffer
- puls : show puses buffer
- pulg : grab pulses <offset><min_time_us><max_space_time_us>
- pulp : play pulses <offset><pulses>
- pula : add pulses <pulses> ..
- puao : add pulses <offset> <pulses> ...
- jam : jam current channel <time millis> 0 - until ctrl-c
```

monitor
```
Monitoring protocol utility, use: 'monitor help'
- start : starts monitoring, stops with ctrl-c
- prot : list registered protocols
```
opensesame
```
- prot : list registered protocols
- somfy : somfy iterated frames flood
```



If you like my work and like to support me ;) --> [Paypal](https://www.paypal.com/donate/?business=CE4764JYLBHK6&no_recurring=0&currency_code=USD).
