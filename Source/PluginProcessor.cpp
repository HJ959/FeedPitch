/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FeedPitchAudioProcessor::FeedPitchAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

FeedPitchAudioProcessor::~FeedPitchAudioProcessor()
{
}

//==============================================================================
const String FeedPitchAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FeedPitchAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FeedPitchAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FeedPitchAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FeedPitchAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FeedPitchAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FeedPitchAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FeedPitchAudioProcessor::setCurrentProgram (int index)
{
}

const String FeedPitchAudioProcessor::getProgramName (int index)
{
    return {};
}

void FeedPitchAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FeedPitchAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void FeedPitchAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FeedPitchAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FeedPitchAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // raw volume
    rawVolume = 0.015;
    
    // pitch shift value
    pitchShift = 2;
    fftFrameSize = 32;
    osamp = 8;
    sampleRate = 441000;
    
    // for every channel in the available channels list
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // get the channel data from the buffer of the channel
        auto* channelDataOut = buffer.getWritePointer (channel);
        auto* channelDataIn = buffer.getReadPointer (channel);
        
        smbPitchShift(pitchShift,
                      buffer.getNumSamples(),
                      fftFrameSize,
                      osamp,
                      sampleRate,
                      channelDataIn,
                      channelDataOut);
        
        // for every sample within that channel buffer
        //for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        //{
            // multiply each sample with the volume silder
            //channelDataOut[sample] = buffer.getSample(channel, sample) * rawVolume;
        //}
    }
}

//==============================================================================
bool FeedPitchAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FeedPitchAudioProcessor::createEditor()
{
    return new FeedPitchAudioProcessorEditor (*this);
}

//==============================================================================
void FeedPitchAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FeedPitchAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FeedPitchAudioProcessor();
}

//==============================================================================

void FeedPitchAudioProcessor::smbPitchShift(float pitchShift,
                                            long numSampsToProcess,
                                            long fftFrameSize,
                                            long osamp,
                                            float sampleRate,
                                            const float *indata,
                                            float *outdata)
