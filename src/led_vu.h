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
    static constexpr float line_thickness_ = 0.2f;
    const juce::Colour good_col_ = juce::Colour(0xFFABFF00);;
    const juce::Colour limit_col_ = juce::Colours::red;
    const juce::Colour outline_col_ = juce::Colours::black;
};

} // namespace sapphire
