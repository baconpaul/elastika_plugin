#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ElastikaProcessor.h"

class ElastikaEditor : public juce::AudioProcessorEditor
{
  public:
    ElastikaEditor(ElastikaAudioProcessor &);
    ~ElastikaEditor();

    void resized() override;

  private:
    ElastikaAudioProcessor &processor;
    std::unique_ptr<juce::LookAndFeel_V4> lnf;
    std::unique_ptr<juce::Drawable> background;
    std::vector<std::unique_ptr<juce::Slider>> knobs;
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> attachments;
    std::vector<std::unique_ptr<juce::Label>> labels;

    // Offsets from declared controls in the SVG. Determined through extensive trial and error.
    float dx = 1.05f;
    float dy = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElastikaEditor)
};
