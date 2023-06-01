#pragma once

#include <memory>
#include "juce_gui_basics/juce_gui_basics.h"

namespace sapphire
{
class LookAndFeel : public juce::LookAndFeel_V4
{
  public:
    LookAndFeel(std::unique_ptr<juce::Drawable> knob, std::unique_ptr<juce::Drawable> marker);

    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider &slider) override;

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider &slider) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider &slider) override;

  private:
    std::unique_ptr<juce::Drawable> knob_;
    std::unique_ptr<juce::Drawable> knob_marker_;
    std::unique_ptr<juce::Image> knob_cache_;

    int rotary_scale_factor_;
};
} // namespace sapphire
