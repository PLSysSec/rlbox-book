# Build and install RLBox

First you should clone the repo:

```bash
git clone git@github.com:PLSysSec/rlbox.git
```

Then, setup a build folder using cmake:

```bash
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
```

Third, build:

```bash
cmake --build ./build --config Release --parallel
```

Finally, install:

```bash
cd build && sudo make install
```
