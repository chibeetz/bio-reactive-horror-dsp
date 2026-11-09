/*
  ==============================================================================

    StutterEngine.h
    Created: 7 Nov 2026 5:37:00pm
    Author:  Daniel Okpara

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class StutterEngine
{
public:
    StutterEngine() = default;
    ~StutterEngine() = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock)
    {
        currentSampleRate = sampleRate;
        // Allocate a 2-second circular buffer for stereo audio
        bufferLength = static_cast<int>(sampleRate * 2.0);
        circularBuffer.setSize(2, bufferLength);
        circularBuffer.clear();
        writePos = 0;
        readPos = 0;
    }

    // --- MIDI CC Setter Methods ---
    void setStutterSizeNormalized (float normVal)
    {
        // Maps 0.0..1.0 to 10ms..500ms grain length
        float ms = 10.0f + (normVal * 490.0f);
        stutterSampleLength = static_cast<int>((ms / 1000.0f) * currentSampleRate);
        stutterSampleLength = juce::jmax(1, stutterSampleLength);
    }

    void setMix (float newMix)
    {
        mix = juce::jlimit(0.0f, 1.0f, newMix);
    }

    void setSpeed (float newSpeed)
    {
        speed = juce::jlimit(0.1f, 2.0f, newSpeed);
    }

    void setTriggerState (bool isTriggered)
    {
        if (isTriggered && !isGlitching)
        {
            readPos = writePos - stutterSampleLength;
            if (readPos < 0) readPos += bufferLength;
        }
        isGlitching = isTriggered;
    }

    // --- DSP Processing Loop ---
    void process (juce::AudioBuffer<float>& buffer)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        if (bufferLength == 0) return;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Continuously record incoming audio into the circular buffer
            for (int ch = 0; ch < numChannels; ++ch)
                circularBuffer.setSample(ch, writePos, buffer.getSample(ch, sample));

            if (isGlitching)
            {
                // Play back looped audio grain
                for (int ch = 0; ch < numChannels; ++ch)
                {
                    float dry = buffer.getSample(ch, sample);
                    float wet = circularBuffer.getSample(ch, readPos);
                    buffer.setSample(ch, sample, (dry * (1.0f - mix)) + (wet * mix));
                }

                readPos++;
                if (readPos >= writePos || readPos >= (writePos - writePos % stutterSampleLength) + stutterSampleLength)
                {
                    readPos = writePos - stutterSampleLength;
                    if (readPos < 0) readPos += bufferLength;
                }
            }

            writePos++;
            if (writePos >= bufferLength)
                writePos = 0;
        }
    }

private:
    juce::AudioBuffer<float> circularBuffer;
    int bufferLength = 0;
    int writePos = 0;
    int readPos = 0;
    int stutterSampleLength = 4410; // Default ~100ms
    double currentSampleRate = 44100.0;

    float mix = 0.5f;
    float speed = 1.0f;
    bool isGlitching = false;
};