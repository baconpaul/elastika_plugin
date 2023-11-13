#include "led_vu.h"

namespace sapphire
{

LedVu::LedVu(const std::atomic<float> &block_max) : block_max_(block_max)
{
    setPaintingIsUnclipped(true);

    led_picker_.addColour(0, good_col_);
    led_picker_.addColour(0.8, good_col_);
    led_picker_.addColour(1, limit_col_);
    led_shadow_picker_.addColour(0, good_shadow_);
    led_shadow_picker_.addColour(0.8, good_shadow_);
    led_shadow_picker_.addColour(1, limit_shadow_);
}

void LedVu::paint(juce::Graphics &g)
{
    const float diam = std::min<float>(getWidth(), getHeight()) - line_thickness_;
    const float bezel_diam = diam + line_thickness_;
    juce::Path p;
    p.addEllipse(0, 0, diam, diam);

    const double point = static_cast<double>(block_max_.load(std::memory_order_relaxed));

    // To determine the color to be used, construct a gradient and pull from
    // along it.
    juce::Colour shadow_color;
    juce::Colour led_color;
    shadow_color = led_shadow_picker_.getColourAtPosition(point);
    led_color = led_picker_.getColourAtPosition(point);

    juce::DropShadow(shadow_color.withAlpha(0.7f), 1, {}).drawForPath(g, p);

    juce::ColourGradient grad(juce::Colours::white, 0, 0, led_color, diam, diam, true);
    g.setGradientFill(grad);
    g.fillPath(p);

    // Add the outer line.
    g.setColour(outline_col_);
    g.setOpacity(0.7f);
    g.drawEllipse(0, 0, bezel_diam, bezel_diam, line_thickness_);
}

} // namespace sapphire
