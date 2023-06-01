#pragma once

#include "elastika_engine.hpp"
#include "juce_audio_processors/juce_audio_processors.h"

class ElastikaAudioProcessor : public juce::AudioProcessor
{
  public:
    ElastikaAudioProcessor();
    ~ElastikaAudioProcessor();

    std::unique_ptr<Sapphire::ElastikaEngine> engine;
    double sampleRate{0};

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

    juce::AudioParameterFloat *friction;
    juce::AudioParameterFloat *span;
    juce::AudioParameterFloat *stiffness;
    juce::AudioParameterFloat *curl;
    juce::AudioParameterFloat *mass;
    juce::AudioParameterFloat *drive;
    juce::AudioParameterFloat *gain;
    juce::AudioParameterFloat *inputTilt;
    juce::AudioParameterFloat *outputTilt;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElastikaAudioProcessor)
};
