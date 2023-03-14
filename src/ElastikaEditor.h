
#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ElastikaProcessor.h"

//==============================================================================
/**
 */
class ElastikaEditor : public juce::AudioProcessorEditor
{
  public:
    ElastikaEditor(ElastikaAudioProcessor &);
    ~ElastikaEditor();

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

  private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ElastikaAudioProcessor &processor;
    std::vector<std::unique_ptr<juce::Slider>> knobs;
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> attachments;
    std::vector<std::unique_ptr<juce::Label>> labels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElastikaEditor)
};
