#include <exception>
#include <unordered_map>
#include <utility>

#include "ElastikaProcessor.h"
#include "ElastikaEditor.h"
#include "ElastikaBinary.h"
#include "led_vu.h"
#include "sapphire_lnf.h"

using juce::SliderParameterAttachment;

namespace
{

// Helper function for placing a control exactly. Juce positioning doesn't take subpixel locations,
// unlike the rest of Juce. So apply a rounded version of that and add a transform that shunts it to
// the exact right location.
//
// cx and cy are the coordinates from the SVG. These are, for whatever reason, slightly off when
// used on top of our background. dx and dy are the correction for that off-ness (determined
// experimentally).
void set_control_position(juce::Component &control, float cx, float cy, float dx, float dy)
{
    juce::Point<float> real{cx + dx, cy + dy};
    juce::Point<int> rounded = real.toInt();
    control.setCentrePosition(rounded);
    control.setTransform(juce::AffineTransform::translation(real.getX() - rounded.getX(),
                                                            real.getY() - rounded.getY()));
}

} // namespace

ElastikaEditor::ElastikaEditor(ElastikaAudioProcessor &p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    auto knob_xml = juce::XmlDocument::parse(ElastikaBinary::knob_svg);
    auto marker_xml = juce::XmlDocument::parse(ElastikaBinary::knobmarker_svg);
    auto small_knob_xml = juce::XmlDocument::parse(ElastikaBinary::knobsmall_svg);
    auto small_marker_xml = juce::XmlDocument::parse(ElastikaBinary::knobmarkersmall_svg);
    lnf = std::make_unique<sapphire::LookAndFeel>(juce::Drawable::createFromSVG(*knob_xml),
                                                  juce::Drawable::createFromSVG(*marker_xml));
    small_lnf =
        std::make_unique<sapphire::LookAndFeel>(juce::Drawable::createFromSVG(*small_knob_xml),
                                                juce::Drawable::createFromSVG(*small_marker_xml));
    setLookAndFeel(lnf.get());

    setSize(300, 600);
    setResizable(true, true);

    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(ElastikaBinary::elastika_svg);
    background = juce::Drawable::createFromSVG(*xml);
    background->setInterceptsMouseClicks(false, true);
    addAndMakeVisible(*background);

    // Find controls.
    auto controls = xml->getChildByAttribute("id", "ControlLayer");
    if (!controls)
    {
        throw std::out_of_range("Missing ControlLayer in SVG");
    }
    std::unordered_map<std::string, std::pair<float, float>> found;
    for (const auto *control : controls->getChildWithTagNameIterator("circle"))
    {
        const juce::String &id = control->getStringAttribute("id");
        float cx = control->getStringAttribute("cx").getFloatValue();
        float cy = control->getStringAttribute("cy").getFloatValue();
        found.insert({id.toStdString(), {cx, cy}});
    }

    float x, y;
    // Add input tilt section.
    std::tie(x, y) = found.at("input_tilt_atten");
    tilt_in.atten = make_small_knob(x, y);
    std::tie(x, y) = found.at("input_tilt_cv");
    tilt_in.vu = make_led_vu(x, y, processor.internal_distortion);  // fixme
    std::tie(x, y) = found.at("input_tilt_knob");
    tilt_in.slider = make_large_knob(x, y);
    attachments.push_back(std::make_unique<SliderParameterAttachment>(*(processor.inputTilt.param),
                                                                      *(tilt_in.slider)));
    // Add output tilt section.
    std::tie(x, y) = found.at("output_tilt_atten");
    tilt_out.atten = make_small_knob(x, y);
    std::tie(x, y) = found.at("output_tilt_cv");
    tilt_out.vu = make_led_vu(x, y, processor.internal_distortion);  // fixme
    std::tie(x, y) = found.at("output_tilt_knob");
    tilt_out.slider = make_large_knob(x, y);
    attachments.push_back(std::make_unique<SliderParameterAttachment>(*(processor.outputTilt.param),
                                                                      *(tilt_out.slider)));
    // Add friction section.
    std::tie(x, y) = found.at("fric_atten");
    fric.atten = make_small_knob(x, y);
    std::tie(x, y) = found.at("fric_cv");
    // fric.vu = make_led_vu(x, y);
    std::tie(x, y) = found.at("fric_slider");
    fric.slider = make_slider(x, y);
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.friction.param), *(fric.slider)));
    // Add stiffness section.
    std::tie(x, y) = found.at("stif_atten");
    stif.atten = make_small_knob(x, y);
    std::tie(x, y) = found.at("stif_cv");
    // stif.vu = make_led_vu(x, y);
    std::tie(x, y) = found.at("stif_slider");
    stif.slider = make_slider(x, y);
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.stiffness.param), *(stif.slider)));
    // Add span section.
    std::tie(x, y) = found.at("span_atten");
    span.atten = make_small_knob(x, y);
    std::tie(x, y) = found.at("span_cv");
    // span.vu = make_led_vu(x, y);
    std::tie(x, y) = found.at("span_slider");
    span.slider = make_slider(x, y);
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.span.param), *(span.slider)));
    // Add curl section.
    std::tie(x, y) = found.at("curl_atten");
    curl.atten = make_small_knob(x, y);
    std::tie(x, y) = found.at("curl_cv");
    // curl.vu = make_led_vu(x, y);
    std::tie(x, y) = found.at("curl_slider");
    curl.slider = make_slider(x, y);
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.curl.param), *(curl.slider)));
    // Add mass section.
    std::tie(x, y) = found.at("mass_atten");
    mass.atten = make_small_knob(x, y);
    std::tie(x, y) = found.at("mass_cv");
    // mass.vu = make_led_vu(x, y);
    std::tie(x, y) = found.at("mass_slider");
    mass.slider = make_slider(x, y);
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.mass.param), *(mass.slider)));
    // Input drive.
    std::tie(x, y) = found.at("drive_knob");
    in_drive = make_large_knob(x, y);
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.drive.param), *(in_drive)));
    std::tie(x, y) = found.at("audio_left_input");
    inl_vu = make_led_vu(x, y, processor.inl_level);
    std::tie(x, y) = found.at("audio_right_input");
    inr_vu = make_led_vu(x, y, processor.inr_level);
    // Output level.
    std::tie(x, y) = found.at("level_knob");
    out_level = make_large_knob(x, y);
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.gain.param), *(out_level)));
    std::tie(x, y) = found.at("audio_left_output");
    outl_vu = make_led_vu(x, y, processor.outl_level);
    std::tie(x, y) = found.at("audio_right_output");
    outr_vu = make_led_vu(x, y, processor.outr_level);
    // Limiter warning light.
    std::tie(x, y) = found.at("power_toggle");
    limiter_warning = make_led_vu(x, y, processor.internal_distortion);
    // Power toggle. FIXME: add.
    std::tie(x, y) = found.at("power_gate_input");

    resized();
}

