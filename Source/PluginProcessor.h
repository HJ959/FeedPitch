/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/****************************************************************************
 *
 * NAME: smbPitchShift.cpp
 * VERSION: 1.2
 * HOME URL: http://blogs.zynaptiq.com/bernsee
 * KNOWN BUGS: none
 *
 * SYNOPSIS: Routine for doing pitch shifting while maintaining
 * duration using the Short Time Fourier Transform.
 *
 * DESCRIPTION: The routine takes a pitchShift factor value which is between 0.5
 * (one octave down) and 2. (one octave up). A value of exactly 1 does not change
 * the pitch. numSampsToProcess tells the routine how many samples in indata[0...
 * numSampsToProcess-1] should be pitch shifted and moved to outdata[0 ...
 * numSampsToProcess-1]. The two buffers can be identical (ie. it can process the
 * data in-place). fftFrameSize defines the FFT frame size used for the
 * processing. Typical values are 1024, 2048 and 4096. It may be any value <=
 * MAX_FRAME_LENGTH but it MUST be a power of 2. osamp is the STFT
 * oversampling factor which also determines the overlap between adjacent STFT
 * frames. It should at least be 4 for moderate scaling ratios. A value of 32 is
 * recommended for best quality. sampleRate takes the sample rate for the signal
 * in unit Hz, ie. 44100 for 44.1 kHz audio. The data passed to the routine in
 * indata[] should be in the range [-1.0, 1.0), which is also the output range
 * for the data, make sure you scale the data accordingly (for 16bit signed integers
 * you would have to divide (and multiply) by 32768).
 *
 * COPYRIGHT 1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
 *
 *                         The Wide Open License (WOL)
 *
 * Permission to use, copy, modify, distribute and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice and this license appear in all source copies.
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
 * ANY KIND. See http://www.dspguru.com/wol.htm for more information.
 *
 *****************************************************************************/

#include <string.h>
#include <math.h>
#include <stdio.h>

#define MAX_FRAME_LENGTH 16

//==============================================================================
class FeedPitchAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FeedPitchAudioProcessor();
    ~FeedPitchAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    double rawVolume{-20.0f};
    double rawVolumePrev;
    float pitchShift{1.0f};
    float pitchShiftPrev;
    long fftFrameSize{16};
    long osamp{4};
    float sampleRate;
    float gInFIFO[MAX_FRAME_LENGTH];
    float gOutFIFO[MAX_FRAME_LENGTH];
    float gFFTworksp[2*MAX_FRAME_LENGTH];
    float gLastPhase[MAX_FRAME_LENGTH/2+1];
    float gSumPhase[MAX_FRAME_LENGTH/2+1];
    float gOutputAccum[2*MAX_FRAME_LENGTH];
    float gAnaFreq[MAX_FRAME_LENGTH];
    float gAnaMagn[MAX_FRAME_LENGTH];
    float gSynFreq[MAX_FRAME_LENGTH];
    float gSynMagn[MAX_FRAME_LENGTH];
    long gRover = false, gInit = false;
    double magn, phase, tmp, window, real, imag;
    double freqPerBin, expct;
    long i,k, qpd, index, inFifoLatency, stepSize, fftFrameSize2;

    double smbAtan2(double x, double y);
    void smbFft(float *fftBuffer, long fftFrameSize, long sign);
    
    void smbPitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, const float *indata, float *outdata);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FeedPitchAudioProcessor)
};
