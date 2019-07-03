.. _google-iot-device-sdk-embedded-c: https://github.com/GoogleCloudPlatform/iot-device-sdk-embedded-c

1st draft, Google IOT Device Sample
###################################

Overview
********

This sample application connects to Google Cloud Platform IoT Core's
MQTT service. This solution demonstrates the usage of Google IoT
Device SDK from inside a "full stack" Zephyr application. The solution
currently (Jul 3rd 2019) is in a draft version. This means there are
many values in it but at the same time work has to be done to enhance
Zephyr compatibility, user friendliness and a major security bug have
to be addressed as well. The values and what's missing are enumerated
in the not exhaustive lists below:

Values:

- The Google IoT Connectivity Zephyr sample app is freed of doing
  Google specific tasks. The google-iot-device-sdk-embedded-c_ library
  provides all Google specifics, such as MQTT comm, JWT composition
  (authentication).
- Higher level MQTT abstraction (compared to the
  samples/net/google_iot_mqtt version).
- Uses the Google IoT Connectivity library
  (google-iot-device-sdk-embedded-c_) as a `west` module.
- Can be built the same way as any other Zephyr sample application.
- Uses Zephyr's mbedTLS library.
- The BSP glue code between Zephyr and Google's Device SDK is now
  located under samples/net/google_iot_device/src/bsp. This lets the
  Zephyr community maintain the glue code to Google Device SDK
  directly in the Zephyr repo without touching external codebase.

Missing:

- Security issue: server cert verification is disabled (the
  IOTC_DISABLE_CERTVERIFY compiler flag is defined), this must be
  re-enabled. Was disable because cert verification fails for some
  reason. Some investigation was done but the problem isn't solved
  yet.
- Only tested on `native_posix` board. This is where Google IoT looks
  for value from Zephyr community: the board support.
- Sample app loads `ec_private.pem` private key from file in CWD using
  `fopen`.
- BSP IO FS (the file system glue code) has to be updated using Zephyr
  file system API.
- Use safer entropy source. There is runtime warning message:
  "WARNING: Using a test - not safe - entropy source".
- The sample code should demonstrate more deeper integration of the
  Google IoT Device SDK in Zephyr RTOS. E.g. running Zephyr task in
  parallel to the Google IoT connection.
- Memory review, optimization required in file:
  `src/bsp/iotc_bsp_mem_zephyr.c`.
- Multiple socket support in
  `src/bsp/iotc_bsp_io_net_zephyr.c:iotc_bsp_io_net_select` function.
- Review how mbedTLS is aware of time. Currently there is no such
  solution implemented like in:
  https://github.com/zephyrproject-rtos/zephyr/blob/master/samples/net/google_iot_mqtt/src/protocol.c#L227.
- The sample code under `samples/net/google_iot_device` has lot of
  noise for a visitor, should be simplified. It has been lifted from
  `iot-device-sdk-embedded-c` intact.
- Review licensing. Is Apache-2.0 suitable for this project? Note the
  `iot-device-sdk-embedded-c` as a west module is still BSD 3-Clause.
- Change parameter passing from command line arg to Kconfig style if
  the latter is more preferred by Zephyr sample app.

The source code for this sample application can be found at:
`samples/net/google_iot_device`.

Requirements
************
- Entropy source
- Google IOT Cloud account
- Google IOT Cloud credentials and required information
- Network connectivity

Building and Running
********************
This application has been built and tested only on the `native_posix`
board. ECDSA private key is required in CWD in file `ec_private.pem`
to authenticate to the Google IOT Cloud.

- Create a new dir for the Zephyr west project, cd into it.
- `west init -m https://github.com/atigyi/zephyr.git --mr
  google_iot_device_sdk_integration`
Note: the Google device sdk - Zephyr integration code currently sits
in a Zephyr github repo clone in github/atigyi account

- `west update`
- `export ZEPHYR_TOOLCHAIN_VARIANT=zephyr`
- `cd zephyr/samples/net/google_iot_device`
- `west build -b native_posix -- -G'Unix Makefiles'`
- provide an ECDSA private key in PEM format in the CWD in a file
  named `ec_private.pem`. This private key is associated with the
  Goolge IoT Device the sample app will impersonate.
- `./build/zephyr/zephyr.exe -testargs -p [GOOGLE_CLOUD_PROJECT_ID] -d
  [GOOGLE_CLOUD_DEVICE_PATH] -t [GOOGLE_CLOUD_PUBLIS_TOPIC]`
- example command: `./build/zephyr/zephyr.exe -testargs -p
  macr-o-matic-3100t -d
  projects/macr-o-matic-3100t/locations/europe-west1/registries/garage-door-meters/devices/sz11-front-reel
  -t /devices/sz11-front-reel/state`

See `Google Cloud MQTT Documentation
<https://cloud.google.com/iot/docs/how-tos/mqtt-bridge>`_.

Troubleshooting
***************

Setting up internet access on the native_posix board:

By default, the Zephyr application claims IP 192.0.2.1 and is in the
same subnet with the zeth virtual network adapter at IP 192.0.2.2.
This subnet must be connected to the internet.

To ensure internet connectivity, run the socket HTTP GET example.

Read the following references to start the zeth virtual network
adapter and connect the subnet to internet see the Zephyr
instructions.

Networking with native_posix board:
https://docs.zephyrproject.org/latest/guides/networking/native_posix_setup.html.
Setting up Zephyr and NAT and masquerading on host to access internet:
https://docs.zephyrproject.org/latest/guides/networking/qemu_setup.html#setting-up-zephyr-and-nat-masquerading-on-host-to-access-internet
