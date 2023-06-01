#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

namespace sapphire
{

class LedVu : public juce::Component
{
  public:
    void paint(juce::Graphics &g) override;

  private:
    static constexpr float outerLine = 0.2f;
    const juce::Colour col = juce::Colours::white;
};

} // namespace sapphire
