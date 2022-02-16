
#ifndef AUDIOPLAYER_H_
#define AUDIOPLAYER_H_

#include <alsa/asoundlib.h>

class AudioPlayer
{
private:
	static char *device_name;            /* pcm device */
	// snd_output_t *output;
	unsigned char pcm_buffer;              /* some sound data */
	// int err;
	
	// unsigned int i;
	snd_pcm_t *handle;
	snd_pcm_sframes_t sframes;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_uframes_t uframes;
public:
	unsigned int sample_rat;
	int channel_num;
	AudioPlayer();
	~AudioPlayer();
	void Write(unsigned char * buf);
	void Write(unsigned char * buf,int buf_len);
	void Clear_Write();
	void WriteTest();
	void Test();
	// PlaySound();
	// virtual bool startRecord();
	// virtual void stopRecord();
};
#endif /* AUDIOPLAYER_H_ */
