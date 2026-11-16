# mcpp-platforms

Rows for systems mcpp's built-in table does not carry, as one platform package,
`mcpp.platforms`: each system's triples, the base each row links against, and a
runner that boots the system to run its programs. The engine names no system;
these rows are data it reads (mcpp SPEC-013, SPEC-016).

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

Every row compiles `import std`. NetBSD's own C++ library is GCC 10's
libstdc++, which ships no module sources; its rows take the C++ layer from
libc++ built over NetBSD's C library (the providers' `layers`), and the
compiler runtime, libgcc_s, stays the system's.

A runner fetches a machine image on its first use, so it is off until a
project or a machine enables it:

```toml
[target.x86_64-netbsd]
runner = { enabled = true }
```

Adding a system: [docs/adding-a-system.md](docs/adding-a-system.md).