ElastikaEditor::~ElastikaEditor() {}

void ElastikaEditor::resized()
{
    if (background)
    {
        background->setTransformToFit(getLocalBounds().toFloat(), juce::RectanglePlacement());
    }
}

std::unique_ptr<juce::Slider> ElastikaEditor::make_large_knob(float cx, float cy)
{
    static constexpr float dx = 0.5f;
    static constexpr float dy = 0.5f;
    auto kn = std::make_unique<juce::Slider>();
    kn->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    kn->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    kn->setPopupMenuEnabled(true);
    background->addAndMakeVisible(*kn);
    kn->setSize(11, 11);
    kn->setRange(0, 1);
    kn->setValue(0.5);
    kn->setDoubleClickReturnValue(true, 0.5);
    set_control_position(*kn, cx, cy, dx, dy);
    return kn;
}

std::unique_ptr<juce::Slider> ElastikaEditor::make_small_knob(float cx, float cy)
{
    static constexpr float dx = 0.9166f;
    static constexpr float dy = 0.9166f;
    auto kn = std::make_unique<juce::Slider>();
    kn->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    kn->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    kn->setPopupMenuEnabled(true);
    background->addAndMakeVisible(*kn);
    kn->setLookAndFeel(small_lnf.get());
    kn->setSize(6, 6);
    set_control_position(*kn, cx, cy, dx, dy);
    return kn;
}

std::unique_ptr<sapphire::LedVu> ElastikaEditor::make_led_vu(float cx, float cy,
                                                             const std::atomic<float> &source)
{
    static constexpr float dx = 0.5f;
    static constexpr float dy = 0.5f;
    auto led = std::make_unique<sapphire::LedVu>(source);
    background->addAndMakeVisible(*led);
    led->setSize(3, 3);
    set_control_position(*led, cx, cy, dx, dy);
    return led;
}

std::unique_ptr<juce::Slider> ElastikaEditor::make_slider(float cx, float cy)
{
    static constexpr float dx = 0.6875f;
    static constexpr float dy = 0.6875f;
    auto sl = std::make_unique<juce::Slider>();
    sl->setSliderStyle(juce::Slider::LinearVertical);
    sl->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    background->addAndMakeVisible(*sl);
    sl->setSize(8, 28);
    sl->setMouseDragSensitivity(28);
    sl->setRange(0, 1);
    sl->setValue(0.5);
    sl->setDoubleClickReturnValue(true, 0.5);
    // TODO: Get the "snap to mouse position" to work with the scaling we have where we only use 90%
    // of the track (the remaining 10% is for the bottom part of the thumb; the thumb's "position"
    // is the very top pixel of the thumb). Until then, it doesn't work right throughout the whole
    // track, so we set this to false.
    sl->setSliderSnapsToMousePosition(false);
    set_control_position(*sl, cx, cy, dx, dy);
    return sl;
}
