#pragma once

#include <atomic>

#include "juce_gui_basics/juce_gui_basics.h"

namespace sapphire
{

class LedVu : public juce::Component
{
  public:
    LedVu(const std::atomic<float>& source);

    void paint(juce::Graphics &g) override;

  private:
    static constexpr float line_thickness_ = 0.25f;
    const juce::Colour good_col_ = juce::Colours::green;
    const juce::Colour good_shadow_ = juce::Colours::lightgreen;
    const juce::Colour limit_col_ = juce::Colours::red;
    const juce::Colour limit_shadow_ = juce::Colours::mediumvioletred;
    const juce::Colour warning_col_ = juce::Colours::yellow;
    const juce::Colour warning_shadow_ = juce::Colours::lightyellow;
    const juce::Colour outline_col_ = juce::Colours::black;

    const std::atomic<float>& source_;
};

} // namespace sapphire
