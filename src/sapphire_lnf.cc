#include <cmath>

#include "sapphire_lnf.h"

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
    const int swidth = width * sf;
    const int sheight = height * sf;
    const float xmid = float(x + width) / 2.f;
    const float ymid = float(y + width) / 2.f;
    if (sf != rotary_scale_factor_)
    {
        // Must recreate and recache the SVG image.
        knob_cache_ =
            std::make_unique<juce::Image>(juce::Image::ARGB, width * sf, height * sf, true);
        juce::Graphics cg(*knob_cache_);
        // Opacities taken from the SVG files, since Juce isn't smart enough to just use them, sigh.
        knob_->drawWithin(cg, juce::Rectangle{0, 0, swidth, sheight}.toFloat(),
                          juce::RectanglePlacement(), 1.f);
        knob_marker_->drawWithin(cg, juce::Rectangle{0, 0, swidth, sheight}.toFloat(),
                                 juce::RectanglePlacement(), 1.f);

        rotary_scale_factor_ = sf;
    }

    // sliderPos is in range [0,1]. Map it onto the start/end angles. 0.5 should be noon by default.
    auto rot_params = slider.getRotaryParameters();
    float rads = juce::jmap(sliderPos, rot_params.startAngleRadians, rot_params.endAngleRadians);
    auto rotation = juce::AffineTransform::rotation(rads, xmid, ymid);
    g.addTransform(rotation);
#if 1
    g.drawImage(*knob_cache_, x, y, width, height, 0, 0, swidth, sheight);
#else
    // Maybe do this instead, and set to a buffered image at the component level.
    knob_->drawWithin(g, juce::Rectangle{x, y, width, height}.toFloat(), juce::RectanglePlacement(),
                      1.f);
    knob_marker_->drawWithin(g, juce::Rectangle{x, y, width, height}.toFloat(),
                             juce::RectanglePlacement(), 1.f);
#endif
}

} // namespace sapphire
