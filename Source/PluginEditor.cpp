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

    pitchSlider.setRange (0.5, 2.0, 0.02);
    pitchSlider.setSliderStyle (Slider::Rotary);
    pitchSlider.setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    pitchSlider.setValue(processor.pitchShift);
    addAndMakeVisible(&pitchSlider);
    // add the listener to the slider
    pitchSlider.addListener (this);
    
    volumeSlider.setRange (0.0, 1.0, 0.01);
    volumeSlider.setSliderStyle (Slider::Rotary);
    volumeSlider.setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    // Keeps the slider from reseting when the plugin window is opened
    volumeSlider.setValue(processor.rawVolume);
    addAndMakeVisible(&volumeSlider);
    volumeSlider.addListener (this);
}

FeedPitchAudioProcessorEditor::~FeedPitchAudioProcessorEditor()
{
}

//==============================================================================
void FeedPitchAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    // fill the whole window white
    g.fillAll (Colours::white);
    
    // set the current drawing colour to black
    g.setColour (Colours::black);
    
    // set the font size and draw text to the screen
    g.setFont (15.0f);
}

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

void FeedPitchAudioProcessorEditor::sliderValueChanged(Slider *slider)
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

