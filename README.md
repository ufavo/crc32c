# crc32c
Simple c99 crc32c library that makes use of x86_64/amd64 and aarch64 crc32c instructions.

Includes a software fallback.

## How to build
Run `make` to build. `[sudo] make install` to install.

To cross compile or explicitly set the desired instruction set:
* `make ARCH=x86_64`
* `make ARCH=aarch64`

To forcefully compile the software implementation: `make NOHWACCEL=1`

## Usage examples
### Cli
```bash
$ echo Hello | crc32c LICENSE -
af9626d4  LICENSE
b0c9ce33  (stdin)
```
```bash
$ crc32c *.c
450233f1  cli-tool.c
35b0eb45  tablegen.c
82b6ae4e  test.c
```

### Library
#### One shot
```c
#include <crc32c.h>

crc32c_t crc = crc32c(data_ptr, data_length);
```
#### With updates
```c
#include <crc32c.h>

// Initialize the `crc`
crc32c_t crc = crc32c_init();

    // In your loop, feed the update function with the current `crc` and chunks of your data
    crc = crc32c_update(crc, data_ptr, data_length);

// When you are done, finalize the `crc`
crc = crc32c_finalize(crc);
// `crc` now contains the computed crc32c
```

`crc32c_hwaccel()` can be used to check at runtime if hardware acceleration is available. Useful to make decisions at runtime.

For real examples see [header](include/crc32c.h) and [cli tool](cli-tool.c)

## License
Licensed under the MIT License. See [LICENSE](LICENSE).
