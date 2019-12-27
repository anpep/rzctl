# `rzctl(1)`
![version: 0.1.0](https://img.shields.io/badge/version-1.0-blue.svg)
![license: GNU GPL v2](https://img.shields.io/badge/license-GNU_GPL_v2-brightgreen.svg)
![works: not really](https://img.shields.io/badge/works-not_really-brightgreen.svg)
#### Razer mouse control utility
> 🌈️ Configure Razer Chroma devices from the command line

## What's this?
`rzctl(1)` is a command line utility that allows changing the colorization parameters of Razer Chroma devices.

## Supported devices
For the time being it only supports the one (and probably last) Razer device I own, the (seemingly discontinued) Razer Deathadder Chroma I found laying around and repaired for fun.
Feel free to add your own via PR!

- `1532:0043` Razer Deathadder Chroma

## Quick start
```sh
# Clone this repository
git clone https://github.com/my-nick-is-taken/rzctl
# Build rzctl
cd rzctl && make
# Try it out!
rzctl list-devices
# Try rzctl --help for all supported commands
```

## Open-source code
This software depends on [argtable/argtable3](https://github.com/argtable/argtable3/) because I wanted to try out something new for parsing command line arguments. I'm very happy with the outcome.
Argtable is Copyright (C) 1998-2001, 2003-2011 Stewart Heitmann. 

`rzctl(1)` itself is licensed under the GNU GPL v2. You can find a copy of the license terms in the `LICENSE.md` file.

```
rzctl(1) -- Razer mouse control utility
Copyright (c) 2019 Angel <angel@ttm.sh>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 2.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
```
