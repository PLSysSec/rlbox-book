# Build and install RLBox

First clone the RLBox repo:

```bash
git clone git@github.com:PLSysSec/rlbox.git
```
And enter the root directory.

```bash
cd rlbox
```

Then, setup a build folder using cmake:

```bash
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
```

Next, build:

```bash
cmake --build ./build --config Release --parallel
```

Finally, install:

```bash
cd build && sudo make install
```
