#include <AudioRecorder.h>

using namespace std;
int AudioRecorder::business_proc_callback(business_msg_t businessMsg)
{
	int res = 0;
	char fileName[] = "./vvui_deno.pcm";
	char fileName_ori[] = "./vvui_ori.pcm";
	static int index = 0;
	unsigned char buf[4096];
	printf("business proc modId = %d, msgId = %d, size = %d", businessMsg.modId, businessMsg.msgId, businessMsg.length);
	switch (businessMsg.modId)
	{
	case 0x01:
		if (businessMsg.msgId == 0x01)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data,key);
			if (status == 0)
			{
				printf("您已开启录音");
			}
		}
		else if (businessMsg.msgId == 0x02)
		{
			char fileName[] = "../audio/vvui_deno.pcm";
			get_denoised_sound(fileName, businessMsg.data);
		}
		else if (businessMsg.msgId == 0x06)
		{
			char fileName_ori[] = "../audio/vvui_ori.pcm";
			get_original_sound(fileName_ori, businessMsg.data);
		}
		else if (businessMsg.msgId == 0x03)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data,key);
			if (status == 0)
			{
				printf("您已停止录音");
			}
		}
		else if (businessMsg.msgId == 0x04)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data,key);
			if (status == 0)
			{
				printf("开/关原始音频成功");
			}
		}
		else if (businessMsg.msgId == 0x05)
		{
			unsigned char key[] = "errcode";
			int status = whether_set_succeed(businessMsg.data,key);
			if (status == 0)
			{
				printf("识音角度设置成功");
			}
		}

		break;
	case 0x02:
		if (businessMsg.msgId == 0x01)
		{
			unsigned char key[] = "beam";
			int mic_id = get_mic_id(businessMsg.data,key);
			if (mic_id <= 5 && mic_id >= 0)
			{
				printf("第%d个麦克风被唤醒", mic_id);
				whether_awake = 1;
                if_success_boot = true;
			}
		}

		break;
	case 0x03:
		if (businessMsg.msgId == 0x01)
		{
			unsigned char key[] = "status";
			int status = whether_set_succeed(businessMsg.data,key);
			printf("系统%s", (status == 0 ? "正常开机" : "正在升级"));
			if (status == 1)
			{ 
				char* fileName = "../tmp/config.txt";
                send_file(fileName,0);
                //send_resource_info(fileName,0);
			}
			else if (status == 0)
			{
				whether_boot = 1;
                if_success_boot = true;
			}
		}
		break;
	case 0x04:
		if (businessMsg.msgId == 0x01)
		{
            whether_set_resource_info(businessMsg.data);
		}
		else if (businessMsg.msgId == 0x02) //3605D请求下载文件
		{
			char* fileName = "../tmp/system.tar";
			send_resource_info(fileName,1);
		}
		else if (businessMsg.msgId == 0x03) //文件接收结果
		{
            whether_set_resource_info(businessMsg.data);
		}
		else if (businessMsg.msgId == 0x05) //3605D请求下载文件
		{
            char fileName[] = "../tmp/system.tar";
			send_resource(businessMsg.data,fileName,1);
		}
		else if (businessMsg.msgId == 0x04) //3605D上报升级结果
		{
			whether_upgrade_succeed(businessMsg.data);
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

int main(){
    string path = "path";
    AudioRecorder recorder(path);
    int cmd;
    while (1)
	{

        cout << "input cmd" <<endl;
		scanf("%d", &cmd);
		if (cmd == 0)
			break;
		switch (cmd)
		{
        case 1:
            recorder.getStatus();
			
			break;
		case 2:
			if(recorder.startRecord()){
                cout << "startRecord sucess" <<endl;
            }
            else
            {
                cout << "startRecord fail" <<endl;
            }
			break;
		case 3:
			recorder.stopRecord();
			break;
		case 4:
			int angle = 60;
            if(recorder.setRecordAngle(angle)){
                cout << "setRecordAngle sucess" <<endl;
            }
			else
            {
                cout << "setRecordAngle fail" <<endl;
            }         
			break;
        
		}
		sleep(2);
	}
}