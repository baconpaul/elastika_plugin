#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

namespace sapphire
{
class LookAndFeel : public juce::LookAndFeel_V4
{
  public:
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider &slider) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider &slider) override;

  private:
    juce::Colour grey = juce::Colour::fromRGB(34, 34, 34);
    juce::Colour light_grey = juce::Colour::fromRGB(136, 136, 136);
    juce::Colour dark_grey = juce::Colour::fromRGB(25, 25, 25);
};
} // namespace sapphire
