/*******************************************************
 This contents of this file may be used by anyone
 for any reason without any conditions and may be
 used as a starting point for your own applications
 which use HIDAPI.
********************************************************/
#include <ros/ros.h>
#include <std_msgs/Int32.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "user_interface.h"

//#include "xf_mic_package/xf_mic.h"

#define _GNU_SOURCE
#define TTS_TOPIC "/xf_tts"

using namespace std;
int write_first_data = 0;
int is_awake=0;
int mic_id;
/*获取文件大小*/
int FileSize(const char *fname)
{
	struct stat statbuf;
	if (stat(fname, &statbuf) == 0)
		return statbuf.st_size;
	return -1;
}

char *join(std::string b, char *s2)
{
	char s1[600] = "";
	try
	{
		strcpy(s1, b.c_str());
	}
	catch (...)
	{
		cout << ">>>>>join拷贝失败" << endl;
	}
	char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);
	if (result == NULL)
		exit(1);

	try
	{
		strcpy(result, s1);
		strcat(result, s2);
	}
	catch (...)
	{
		cout << ">>>>>join拷贝失败" << endl;
	}
	return result;
}

Effective_Result show_result(char *string) //
{
	Effective_Result current;
	if (strlen(string) > 250)
	{
		char asr_result[32];	//识别到的关键字的结果
		char asr_confidence[3]; //识别到的关键字的置信度
		char *p1 = strstr(string, "<rawtext>");
		char *p2 = strstr(string, "</rawtext>");
		int n1 = p1 - string + 1;
		int n2 = p2 - string + 1;

		char *p3 = strstr(string, "<confidence>");
		char *p4 = strstr(string, "</confidence>");
		int n3 = p3 - string + 1;
		int n4 = p4 - string + 1;
		for (int i = 0; i < 32; i++)
		{
			asr_result[i] = '\0';
		}

		strncpy(asr_confidence, string + n3 + strlen("<confidence>") - 1, n4 - n3 - strlen("<confidence>"));
		asr_confidence[n4 - n3 - strlen("<confidence>")] = '\0';
		int confidence_int = 0;
		confidence_int = atoi(asr_confidence);
		if (confidence_int >= confidence)
		{
			strncpy(asr_result, string + n1 + strlen("<rawtext>") - 1, n2 - n1 - strlen("<rawtext>"));
			asr_result[n2 - n1 - strlen("<rawtext>")] = '\0'; //加上字符串结束符。
		}
		else
		{
			strncpy(asr_result, "", 0);
		}

		current.effective_confidence = confidence_int;
		strcpy(current.effective_word, asr_result);
		return current;
	}
	else
	{
		current.effective_confidence = 0;
		strcpy(current.effective_word, " ");
		return current;
	}
}

