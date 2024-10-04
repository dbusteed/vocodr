#include <stdlib.h>
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

#define N_CHANNELS 26
#define MAX_DELAY static_cast<size_t>(48000 * 0.75f)

DaisySeed hw;
HarmonicOscillator<4> harm;
Svf low_pass;
DelayLine<float, MAX_DELAY> delay;

Svf filters[N_CHANNELS][2];
float freq[N_CHANNELS] = {
	0.0,
	20.0,
	25.0,
	32.0,
	40.0,
	50.0,
	63.0,
	80.0,
	100.0,
	125.0,
	160.0,
	200.0,
	250.0,
	315.0,
	400.0,
	500.0,
	630.0,
	800.0,
	1000.0,
	1250.0,
	1600.0,
	2000.0,
	2500.0,
	3150.0,
	4000.0,
	5000.0,
	// 6300.0,
	// 8000.0,
	// 10000.0,
	// 12500.0,
	// 16000.0,
	// 20000.0,
	// 22025.0
};

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	float mod, car, sig, sig_out, delay_out, feedback;
	for (size_t i = 0; i < size; i++)
	{
		float harm_sig = harm.Process();
		
		sig = 0.0;
		for (size_t ch = 0; ch < N_CHANNELS; ch++) {
			// band pass filter
			filters[ch][0].Process(in[0][i]);
			mod = filters[ch][0].Band();
			
			// envelope detector
			mod = fabs(mod);
			
			// low pass filter
			filters[ch][0].Process(mod);
			mod = filters[ch][0].Low();
			
			// band pass on the harmonic signal
			filters[ch][1].Process(harm_sig);
			car = filters[ch][1].Band();

			// combind signals
			sig += (mod * car);
		}
	

		delay_out = delay.Read();
		sig_out = sig + delay_out;
		feedback = (delay_out * 0.1f) + sig;
		delay.Write(feedback);

		low_pass.Process(sig_out);
		sig_out = low_pass.Low();

		out[0][i] = sig_out;
		out[1][i] = sig_out;
	}	
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4);
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	
	float sample_rate = hw.AudioSampleRate();

	for (size_t i = 0; i < N_CHANNELS; i++) {
	    for (size_t j = 0; j < 2; j++) {
	        Svf filt;
			filt.Init(sample_rate);
			filt.SetFreq(freq[i]);
			filters[i][j] = filt;
	    }
	}

	harm.Init(sample_rate);
	harm.SetFirstHarmIdx(1);
	
	low_pass.Init(sample_rate);
	low_pass.SetFreq(800.0);

    delay.SetDelay(sample_rate * 0.1f);

	hw.StartAudio(AudioCallback);
	while(1) {}
}
