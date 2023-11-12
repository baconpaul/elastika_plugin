#include "led_vu.h"

namespace sapphire
{

LedVu::LedVu() { setPaintingIsUnclipped(true); }

void LedVu::paint(juce::Graphics &g)
{
    float diam = std::min<float>(getWidth(), getHeight()) - line_thickness_;
    juce::Path p;
    p.addEllipse(0, 0, diam, diam);

    juce::ColourGradient grad(juce::Colours::white, 0, 0, juce::Colours::green, diam, diam, true);
    g.setGradientFill(grad);
    g.fillPath(p);

    // Add the outer line.
    g.setColour(outline_col_);
    g.setOpacity(0.4f);
    g.drawEllipse(0, 0, diam, diam, line_thickness_);

    juce::DropShadow(juce::Colours::green.withAlpha(0.7f), 1, {}).drawForPath(g, p);
}

} // namespace sapphire
