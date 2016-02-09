# wdb40
Wireless Network Manager

![wd40 can](http://wd40.com/img/wd-40-smart-straw-voc-12oz_straw_up_right.png)
Making moving between networks super easy!

# wdb40tool

The C++ binary that does the following:
* Read current network configuration
* Scan for currently available networks
* Enable and disable configured networks based on availability

## Command Line Options

The wdb40tool binary is meant to be called with an argument outlining the action to be taken:
```
wdb40tool <action>
```

Available actions:
* init
* read
* wait
* waitWwan
* scan
* connect
* disable

## Actions

### `init`
```
wdb40tool init
```

Uses `libuci` to read the networks outlined in `/etc/config/wireless`, prints network list to `/tmp/wifi_config`. Networks are listed in `/etc/config/wireless` in order of priority.

Restarts the wifi adapter so the changes take effect.

### `read`
```
wdb40tool read
```

Uses `libuci` to read the networks outlined in `/etc/config/wireless`, prints network list to `/tmp/wifi_config`. Networks are listed in `/etc/config/wireless` in order of priority.


### `wait`
```
wdb40tool [-t <seconds>] wait
```

Uses `libubus` to poll **network wireless device** status until the device is up or the poll times out.

Default timeout is 10 seconds, the `-t <seconds>` option can be used to specify a longer or shorter timeout.


### `waitWwan`
```
wdb40tool [-t <seconds>] waitWwan
```

Uses `libubus` to poll **network wwan interface** (client connection to network) status until the interface is up or the poll times out.

Default timeout is 10 seconds, the `-t <seconds>` option can be used to specify a longer or shorter timeout.


### `scan`
```
wdb40tool scan
```

Uses `libiwinfo` to scan for currently available wifi networks, prints list to `/tmp/wifi_scan`.

Then it checks list of available networks against the configured networks, creates a list of matches. The list of matches is printed to `/tmp/wifi_match`


### `connect`
```
wdb40tool [-f] connect
```

Uses `libuci` to enable a configured network that is currently available. 

**No `-f` Option**

If the network is currently enabled, the wifi adapter will not be reset.
If the network is to change state from disabled to enabled, the wifi adapter will be reset.

**`-f` Option Specified**

The wifi adapter will be reset not matter what (interrupts any existing connections)


### `disable`
```
wdb40tool [-f] connect
```

Uses `libuci` to disable all client networks and enable the AP network, if any.

