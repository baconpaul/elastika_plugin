#include "ElastikaProcessor.h"
#include "ElastikaEditor.h"

//==============================================================================
ElastikaAudioProcessor::ElastikaAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    engine = std::make_unique<Sapphire::ElastikaEngine>();
    addParameter(friction =
                     new juce::AudioParameterFloat({"friction", 1}, "Friction", 0.f, 1.f, 0.5f));
    addParameter(span = new juce::AudioParameterFloat({"span", 1}, "Span", 0.f, 1.f, 0.5f));
    addParameter(stiffness =
                     new juce::AudioParameterFloat({"stiffness", 1}, "Stiffness", 0.f, 1.f, 0.5f));
    addParameter(curl = new juce::AudioParameterFloat({"curl", 1}, "Curl", 0.f, 1.f, 0.0f));
    addParameter(mass = new juce::AudioParameterFloat({"mass", 1}, "Mass", 0.f, 1.f, 0.0f));
    addParameter(drive = new juce::AudioParameterFloat({"drive", 1}, "Drive", 0.f, 1.f, 1.0f));
    addParameter(gain = new juce::AudioParameterFloat({"gain", 1}, "Gain", 0.f, 1.f, 1.0f));
    addParameter(inputTilt =
                     new juce::AudioParameterFloat({"inputTilt", 1}, "InputTilt", 0.f, 1.f, 0.5f));
    addParameter(outputTilt = new juce::AudioParameterFloat({"outputTilt", 1}, "OutputTilt", 0.f,
                                                            1.f, 0.5f));
}

ElastikaAudioProcessor::~ElastikaAudioProcessor() {}

//==============================================================================
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

//==============================================================================
void ElastikaAudioProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    // Set sample rate
    sampleRate = sr;
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

    // FIXME - obviously use a smoothing strategy here but for now
    engine->setFriction(*friction);
    engine->setSpan(*span);
    engine->setCurl(*curl);
    engine->setMass(*mass);
    engine->setDrive(*drive);
    engine->setGain(*gain);
    engine->setInputTilt(*inputTilt);
    engine->setOutputTilt(*outputTilt);

    auto *isL = mainInput.getReadPointer(inChanL);
    auto *isR = mainInput.getReadPointer(inChanR);
    auto *osL = mainOutput.getWritePointer(0);
    auto *osR = mainOutput.getWritePointer(1);
    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        float opl, opr;
        engine->process(sampleRate, isL[s], isR[s], opl, opr);
        osL[s] = opl;
        osR[s] = opr;
    }
}

//==============================================================================
bool ElastikaAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *ElastikaAudioProcessor::createEditor()
{
    return new ElastikaEditor(*this);
}

//==============================================================================
void ElastikaAudioProcessor::getStateInformation(juce::MemoryBlock &destData) { jassertfalse; }

void ElastikaAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    jassertfalse;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new ElastikaAudioProcessor(); }
