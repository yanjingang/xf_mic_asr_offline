
#include <ros/ros.h>
#include <signal.h>
#include <stdlib.h>
#include <joint.h>
#include <std_msgs/String.h>
#include <std_msgs/Int8.h>
#include <geometry_msgs/Twist.h>
#include "geometry_msgs/PoseStamped.h"

using namespace std;

ros::Publisher cmdvel_pub;     //速度信息发布者

geometry_msgs::Twist cmd_vel_msg;    //速度控制信息数据

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
		OTHER = (char*) "/feedback_voice/rplidar_open.wav";
		WHOLE = join((head + audio_path),OTHER);
		system(WHOLE);

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
		OTHER = (char*) "/feedback_voice/rrt_open.wav";
		WHOLE = join((head + audio_path),OTHER);
		system(WHOLE);
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
		OTHER = (char*) "/feedback_voice/visual_open.wav";
		WHOLE = join((head + audio_path),OTHER);
		system(WHOLE);
		cout<<"色块跟随打开成功"<<endl;
		
	}
	
		printf("%d\n",visual_follow_flag);


}

void voice_choose_callback(const std_msgs::String& msg)
{
	/***指令***/

	string str1 = msg.data.c_str();    //取传入数据
	string str2 = "关闭雷达跟随";
	string str3 = "关闭色块跟随";
	string str4 = "关闭自主建图";
	string str5 = "关闭导航";
	 

	if(str1 == str2)
	{
		OTHER = (char*) "/feedback_voice/rplidar_close.wav";
		WHOLE = join((head + audio_path),OTHER);
		sleep(2);
		system(WHOLE);
		cout<<"已关闭雷达跟随"<<endl;
	}


	else if(str1 == str3)
	{
		OTHER = (char*) "/feedback_voice/visual_close.wav";
		WHOLE = join((head + audio_path),OTHER);
		sleep(2);
		system(WHOLE);
		cout<<"已关闭色块跟随"<<endl;
	}


	else if(str1 == str4)
	{
		sleep(2);
		OTHER = (char*) "/feedback_voice/rrt_close.wav";
		WHOLE = join((head + audio_path),OTHER);
		system(WHOLE);

		cout<<"已关闭自主建图"<<endl;
	}

	else if(str1 == str5)
	{
		OTHER = (char*) "/feedback_voice/OK.wav";
		WHOLE = join((head + audio_path),OTHER);
		system(WHOLE);
		sleep(0.5);
		cmd_vel_msg.linear.x = 0;
		cmd_vel_msg.angular.z = 0;
		cmdvel_pub.publish(cmd_vel_msg);
		OTHER = (char*) "/feedback_voice/navigation_close.wav";
		WHOLE = join((head + audio_path),OTHER);
		sleep(2);
		system(WHOLE);
		cout<<"已关闭导航"<<endl;
	}

} 


int main(int argc, char** argv)
{

	ros::init(argc, argv, "feedback_node");  //初始化节点 

	ros::NodeHandle nha; //初始化句柄

	/***创建节点关闭判断语句订阅者***/

	ros::Subscriber voice_choose_sub = nha.subscribe("voice_words",1,voice_choose_callback);

	ros::Subscriber laser_follow_flag_sub = nha.subscribe("laser_follow_flag", 1, laser_follow_flagCallback);//雷达跟随开启标志位订阅

	ros::Subscriber visual_follow_flag_sub = nha.subscribe("visual_follow_flag", 1, visual_follow_flagCallback);//视觉跟随开启标志位订阅

	ros::Subscriber rrt_flag_sub = nha.subscribe("rrt_flag", 1, rrt_flagCallback);//自主探索建图开启标志位订阅

	nha.param("/node_feedback/audio_path", audio_path, std::string("~/catkin_ws/src/xf_mic_asr_offline/feedback_voice"));

	/***创建关闭速度信息发布者***/
	cmdvel_pub = nha.advertise<geometry_msgs::Twist>("cmd_vel", 1);

	ros::spin();

	return 0;

	
}

