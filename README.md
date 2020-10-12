# PipsqueakRTOS

The real-time operating system for v4 (ESP-32 based) Pipsqueak hardware.
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

## Lineage

I've been tinkering with "pipsqueaks" for about five years. The earliest versions
consisted of a python script running on a Raspberry Pi. I moved on to
microcontrollers using the Arduino platform a few years later and quickly progressed
from a Nano to a Photon to a Wemos Mini D1 and on to custom Espressif esp8266 boards
as a I sought smaller footprints, higher reliability and more features.

All of the microcontroller-based pipsqueaks have used a bespoke TCP/IP protocol and
matched Node.js server running on an AWS EC2 instance. While great for learning, this
is getting old from a maintenance perspective. AWS IoT is fairly mature now, offering
great security, complete communications solutions and serverless technologies in the
cloud. But the esp8266 felt a bit underpowered for all the extra overhead, and after
a couple of years of Arduino programming, I'd like to sink my teeth into something a
little closer to the hardware.

Which brings us to now. My latest effort is porting the Arduino-based v3 operating
system over to FreeRTOS, an ecosystem that is new to me, and to migrate from
bespoke binary protocols and servers to the AWS IoT platform.

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
