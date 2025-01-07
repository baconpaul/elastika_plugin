#include <exception>
#include <unordered_map>
#include <utility>

#include "ElastikaProcessor.h"
#include "ElastikaEditor.h"
#include "ElastikaBinary.h"
#include "led_vu.h"
#include "sapphire_lnf.h"
#include "sapphire_panel.hpp"

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

    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(ElastikaBinary::elastika_svg);
    background = juce::Drawable::createFromSVG(*xml);
    background->setInterceptsMouseClicks(false, true);
    addAndMakeVisible(*background);


    tilt_in.atten = make_small_knob("input_tilt_atten");
    tilt_in.slider = make_large_knob("input_tilt_knob");
    tilt_in.vu = make_led_vu("input_tilt_cv", processor.inputTilt.level);
    attachments.push_back(std::make_unique<SliderParameterAttachment>(*(processor.inputTilt.param),
                                                                      *(tilt_in.slider)));

    tilt_out.atten = make_small_knob("output_tilt_atten");
    tilt_out.slider = make_large_knob("output_tilt_knob");
    tilt_out.vu = make_led_vu("output_tilt_cv", processor.outputTilt.level);
    attachments.push_back(std::make_unique<SliderParameterAttachment>(*(processor.outputTilt.param),
                                                                      *(tilt_out.slider)));

    // these are all missing vus
    fric.atten = make_small_knob("fric_atten");
    fric.slider = make_slider("fric_slider");
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.friction.param), *(fric.slider)));

    stif.atten = make_small_knob("stif_atten");
    stif.slider = make_slider("stif_slider");
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.stiffness.param), *(stif.slider)));

    span.atten = make_small_knob("span_atten");
    span.slider = make_slider("span_slider");
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.span.param), *(span.slider)));

    curl.atten = make_small_knob("curl_atten");
    curl.slider = make_slider("curl_slider");
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.curl.param), *(curl.slider)));

    mass.atten = make_small_knob("mass_atten");
    mass.slider = make_slider("mass_slider");
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.mass.param), *(mass.slider)));

    in_drive = make_large_knob("drive_knob");
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.drive.param), *(in_drive)));
    inl_vu = make_led_vu("audio_left_input", processor.inl_level);
    inr_vu = make_led_vu("audio_right_input", processor.inr_level);


    out_level = make_large_knob("level_knob");
    attachments.push_back(
        std::make_unique<SliderParameterAttachment>(*(processor.gain.param), *(out_level)));
    outl_vu = make_led_vu("audio_left_output", processor.outl_level);
    outr_vu = make_led_vu("audio_right_output", processor.outr_level);

    limiter_warning = make_led_vu("power_toggle", processor.internal_distortion);

    setSize(300, 600);
    setResizable(true, true);
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

std::unique_ptr<juce::Slider> ElastikaEditor::make_large_knob(const std::string &pos)
{
    auto r = Sapphire::FindComponent("elastika", pos);
    auto cx = r.cx;
    auto cy = r.cy;

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
    kn->setMouseDragSensitivity(100);
    kn->setDoubleClickReturnValue(true, 0.5);
    set_control_position(*kn, cx, cy, dx, dy);
    return kn;
}

std::unique_ptr<juce::Slider> ElastikaEditor::make_small_knob(const std::string &pos)
{
    auto r = Sapphire::FindComponent("elastika", pos);
    auto cx = r.cx;
    auto cy = r.cy;

    static constexpr float dx = 0.9166f;
    static constexpr float dy = 0.9166f;
    auto kn = std::make_unique<juce::Slider>();
    kn->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    kn->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    kn->setPopupMenuEnabled(true);
    kn->setMouseDragSensitivity(100);
    background->addAndMakeVisible(*kn);
    kn->setLookAndFeel(small_lnf.get());
    kn->setSize(6, 6);
    set_control_position(*kn, cx, cy, dx, dy);
    return kn;
}

std::unique_ptr<sapphire::LedVu> ElastikaEditor::make_led_vu(const std::string &pos,
                                                             const std::atomic<float> &source)
{
    auto r = Sapphire::FindComponent("elastika", pos);
    auto cx = r.cx;
    auto cy = r.cy;

    static constexpr float dx = 0.5f;
    static constexpr float dy = 0.5f;
    auto led = std::make_unique<sapphire::LedVu>(source);
    background->addAndMakeVisible(*led);
    led->setSize(3, 3);
    set_control_position(*led, cx, cy, dx, dy);
    return led;
}

std::unique_ptr<juce::Slider> ElastikaEditor::make_slider(const std::string &pos)
{
    auto r = Sapphire::FindComponent("elastika", pos);
    auto cx = r.cx;
    auto cy = r.cy;

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