int business_proc_callback(business_msg_t businessMsg)
{
	int res = 0;
	char *fileName = join(source_path, DENOISE_SOUND_PATH);
	char *fileName_ori = join(source_path, ORIGINAL_SOUND_PATH);
	static int index = 0;
	unsigned char buf[4096];
	//printf("business proc modId = %d, msgId = %d, size = %d", businessMsg.modId, businessMsg.msgId, businessMsg.length);
	//printf(businessMsg.data);
	switch (businessMsg.modId)
	{
	case 0x01:
		if (businessMsg.msgId == 0x01)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				printf(">>>>>您已开启录音\n");
			}
		}
		else if (businessMsg.msgId == 0x02)
		{
			int len = PCM_MSG_LEN;
			char *pcm_buffer = new char[len]; //在堆中创建空间
#if whether_print_log
			if (pcm_buffer == NULL)
			{
				cout << "buffer is null" << endl;
			}
			else
			{
				cout << "buffer alloced successfully" << endl;
			}
			//cout << "data size:" << businessMsg.length << "len:" << len << endl;
#endif
			try
			{
				memcpy(pcm_buffer, businessMsg.data, len);
			}
			catch (...)
			{
				cout << ">>>>>拷贝失败" << endl;
			}
			if (businessMsg.length < len)
			{
				len = businessMsg.length;
				cout << "businessMsg size is noenough" << endl;
			}
			if (save_pcm_local)
			{
				char *denoise_sound_path = join(source_path, DENOISE_SOUND_PATH);
				if (-1 != FileSize(denoise_sound_path))
				{
					int file_size = FileSize(denoise_sound_path);
					if (file_size > max_pcm_size) //超出最大文件限制,将删除,以节省磁盘空间
					{
						remove(denoise_sound_path);
					}
				}
				get_denoised_sound(denoise_sound_path, businessMsg.data);
			}
			/*写入第一块音频*/
			// if(is_awake)
			// {
			if (write_first_data++ == 0)
			{
#if whether_print_log
				printf("***************write the first voice**********\n");
#endif
				demo_xf_mic(pcm_buffer, len, 1);
			}

			else
			{
#if whether_print_log
				printf("***************write the middle voice**********\n");
#endif
				demo_xf_mic(pcm_buffer, len, 2);
			}
		}
		else if (businessMsg.msgId == 0x03)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				printf(">>>>>您已停止录音\n");
			}
		}
		else if (businessMsg.msgId == 0x04)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				printf(">>>>>开/关原始音频成功\n");
			}
		}
		else if (businessMsg.msgId == 0x05)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data, key);
			if (status == 0)
			{
				printf(">>>>>设置主麦克风成功\n");
			}
		}
		else if (businessMsg.msgId == 0x06)
		{
			get_original_sound(fileName_ori, businessMsg.data);
		}
		else if (businessMsg.msgId == 0x07)
		{
			unsigned char key1[] = "errcode";
			unsigned char key2[] = "beam";
			int status = whether_set_succeed(businessMsg.data, key1);
			int major_id = whether_set_succeed(businessMsg.data, key2);
			if (status == 0)
			{
				printf(">>>>>主麦克风id为%d号麦克风\n", major_id + 1);
			}
			else
			{
				printf(">>>>>获取主麦克风id失败,请检测协议\n");
			}
		}
		break;
	case 0x02:
		if (businessMsg.msgId == 0x01)
		{
			unsigned char key1[] = "beam";
			unsigned char key2[] = "angle";
			mic_id = get_awake_mic_id(businessMsg.data, key1);
			int mic_angle = get_awake_mic_angle(businessMsg.data, key2);
			if (mic_id <= 5 && mic_id >= 0 && mic_angle <= 360 && mic_angle >= 0)
			{
				is_awake = 1;
				led_id = get_led_based_angle(mic_angle);
				set_major_mic_led_on(mic_id, led_id);
				printf(">>>>>第%d个麦克风被唤醒", mic_id);
				printf("唤醒角度为:%d", mic_angle);
				printf("已点亮%d灯\n", led_id);
			}
		}

		break;
	case 0x03:
		if (businessMsg.msgId == 0x01)
		{
			unsigned char key[] = "status";
			int status = whether_set_succeed(businessMsg.data, key);
			//printf("系统%s\n", (status == 0 ? "正常开机" : "正在升级"));
			if (status == 1)
			{
				char *fileName_config_path = join(source_path, SYSTEM_CONFIG_PATH);
				send_resource_info(fileName_config_path, 0);
				//printf(">>>>>开机中，请稍等！\n");
			}
			else
			{
				is_boot = 5;
			}
		}

		break;

	case 0x04:
		if (businessMsg.msgId == 0x01)
		{
			whether_set_resource_info(businessMsg.data);
		}
		else if (businessMsg.msgId == 0x03) //文件接收结果
		{
			whether_set_resource_info(businessMsg.data);
		}
		else if (businessMsg.msgId == 0x04) //查看设备升级结果
		{
			whether_upgrade_succeed(businessMsg.data);
		}
		else if (businessMsg.msgId == 0x05) //下发文件
		{
			char *fileName_system_path = join(source_path, SYSTEM_PATH);
			send_resource(businessMsg.data, fileName_system_path, 1);
		}
		else if (businessMsg.msgId == 0x08) //获取升级配置文件
		{
			printf("config.json: %s", businessMsg.data);
		}
		break;

	default:
		break;
	}
	return 0;
}

int Question_Ans(const char *data)
{
	int result;
	result = 0;
	if (strstr(data, "来了") != NULL)
	{
		result = 1;
	}
	else if (strstr(data, "到了") != NULL)
	{
		result = 2;
	}
	

	return result;
}

