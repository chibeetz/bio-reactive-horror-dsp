/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AVGlitchEngineAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    AVGlitchEngineAudioProcessorEditor (AVGlitchEngineAudioProcessor&);
    ~AVGlitchEngineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AVGlitchEngineAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AVGlitchEngineAudioProcessorEditor)
};
