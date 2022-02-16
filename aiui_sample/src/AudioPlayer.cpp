#include <AudioPlayer.h>
#include <iostream>

char *AudioPlayer::device_name;

AudioPlayer::AudioPlayer()
{
	device_name = "default"; //"plughw:0,0"   "default"   /* playback device */
	sample_rat = 16000;
	channel_num = 1;
	// unsigned int * sample_rat_p = &sample_rat;
	// output = NULL;
	int err;
	if ((err = snd_pcm_open(&handle, device_name, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
	{
		fprintf(stderr, "cannot open audio device %s (%s)\n",
				device_name,
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
	{
		fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_any(handle, hw_params)) < 0)
	{
		fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		fprintf(stderr, "cannot set access type (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_format(handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
	{
		fprintf(stderr, "cannot set sample format (%s)\n",
				snd_strerror(err));
		exit(1);
	}
	if ((err = snd_pcm_hw_params_set_rate_near(handle, hw_params, &sample_rat, 0)) < 0)
	{
		fprintf(stderr, "cannot set sample rate (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params_set_channels(handle, hw_params, channel_num)) < 0)
	{
		fprintf(stderr, "cannot set channel count (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	if ((err = snd_pcm_hw_params(handle, hw_params)) < 0)
	{
		fprintf(stderr, "cannot set parameters (%s)\n",
				snd_strerror(err));
		exit(1);
	}

	snd_pcm_hw_params_free(hw_params);

	if ((err = snd_pcm_prepare(handle)) < 0)
	{
		fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
				snd_strerror(err));
		exit(1);
	}
	snd_pcm_hw_params_get_period_size(hw_params, &uframes, 0);
}
void AudioPlayer::Write(unsigned char *buf)
{
	int err, cptr;
	cptr = 512;
	while (cptr > 0)
	{
		err = snd_pcm_writei(handle, buf, cptr);
		sleep(10000);
		if (err == -EAGAIN)
			continue;
		if (err < 0)
		{
			// if (xrun_recovery(handle, err) < 0) {
			//     printf("Write error: %s\n", snd_strerror(err));
			//     exit(EXIT_FAILURE);
			// }
			std::cout << "skip one period" << std::endl;
			break; /* skip one period */
		}
		buf += err * 1;
		cptr -= err;
	}
}

int xrun_recovery(snd_pcm_t *handle, int err)
{
	if (err == -EPIPE)
	{ /* under-run */
		err = snd_pcm_prepare(handle);

		if (err < 0)
			printf("Can't recovery from underrun, prepare failed: %s\n",
				   snd_strerror(err));
		return 0;
	}
	else if (err == -ESTRPIPE)
	{
		while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			sleep(1); /* wait until the suspend flag is released */
		if (err < 0)
		{
			err = snd_pcm_prepare(handle);
			if (err < 0)
				printf("Can't recovery from suspend, prepare failed: %s\n",
					   snd_strerror(err));
		}
		return 0;
	}
	return err;
}

void AudioPlayer::Write(unsigned char *buf, int buf_len)
{
	int err;
	err = snd_pcm_writei(handle, buf, buf_len / 2); //采样位数/8*声道数,frame不是buffer数据的字节数， 而是采样数,一帧两次采样
	err = xrun_recovery(handle, err);
	if (err < 0)
	{
		printf("Write error: %s\n", snd_strerror(err));
	}
	// if (err == -EAGAIN)
	// {

	// 	std::cout << "-----------AudioPlayer::Write: err == -EAGAIN" << std::endl;
	// }

	// if (err < 0)
	// {
	// 	err = snd_pcm_recover(handle, err, 0);
	// 	std::cout << "-----------AudioPlayer::Write: snd_pcm_recover" << std::endl;
	// }
	//free(buffer);
}
void AudioPlayer::Clear_Write()
{
	int err;
	//err = snd_pcm_drop(handle);
//snd_pcm_drain(handle);
//snd_pcm_close(handle);
	// snd_pcm_resume(handle);
	snd_pcm_prepare(handle);
}
void AudioPlayer::WriteTest()
{
	// int err;
	// if ((err = snd_pcm_writei (handle, buf, 128)) != 128) {
	// 	fprintf (stderr, "write to audio interface failed (%s)\n",
	// 			snd_strerror (err));
	// 	exit (1);
	// }
	unsigned char samples[1280];
	unsigned char *ptr;
	int err, cptr;
	// while (1) {
	ptr = samples;
	cptr = 1280;
	while (cptr > 0)
	{
		err = snd_pcm_writei(handle, ptr, cptr);
		if (err == -EAGAIN)
			continue;
		if (err < 0)
		{
			std::cout << "snd_pcm_writei() err < 0" << std::endl;
			break;
		}
		ptr += err * 1;
		cptr -= err;
	}

	// }
}
AudioPlayer::~AudioPlayer()
{
	snd_pcm_close(handle);
}
void AudioPlayer::Test()
{
	// int i;
	// buffer[]
	printf("AudioPlayer::Test()\n");
	unsigned char buffer[1024];
	for (int i = 0; i < 160; i++)
	{
		int rc;
		rc = snd_pcm_writei(handle, buffer, sizeof(buffer));
		if (rc < 0)
			rc = snd_pcm_recover(handle, rc, 0);
		if (rc < 0)
		{
			printf("snd_pcm_writei failed: %s\n", snd_strerror(rc));
			break;
		}
		if (rc > 0 && rc < (long)sizeof(buffer))
			printf("Short write (expected %li, wrote %li)\n", (long)sizeof(buffer), rc);
		sleep(2);
	}
}
