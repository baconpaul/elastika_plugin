#include <cmath>

#include "sapphire_lnf.h"

using juce::degreesToRadians;

namespace sapphire
{

LookAndFeel::LookAndFeel(std::unique_ptr<juce::Drawable> knob,
                         std::unique_ptr<juce::Drawable> marker)
    : knob_(std::move(knob)), knob_marker_(std::move(marker)), rotary_scale_factor_(0)
{
}

void LookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                   float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                   juce::Slider &slider)
{
    const int sf = static_cast<int>(
        std::ceil(juce::Component::getApproximateScaleFactorForComponent(&slider)));
    if (sf != rotary_scale_factor_)
    {
        // Must recreate and recache the SVG image.
        knob_cache_ =
            std::make_unique<juce::Image>(juce::Image::ARGB, width * sf, height * sf, true);
        juce::Graphics cg(*knob_cache_);
        // Opacities taken from the SVG files, since Juce isn't smart enough to just use them, sigh.
        knob_->drawWithin(cg, juce::Rectangle{0, 0, width * sf, height * sf}.toFloat(),
                          juce::RectanglePlacement(), 1.f);
        knob_marker_->drawWithin(cg, juce::Rectangle{0, 0, width * sf, height * sf}.toFloat(),
                                 juce::RectanglePlacement(), 1.f);
        rotary_scale_factor_ = sf;
    }

#if 1
    g.drawImage(*knob_cache_, x, y, width, height, 0, 0, width * sf, height * sf);
#else
    // Maybe do this instead, and set to a buffered image at the component level.
    knob_->drawWithin(g, juce::Rectangle{x, y, width, height}.toFloat(), juce::RectanglePlacement(),
                      1.f);
    knob_marker_->drawWithin(g, juce::Rectangle{x, y, width, height}.toFloat(),
                             juce::RectanglePlacement(), 1.f);
#endif
}

} // namespace sapphire
