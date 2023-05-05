#include "sapphire_lnf.h"

using juce::degreesToRadians;

namespace sapphire
{

LookAndFeel::LookAndFeel(std::unique_ptr<juce::Drawable> knob,
                         std::unique_ptr<juce::Drawable> marker)
    : knob_(std::move(knob)), knob_marker_(std::move(marker)), last_rotary_width_(0),
      last_rotary_height_(0)
{
}

void LookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                   float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                   juce::Slider &slider)
{
    if (last_rotary_width_ != width || last_rotary_height_ != height)
    {
        // Must recreate and recache the SVG image.
        knob_cache_ = std::make_unique<juce::Image>(juce::Image::ARGB, width, height, true);
        juce::Graphics cg(*knob_cache_);
        knob_->drawWithin(cg, juce::Rectangle{0, 0, width, height}.toFloat(),
                          juce::RectanglePlacement(64), 1.f);
        std::cout << "Recached at width " << width << " and height " << height << std::endl;
    }

    g.drawImageAt(*knob_cache_, x, y);
}

} // namespace sapphire
