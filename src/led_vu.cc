#include "led_vu.h"

namespace sapphire
{

void LedVu::paint(juce::Graphics &g)
{
    float diam = std::min<float>(getWidth(), getHeight()) - outerLine;
    juce::LookAndFeel_V2::drawGlassSphere(g, 0, 0, diam, col, outerLine);
}

} // namespace sapphire
