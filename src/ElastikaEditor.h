
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
    void paintOverChildren(juce::Graphics &) override;
    void resized() override;

  private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ElastikaAudioProcessor &processor;
  std::unique_ptr<juce::LookAndFeel_V4> lnf;
  std::unique_ptr<juce::Drawable> background;
  juce::AffineTransform tosvg;
  juce::Rectangle<float> svgDefaultBounds{0.f, 0.f, 60.96f, 128.5f};
  //std::vector<std::pair<float, float>> knobs;
  std::vector<std::unique_ptr<juce::Slider>> knobs;
  std::vector<std::pair<float, float>> knob_positions;
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> attachments;
    std::vector<std::unique_ptr<juce::Label>> labels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElastikaEditor)
};
