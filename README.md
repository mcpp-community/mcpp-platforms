# mcpp-platforms

Rows for systems mcpp's built-in table does not carry, as one platform package,
`mcpp.platforms`: each system's triples, the base each row links against, and a
runner that boots the system to run its programs. The engine names no system;
these rows are data it reads (mcpp [SPEC-013](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/platforms.md),
[SPEC-016](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/targets-and-providers.md)).

```sh
mcpp build --target x86_64-netbsd        # the package index names this platform for the row
mcpp run   --target x86_64-netbsd        # once the row's runner is enabled
```

| Row | Base | C++ library | Runner |
|---|---|---|---|
| `x86_64-freebsd` | `xim:x86_64-freebsd-sysroot@14.5` | the base's libc++ | FreeBSD 14.5 in QEMU (dirun) |
| `aarch64-freebsd` | `xim:aarch64-freebsd-sysroot@14.5` | the base's libc++ | FreeBSD 14.5 in QEMU (dirun) |
| `x86_64-netbsd` | `xim:x86_64-netbsd-sysroot@10.2` | libc++ from source (`llvm.libcxx`) | NetBSD 10.2 in QEMU (dirun) |
| `aarch64-netbsd` | `xim:aarch64-netbsd-sysroot@10.2` | libc++ from source (`llvm.libcxx`) | none |
| `x86_64-openbsd` | `xim:x86_64-openbsd-sysroot@8.0` | the base's libc++ | OpenBSD 8.0 in QEMU (dirun) |
| `aarch64-openbsd` | `xim:aarch64-openbsd-sysroot@8.0` | the base's libc++ | none |
| `x86_64-hurd` | `xim:x86_64-hurd-sysroot@20260314` | the base's libstdc++ | Debian GNU/Hurd in QEMU (dirun) |
| `x86_64-haiku` | `xim:x86_64-haiku-sysroot@hrev60431` | the base's libstdc++ | Haiku hrev60431 in QEMU (dirun) |

The rows build on every host mcpp runs on (Linux x86_64 and aarch64, macOS
arm64, Windows x86_64) from mcpp 2026.11.18.1, but for three pairs: GNU/Hurd and
Haiku on Windows (mcpp#965), and Haiku on macOS, whose default file system does
not tell `string.h` from Haiku's `String.h`
([docs/adding-a-system.md](docs/adding-a-system.md#hosts)).

Every row compiles `import std`. NetBSD's own C++ library is GCC 10's
libstdc++, which ships no module sources; its rows take the C++ layer from
libc++ built over NetBSD's C library (the providers' `layers`), and the
compiler runtime, libgcc_s, stays the system's. GNU/Hurd's and Haiku's is
GCC 15's libstdc++, whose module sources their bases state.

A runner fetches a machine image on its first use, so it is off until a
project or a machine enables it, for one row or for every row of this
platform:

```toml
[target.x86_64-netbsd]
runner = true

[target.'cfg(platform = "platforms")']
runner = true
```

Adding a system: [docs/adding-a-system.md](docs/adding-a-system.md).

## Further reading

| Topic | mcpp document |
|---|---|
| What a platform is, how a project selects one, and the index's platform column | [docs/35 Platforms](https://github.com/mcpp-community/mcpp/blob/main/docs/35-platforms.md); [SPEC-013 Platforms](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/platforms.md) |
| Rows, providers, systems and runners as data | [SPEC-016 Targets and providers](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/targets-and-providers.md) |
| The layers of the target side and base packages (a sysroot with `mcpp.toml`) | [docs/22 The target side](https://github.com/mcpp-community/mcpp/blob/main/docs/22-target-side.md); [SPEC-002](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/target-side.md) |
| Running a program on another system: runners, `enabled`, `--file` | [docs/41 Devices](https://github.com/mcpp-community/mcpp/blob/main/docs/41-devices.md); [docs/09 Commands by scenario](https://github.com/mcpp-community/mcpp/blob/main/docs/09-commands-by-scenario.md) |
| `[target.<selector>]` and `cfg(...)` | [docs/04 mcpp.toml](https://github.com/mcpp-community/mcpp/blob/main/docs/04-mcpp-toml.md); [manifest semantics](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/manifest-semantics.md) |
| The sysroot and disk image packages, and their dirun profiles | [xim-pkgindex docs/12 Disk image assets](https://github.com/openxlings/xim-pkgindex/blob/main/docs/12-disk-image-assets.md) |
