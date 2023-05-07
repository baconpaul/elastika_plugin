#include <cmath>

#include "sapphire_lnf.h"

using juce::Colour;
using juce::Point;
using juce::Slider;

namespace sapphire
{

LookAndFeel::LookAndFeel(std::unique_ptr<juce::Drawable> knob,
                         std::unique_ptr<juce::Drawable> marker)
    : knob_(std::move(knob)), knob_marker_(std::move(marker)), rotary_scale_factor_(0)
{
    setColour(Slider::thumbColourId, Colour(171, 157, 74));
}

void LookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height,
                                   float sliderPos, float minSliderPos, float maxSliderPos,
                                   const Slider::SliderStyle style, Slider &slider)
{
    if (style != Slider::LinearVertical)
    {
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos,
                                               maxSliderPos, style, slider);
        return;
    }
    std::cout << "y = " << y << "; height = " << height << "; "
              << "sliderPos = " << sliderPos << "; minSliderPos = " << minSliderPos
              << "; maxSliderPos = " << maxSliderPos << std::endl;

    const float unit = float(height) / 100.f;

    // Draw the slider track.
    // The width value is taken from LookAndFeel_V4.
    Colour bgc = findColour(Slider::trackColourId);
    float trackwidth = std::min(6.f, float(width) * 0.25f);
    float realX = float(x) + float(width) * 0.5f - trackwidth * 0.5f;
#if 0
    const Point<float> start{float(x) + float(width) * 0.5f, float(height + y)};
    const Point<float> end{start.x, float(y)};
    juce::Path track;
    track.startNewSubPath(start);
    track.lineTo(end);
    g.setColour(findColour(Slider::backgroundColourId));
    g.strokePath(track, {trackwidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});
#else
    g.setColour(findColour(Slider::backgroundColourId));
    g.fillRoundedRectangle(realX, y, trackwidth, height, 0.5f);
#endif

    // Draws a slider thumb reminiscent of the one used in VCV rack.
    // Composed of a darker color in the middle, flanked by two lighter colors, flanked by black on
    // top and white on the bottom, flanked by an even lighter color.
    //
    // The slider thumb should take up a total of 10% of the track. It shouldn't go over the track
    // edges. So we need to rescale the actual slider position to only go to 90% of the maximum
    // value.
    const float normalizedPos = juce::jmap<float>(sliderPos, height, y, 0.f, 0.9f);
    const float sliderBot = (1.f - normalizedPos) * float(height);

    Colour thumb = findColour(Slider::thumbColourId);
    realX += trackwidth * 0.15f;
    trackwidth *= 0.7f;

    // Bottom part, the brightest color.
    juce::Rectangle<float> part{realX, sliderBot - 2 * unit, trackwidth, 2 * unit};
    g.setColour(thumb.brighter().brighter());
    g.fillRect(part);
    // White part.
    part.setY(part.getY() - part.getHeight() - unit);
    part.setHeight(unit);
    g.setColour(juce::Colours::antiquewhite);
    g.fillRect(part);
    // Slightly less bright color.
    part.setY(part.getY() - part.getHeight() - unit);
    part.setHeight(unit);
    g.setColour(thumb.brighter());
    g.fillRect(part);
    // Center part, regular color.
    part.setY(part.getY() - part.getHeight() - 2 * unit);
    part.setHeight(2 * unit);
    g.setColour(thumb);
    g.fillRect(part);
    // Continuing up, less bright color.
    part.setY(part.getY() - part.getHeight() - unit);
    part.setHeight(unit);
    g.setColour(thumb.brighter());
    g.fillRect(part);
    // Continuing up, black part.
    part.setY(part.getY() - part.getHeight() - unit);
    part.setHeight(unit);
    g.setColour(juce::Colours::black);
    g.fillRect(part);
    // Final part, brightest color again.
    part.setY(part.getY() - part.getHeight() - 2 * unit);
    part.setHeight(unit);
    g.setColour(thumb.brighter().brighter());
    g.fillRect(part);
}

void LookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                   float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                   Slider &slider)
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