/*
 Routine smbPitchShift(). See top of file for explanation
 Purpose: doing pitch shifting while maintaining duration using the Short
 Time Fourier Transform.
 Author: (c)1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
 */
{
    
    static float gInFIFO[MAX_FRAME_LENGTH];
    static float gOutFIFO[MAX_FRAME_LENGTH];
    static float gFFTworksp[2*MAX_FRAME_LENGTH];
    static float gLastPhase[MAX_FRAME_LENGTH/2+1];
    static float gSumPhase[MAX_FRAME_LENGTH/2+1];
    static float gOutputAccum[2*MAX_FRAME_LENGTH];
    static float gAnaFreq[MAX_FRAME_LENGTH];
    static float gAnaMagn[MAX_FRAME_LENGTH];
    static float gSynFreq[MAX_FRAME_LENGTH];
    static float gSynMagn[MAX_FRAME_LENGTH];
    static long gRover = false, gInit = false;
    double magn, phase, tmp, window, real, imag;
    double freqPerBin, expct;
    long i,k, qpd, index, inFifoLatency, stepSize, fftFrameSize2;
    
    /* set up some handy variables */
    fftFrameSize2 = fftFrameSize/2;
    stepSize = fftFrameSize/osamp;
    freqPerBin = sampleRate/(double)fftFrameSize;
    expct = 2.*M_PI*(double)stepSize/(double)fftFrameSize;
    inFifoLatency = fftFrameSize-stepSize;
    if (gRover == false) gRover = inFifoLatency;
    
    /* initialize our static arrays */
    if (gInit == false) {
        memset(gInFIFO, 0, MAX_FRAME_LENGTH*sizeof(float));
        memset(gOutFIFO, 0, MAX_FRAME_LENGTH*sizeof(float));
        memset(gFFTworksp, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
        memset(gLastPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
        memset(gSumPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
        memset(gOutputAccum, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
        memset(gAnaFreq, 0, MAX_FRAME_LENGTH*sizeof(float));
        memset(gAnaMagn, 0, MAX_FRAME_LENGTH*sizeof(float));
        gInit = true;
    }
    
    /* main processing loop */
    for (i = 0; i < numSampsToProcess; i++){
        
        /* As long as we have not yet collected enough data just read in */
        gInFIFO[gRover] = indata[i];
        outdata[i] = gOutFIFO[gRover-inFifoLatency];
        gRover++;
        
        /* now we have enough data for processing */
        if (gRover >= fftFrameSize) {
            gRover = inFifoLatency;
            
            /* do windowing and re,im interleave */
            for (k = 0; k < fftFrameSize;k++) {
                window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize)+.5;
                gFFTworksp[2*k] = gInFIFO[k] * window;
                gFFTworksp[2*k+1] = 0.;
            }
            
            
            /* ***************** ANALYSIS ******************* */
            /* do transform */
            smbFft(gFFTworksp, fftFrameSize, -1);
            
            /* this is the analysis step */
            for (k = 0; k <= fftFrameSize2; k++) {
                
                /* de-interlace FFT buffer */
                real = gFFTworksp[2*k];
                imag = gFFTworksp[2*k+1];
                
                /* compute magnitude and phase */
                magn = 2.*sqrt(real*real + imag*imag);
                phase = atan2(imag,real);
                
                /* compute phase difference */
                tmp = phase - gLastPhase[k];
                gLastPhase[k] = phase;
                
                /* subtract expected phase difference */
                tmp -= (double)k*expct;
                
                /* map delta phase into +/- Pi interval */
                qpd = tmp/M_PI;
                if (qpd >= 0) qpd += qpd&1;
                else qpd -= qpd&1;
                tmp -= M_PI*(double)qpd;
                
                /* get deviation from bin frequency from the +/- Pi interval */
                tmp = osamp*tmp/(2.*M_PI);
                
                /* compute the k-th partials' true frequency */
                tmp = (double)k*freqPerBin + tmp*freqPerBin;
                
                /* store magnitude and true frequency in analysis arrays */
                gAnaMagn[k] = magn;
                gAnaFreq[k] = tmp;
                
            }
            
            /* ***************** PROCESSING ******************* */
            /* this does the actual pitch shifting */
            memset(gSynMagn, 0, fftFrameSize*sizeof(float));
            memset(gSynFreq, 0, fftFrameSize*sizeof(float));
            for (k = 0; k <= fftFrameSize2; k++) {
                index = k*pitchShift;
                if (index <= fftFrameSize2) {
                    gSynMagn[index] += gAnaMagn[k];
                    gSynFreq[index] = gAnaFreq[k] * pitchShift;
                }
            }
            
            /* ***************** SYNTHESIS ******************* */
            /* this is the synthesis step */
            for (k = 0; k <= fftFrameSize2; k++) {
                
                /* get magnitude and true frequency from synthesis arrays */
                magn = gSynMagn[k];
                tmp = gSynFreq[k];
                
                /* subtract bin mid frequency */
                tmp -= (double)k*freqPerBin;
                
                /* get bin deviation from freq deviation */
                tmp /= freqPerBin;
                
                /* take osamp into account */
                tmp = 2.*M_PI*tmp/osamp;
                
                /* add the overlap phase advance back in */
                tmp += (double)k*expct;
                
                /* accumulate delta phase to get bin phase */
                gSumPhase[k] += tmp;
                phase = gSumPhase[k];
                
                /* get real and imag part and re-interleave */
                gFFTworksp[2*k] = magn*cos(phase);
                gFFTworksp[2*k+1] = magn*sin(phase);
            }
            
            /* zero negative frequencies */
            for (k = fftFrameSize+2; k < 2*fftFrameSize; k++) gFFTworksp[k] = 0.;
            
            /* do inverse transform */
            smbFft(gFFTworksp, fftFrameSize, 1);
            
            /* do windowing and add to output accumulator */
            for(k=0; k < fftFrameSize; k++) {
                window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize)+.5;
                gOutputAccum[k] += 2.*window*gFFTworksp[2*k]/(fftFrameSize2*osamp);
            }
            for (k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];
            
            /* shift accumulator */
            memmove(gOutputAccum, gOutputAccum+stepSize, fftFrameSize*sizeof(float));
            
            /* move input FIFO */
            for (k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------


void FeedPitchAudioProcessor::smbFft(float *fftBuffer, long fftFrameSize, long sign)
/*
 FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
 Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
 time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
 and returns the cosine and sine parts in an interleaved manner, ie.
 fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
 must be a power of 2. It expects a complex input signal (see footnote 2),
 ie. when working with 'common' audio signals our input signal has to be
 passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
 of the frequencies of interest is in fftBuffer[0...fftFrameSize].
 */
{
    float wr, wi, arg, *p1, *p2, temp;
    float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
    long i, bitm, j, le, le2, k;
    
    for (i = 2; i < 2*fftFrameSize-2; i += 2) {
        for (bitm = 2, j = 0; bitm < 2*fftFrameSize; bitm <<= 1) {
            if (i & bitm) j++;
            j <<= 1;
        }
        if (i < j) {
            p1 = fftBuffer+i; p2 = fftBuffer+j;
            temp = *p1; *(p1++) = *p2;
            *(p2++) = temp; temp = *p1;
            *p1 = *p2; *p2 = temp;
        }
    }
    for (k = 0, le = 2; k < (long)(log(fftFrameSize)/log(2.)+.5); k++) {
        le <<= 1;
        le2 = le>>1;
        ur = 1.0;
        ui = 0.0;
        arg = M_PI / (le2>>1);
        wr = cos(arg);
        wi = sign*sin(arg);
        for (j = 0; j < le2; j += 2) {
            p1r = fftBuffer+j; p1i = p1r+1;
            p2r = p1r+le2; p2i = p2r+1;
            for (i = j; i < 2*fftFrameSize; i += le) {
                tr = *p2r * ur - *p2i * ui;
                ti = *p2r * ui + *p2i * ur;
                *p2r = *p1r - tr; *p2i = *p1i - ti;
                *p1r += tr; *p1i += ti;
                p1r += le; p1i += le;
                p2r += le; p2i += le;
            }
            tr = ur*wr - ui*wi;
            ui = ur*wi + ui*wr;
            ur = tr;
        }
    }
}


// -----------------------------------------------------------------------------------------------------------------

/*
 
 12/12/02, smb
 
 PLEASE NOTE:
 
 There have been some reports on domain errors when the atan2() function was used
 as in the above code. Usually, a domain error should not interrupt the program flow
 (maybe except in Debug mode) but rather be handled "silently" and a global variable
 should be set according to this error. However, on some occasions people ran into
 this kind of scenario, so a replacement atan2() function is provided here.
 
 If you are experiencing domain errors and your program stops, simply replace all
 instances of atan2() with calls to the smbAtan2() function below.
 
 */


double smbAtan2(double x, double y)
{
    double signx;
    if (x > 0.) signx = 1.;
    else signx = -1.;
    
    if (x == 0.) return 0.;
    if (y == 0.) return signx * M_PI / 2.;
    
    return atan2(x, y);
}


// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------