#include "ElastikaProcessor.h"
#include "ElastikaEditor.h"
#include "ElastikaBinary.h"
#include "sapphire_lnf.h"

ElastikaEditor::ElastikaEditor(ElastikaAudioProcessor &p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    auto knob_xml = juce::XmlDocument::parse(ElastikaBinary::knob_svg);
    auto marker_xml = juce::XmlDocument::parse(ElastikaBinary::knobmarker_svg);
    lnf = std::make_unique<sapphire::LookAndFeel>(juce::Drawable::createFromSVG(*knob_xml),
                                                  juce::Drawable::createFromSVG(*marker_xml));
    setLookAndFeel(lnf.get());

    setSize(300, 600);
    setResizable(true, true);

    std::unique_ptr<juce::XmlElement> xml = juce::XmlDocument::parse(ElastikaBinary::elastika_svg);
    background = juce::Drawable::createFromSVG(*xml);

    //  Find knobs.
    std::cout << "Top tag name: " << xml->getTagName() << std::endl;
    auto controls = xml->getChildByAttribute("id", "ControlLayer");
    if (!controls)
    {
        std::cout << "Big failure!" << std::endl;
        return;
    }
    for (const auto *control : controls->getChildWithTagNameIterator("circle"))
    {
        std::cout << "id: " << control->getStringAttribute("id")
                  << "; x: " << control->getStringAttribute("cx")
                  << "; y: " << control->getStringAttribute("cy") << std::endl;
        const juce::String &id = control->getStringAttribute("id");
        if (id.endsWith("slider"))
        {
            float cx = control->getStringAttribute("cx").getFloatValue();
            float cy = control->getStringAttribute("cy").getFloatValue();
            auto kn = std::make_unique<juce::Slider>();
            kn->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            kn->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
            background->addAndMakeVisible(*kn);
            kn->setSize(8, 8);
            juce::Point<float> real{cx + dx, cy + dy};
            juce::Point<int> rounded = real.toInt();
            kn->setCentrePosition(rounded);
            kn->setTransform(juce::AffineTransform::translation(real.getX() - rounded.getX(),
                                                                real.getY() - rounded.getY()));
            knobs.push_back(std::move(kn));
        }
    }

#if 0
      auto addKnob = [this](const std::string label, auto &paramRef) {
        auto kn = std::make_unique<juce::Slider>();
        kn->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        kn->setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        addAndMakeVisible(*kn);

        auto spa = std::make_unique<juce::SliderParameterAttachment>(paramRef, *kn);
        attachments.push_back(std::move(spa));
        knobs.push_back(std::move(kn));
        auto lb = std::make_unique<juce::Label>(label, label);
        lb->setColour(juce::Label::textColourId, juce::Colours::black);
        lb->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*lb);
        labels.push_back(std::move(lb));
    };
    addKnob("Friction", *(processor.friction));
    addKnob("Span", *(processor.span));
    addKnob("Curl", *(processor.curl));
    addKnob("Mass", *(processor.mass));
    addKnob("Drive", *(processor.drive));
    addKnob("Gain", *(processor.gain));
    addKnob("InTilt", *(processor.inputTilt));
    addKnob("OutTilt", *(processor.outputTilt));
#endif

    addAndMakeVisible(*background);
    resized();
}

ElastikaEditor::~ElastikaEditor() {}

void ElastikaEditor::resized()
{
    if (background)
    {
        background->setTransformToFit(getLocalBounds().toFloat(), juce::RectanglePlacement());
    }
#if 0
    auto slsz = getWidth() / 4 * 0.8;
    auto slmg = getWidth() / 4 * 0.1;
    auto p = getLocalBounds().withTrimmedTop(80).withWidth(slsz).withHeight(slsz).translated(slmg, 0);

    for (int i=0; i<knobs.size(); ++i)
    {
        knobs[i]->setBounds(p);
        auto l = p.translated(0, slsz + 2).withHeight(20);
        labels[i]->setBounds(l);;

        p = p.translated(slsz + 2 * slmg, 0);


        if (i == 3)
            p = getLocalBounds().withTrimmedTop(80 + slsz + 30).withWidth(slsz).withHeight(slsz).translated(slmg, 0);
    }
#endif
}
