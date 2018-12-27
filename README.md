KRF
===

KRF is a **K**ernelspace **R**andomized **F**aulter for the Linux kernel.

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

When loaded, KRF rewrites the Linux system call table: faultable syscalls are replaced with
thin wrappers.

Each wrapper then performs a check to see whether the call should be faulted (currently
`personality(2)` + RNG). If the process **shouldn't** be faulted, the original syscall is
invoked.

Finally, the targeted call is faulted via a random failure function. For example,
a `read(2)` call might receive one of `EBADF`, `EINTR`, `EIO`, and so on.

## Setup

### Compatibility

**NOTE**: If you have Vagrant, just use the Vagrantfile and jump to the build steps.

KRF should work on any recent-ish (4.15+) Linux kernel with `CONFIG_KALLSYMS=1`.

This includes the default kernel on Ubuntu 18.04 and probably many other recent distros.

### Dependencies

**NOTE**: Ignore this if you're using Vagrant.

Apart from a C toolchain (GCC will probably work best), KRF's only dependencies should be
`libelf` and the kernel headers.

For systems with `apt`:

```bash
sudo apt install libelf-dev linux-headers-$(uname -r)
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

## Usage

KRF has two main components: a kernel module (`krfx`) and a userspace utility (`krfexec`).

To load the kernel module, run `make insmod` (or run `insmod krfx.ko` directly). To unload
it, run `make rmmod` (or `rmmod krfx` directly).

Once KRF is loaded, you can instruct it to fault a program by running that program with
`krfexec`:

```bash
# no faults, even with KRF loaded
ls

# may fault!
./src/krfexec/krfexec ls

# krfexec will pass options correctly as well
./src/krfexec/krfexec echo -n 'no newline'
```

## Configuration

Most users will not need to configure KRF at all. For those who do, `/proc/krf/rng_state` and
`/proc/krf/probability` will probably be the most useful files.

### `/proc/krf/rng_state`

This file allows a user to read and modify the internal state of KRF's PRNG.

For example, each of the following will correctly update the state:

```bash
echo "1234" | sudo tee /proc/krf/rng_state
echo "0777" | sudo tee /proc/krf/rng_state
echo "0xFF" | sudo tee /proc/krf/rng_state
```

The state is a 32-bit unsigned integer; attempting to change it beyond that will fail.

### `/proc/krf/personality`

This file allows a user to read and write the `personality(2)` value used by KRF for syscall
targeting.

**NOTE**: KRF uses a personality not currently used by the Linux kernel by default. If you change
this, you should be careful to avoid making it something that Linux cares about. `man 2 personality`
has the details.

```bash
echo "28" | sudo tee /proc/krf/personality
```

This value gets read by `krfexec`.

### `/proc/krf/probability`

This file allows a user to read and write the probability of inducing fault for a given
(faultable) syscall.

The probability is represented as a reciprocal, e.g. `1000` means that, on average, `0.1%` of
faultable syscalls will be faulted.

```bash
echo "100000" | sudo tee /proc/krf/probability
```

## TODO

* Replace the current macro hell in the syscall wrapper layer with code generation.
* Add a `/proc/krf/filter` or similar file to allows users to disable specific faults.
  * For example, `echo "fork" | sudo tee /proc/krf/filter` would prevent KRF from faulting `fork`s.
* Allow users to specify a particular class of faults, e.g. memory pressure (`ENOMEM`).
  * This should be do-able by adding some more bits to the `personality(2)` value.

## Licensing

KRF is licensed under the terms of the GNU GPLv3.

See the [LICENSE](./LICENSE) file for the exact terms.
