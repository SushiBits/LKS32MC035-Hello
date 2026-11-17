# LKS32MD035-Hello

A sensorless six-step commutation BLDC controller for the Linko
LKS32MC035DL6S8B, on a board called minispin.

Likely the first ever LKS32MD03x project written without a vendor library. There
is no SDK, no HAL, and no generated init code: every register write is in this
repository, with a comment saying why. Recreating that from public documentation
took longer than the firmware will.

## Status

Early. The motor has not turned yet.

What exists:

- 48MHz PLL clock bring-up, with the watchdog disarmed so the debugger survives
  a session (see [Known issues](#known-issues) - it is disarmed unconditionally).
- GPIO: comparator inputs with pull-ups, MCPWM pin muxing applied without
  glitching the gate drivers, EXTI edge triggers configured.
- 1ms SysTick timebase with `millis()`, `micros()`, `delay()` and `delay_us()`.
  `micros()` is the tachometer; speed will be measured from commutation
  intervals, so its resolution is the resolution of the speed loop.
- On the board but not yet in software: the CC6905 current sensor and the six
  gate-driver pins, which are muxed to MCPWM and driven to a safe level but not
  yet commutated.

What does not exist yet: commutation, the zero-crossing state machine, and
anything that closes a loop. `main()` initialises the clock, GPIO and the
timebase, then idles.

## Hardware

KiCad project in [hardware/](hardware/), schematic as
[doc/minispin.pdf](doc/minispin.pdf). 3P3N gate drivers, CBM2091 high-voltage
comparators for back-EMF zero crossings, and a CC6905 Hall-effect current sensor
(U2) on the high side. The sensor is on the board; nothing in the firmware reads
it yet.

## Firmware

Bare metal, C17, no vendor library. Layout:

| Path | Contents |
|---|---|
| `src/` | Application: `main.c`, `gpio.c`, `systick.c` |
| `include/` | Their headers |
| `platform/src/` | Reset handler and clock setup |
| `platform/include/` | Register map (generated) and the hand-written platform headers |
| `platform/share/` | SVD, J-Link script, J-Link device package |
| `lib/` | Linker script |
| `tools/` | Generators, as submodules |

Register access goes through a header generated from
[platform/share/LKS32MC03x.svd](platform/share/LKS32MC03x.svd), so field names
and bit positions exist as macros rather than as hex literals.

## Building

Requirements: `arm-none-eabi-gcc`, `make`, `python3`, and git submodules
initialised. No Keil, no Windows-only tools, no bash.

```sh
git submodule update --init --recursive
make            # headers + minispin.elf
make hex        # minispin.ihx
make bin        # minispin.bin
make clean      # objects and image
make distclean  # also removes the generated headers
```

Objects land in `.build/`, mirroring the source tree. The two generated headers
under `platform/include/` are gitignored and rebuilt by `make headers`:
`LKS32MC03x.h` is generated from the SVD by `tools/svdconv`, and
`LKS32MC03x_handlers.h` from that header by `tools/irqn`.

The workspace also carries VS Code build tasks, so `Ctrl+Shift+B` runs `make
all`.

## Debugging

The workspace is set up for [cortex-debug](https://github.com/Marus/cortex-debug)
over J-Link, with an SVD-backed peripheral view. Two project-local pieces make
that work:

- [platform/share/LKS32MC03x.jlinkscript](platform/share/LKS32MC03x.jlinkscript)
  unlocks the system registers and enables the debug bits after reset. This is
  not a convenience: the IWDG is armed at reset, and an armed IWDG resets the
  core during the attach sequence, which presents as a target that drops out
  mid-session with no explanation.
- [platform/share/JLinkDevices/Linko/](platform/share/JLinkDevices/Linko/) is a
  device definition for these parts. Copy the `Linko` folder into your J-Link
  installation's `JLinkDevices/` directory and the parts appear in the device
  list. It exists because the vendor package is pinned to J-Link v6.19, and
  because installing it replaces `JLinkDevices.xml` wholesale with a file
  containing little besides Linko's own parts, which removes every other
  vendor's devices from your installation.

Note that [.vscode/launch.json](.vscode/launch.json) points at a remote probe
(`jlink.maxchan.xyz`). Change `ipAddress`, or remove it, for local hardware.

## Documentation

The datasheet and reference manual are pinned in [doc/](doc/):

- `LKS32MC03x_3P3N_Datasheet_v3.07.pdf`
- `LKS32MC03x_User_Manual_v1.83.pdf`

They are here because everything in this repository that disagrees with the
vendor's SVD is a claim about a specific paragraph in one of those two files.
Citations should name the document and version.

## Known issues

- **`SystemCoreClock` is an assumption.** It is derived by reading back the
  `CLK_CFG` register that `SystemInit()` just wrote, and interpreting `CLK_DIV`
  with a lookup ladder over a field whose real encoding is a bit pattern rather
  than a magnitude. The one configuration this project uses (`DIV1`) is
  unaffected, but the PLL frequency itself has never been measured. Since
  `micros()` divides by this number, a wrong value produces a tachometer that
  reports plausible, wrong speeds without complaining.
- **The watchdog is disabled unconditionally** in `main()`, which is why the
  firmware is debuggable at all. Re-enabling it after peripheral init and before
  the idle loop is the intended production shape, and needs `IWDG->PSW`, `WTH`,
  `RTH` and `CLR` - registers whose access protocol is not fully documented.
- **EXTI has no interrupt vector.** The SVD declares no `<interrupt>` node for
  the peripheral, so there is no `EXTI_IRQn` and vector slot 6 falls through to
  `Default_IRQHandler`, which is a breakpoint followed by an infinite loop. The
  zero-crossing triggers are configured; the handler they land in does not exist.
- **`WorkRAMSize` disagrees with the linker script.** The device definition says
  `0x2000` (8k), [platform/lib/LKS32MC03x.ld](platform/lib/LKS32MC03x.ld) says
  `4k`. J-Link uses the former to place the flash loader, so one of them is
  wrong in a way that affects programming rather than linking.
- **Nothing has been tested on hardware.** The bench supply was off. Commits say
  "verified" only when they mean it compiles and links.

## The SVD is not a vendor artifact

`platform/share/LKS32MC03x.svd` started as Linko's file and has been corrected
extensively: a wrong IWDG reset value, two mirrored power-down conventions in
the same register, missing fields, duplicate enumerated values, and duplicated
peripherals merged with `derivedFrom`. Some of those corrections were made
without hardware to confirm them, and are marked as such where they appear.

Treat the SVD as a best-effort reconstruction rather than as authority.
`git log -- platform/share/LKS32MC03x.svd` is the changelog, and each commit says
what was wrong and what was measured versus inferred.

## Copyright

Copyright &copy; 2026 HIKARU Infotech Shanghai Ltd. All rights reserved.

This is [open-source](LICENSE.md) hardware and software under 3-clause BSD
license.

Third-party material:

- `doc/LKS32MC03x*.pdf` and `platform/share/LKS32MC03x.svd` are Linko
  Semiconductor Co. Ltd.'s, the latter with my edits.
- `platform/share/JLinkDevices/Linko/*.FLM` are Linko's flash algorithms,
  redistributed unmodified. The `Devices.xml` that indexes them is mine.
- `cmsis/` is [ARM-software/CMSIS_6](https://github.com/ARM-software/CMSIS_6),
  copyright Arm Inc., tracked as a submodule at v6.4.0. See
  [its own license](cmsis/LICENSE).

The tooling in `tools/` is mine, and also BSD-3-Clause.
