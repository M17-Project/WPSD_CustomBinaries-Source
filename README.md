> **This is the M17 Project's fork of WPSD,** based on [W0CHP's WPSD](https://w0chp.radio/wpsd/), with changes for M17. It is not kept in sync with upstream WPSD, whose code is hosted on W0CHP's own Git server ([WPSD-Dev](https://repo.w0chp.net/WPSD-Dev/)), not on GitHub.
>
> **Contributions:** this fork only accepts M17-related changes and bug reports. For anything else, please go to upstream WPSD, as described in [How to Contribute to the WPSD Project](https://w0chp.radio/articles/how-to-contribute-to-wpsd/). Non-M17 pull requests here will be closed.

MMDVMHost, M17Gateway and modem firmware: the M17 fork no longer builds these from this repository. The release workflow in WPSD-Binaries builds MMDVMHost (v1.0 branch) and M17Gateway (main) from the M17 Project's MMDVMHost and M17Gateway repositories, and copies modem firmware from MMDVM_HS and MMDVM. The MMDVMHost and M17Gateway directories here are WPSD's last versions, kept for reference; send changes to those repositories instead. The other programs here are still WPSD's. The release workflow still builds against the ArduiPi_OLED library here.

# Description

Since I custom-compile and distribute third-party
[programs/binaries](https://repo.w0chp.net/Chipster/W0CHP-PiStar-bin) for
`W0CHP-PiStar-Dash`, I include all sources from upstream .

I also include a crude build script that builds all of the binaries.
