/*********************************************************************
功能：刷新主麦方向
*********************************************************************/
#include <ros/ros.h>
#include <signal.h>
#include <xf_mic_asr_offline/Set_Major_Mic_srv.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <std_msgs/Int32.h>
#include <std_msgs/Int8.h>
//#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <nav_msgs/Odometry.h>

double orientation_z;    
double orientation_w;
double direction_r;
double direction;
double angle = 0;    //语音唤醒角度
double last_direction = 0;
int awake_flag = 0;    //唤醒标志位
int last_mic=0;    //主麦编号历史值
ros::ServiceClient Set_Major_Mic_client;    //定义设置主麦服务的客户端
ros::Publisher current_angle_pub;    //定义当前小车的相对唤醒角度



/*********************************************************************
函数功能：唤醒角度sub回调函数
入口参数：唤醒角度值awake_angle  voice_control.cpp
返回  值：无
*********************************************************************/
void angle_Callback(std_msgs::Int32 msg)
{
	angle = msg.data;    

	if(angle>=30 && angle<=90)    //判断唤醒时主麦方向
		last_mic = 1;
	else if(angle>=90 && angle<=150)
		last_mic = 2;
	else if(angle>=150 && angle<=210)
		last_mic = 3;
	else if(angle>=210 && angle<=270)
		last_mic = 4;
	else if(angle>=270 && angle<=330)
		last_mic = 5;
	else
		last_mic = 0;
	printf("last_mic_init = %d-----\n",last_mic);

}


/*********************************************************************
函数功能：唤醒标志sub回调函数
入口参数：唤醒标志位awake_flag_msg  voice_control.cpp
返回  值：无
*********************************************************************/
void awake_flag_Callback(std_msgs::Int8 msg)
{	
	last_direction = direction;
	awake_flag = msg.data;
	printf("awake_flag = %d\n",awake_flag);
	
}

 
/*********************************************************************
函数功能：小车姿态sub回调函数
入口参数：Pose  turn_on_wheeltec_robot.launch
返回  值：无
*********************************************************************/
//void pose_callback(const geometry_msgs::PoseWithCovarianceStamped& msg){
void pose_callback(const nav_msgs::Odometry& msg){
	orientation_z = msg.pose.pose.orientation.z;    //获取最新的里程计方位值
	orientation_w = msg.pose.pose.orientation.w;

	printf("z = %f-----\n",orientation_z);
	printf("w = %f-----\n",orientation_w);
	direction_r = atan2(2*(orientation_w*orientation_z),1-2*(pow(orientation_z,2.0)));

	direction = direction_r*180/3.1415926;
	if(direction<0)
		{direction+=360;}
	
	if(awake_flag){    //若在唤醒状态则进入主麦刷新检测
		//printf("direction = %f-----\n",direction);
		double dire_error = direction-last_direction;    //计算方向误差值，弧度制换算成角度制
		//printf("dire_error = %f-----\n",dire_error);
		angle+=dire_error;    //唤醒角度根据小车转向角度更新
		//printf("angle = %f-----\n",angle);
		last_direction = direction;    //当前方向替换成历史方向
		
		if(angle > 360)    //处理angle大于360或小于0 的情况
			angle-=360;
		else if(angle < 0) 
			angle+=360;
		
		//定义当前小车相对唤醒角度话题信息
		std_msgs::Int32 current_angle_msg;
		current_angle_msg.data = int(angle);
		current_angle_pub.publish(current_angle_msg);    //发布话题，用于小车寻找声源

		//定义设置主麦服务客户端信息
		xf_mic_asr_offline::Set_Major_Mic_srv num;
		if(angle>=30 && angle<=90)    //根据角度设定主麦方向
			num.request.mic_id = 1;
		else if(angle>=90 && angle<=150)
			num.request.mic_id = 2;
		else if(angle>=150 && angle<=210)
			num.request.mic_id = 3;
		else if(angle>=210 && angle<=270)
			num.request.mic_id = 4;
		else if(angle>=270 && angle<=330)
			num.request.mic_id = 5;
		else
			num.request.mic_id = 0;

		if(last_mic != num.request.mic_id)    //若计算当前主麦值与历史值不等，则重新刷新主麦编号
		{
			Set_Major_Mic_client.call(num);
			last_mic = num.request.mic_id;

		}
	}
}

/*********************************************************************
函数功能：主函数
入口参数：无
返回  值：无
*********************************************************************/
int main(int argc, char** argv)
{

	ros::init(argc, argv, "refresh_mic");    //初始化ROS节点

	ros::NodeHandle node;    //创建句柄

	//创建当前唤醒角度话题发布者
	current_angle_pub = node.advertise<std_msgs::Int32>("current_angle",1);

	//创建唤醒角度话题订阅者
	ros::Subscriber angle_sub = node.subscribe("/mic/awake/angle", 1, angle_Callback);

	//创建唤醒标志位话题订阅者
	ros::Subscriber awake_flag_sub = node.subscribe("awake_flag", 1, awake_flag_Callback);

	//创建小车姿态话题订阅者
	ros::Subscriber pose_sub = node.subscribe("/odom",1, pose_callback); //"/robot_pose_ekf/odom_combined",1,pose_callback);

	//创建设置主麦服务客户端
	Set_Major_Mic_client = node.serviceClient<xf_mic_asr_offline::Set_Major_Mic_srv>("voice_control/set_major_mic_srv");
	
	double rate2 = 5;    //刷新频率5Hz
	ros::Rate loopRate2(rate2);

	while(ros::ok()){
        ros::spinOnce();
        loopRate2.sleep();    
		//ros::spin();     
	}
	return 0;

}

