# LKS32MD035-Hello

This project implements a simple sensorless six-step commutation BLDC
controller based on the Linko LKS32MC035DL6S8B microcontroller.

Likely the first ever LKS32MD03x project written without vendor library.

## Hardware

See [here](hardware/) for the KiCad project. See schematics
[here](doc/minispin.pdf).

## Firmware

This project is a self-contained Visual Studio Code workspace. Due to my heavy
customization of vendor SDK files, I am not involving PlatformIO.

## Copyright

Copyright &copy; 2026 HIKARU Infotech Shanghai Ltd. All rights reserved.

This is [open-source](LICENSE.md) hardware and software under 3-clause BSD
license.

The `doc/LKS32MC03x*.pdf` and `platform/share/LKS32MC03x.svd` files' copyright
belong to Linko Semiconductor Co. Ltd. with my edits.

Files under `cmsis` copyright Arm Inc. See [its own license](cmsis/LICENSE).
