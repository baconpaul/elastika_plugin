/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "ElastikaProcessor.h"
#include "ElastikaEditor.h"

//==============================================================================
ElastikaEditor::ElastikaEditor(ElastikaAudioProcessor &p)
    : juce::AudioProcessorEditor(&p), processor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(500, 350);
    setResizable(true, true);

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

    resized();
}

ElastikaEditor::~ElastikaEditor() {}

//==============================================================================
void ElastikaEditor::paint(juce::Graphics &g)
{
    juce::ColourGradient cg(juce::Colour(240, 220, 220), 0, 0, juce::Colour(220, 220, 220), 0, getHeight(),
                            false);
    g.setGradientFill(cg);
    g.fillRect(getLocalBounds());
    g.setColour(juce::Colours::black);
    g.setFont(30);
    g.drawText("Elastika", getLocalBounds(), juce::Justification::centredTop);
}

void ElastikaEditor::resized() {
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

}
