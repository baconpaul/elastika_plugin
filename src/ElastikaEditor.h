#pragma once

#include <memory>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include "ElastikaProcessor.h"
#include "led_vu.h"

// A tuple of several UI elements that control the physics of the simulation.
// (1) An input knob for attenuverting the sidechain input, if it exists.
// (2) An associated LED VU meter for that sidechain input, if it exists.
// (3) A slider for controlling the overall effect of the physics parameter.
struct PhysicsControl
{
    std::unique_ptr<juce::Slider> atten;
    std::unique_ptr<sapphire::LedVu> vu;
    std::unique_ptr<juce::Slider> slider;
};

class ElastikaEditor : public juce::AudioProcessorEditor
{
  public:
    ElastikaEditor(ElastikaAudioProcessor &);
    ~ElastikaEditor();

    void resized() override;

  private:
    // Convenience functions for constructing controls.
    std::unique_ptr<juce::Slider> make_large_knob(float cx, float cy);
    std::unique_ptr<juce::Slider> make_small_knob(float cx, float cy);
    std::unique_ptr<sapphire::LedVu> make_led_vu(float cx, float cy);
    std::unique_ptr<juce::Slider> make_slider(float cx, float cy);

    ElastikaAudioProcessor &processor;
    std::unique_ptr<juce::LookAndFeel_V4> lnf;
    std::unique_ptr<juce::LookAndFeel_V4> small_lnf; // Specifically for small elements.
    std::unique_ptr<juce::Drawable> background;
    PhysicsControl tilt_in;
    PhysicsControl tilt_out;
    PhysicsControl fric;
    PhysicsControl stif;
    PhysicsControl span;
    PhysicsControl curl;
    PhysicsControl mass;
    std::unique_ptr<juce::Slider> in_drive;
    std::unique_ptr<sapphire::LedVu> inl_vu;
    std::unique_ptr<sapphire::LedVu> inr_vu;
    std::unique_ptr<juce::Slider> out_level;
    std::unique_ptr<sapphire::LedVu> outl_vu;
    std::unique_ptr<sapphire::LedVu> outr_vu;
    std::unique_ptr<sapphire::LedVu> limiter_warning;
    std::vector<std::unique_ptr<juce::Slider>> knobs;
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> attachments;
    std::vector<std::unique_ptr<juce::Label>> labels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElastikaEditor)
};
