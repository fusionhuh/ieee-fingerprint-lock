The code in this repository is split up into several parts:

1) **main script (main.ino) in main/**

Implements a state machine that defines the behavior of the lock system. Uses high-level functions from
the sensor, keypad, and lock libraries to achieve desired functionality instead of obfuscating clarity with
complex code.

2) **sensor library in main/src/sensor/**

Utility library that provides high-level functions for interacting with the fingerprint sensor. 

3) **keypad library in main/src/keypad/**

Utility library that provides high-level functions for interacting with the keypad.

3) **lock library in main/src/lock/**

Utility library that provides high-level functions for interacting with the lock.

For all of the utility libraries, descriptions of available functions are listed in the header files.
