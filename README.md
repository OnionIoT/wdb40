# wdb40
Wireless Network Manager

![wd40 can](http://wd40.com/img/wd-40-smart-straw-voc-12oz_straw_up_right.png)

Making moving between networks super easy!

The wdb40 tool consists of several parts:
* `wdb40tool`, a C++ binary
* `wdb40`, a bash script
* `wifisetup`, a script to manage saved networks on the Omega

The `wdb40tool` does all of the heavy lifting in changing configurations, reading info, and interacting with the hardware. 

The `wdb40` script runs chunks of the C++ binary in sequences. This is the program with which users will interact.

The `wifisetup` script interacts with `/etc/config/wireless` to add, edit, or remove configured networks to the Omega.

All are described below.




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




# wdb40 Script

Unfortunately, wdb40tool, the C++ cannot do everything itself, the `wdb40` script was written to accomodate this issue.

## Use

The script runs two different sequences:


### The Boot Sequence

Meant to run when the Omega boots to ensure it either:
* Connects to an available network that is configured in `/etc/config/wireless`
* Disables all STA connections and enables the AP so the Omega can be configured

**Command**
The wdb40 installation will enable this sequence being run at boot (by adding a `/etc/init.d/wdb40` file).

To run the sequence manually:
```
wdb40 -boot
```

**Sequence:**

* `wdb40tool -t 60 wait`
  * Wait for wireless device to be up (60 second timeout)
* `wdb40tool init`
  * Read configured networks, disable all STA networks, reset the wifi adapter
* `wdb40tool wait`
  * Wait for wireless device to be up (default timeout)
* `wdb40tool scan`
  * Scan available networks, check against configured networks for a match
* `wdb40tool -f connect`
  * Enable the matched network, forcing the wifi adapter to restart
* `wdb40tool -t 20 waitWwan`
  * Ensure connection to the matched network is successful (20 second timeout)



### The Regular Sequence

Meant to be run by the user when the available wifi networks might have changed; the Omega was moved to another location, a wifi network went down, etc.

**Command**
```
wdb40
```

**Sequence:**

* `wdb40tool wait`
  * Ensure the wireless device is up
* `wdb40tool read`
  * Read the configured networks
* `wdb40tool scan`
  * Scan for available networks, check against configured networks for a match
* `wdb40tool connect`
  * Enable the matched network
* `wdb40tool -t 20 waitWwan`
  * Ensure connection to the matched network is successful (20 second timeout)


### Connection Failure

If the final command, `wdb40tool -t 20 waitWwan`, in both the Regular and Boot Sequences times out meaning the connection was unsuccessful, the following is done:

* `wdb40tool -t 20 disable`
  * Disable all client wifi connections and enable the Omega's AP network
* Attempt the connection again:
  * `wdb40tool connect`
  * `wdb40tool -t 20 waitWwan`



# wifisetup Script

This script modifies the contents of the `/etc/config/wireless` file that contains all of the information for configured wifi networks on the Omega.

## Actions

### Add WiFi Network
Add a new WiFi network to the Omega's settings
```
wifisetup add -ssid <ssid> -encr <encryption type> -password <password>
```

### Edit a Network
Edit the information of a configured WiFi network
```
wifisetup edit -ssid <ssid> -encr <encryption type> -password <password>
```

### Remove a Network
Remove an existing WiFi network from the Omega's settings
```
wifisetup remove -ssid <ssid>
```

### Change a Network's priority
Move a WiFi network up or down in the priority list when attempting to connect
```
wifisetup priority -ssid <ssid> -move <up|down>
```

* `up`:     increase the priority
* `down`:   decrease the priority

### List All Configured Networks
Display a JSON-formatted list of all configured networks
```
wifisetup list
```

### Display a Specific Network's Information
Display a JSON-formatted table of all info for specified network
```
wifisetup info -ssid <ssid>
```

## Additional Info

### Accepted Encryption Types

The following are acceptable encryption type inputs for the commands above:
* wpa2
* wpa
* wep
* none

### Setting up an Access Point

Any of the commands above can be modified for an AP by adding the `-ap` option

