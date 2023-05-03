#include "sapphire_lnf.h"

namespace sapphire
{

void LookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                                   float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                   juce::Slider &slider)
{
    // Setting Bounds and Angles

    auto bounds = juce::Rectangle<int>(x, y, width, height)
                      .toFloat()
                      .reduced(0); // add .reduced for smaller bounds
    rotaryStartAngle =
        -juce::MathConstants<float>::pi * 2.0f * 3.0f / 8.0f; // starting angle in radians
    rotaryEndAngle = juce::MathConstants<float>::pi * 2.0f * 3.0f / 8.0f; // finish angle in radians
    auto toAngle =
        rotaryStartAngle +
        sliderPos * (rotaryEndAngle - rotaryStartAngle); // total angle traversed by slider

    // Setting the inner square where the knob will sit

    bounds = bounds.constrainedWithin(
        juce::Rectangle<int>(x, y, juce::jmin(width, height), juce::jmin(width, height)).toFloat());
    bounds.setPosition(0, 0);

    // Setting Scaling Values

    auto scaling_val = bounds.getHeight(); // 420 @ 600,600
    auto key_arc = bounds.reduced(scaling_val / 7.0f);
    auto minute_arc = bounds.reduced(scaling_val / 8.4f);
    auto minute_marker_thickness = scaling_val / 120.0f;

    // Creating Gradient

    juce::Point<float> p1(bounds.getCentre());
    juce::Point<float> p2(key_arc.reduced(scaling_val / 19.0f).getTopLeft());
    juce::ColourGradient black_fade(juce::Colours::black, p1, grey, p2, true); // move this

    g.setGradientFill(black_fade);
    g.fillEllipse(bounds);

    // Creating minute markers

    g.setColour(light_grey);

    juce::Point<float> circle_edge(minute_arc.getTopLeft().getX(),
                                   bounds.getCentreY()); // scale around to make full circle
    circle_edge.applyTransform(
        juce::AffineTransform::rotation(-2.0f * juce::MathConstants<float>::pi * 40.0f / 360.0f,
                                        bounds.getCentreX(), bounds.getCentreY()));
    juce::Line<float> horizontal_line(bounds.getCentre(), circle_edge);
    g.drawLine(horizontal_line, minute_marker_thickness);

    for (int degree = 0; degree < 26; degree++)
    {
        circle_edge.applyTransform(
            juce::AffineTransform::rotation(2.0f * juce::MathConstants<float>::pi * 10.0f / 360.0f,
                                            bounds.getCentreX(), bounds.getCentreY()));
        horizontal_line.setEnd(circle_edge);
        g.drawLine(horizontal_line, minute_marker_thickness);
    }

    // Creating knob face

    g.setColour(grey);
    g.fillEllipse(key_arc);

    // Creating Knob Indicator

    auto arcRadius = key_arc.getWidth() / 2.0f;

    juce::Point<float> LineEndPoint(
        bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
    auto LineStartX = (bounds.getCentre().getX() + LineEndPoint.getX()) / 2.0f;
    auto LineStartY = (bounds.getCentre().getY() + LineEndPoint.getY()) / 2.0f;
    juce::Point<float> LinesStartPoint(LineStartX, LineStartY);

    juce::Line<float> marker_line(LinesStartPoint, LineEndPoint);

    g.setColour(light_grey);
    g.drawLine(marker_line, minute_marker_thickness * 2);
}

juce::Slider::SliderLayout LookAndFeel::getSliderLayout(juce::Slider &slider)
{
    juce::Slider::SliderLayout layout;

    layout.textBoxBounds = slider.getBounds();
    layout.sliderBounds = slider.getBounds();

    return layout;
}

} // namespace sapphire
