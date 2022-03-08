
#include <ros/ros.h>
#include <signal.h>
#include <stdlib.h>
#include <std_msgs/Int8.h>
#include <std_msgs/String.h>
#include <stdio.h>
#include "geometry_msgs/PoseStamped.h"

using namespace std;

int laser_follow_flag = 0;    //雷达跟随标志位
int visual_follow_flag = 0;	  //色块跟随标志位
int rrt_flag = 0;	  //自主建图标志位



/**************************************************************************
函数功能：雷达跟随开启成功标志位sub回调函数
入口参数：laser_follow_flag.msg  laser.py
返回  值：无
**************************************************************************/
void laser_follow_flagCallback(std_msgs::Int8 msg)
{
	laser_follow_flag = msg.data;
	
	if(laser_follow_flag == 1)
	{
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/rplidar_open.wav");

		cout<<"雷达跟随打开成功"<<endl;
	}
	printf("%d\n",laser_follow_flag);
}

/**************************************************************************
函数功能：自主建图开启成功标志位sub回调函数
入口参数：rrt_flag.msg  wait_for_fin.cpp
返回  值：无
**************************************************************************/
void rrt_flagCallback(std_msgs::Int8 msg)
{
	rrt_flag = msg.data;
	if(rrt_flag == 1)
	{
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/rrt_open.wav");
		cout<<"自主建图打开成功"<<endl;
	}
	
		printf("%d\n",rrt_flag);

}

/**************************************************************************
函数功能：色块跟随开启成功标志位sub回调函数
入口参数：visual_follow_flag.msg  laser_follow.py
返回  值：无
**************************************************************************/
void visual_follow_flagCallback(std_msgs::Int8 msg)
{
	visual_follow_flag = msg.data;
	if(visual_follow_flag == 1)
	{
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/visual_open.wav");
		cout<<"色块跟随打开成功"<<endl;
		
	}
	
		printf("%d\n",visual_follow_flag);


}

int main(int argc, char** argv)
{

	ros::init(argc, argv, "node_open");  //初始化节点  

	ros::NodeHandle nd; //初始化句柄
	
	ros::Subscriber laser_follow_flag_sub = nd.subscribe("laser_follow_flag", 1, laser_follow_flagCallback);//雷达跟随开启标志位订阅

	ros::Subscriber visual_follow_flag_sub = nd.subscribe("visual_follow_flag", 1, visual_follow_flagCallback);//视觉跟随开启标志位订阅

	ros::Subscriber rrt_flag_sub = nd.subscribe("rrt_flag", 1, rrt_flagCallback);//自主探索建图开启标志位订阅

	ros::spin();



}

