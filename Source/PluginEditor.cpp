/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FeedPitchAudioProcessorEditor::FeedPitchAudioProcessorEditor (FeedPitchAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    pitchSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    pitchSlider.setRange (0.01f, 2.0f, 0.01f);
    pitchSlider.setValue(processor.pitchShift);
    pitchSlider.addListener (this);
    addAndMakeVisible(&pitchSlider);
    
    volumeSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    volumeSlider.setRange (-60.0f, 0.0f, 0.01f);
    volumeSlider.setValue(processor.rawVolume);
    volumeSlider.addListener (this);
    addAndMakeVisible(&volumeSlider);
}

FeedPitchAudioProcessorEditor::~FeedPitchAudioProcessorEditor()
{
}

//==============================================================================
void FeedPitchAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    // fill the whole window white
    g.fillAll (juce::Colours::white);
    
    // set the current drawing colour to black
    g.setColour (juce::Colours::black);
    
    // set the font size and draw text to the screen
    g.setFont (15.0f);
}
//==============================================================================
void FeedPitchAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //pitchSlider.setBounds(getLocalBounds());
    auto area = getLocalBounds();
    
    
    auto contentItemHeight = 200;
    pitchSlider.setBounds(area.removeFromTop (contentItemHeight)); // [1]
    volumeSlider.setBounds(area.removeFromTop (contentItemHeight));

    
}
//==============================================================================
void FeedPitchAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &pitchSlider)
    {
        processor.pitchShift = pitchSlider.getValue();
    }
    if (slider == &volumeSlider)
    {
        processor.rawVolume = volumeSlider.getValue();
    }
}
//==============================================================================
