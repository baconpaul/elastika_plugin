#pragma once

#include <memory>
#include <string>
#include "juce_gui_basics/juce_gui_basics.h"

namespace sapphire
{
class LookAndFeel : public juce::LookAndFeel_V4
{
  public:
    LookAndFeel(std::unique_ptr<juce::Drawable> knob, std::unique_ptr<juce::Drawable> marker);

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider &slider) override;

  private:
    std::unique_ptr<juce::Drawable> knob_;
    std::unique_ptr<juce::Drawable> knob_marker_;
    std::unique_ptr<juce::Image> knob_cache_;

    int rotary_scale_factor_;
};
} // namespace sapphire