int main(int argc, char *argv[])
{
	ros::init(argc, argv, "xf_offline_node");
	ros::NodeHandle ndHandle("~");
	ndHandle.param("confidence", confidence, 50);
	ndHandle.param("seconds_per_order", time_per_order, 3); //921600 115200
	ndHandle.param("source_path", source_path, std::string("./"));
	ndHandle.param("appid", appid, std::string("5e159d11"));
    ros::Publisher pub = ndHandle.advertise<std_msgs::Int32>(TTS_TOPIC, 1);
    //ros::Publisher pub_cw = ndHandle.advertise<xf_mic_package::xf_mic>("xf_mic_msg", 1000);

	hid_device *handle;
	handle = hid_open();
	if (!handle)
	{
		printf("无法打开麦克风设备，尝试重新插拔进行测试\n");
		return -1;
	}
	printf("成功打开麦克风\n");


	
	protocol_proc_init(send_to_usb_device, recv_from_usb_device, business_proc_callback, err_proc);
	get_system_status();
	
	std::string begin = "fo|";
	std::string quit_begin = source_path;
   
	char *jet_path = join((begin + source_path), ASR_RES_PATH);
	char *grammer_path = join(source_path, GRM_BUILD_PATH);
	char *bnf_path = join(source_path, GRM_FILE);
    //IN_PCM = join(source_path, IN_PCM);
	//[1-1] 通用登录及语法构建
	//Recognise_Result inital = initial_asr_paramers(jet_path, grammer_path, bnf_path, LEX_NAME);
	sleep(1);



	if (!is_boot)
	{
		printf(">>>>>开机中，请稍等！\n");
	}
	while (!is_boot)
	{
		if (is_reboot)
		{
			break;
		}
	}
	printf(">>>>>开机成功！\n");
	//ros::Rate loop_rate(10);
	while (ros::ok())
	{
		if (!is_awake)
		{
			printf(">>>>>待唤醒，请用唤醒词进行唤醒！\n");
		}
		while (!is_awake)
		{
			sleep(1);
		}


		set_major_mic_led_on(mic_id, led_id);

		
		start_to_record_denoised_sound();
		/*[1-2].开始创建一次语音识别了,首先传递了一些参数,作为QISRbegin()的输入]*/
		int ret = 0;
		ret = create_asr_engine(&asr_data);
		if (MSP_SUCCESS != ret)
		{
#if whether_print_log
			printf("[01]创建语音识别引擎失败！\n");
#endif
		}
#if whether_print_log
		printf("[1]开始一次语音识别！\n");
#endif
		//获取当前时间
		clock_t start, finish;
		double total_time;
		start = clock();
		while (whether_finised != 1)
		{
			finish = clock();
			total_time = (double)(finish - start) / CLOCKS_PER_SEC;
			if (total_time > 10)
			{
				cout << "超出离线命令词最长识别时间\n"
					 << endl;
				break;
			}
		}
		finish_to_record_denoised_sound();
		sleep(1);

		if (whole_result)
		{
			//printf(">>>>>全部返回结果:　[ %s ]\n", whole_result);
			Effective_Result effective_ans = show_result(whole_result);
			if (effective_ans.effective_confidence >= confidence) //如果大于置信度阈值则进行显示或者其他控制操作
			{
				printf(">>>>>是否识别成功:　[ %s ]\n", "是");
				printf(">>>>>关键字的置信度: [ %d ]\n", effective_ans.effective_confidence);
				printf(">>>>>关键字识别结果: [ %s ]\n", effective_ans.effective_word);
				/*发布结果*/
				//control_jetbot(effective_ans.effective_word);
			}
			else
			{
				printf(">>>>>是否识别成功:　[ %s ]\n", "否");
				printf(">>>>>关键字的置信度: [ %d ]\n", effective_ans.effective_confidence);
				printf(">>>>>关键字置信度较低，文本不予显示\n");
			}
		}
		/*[1-3]语音识别结束]*/
		delete_asr_engine();
		write_first_data = 0;
		is_awake = 0;
		whether_finised = 0;
	}
	APPID = &appid[0];
	ros::waitForShutdown();
	hid_close();
	/* Free static HIDAPI objects. */
	hid_exit();
	return 0;
}
