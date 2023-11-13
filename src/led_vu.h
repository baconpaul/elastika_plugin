#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

namespace sapphire
{

class LedVu : public juce::Component
{
  public:
    LedVu();

    void paint(juce::Graphics &g) override;

  private:
    static constexpr float line_thickness_ = 0.25f;
    const juce::Colour good_col_ = juce::Colours::green;
    const juce::Colour good_shadow_ = juce::Colours::lightgreen;
    const juce::Colour limit_col_ = juce::Colours::red;
    const juce::Colour limit_shadow_ = juce::Colours::mediumvioletred;
    const juce::Colour outline_col_ = juce::Colours::black;

    juce::ColourGradient led_picker_;
    juce::ColourGradient led_shadow_picker_;
};

} // namespace sapphire
