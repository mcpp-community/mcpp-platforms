# Adding a system

A system enters `mcpp.platforms` as rows of its triples, each served by a
provider that names the system's base and, where the system can be booted, a
runner. The semantics are mcpp's ([SPEC-013 Platforms](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/platforms.md),
[SPEC-016 Targets and providers](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/targets-and-providers.md)); this
document states the procedure and what each part must hold.

## The systems

| System | Rows | Base | Runner | State |
|---|---|---|---|---|
| FreeBSD 14.5 | `x86_64-freebsd`, `aarch64-freebsd` (mcpp's built-in rows) | the release's base set | both | `preview` |
| NetBSD 10.2 | `x86_64-netbsd`, `aarch64-netbsd` | the release's base and comp sets; the C++ layer is libc++ from source (`llvm.libcxx`) | x86_64 | `preview` |
| OpenBSD 8.0 | `x86_64-openbsd`, `aarch64-openbsd` | the release's base and comp sets | x86_64 | `preview` |
| illumos (OmniOS r151054) | none | assembled, not published | measured | not included: see below |

illumos is not included. Its sysroot and disk image were assembled, and C and
C++ programs linked with lld ran on OmniOS r151054. But clang's Solaris driver
links only through the system's own linker: it runs `/usr/bin/ld` with Solaris
options (`-C`, `-z ignore`), ignores `--ld-path` and `-B`, and refuses lld as a
`-fuse-ld` name. A row can state no program that translates those options for
lld. The row enters when either a platform can name the program the compiler
links with, or clang's Solaris driver gives lld the GNU options.

## Hosts

The rows build on every host mcpp runs on: Linux x86_64 and aarch64, macOS
arm64 and Windows x86_64, each checked by this repository's CI. The row, not
the host, decides a cross build: the artifact is an ELF executable of the row's
system on each of them, and the sysroot package is the only source of headers,
crt objects and libraries.

This holds from mcpp 2026.11.17.2 and xlings 2026.11.17.1. Measured in the CI
of 0.1.0 with mcpp 2026.11.17.1, two defects let the host's conventions reach
the row, and both are corrected there:

| Defect | Seen as | Corrected in |
|---|---|---|
| The object format of a row with no format word in its triple was taken from the host | macOS: the Apple SDK's headers in the std module's compile, the Mach-O linker given ELF options; Windows: the program named `.exe` | mcpp 2026.11.17.2 |
| A link to a directory extracted on Windows was a file link | Windows: the sysroot's headers not opened (`sys/cdefs.h`), the crt objects not found | xlings 2026.11.17.1 |

A row runs where its runner does: the disk images boot under QEMU on Linux
x86_64, which is where the CI runs them.

## What a system brings

| Part | Where | Holds |
|---|---|---|
| The system | `[[platform.system]]` in `mcpp.toml` | `name`, the row's os word; `uname`, what `uname -s` answers there |
| A row per architecture | `[[platform.define]]` | `triple`, `clang-triple` (with the release the sysroot is), `loader`, `object-format = "elf"`, `family`, `execution`, `tier = "preview"`, `standard` |
| A provider per row | `[[platform.provider]]` | `serves`, `sysroot = "xim:<arch>-<system>-sysroot@<release>"`, `hosted = true`, and for a row whose image exists a `runner` with `enabled = false`; `layers` when a layer comes from a package rather than from the base (below) |
| A sysroot package | xim-pkgindex, `<arch>-<system>-sysroot` | the release's own headers, crt objects, shared libraries and the static archives of the C library, the C++ run-time and the compiler run-time, with a `mcpp.toml` at its root stating the layers it provides ([mcpp docs/22](https://github.com/mcpp-community/mcpp/blob/main/docs/22-target-side.md), base packages; [SPEC-002 §2.5](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/target-side.md)) |
| A disk image package | xim-pkgindex, `<system>-disk-image` | the release's official media made a bootable qcow2 with a serial console, and the dirun profile that boots it ([xim-pkgindex docs/12](https://github.com/openxlings/xim-pkgindex/blob/main/docs/12-disk-image-assets.md)) |
| A check | `.github/workflows/ci.yml` | the row builds a C program and a C++ program with an exception; where it has a runner, both run and their status returns |

The assembly of each package is a script in xim-pkgindex `.agents/tools/`
that starts from the publisher's signed or digest-listed release files and
records them in the package's `PROVENANCE.md`. A change the image needs (a
serial console, a login) is stated there, at its byte or as the script that
made it.

## `import std`

Every row compiles `import std`. A base whose C++ library ships module sources
states them (`std-module`, [mcpp docs/22](https://github.com/mcpp-community/mcpp/blob/main/docs/22-target-side.md)); FreeBSD and OpenBSD ship libc++, and
their sysroots carry the module sources of its release. A system whose C++
library ships none (NetBSD: GCC 10's libstdc++) takes the C++ layer from a
package instead, libc++ built from source over the system's C library:

```toml
[[platform.provider]]
serves  = ["x86_64-netbsd"]
sysroot = "xim:x86_64-netbsd-sysroot@10.2"
layers  = { "c++-abi" = "llvm.libcxx@22.1.8.3" }
```

The package replaces the base for that layer alone: the base's C++ headers and
libraries leave the build, and the C library and the compiler runtime stay the
system's. What the replacement needs of the compiler runtime and the driver
does not name is stated for the system's rows (`[platform.build.'cfg(os =
"netbsd")'] ldflags`: libc++abi unwinds through libgcc_s).

## The image

The image's profile states what the image answers: the boot prompts, the
login, the shell's quoting and where a disk of files is mounted. It needs no
network and no interaction beyond the profile's steps. A runner is
`dirun --profile {pkg:<system>-disk-image}/dirun.toml -- {}`, with the
emulator a package of xim; it stays `enabled = false`, because its first use
fetches a machine image ([SPEC-016 §6](https://github.com/mcpp-community/mcpp/blob/main/docs/specs/targets-and-providers.md)). A project or a machine enables it:

```toml
[target.x86_64-netbsd]
runner = true
```

## Tiers

- `preview`: the row builds in this repository's CI; where it has a runner,
  its programs run there.
- `stable`: in addition, a project outside this repository has built and run
  a program of the row through mcpp's own commands (`mcpp build`, `mcpp run`,
  `mcpp test`), and the row's packages have had one release without a
  correction.
- A row moves into mcpp's built-in table when its system is common enough that
  a project should not need a platform for it, and the engine then carries the
  row's data; its provider stays here.

## What may not be added

- A system name in the engine: the engine reads the rows as data.
- A sysroot or image assembled from anything but the publisher's media.
- A runner that needs a network or a person to finish booting.
