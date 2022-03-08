
#include <ros/ros.h>
#include <signal.h>
#include <stdlib.h>
#include <std_msgs/String.h>
#include <std_msgs/Int8.h>
#include <geometry_msgs/Twist.h>

using namespace std;

ros::Publisher cmdvel_pub;     //速度信息发布者

geometry_msgs::Twist cmd_vel_msg;    //速度控制信息数据

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
		system("rosnode kill /follower");
		cmd_vel_msg.linear.x = 0;
		cmd_vel_msg.angular.z = 0;
		cmdvel_pub.publish(cmd_vel_msg);
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/rplidar_close.wav");
		cout<<"已关闭雷达跟随"<<endl;
	}


	else if(str1 == str3)
	{
		system("rosnode kill /follower");
		cmd_vel_msg.linear.x = 0;
		cmd_vel_msg.angular.z = 0;
		cmdvel_pub.publish(cmd_vel_msg);
		system("rosnode kill /usb_cam");
		system("rosnode kill /visual_tracker");    
		system("rosnode kill /camera/driver");
		system("rosnode kill /camera/camera_nodelet_manager");
		system("rosnode kill /camera/depth_metric");
		system("rosnode kill /camera/depth_metric_rect");
		system("rosnode kill /camera/depth_points");
		system("rosnode kill /camera/depth_rectify_depth");
		system("rosnode kill /camera/depth_registered_hw_metric_rect");
		system("rosnode kill /camera/depth_registered_metric");
		system("rosnode kill /camera/depth_registered_rectify_depth");
		system("rosnode kill /camera/points_xyzrgb_hw_registered");
		system("rosnode kill /camera/rgb_rectify_color");
		system("rosnode kill /camera_base_link");
		system("rosnode kill /camera_base_link1");
		system("rosnode kill /camera_base_link2");
		system("rosnode kill /camera_base_link3");
		system("dbus-launch gnome-terminal -- roslaunch simple_follower laserTracker.launch");
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/visual_close.wav");
		cout<<"已关闭色块跟随"<<endl;
	}


	else if(str1 == str4)
	{
		system("rosnode kill /save_map");
		system("rosnode kill /slam_gmapping");
		system("rosnode kill /wait_for_fin");
		system("rosnode kill /filter");
		system("rosnode kill /local_detector");
		system("rosnode kill /global_detector");
		system("rosnode kill /move_base");
		system("dbus-launch gnome-terminal -- roslaunch xf_mic_asr_offline voi_navigation.launch");
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/rrt_close.wav");

		cout<<"已关闭自主建图"<<endl;
	}

	else if(str1 == str5)
	{
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/OK.wav");
		sleep(0.5);
		system("rosnode kill /send_mark_1");
		system("rosnode kill /move_base");
		system("rosnode kill /map_server_for_test");
		system("rosnode kill /amcl");
		cmd_vel_msg.linear.x = 0;
		cmd_vel_msg.angular.z = 0;
		cmdvel_pub.publish(cmd_vel_msg);
		system("dbus-launch gnome-terminal -- roslaunch xf_mic_asr_offline voi_navigation.launch");
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/navigation_close.wav");
		cout<<"已关闭导航"<<endl;
	}

} 


int main(int argc, char** argv)
{

	ros::init(argc, argv, "close_node");  //初始化节点 

	ros::NodeHandle nha; //初始化句柄

	/***创建节点关闭判断语句订阅者***/

	ros::Subscriber voice_choose_sub = nha.subscribe("voice_words",1,voice_choose_callback);

	/***创建关闭速度信息发布者***/
	cmdvel_pub = nha.advertise<geometry_msgs::Twist>("cmd_vel", 1);

	ros::spin();

	return 0;

	
}

