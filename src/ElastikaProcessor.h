#pragma once

#include <atomic>

#include "elastika_engine.hpp"
#include "juce_audio_processors/juce_audio_processors.h"
#include <Lag.h>

// Ties together an audio parameter and its smoother.
struct AudioParameter
{
    juce::AudioParameterFloat *param;
    sst::basic_blocks::dsp::SurgeLag<float> lag;
    float last_value;
    std::atomic<float> level;  // currently unused.

    void updateLag()
    {
        last_value = lag.v;
        lag.newValue(*param);
    }
};

class ElastikaAudioProcessor : public juce::AudioProcessor
{
  public:
    ElastikaAudioProcessor();
    ~ElastikaAudioProcessor();

    std::unique_ptr<Sapphire::ElastikaEngine> engine;
    double sampleRate{0};
    int perBlockRate{0};

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    AudioParameter friction;
    AudioParameter span;
    AudioParameter stiffness;
    AudioParameter curl;
    AudioParameter mass;
    AudioParameter drive;
    AudioParameter gain;
    AudioParameter inputTilt;
    AudioParameter outputTilt;

    std::atomic<float> internal_distortion;
    std::atomic<float> inl_level;
    std::atomic<float> inr_level;
    std::atomic<float> outl_level;
    std::atomic<float> outr_level;

  private:
    static constexpr const float decay_rate = 0.707;

    void updateEngineParameters();
    void updateLagRates();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElastikaAudioProcessor)
};
