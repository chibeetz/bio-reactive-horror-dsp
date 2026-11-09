/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AVGlitchEngineAudioProcessorEditor::AVGlitchEngineAudioProcessorEditor (AVGlitchEngineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    startTimerHz (30);
}

AVGlitchEngineAudioProcessorEditor::~AVGlitchEngineAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void AVGlitchEngineAudioProcessorEditor::timerCallback()
{
    repaint(); // Refresh screen to show parameter updates
}
void AVGlitchEngineAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1e1e24)); // Dark background

    g.setColour (juce::Colours::cyan);
    g.setFont (24.0f);
    g.drawFittedText ("STANDALONE AV GLITCH ENGINE", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);

    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    g.drawText ("Teensy 4.1 Status: Connected (MIDI CC 14-17)", 20, 80, 360, 30, juce::Justification::left);

    // Draw visual feedback box for parameters
    g.setColour (juce::Colour (0xff2a2a35));
    g.fillRect (20, 130, 360, 120);

    g.setColour (juce::Colours::lightgreen);
    g.drawText ("CC 14: Stutter Size (Pot A0)", 30, 140, 340, 25, juce::Justification::left);
    g.drawText ("CC 15: Wet / Dry Mix (Pot A1)", 30, 165, 340, 25, juce::Justification::left);
    g.drawText ("CC 16: Playback Speed (Pot A2)", 30, 190, 340, 25, juce::Justification::left);
    g.drawText ("CC 17: Glitch Trigger (Pin 2)", 30, 215, 340, 25, juce::Justification::left);
}

void AVGlitchEngineAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
