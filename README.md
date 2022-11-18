To build

```
git clone (this)
git submodule update --init --recursive
cmake -Bbuild -DCMAKE_BUILD_TYPE=RELEASE
cmake --build build --target elastika-filter_VST3
```

and you get a VST3 in build/elastika-filter_artefacts/Release/VST3


