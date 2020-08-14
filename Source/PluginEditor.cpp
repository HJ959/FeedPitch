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

    pitchSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    pitchSlider.setRange(0.0, 2.0);
    pitchSlider.setValue(1.0);
    pitchSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);
}

FeedPitchAudioProcessorEditor::~FeedPitchAudioProcessorEditor()
{
}

//==============================================================================
void FeedPitchAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    

}

void FeedPitchAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    pitchSlider.setBounds(getLocalBounds());
    
}

void FeedPitchAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
    if (slider == &pitchSlider)
    {
        processor.pitchShift = pitchSlider.getValue();
    }
}

