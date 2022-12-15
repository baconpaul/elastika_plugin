# Proof of Concept Elastika VST/CLAP/AU

This repo exposes the Elastika VCV Rack plugin by Don Cross / cosinekitty
as a VST3/CLAP/AU/Standalone using the JUCE framework. It is a runnable proof
of concept which runs the entire Elastika engine inside an FX plugin with 
parameters which allow you to modify the underlying physics model.

To move from this proof of concept to a final product there are three
big bits of work to do

- Add a JUCE User Interface. Right now this is a parameters-only plugin so uses
the host default pulgin view
- Implement parameter smoothing. Parameters are applied directly at the start of
the process block. (The CLAP forces a maximum block size of 32 but the VST3 and AU
do not)
- Set up an automated build, installer package, etc... right now this plugin is
only available if you self build.

## Building the Elastika Proof of Concept Plugin

```
git clone (this repo or a fork of it)
git submodule update --init --recursive
cmake -Bbuild -DCMAKE_BUILD_TYPE=RELEASE
cmake --build build --target elastika-filter_VST3
```

and you get a VST3 in build/elastika-filter_artefacts/Release/VST3. 
Similar targets exist for `_CLAP`, `_AU` (on macOS) and `_Standalone`.

If you set in the first cmake the option `-DELASTIKA_COPY_PLUGIN_AFTER_BUILD=TRUE` 
built plugins will install in your local area, at least on mac and lin.


