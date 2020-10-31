# PipsqueakRTOS

The real time operating system (RTOS) for v4 (ESP-32 based) Pipsqueak hardware.
Written against the FreeRTOS-based ESP IoT Development Framework (IDF).

## Notice

Both the software and hardware is an early work in progress.

Please note that this is my first foray into real-time operating systems. Friendly
advice is always welcome... and you might not want to use this project as your
north star.

Use at your own risk.

## What is a Pipsqueak?

As a hobby ciderist, I have a strong interest in monitoring and controlling the
temperature of my ferments. As a technologist, I have a strong interest in
learning new technologies. Pipsqueaks are the result: hobbiest electronic devices
with varying capabilities and no reasonable product-market fit. :)

## Getting Started

This repository uses Git submodules and requires some setup.

Begin by cloning this repository:
```shell
# Clone this repository
git clone --recurse-submodules https://github.com/mvolk/pipsqueak-rtos.git
# Clone nested submodules
git submodule update --init --recursive
```

Next, set up your ESP-IDF environment. Refer to the ESP-IDF Programming Guide's
[Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
section and the VS Code Configuration Tips for MacOS below.

You'll need to create an AWS account (if you don't have one already) and create
a Thing. Refer to
[Getting started with AWS IoT Core](https://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html).

The X.509 certificates AWS generates for your Thing need to be added
to `components/psq4-aws-iot/certs` as `certificate.pem.crt`, `private.pem.key` and
`public.pem.key`. Files with these names are ignored by Git, and should not be
committed or shared.

Also take note of your Thing's name, your Thing's ARN, and the custom endpoint
specific to your AWS account. Your custom endpoint can be found in the Settings
section of your AWS IoT dashboard.

Now you're ready to set up the configuration parameters. You can do this with the
ESP-IDF extension to VS Code by running `ESP-IDF: Launch gui configuration tool`,
or you can use the command `idf.py menuconfig` in your terminal. Either way,
you'll need to supply values for at least the following parameters:

* Pipsqueak -> WiFi, both SSID and password
* Pipsqueak -> AWS IoT Thing, both the name and ARN that you recorded earlier
* Pipsqueak -> PSQ4_USE_SNTP must be set to `true` if your external RTC module's
  battery power has been interrupted since the RTC module's clock was last
  synchronized, but should be set to `false` otherwise when developing in order
  to minimize the load on SNTP servers.
* Component config -> Amazon Web Services IoT Platform -> AWS IoT Endpoint Hostname
  must be set to the custom AWS IoT endpoint you recorded earlier.

You're now ready to compile the code and flash it to your Pipsqueak v4 hardware.

## VS Code Configuration Tips for MacOS

Getting ESP-IDF all set up in VS Code was not as painless as I'd hoped on macOS.
Here are some tips:

1. Upgrade to zsh first if you plan to but have been putting it off.
2. Use the VS Code ESP-IDF plugin to perform installation of the toolchain. Don't use
   command-line based installation procedures. You'll still end up with a working
   command-line toolchain.
3. Specify a Python 3.5+ (I used 3.8) binary during VS Code ESP-IDF plugin configuration.
4. Note the recommended PATH during installation in the VS Code terminal. Set this in
   your .zshrc or .bash_profile. Despite having extra paths configured, they didn't get
   picked up reliably in the VS Code-hosted build process.
5. After installing via the VS Code plugin, run ~/esp/esp-inf/install.sh (assuming
   you installed ESP-INF to ~/esp). This may set up another Python virtual environment
   (v2.7 for me). Then add `alias use_idf='. $HOME/esp/esp-idf/export.sh'` to your
   .zshrc or .bash_profile and run `use_idf` before `idf.py` commands in a terminal
   window. The command-line specific path will thus by set up only in the terminals
   where you will use it, while the VS Code-worthy path will be set by default in all
   terminals.
6. Don't expect Intellisense to work until a build is complete. It will need the
   `compile_commands.json` file generated in your build directory.
7. Assign your USB port to the `idf_port` key of your configuration. You might
   consider setting this in your user-level configuration if you use only one port.

I don't commit IDE files as a general practice, but you may find these examples helpful.
Note that `{ROOT}` is a placeholder that needs to be replaced with a value tailored to
your environment. For example, user John Smith working on macOS Catalina might replace
`{ROOT}` with `/Users/johnsmith`. Also note that your versions will probably not match
mine.

### .vscode/settings.json
``` json
{
  "idf.showOnboardingOnInit": false,
  "idf.espIdfPath": "{ROOT}/esp/esp-idf",
  "idf.toolsPath": "{ROOT}/.espressif",
  "idf.pythonBinPath": "{ROOT}/.espressif/python_env/idf4.1_py3.8_env/bin/python",
  "idf.customExtraPaths": "{ROOT}/.espressif/python_env/idf4.1_py3.8_env/bin:/usr/local/bin:{ROOT}/.espressif/tools/xtensa-esp32-elf/esp-2020r2-8.2.0/xtensa-esp32-elf/bin:/{ROOT}/.espressif/tools/xtensa-esp32s2-elf/esp-2020r2-8.2.0/xtensa-esp32s2-elf/bin:{ROOT}/.espressif/tools/esp32ulp-elf/2.28.51-esp-20191205/esp32ulp-elf-binutils/bin:{ROOT}/.espressif/tools/esp32s2ulp-elf/2.28.51-esp-20191205/esp32s2ulp-elf-binutils/bin:{ROOT}/.espressif/tools/openocd-esp32/v0.10.0-esp32-20191114/openocd-esp32/bin",
  "idf.customExtraVars": "{\"OPENOCD_SCRIPTS\":\"{ROOT}/.espressif/tools/openocd-esp32/v0.10.0-esp32-20191114/openocd-esp32/share/openocd/scripts\"}",
  "idf.adapterTargetName": "esp32",
  "idf.port": "/dev/cu.SLAB_USBtoUART",
  "cmake.configureOnOpen": false,
}
```

### .vscode/c_cpp_properties.json
```json
{
    "configurations": [
        {
            "name": "Linux",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "compileCommands": "${workspaceFolder}/build/compile_commands.json"
        }
    ],
    "version": 4
}
```
