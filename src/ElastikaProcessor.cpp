#include "ElastikaProcessor.h"
#include "ElastikaEditor.h"

ElastikaAudioProcessor::ElastikaAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    engine = std::make_unique<Sapphire::ElastikaEngine>();
    addParameter(friction.param =
                     new juce::AudioParameterFloat({"friction", 1}, "Friction", 0.f, 1.f, 0.5f));
    addParameter(span.param = new juce::AudioParameterFloat({"span", 1}, "Span", 0.f, 1.f, 0.5f));
    addParameter(stiffness.param =
                     new juce::AudioParameterFloat({"stiffness", 1}, "Stiffness", 0.f, 1.f, 0.5f));
    addParameter(curl.param = new juce::AudioParameterFloat({"curl", 1}, "Curl", 0.f, 1.f, 0.0f));
    addParameter(mass.param = new juce::AudioParameterFloat({"mass", 1}, "Mass", 0.f, 1.f, 0.0f));
    addParameter(drive.param =
                     new juce::AudioParameterFloat({"drive", 1}, "Drive", 0.f, 1.f, 1.0f));
    addParameter(gain.param = new juce::AudioParameterFloat({"gain", 1}, "Gain", 0.f, 1.f, 1.0f));
    addParameter(inputTilt.param =
                     new juce::AudioParameterFloat({"inputTilt", 1}, "InputTilt", 0.f, 1.f, 0.5f));
    addParameter(outputTilt.param = new juce::AudioParameterFloat({"outputTilt", 1}, "OutputTilt",
                                                                  0.f, 1.f, 0.5f));
}

ElastikaAudioProcessor::~ElastikaAudioProcessor() {}

const juce::String ElastikaAudioProcessor::getName() const { return JucePlugin_Name; }

bool ElastikaAudioProcessor::acceptsMidi() const { return false; }

bool ElastikaAudioProcessor::producesMidi() const { return false; }

bool ElastikaAudioProcessor::isMidiEffect() const { return false; }

double ElastikaAudioProcessor::getTailLengthSeconds() const { return 20.0; }

int ElastikaAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
              // so this should be at least 1, even if you're not really implementing programs.
}

int ElastikaAudioProcessor::getCurrentProgram() { return 0; }

void ElastikaAudioProcessor::setCurrentProgram(int index) {}

const juce::String ElastikaAudioProcessor::getProgramName(int index) { return "Default"; }

void ElastikaAudioProcessor::changeProgramName(int index, const juce::String &newName) {}

void ElastikaAudioProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    // Set sample rate
    sampleRate = sr;
    perBlockRate = samplesPerBlock;
    last_block_size_ = perBlockRate;
    updateLagRates(perBlockRate);
}

void ElastikaAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool ElastikaAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
    bool inputValid = layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono() ||
                      layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo();

    bool outputValid = layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();

    return inputValid && outputValid;
}

void ElastikaAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                          juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto mainInput = getBusBuffer(buffer, true, 0);

    int inChanL = 0;
    int inChanR = 1;
    if (mainInput.getNumChannels() == 1)
        inChanR = 0;

    auto mainOutput = getBusBuffer(buffer, false, 0);

    // Snap lag target values.
    if (buffer.getNumSamples() != last_block_size_)
    {
        last_block_size_ = buffer.getNumSamples();
        updateLagRates(buffer.getNumSamples());
    }
    friction.updateLag();
    span.updateLag();
    curl.updateLag();
    mass.updateLag();
    drive.updateLag();
    gain.updateLag();
    inputTilt.updateLag();
    outputTilt.updateLag();

    auto *isL = mainInput.getReadPointer(inChanL);
    auto *isR = mainInput.getReadPointer(inChanR);
    auto *osL = mainOutput.getWritePointer(0);
    auto *osR = mainOutput.getWritePointer(1);
    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        updateEngineParameters();
        float opl, opr;
        engine->process(sampleRate, isL[s], isR[s], opl, opr);
        osL[s] = opl;
        osR[s] = opr;
    }
}

bool ElastikaAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *ElastikaAudioProcessor::createEditor()
{
    return new ElastikaEditor(*this);
}

void ElastikaAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    juce::XmlElement root{"elastika"};
    juce::XmlElement *params = root.createNewChildElement("parameters");
    for (const juce::AudioProcessorParameter *p : getParameters())
    {
        juce::XmlElement *e = params->createNewChildElement(p->getName(1000));
        // AudioProcessorParameter values are all floats in juce.
        e->setAttribute("value", p->getValue());
    }
    copyXmlToBinary(root, destData);
}

void ElastikaAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> root(getXmlFromBinary(data, sizeInBytes));
    if (!root || !root->hasTagName("elastika"))
    {
        return;
    }
    juce::XmlElement *params = root->getChildByName("parameters");
    if (!params)
    {
        return;
    }

    // Restore parameters.
    for (juce::AudioProcessorParameter *p : getParameters())
    {
        juce::XmlElement *e = params->getChildByName(p->getName(1000));
        if (e)
        {
            // AudioProcessorParameter values are all floats in juce.
            float val = static_cast<float>(e->getDoubleAttribute("value", p->getValue()));
            p->setValue(val);
        }
    }
}

void ElastikaAudioProcessor::updateLagRates(int samplesPerBlock)
{
    const float rate = 1.f / static_cast<float>(samplesPerBlock);
    friction.lag.setRate(rate);
    span.lag.setRate(rate);
    stiffness.lag.setRate(rate);
    curl.lag.setRate(rate);
    mass.lag.setRate(rate);
    drive.lag.setRate(rate);
    gain.lag.setRate(rate);
    inputTilt.lag.setRate(rate);
    outputTilt.lag.setRate(rate);
}

void ElastikaAudioProcessor::updateEngineParameters()
{
    // Possibly update engine parameters, if they're meaningfully different.
    if (juce::approximatelyEqual(friction.last_value, friction.lag.target_v))
    {
        friction.lag.process();
        engine->setFriction(friction.lag.v);
    }
    if (juce::approximatelyEqual(span.last_value, span.lag.target_v))
    {
        span.lag.process();
        engine->setSpan(span.lag.v);
    }
    if (juce::approximatelyEqual(stiffness.last_value, stiffness.lag.target_v))
    {
        stiffness.lag.process();
        engine->setStiffness(stiffness.lag.v);
    }
    if (juce::approximatelyEqual(curl.last_value, curl.lag.target_v))
    {
        curl.lag.process();
        engine->setCurl(curl.lag.v);
    }
    if (juce::approximatelyEqual(mass.last_value, mass.lag.target_v))
    {
        mass.lag.process();
        engine->setMass(mass.lag.v);
    }
    if (juce::approximatelyEqual(drive.last_value, drive.lag.target_v))
    {
        drive.lag.process();
        engine->setDrive(drive.lag.v);
    }
    if (juce::approximatelyEqual(gain.last_value, gain.lag.target_v))
    {
        gain.lag.process();
        engine->setGain(gain.lag.v);
    }
    if (juce::approximatelyEqual(inputTilt.last_value, inputTilt.lag.target_v))
    {
        inputTilt.lag.process();
        engine->setInputTilt(inputTilt.lag.v);
    }
    if (juce::approximatelyEqual(outputTilt.last_value, outputTilt.lag.target_v))
    {
        outputTilt.lag.process();
        engine->setOutputTilt(outputTilt.lag.v);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new ElastikaAudioProcessor(); }
