KRF
===

[![Build Status](https://travis-ci.com/trailofbits/krf.svg?branch=master)](https://travis-ci.com/trailofbits/krf)

KRF is a **K**ernelspace **R**andomized **F**aulter for the Linux and FreeBSD kernels.

## What?

[Fault injection](https://en.wikipedia.org/wiki/Fault_injection) is a software testing technique
that involves inducing failures ("faults") in the functions called by a program. If the callee
has failed to perform proper error checking and handling, these faults can result in unreliable
application behavior or exploitable vulnerabilities.

Unlike the many userspace fault injection systems out there, KRF runs in kernelspace
via a loaded module. This has several advantages:

* It works on static binaries, as it does not rely on `LD_PRELOAD` for injection.
* Because it intercepts raw syscalls and not their libc wrappers, it can inject faults
into calls made by `syscall(3)` or inline assembly.
* It's probably faster and less error-prone than futzing with `dlsym`.

There are also several disadvantages:

* You'll probably need to build it yourself.
* It probably only works on x86(_64), since it twiddles `cr0` manually. There is probably
an architecture-independent way to do that in Linux, somewhere.
* It's essentially a rootkit. You should definitely never, ever run it on a non-testing system.
* It probably doesn't cover everything that the Linux kernel expects of syscalls, and may
destabilize its host in weird and difficult to reproduce ways.

## How does it work?

KRF rewrites the Linux or FreeBSD system call table: when configured via `krfctl`, KRF replaces faultable
syscalls with thin wrappers.

Each wrapper then performs a check to see whether the call should be faulted using a configurable targeting system capable of targeting a specific `personality(2)`, PID, UID, and/or GID. If the process **shouldn't** be faulted, the original syscall is
invoked.

Finally, the targeted call is faulted via a random failure function. For example,
a `read(2)` call might receive one of `EBADF`, `EINTR`, `EIO`, and so on.

## Setup

### Compatibility

**NOTE**: If you have Vagrant, just use the Vagrantfile and jump to the build steps.

KRF should work on any recent-ish (4.15+) Linux kernel with `CONFIG_KALLSYMS=1`.

This includes the default kernel on Ubuntu 18.04 and probably many other recent distros.

It also works on FreeBSD.

### Dependencies

**NOTE**: Ignore this if you're using Vagrant.

Apart from a C toolchain (GCC will probably work best), KRF's only dependencies should be
`libelf`, the kernel headers, and Ruby (for code generation).

For systems with `apt`:

```bash
sudo apt install libelf-dev ruby linux-headers-$(uname -r)
```

### Building

```bash
git clone https://github.com/woodruffw/krf && cd krf
make -j$(nproc)
```

or, if you're using Vagrant:

```bash
git clone https://github.com/woodruffw/krf && cd krf
vagrant up && vagrant ssh
# inside the VM
cd /vagrant
make -j$(nproc)
```

or, if you're using FreeBSD:

```bash
git clone https://github.com/woodruffw/krf && cd krf
cd src/module/freebsd
ruby ../codegen/codegen freebsd
make
```

## Usage

KRF has three components:

* A kernel module (`krfx`)
* An execution utility (`krfexec`)
* A control utility (`krfctl`)

To load the kernel module, run `make insmod` (or run `insmod krfx.ko` directly). To unload
it, run `make rmmod` (or `rmmod krfx` directly). In FreeBSD, use `kldload` and `kldunload`.

KRF begins in a neutral state: no syscalls will be intercepted or faulted until the user
specifies some behavior via `krfctl`:

```bash
# no induced faults, even with KRF loaded
ls

# tell krf to fault read(2) and write(2) calls
# note that krfctl requires root privileges
sudo ./src/krfctl/krfctl -F 'read,write'

# tell krf to fault any program with a
# personality of 28 (the value set by krfexec)
sudo ./src/krfctl/krfctl -T personality=28

# may fault!
./src/krfexec/krfexec ls

# krfexec will pass options correctly as well
./src/krfexec/krfexec echo -n 'no newline'

# clear the fault specification
sudo ./src/krfctl/krfctl -c

# clear the targeting specification
sudo ./src/krfctl/krfctl -C

# no induced faults, since no syscalls are being faulted
./src/krfexec/krfexec firefox
```

## Configuration

**NOTE**: Most users should use `krfctl` instead of manipulating these files by hand.
In FreeBSD, these same values are accessible through `sysctl krf.whatever` instead of procfs.

### `/proc/krf/rng_state`

This file allows a user to read and modify the internal state of KRF's PRNG.

For example, each of the following will correctly update the state:

```bash
echo "1234" | sudo tee /proc/krf/rng_state
echo "0777" | sudo tee /proc/krf/rng_state
echo "0xFF" | sudo tee /proc/krf/rng_state
```

The state is a 32-bit unsigned integer; attempting to change it beyond that will fail.

### `/proc/krf/targeting`

This file allows a user set the values used by KRF for syscall
targeting.

**NOTE**: KRF uses a default personality not currently used by the Linux kernel by default. If you change
this, you should be careful to avoid making it something that Linux cares about. `man 2 personality`
has the details.

```bash
echo "0 28" | sudo tee /proc/krf/targeting
```

A personality of 28 is hardcoded into `krfexec`.

### `/proc/krf/probability`

This file allows a user to read and write the probability of inducing fault for a given
(faultable) syscall.

The probability is represented as a reciprocal, e.g. `1000` means that, on average, `0.1%` of
faultable syscalls will be faulted.

```bash
echo "100000" | sudo tee /proc/krf/probability
```

### `/proc/krf/control`

This file controls the syscalls that KRF faults.

**NOTE**: Most users should use `krfctl` instead of interacting with this file directly &mdash;
the former will perform syscall name-to-number translation automatically and will provide clearer
error messages when things go wrong.

```bash
# replace the syscall in slot 0 (usually SYS_read) with its faulty wrapper
echo "0" | sudo tee /proc/krf/control
```

Passing any number greater than `KRF_NR_SYSCALLS` will cause KRF to flush the entire syscall table,
returning it to the neutral state. Since `KRF_NR_SYSCALLS` isn't necessarily predictable for
arbitrary versions of the Linux kernel, choosing a large number (like 65535) is fine.

Passing a valid syscall number that lacks a fault injection wrapper will cause the `write(2)`
to the file to fail with `EOPNOTSUPP`.

### `/proc/krf/log_faults`

This file controls whether or not KRF emits kernel logs on faulty syscalls. By default, no
logging messages are emitted.

**NOTE**: Most users should use `krfctl` instead of interacting with this file directly.

```bash
# enable fault logging
echo "1" | sudo tee /proc/krf/log_faults
# disable fault logging
echo "0" | sudo tee /proc/krf/log_faults
# read the logging state
cat /proc/krf/log_faults
```

## TODO

* Allow users to specify a particular class of faults, e.g. memory pressure (`ENOMEM`).
  * This should be do-able by adding some more bits to the `personality(2)` value.

## Licensing

KRF is licensed under the terms of the GNU GPLv3.

See the [LICENSE](./LICENSE) file for the exact terms.
