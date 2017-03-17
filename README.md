# stripe-filter

stripe-filter is an application that estimates the thickness of a pipe / stick on a camera image using simple image processing algorithms.
A median filter, a convolutional filter, and thresholding is applied to the input before the edges of the object are searched in the binary image.
All of the stages of this pipeline can be parameterized and even turned off to allow experimentation and proper configuration.
The measurement results are sent to a PLC over the serial line using the Modbus RTU protocol.

stripe-filter works with Basler GigE cameras and was designed to run both on Raspbian and Windows. (Tested on Windows 7.)
Since only the Windows version can display the processed image (this is a limitation
of the Basler pylon 5 SDK and no workaround was made yet), a Windows machine is needed to do the configuration and tuning of both the camera and the filter,
and then the filter parameters can be transferred to a Raspberry Pi 2, which is installed near to the camera and directly connected to it.
The application can be run as a service on the Raspberry Pi to make it start automatically each time the Pi is powered.

Table of contents
=================

  * [Installation on Linux](#installation-on-linux)
    * [Installation as a service](#installation-as-a-service-optional)
  * [Installation on Windows](#installation-on-windows)
  * [Usage](#usage)
  * [Test application for the PLC](#test-application-for-the-plc)
    * [Input format](#input-format)
  * [License](#license)

## Installation on Linux

Make sure the following dependencies are installed:

+ Basler pylon 5 SDK (http://www.baslerweb.com/en/products/software)
+ libglib2.0-dev (version >= 2.40.0)</li>

Then:

```
$ cd grabc/
$ make
```

### Installation as a service (optional)

Copy the service script to the system directory:

```
$ sudo cp grabc.sh /etc/init.d/grabc
$ sudo chmod +x /etc/init.d/grabc
```

Check that the paths in `/etc/init.d/grabc` are correct:

+ `WORKDIR` should point to the `grabc` directory
+ `LOGFILE` should be accessible by the user running the script (given by `USER`)

Start and test your service:

```
$ sudo service grabc start
$ tail -f logs/grabc.log
$ sudo service grabc stop
```

Install the service to be run at boot-time:

```
$ sudo update-rc.d grabc defaults
```

To remove the service later, you can call:

```
$ sudo update-rc.d -f grabc remove
```

It is a good idea to turn on core dumps for the user running the service:

```
$ ulimit -c unlimited
```

This way there is more information to track down bugs in case the application crashes.

## Installation on Windows

First you need to install the Basler pylon 5 SDK for Windows,
then you should be able to build the solution.

## Usage

When running from the command line,
the path of the file containing all the parameters must be passed in as the only argument:

```
$ ./grabc settings.ini
```

Note that the settings file is monitored by the application, and whenever it changes, all the parameters are reloaded.
This way it is not needed to do a restart each time a single parameter is changed during configuration or tuning.

The parameters are the following:

Group | Parameter | Value range or type | Default | Meaning
--- | --- | --- | --- | ---
Filter | draw-overlay | true, false | false | Set to true when tuning the parameters. Filter output and settings will be written to the image.
Filter | frame-width | 0.1 - 1.0 | 0.9 | AOI width relative to the input width.
Filter | frame-height | 0.1 - 1.0 | 0.9 | AOI height relative to the input height.
Filter | frame-border | 0.1 - 1.0 | 1.0 | The AOI can be restricted further with this parameter. It is relative to `frame-width` and selects the region on the left and right hand side of the AOI within which the search for the edge of the object will be performed. (For example, in case of `frame-border` == 1, the algorithm will look for the right edge of the object within the full AOI width. In case of `frame-border` == 0.5, it will look within the right half of the AOI only. It is assumed that the object is centered in the input image.)
Filter | median-kernel-size | 0,3,5 | 0 | The size of the applied median filter.
Filter | conv-kernel | 0,9, or 25 integer values separated by space | | The integer coefficients of the convolutional filter. The normalization factor is given by `conv-kernel-sum`.
Filter | conv-kernel-sum | >0 | | The integer normalization factor.
Filter | threshold | 0-255 | 0 | Threshold for the binary thresholding. 0 means thresholding is turned off.
Filter | stripe-height | odd integer between 1 and 11 | 1 | The number of horizontal lines to scan for the edge of the object. If greater than 1, the final position will be the median of the positions in the individual lines.
Filter | histogram-region | left, right, overlay, none | none | Draw histogram for the selected region: left or right region of the AOI, the full AOI, or none, respectively. The histogram will be visible only when `draw-overlay` is true.
Filter | dark-object | true, false | false | Determines if the search for the edge of the object will look for the first black (true) or white (false) pixel.
Serial | device-file | Path | /dev/ttyUSB0 | File path that identifies the serial port. On Windows, it can be `\\\\.\\COM1`, `\\\\.\\COM2`, etc. On Linux, it is usually `/dev/ttyS0` , `/dev/ttyS1`, etc. with a built-in serial port and `/dev/ttyUSB0`, `/dev/ttyUSB1`, etc. when using a USB to RS-232 converter.
Serial | baud-rate | 50-115200 | 19200 | Baud rate.
Serial | parity | even, odd, none | even | Parity.
Serial | data-bits | 7-8 | 8 | Number of data bits.
Serial | stop-bits | 1-2 | 1 | Number of stop bits.
Application | output | none, stdout, serial | none | Send the measurement result to NULL, the standard output, or the serial line, respectively.
Application | modbus-slave-address | 0-247 | 1 | Address of the target Modbus device (PLC).
Application | modbus-register-address | 0-9999 | 4000 | Address of the Modbus register receiving the result. The next register will receive the program state. State values and their meaning can be found in the next table. The mapping of Modbus register addresses to the actual PLC register addresses can be found in the documentation of the PLC.

State values sent to the PLC:

Value | Meaning
--- | ---
1 | Measurement was successful.
2 | The object cannot be found in the image.
3 | Camera is disconnected.

## Test application for the PLC

The project contains `modbus_player`, an application that sends Modbus RTU messages over the serial line according to its standard input.
It is useful for developing and testing the PLC program that will communicate with the stripe-filter without actually having a camera.

```
$ ./modbus_player settings.ini < test_input.txt
```

Serial line settings are set as defined by the `[Serial]` section of `settings.ini`. (The same file can be used as for `grabc`.)
The meaning of the parameters can be found in the *Usage* section above.

### Input format

Each line of the input contains parameters for a single Modbus message. The parameters are the following:

Parameter | Meaning
--- | ---
DELAY_MS | Wait this many milliseconds before sending the message.
FUNCTION_CODE | Modbus function code. Supported functions are: 6 - preset single register; 16 - preset multiple registers.
DEVICE_ADDRESS | Modbus device address. (The default for the Xinje PLC is: 1.)
REGISTER_ADDRESS | Address of the Modbus register to write. The mapping of Modbus register addresses to actual PLC register addresses can be found in the documentation of the PLC.
NUMBER_OF_VALUES | Number of values to write. (This is only required if multiple registers are written. (In that case, the register address is the address of the first register.)
VALUES | Value(s) to write to the register(s).

Every number can be given in decimal or hexadecimal form (with a `0x` prefix).

## License

MIT license, see [here](LICENSE).
