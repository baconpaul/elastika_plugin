#include "led_vu.h"

namespace sapphire
{

LedVu::LedVu(const std::atomic<float>& source)
    : source_(source)
{
    setPaintingIsUnclipped(true);
}

void LedVu::paint(juce::Graphics &g)
{
    const float diam = std::min<float>(getWidth(), getHeight()) - line_thickness_;
    const float bezel_diam = diam + line_thickness_;
    juce::Path p;
    p.addEllipse(0, 0, diam, diam);

    const float point = source_.load(std::memory_order_relaxed);

    juce::Colour shadow_color;
    juce::Colour led_color;
    if (point < 0.9) {
        led_color = good_col_;
        shadow_color = good_shadow_;
    } else if (point < 1) {
        led_color = warning_col_;
        shadow_color = warning_shadow_;
    } else {
        led_color = limit_col_;
        shadow_color = limit_shadow_;
    }

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
