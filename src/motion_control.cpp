/**************************************************************************
作者：caidx1
功能：底盘运动控制器
**************************************************************************/
#include <ros/ros.h>
#include <signal.h>
#include <geometry_msgs/Twist.h>
#include <xf_mic_asr_offline/Set_Major_Mic_srv.h>
#include <xf_mic_asr_offline/position.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <std_msgs/Int32.h>
#include <std_msgs/Int8.h>
#include <std_msgs/String.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/PointStamped.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <move_base_msgs/MoveBaseActionResult.h>
//#include <move_base_msgs/MoveBaseAction.h>


using namespace std;
 
ros::Publisher cmd_vel_Pub;    //创建底盘速度控制话题发布者
geometry_msgs::Twist cmd_vel_msg;    //速度控制信息数据
ros::Publisher akm_navigation_pub;
//ros::Publisher goal_reach_pub;

float distance1;    //障碍物距离
float dis_angleX;    //障碍物方向
int angle = 0;    //当前唤醒角度
int follow_flag = 0;    //寻找声源标志位
int cmd_vel_flag = 0;    //底盘控制器标志位
int goal_control=0;
int overmap_flag =0;

float radar_range ;
int radar_count ;
double direction;

string reach = "" ;

void gaol_Callback(std_msgs::Int8 msg)
{
	goal_control = msg.data;
}

void overmap_Callback(std_msgs::Int8 msg)
{
	overmap_flag = msg.data;
}
/**************************************************************************
函数功能：sub回调函数
入口参数：  laserTracker.py
返回  值：无
**************************************************************************/
void current_position_Callback(const xf_mic_asr_offline::position& msg)	
{
	distance1 = msg.distance;
	dis_angleX = msg.angleX;

}

/**************************************************************************
函数功能：当前唤醒角度sub回调函数
入口参数：current_angle_msg  refresh_mic.cpp
返回  值：无
**************************************************************************/
void angle_Callback(std_msgs::Int32 msg)
{
	angle = msg.data;

}

/**************************************************************************
函数功能：底盘运动sub回调函数（原始数据）
入口参数：cmd_msg  command_recognition.cpp
返回  值：无
**************************************************************************/
void ori_vel_Callback(const geometry_msgs::Twist& msg)
{
	cmd_vel_msg.linear.x = msg.linear.x;
	cmd_vel_msg.angular.z = msg.angular.z; 
	
}

/**************************************************************************
函数功能：寻找声源标志位sub回调函数
入口参数：follow_flag_msg  command_recognition.cpp
返回  值：无
**************************************************************************/
void follow_flag_Callback(std_msgs::Int8 msg)
{
	follow_flag = msg.data;

}

/**************************************************************************
函数功能：底盘控制器标志位sub回调函数
入口参数：cmd_vel_flag_msg  command_recognition.cpp
返回  值：无
**************************************************************************/
void cmd_vel_flag_Callback(std_msgs::Int8 msg)
{
	cmd_vel_flag = msg.data;

}

/**************************************************************************
函数功能：中断程序
入口参数：
返回  值：无
**************************************************************************/
void shutdown(int sig)
{
	cmd_vel_Pub.publish(geometry_msgs::Twist());
	ROS_INFO("motion_control.cpp ended!");
	ros::shutdown();
}
 





void pose_Callback(const geometry_msgs::PoseWithCovarianceStamped& msg)
{
	//direction = msg.position.z;

	double orientation_z;    
	double orientation_w;
	double direction_r;
	orientation_z = msg.pose.pose.orientation.z;    //»ñÈ¡×îÐÂµÄÀï³ÌŒÆ·œÎ»Öµ
	orientation_w = msg.pose.pose.orientation.w;

	//printf("z = %f-----\n",orientation_z);
	//printf("w = %f-----\n",orientation_w);
	direction_r = atan2(2*(orientation_w*orientation_z),1-2*(pow(orientation_z,2.0)));

	direction = direction_r*180/3.1415926;
	if(direction<0)
		{direction+=360;}
}

void goal_reach_Callback(const move_base_msgs::MoveBaseActionResult& msg)
{
	reach = msg.status.text;
	if(reach == "Goal reached." && goal_control==1)//I、J、K点到达播报
	{
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/reach_goal.wav");
		goal_control=0;
	}
	else if(reach == "Goal reached." && overmap_flag==1)//建图完成播报
	{
		system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/overmap.wav");
		overmap_flag=0;
	}

}
/**************************************************************************
函数功能：寻找声源控制转向部分
入口参数：angle
返回  值：无
**************************************************************************/
void follow_turn(int angle)
{
	int ticks;    //计数变量
	float angular_turn_msg = 0.5;  //转向速度弧度制
	float angular_duration;    //转向时间
	float rate1 = 50;    //控制频率
	ros::Rate loopRate1(rate1);

	printf("angle =%d\n",angle);
	cmd_vel_msg.linear.x = 0;    //保持X轴方向速度为0
	
	/***控制不同方向转向***/
	if(angle<=180)
	{
		cmd_vel_msg.angular.z = -angular_turn_msg;
	}
	else
	{
		angle=360-angle;
		cmd_vel_msg.angular.z = angular_turn_msg;
	}
	
	angular_duration = angle / angular_turn_msg /180 * 3.14159;    //计算转弯时间
	printf("time =%f\n",angular_duration);
	ticks = int(angular_duration * rate1);    //计算控制次数
	printf("ticks =%d\n",ticks);

	/***控制转向***/
	for(int i = 10; i < ticks; i++)
	{
		cmd_vel_Pub.publish(cmd_vel_msg); // 将速度指令发送给机器人
		loopRate1.sleep();    //控制频率50Hz
		printf("i =%d\n",i);
	}
	cmd_vel_msg.angular.z = 0;    //速度置零
	cmd_vel_Pub.publish(geometry_msgs::Twist());
}

void akm_follow_turn(int angle)
{
	//suan jiaodu
	
	float angle_r;
	float goal_angle_r;
	float x_to_robot;
	float y_to_robot;
	tf::TransformListener listener;

	//printf("1111111111222222222222\n");

	angle_r=angle*3.1415926/180;
	//geometry_msgs::PoseStamped robot_point;
	geometry_msgs::PointStamped robot_point;
	geometry_msgs::PointStamped map_point;
	robot_point.header.frame_id = "base_footprint";
	robot_point.header.stamp = ros::Time();
        robot_point.point.x = cos(angle_r);
        robot_point.point.y = -sin(angle_r);
        robot_point.point.z = 0.0;	 

	try{
        
		listener.waitForTransform("map","base_footprint",ros::Time(0),ros::Duration(3.0));
        listener.transformPoint("map", robot_point, map_point);
       // printf("33333333333333333\n");
       }

        /*
        ROS_INFO("base_laser: (%.2f, %.2f. %.2f) -----> base_link: (%.2f, %.2f, %.2f) at time %.2f",
        laser_point.point.x, laser_point.point.y, laser_point.point.z,
        base_point.point.x, base_point.point.y, base_point.point.z, base_point.header.stamp.toSec());
        }
        */
        catch(tf::TransformException& ex){
                ROS_ERROR("Received an exception trying to transform a point from \"laser\" to \"base_link\": %s", ex.what());
        }

//fangxiang 
	goal_angle_r=(direction-angle)/180*3.1415926;
	printf("goal_angle_r=%f\n",goal_angle_r);
    	geometry_msgs::PoseStamped map_pose;
	map_pose.header.seq = 0;
	map_pose.header.frame_id = "map";
	map_pose.pose.orientation.x=0;
	map_pose.pose.orientation.y=0;
	map_pose.pose.orientation.z=sin(goal_angle_r/2);
	map_pose.pose.orientation.w=cos(goal_angle_r/2);

	map_pose.pose.position.x = map_point.point.x;
	map_pose.pose.position.y = map_point.point.y;
	map_pose.pose.position.z = 0;
	//printf("444444444444444\n");
	akm_navigation_pub.publish(map_pose);
	//printf("5555555555555555\n");
	while(reach!="Goal reached.")
	{
		ros::spinOnce();
	}
	cout<<"reach: "<<reach<<endl;
	//std_msgs::Int8 msg;
   // msg.data= 0;
    //goal_reach_pub.publish(msg);
    //printf("666666666666666\n");
	reach = "";
	printf("1111111111222222222222\n");
	cout<<"reach: "<<reach<<endl;
}



/**************************************************************************
函数功能：判断障碍物距离是否小于0.75米
入口参数：无
返回  值：1或0
**************************************************************************/
int distance_judgment(void)
{
	//int a;
	if(distance1<=radar_range) 
		return 1;
	else
		return 0;
	
}
 
/**************************************************************************
函数功能：判断障碍物方向是否在小车运动趋势方向上
入口参数：无
返回  值：1或0
**************************************************************************/
int dis_angleX_judgment(void)
{
	if(cmd_vel_msg.linear.x > 0 && (dis_angleX > 1.57 || dis_angleX < -1.57))
		return 1;

	else if(cmd_vel_msg.linear.x < 0 && dis_angleX > -1.57 && dis_angleX < 1.57)
		return 1;
		
	else if(cmd_vel_msg.angular.z > 0 && dis_angleX < 0)
		return 1;
		
	else if(cmd_vel_msg.angular.z <0 && dis_angleX > 0)
		return 1;
		
	else 
		return 0;
		
}

/**************************************************************************
函数功能：主函数
入口参数：无
返回  值：无
**************************************************************************/
int main(int argc, char** argv)
{
	int turn_fin_flag=0;    //完成转向标志位
	int temp_count = 0;    //计数变量
	string str1 = "遇到障碍物";    //障碍物字符串
	string if_akm ;
	int i;

	ros::init(argc, argv, "motion_ctrl");    //初始化ROS节点

	ros::NodeHandle node;    //创建句柄

	/***创建底盘速度控制话题发布者***/
	cmd_vel_Pub = node.advertise<geometry_msgs::Twist>("cmd_vel", 1);

	/***创建当I、J、K点到达标志位话题订阅者***/
	ros::Subscriber goal_sub = node.subscribe("goal_control_flag", 1, gaol_Callback);//

	/***创建建图完成标志位话题订阅者***/
	ros::Subscriber overmap_sub = node.subscribe("overmap_flag", 1, overmap_Callback);

	/***创建当前唤醒角度话题订阅者***/
	ros::Subscriber angle_sub = node.subscribe("current_angle", 1, angle_Callback);

	/***创建底盘运动话题订阅者***/
	ros::Subscriber vel_sub = node.subscribe("ori_vel", 1, ori_vel_Callback);

	/***创建寻找声源标志位话题订阅者***/
	ros::Subscriber follow_flag_sub = node.subscribe("follow_flag", 1, follow_flag_Callback);

	/***创建底盘运动控制器标志位话题订阅者***/
	ros::Subscriber cmd_vel_flag_sub = node.subscribe("cmd_vel_flag", 1, cmd_vel_flag_Callback);

  /***创建障碍物方位话题订阅者***/
	ros::Subscriber current_position_sub = node.subscribe("/object_tracker/current_position", 1, current_position_Callback);

	/***创建离线命令词识别结果话题发布者***/
	ros::Publisher control1 = node.advertise<std_msgs::String>("voice_words", 10);

	ros::Subscriber pose_sub = node.subscribe("/robot_pose_ekf/odom_combined",1,pose_Callback);

	ros::Publisher cmd_vel_flag_pub = node.advertise<std_msgs::Int8>("cmd_vel_flag",1);

	akm_navigation_pub = node.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal",1);

	ros::Subscriber goal_reach_sub = node.subscribe("/move_base/result", 1, goal_reach_Callback);

	//ros::Subscriber goal_reach_sub = node.subscribe("reach", 1, goal_reach_Callback);

	//goal_reach_pub = node.advertise<std_msgs::Int8>("reach", 1);
	

	//ros::ServiceClient Set_Major_Mic_client = node.serviceClient<xf_mic_asr_offline::Set_Major_Mic_srv>("xf_asr_offline_node/set_major_mic_srv");
	
 
	//execute a shutdown function when exiting
	signal(SIGINT, shutdown);
	ROS_INFO("motion_control.cpp start...");
	
	//xf_mic_asr_offline::Set_Major_Mic_srv num;
	//num.request.mic_id = 0;
	
	double rate2 = 10;    //频率10Hz
	ros::Rate loopRate2(rate2);


	node.param<float>("/radar_range", radar_range, 0.75);
	node.param("/radar_count", radar_count, 3);
	node.param("/if_akm_yes_or_no", if_akm, std::string("no")); 
	if (if_akm=="yes")
		i=1;
	else i=0;

	while(ros::ok())
	{
		if(follow_flag)    //寻找声源转向部分
		{
			if (if_akm=="yes")
			{
				//printf("1111111111111111111\n");
				akm_follow_turn(angle);
			}
			else
			{
				follow_turn(angle);
			}
				follow_flag = 0;
				//Set_Major_Mic_client.call(num);
				turn_fin_flag = 1;
				cmd_vel_msg.angular.z = 0;
				cmd_vel_msg.linear.x = 0.2;
			
		}

		if(cmd_vel_flag || turn_fin_flag)    //底盘运动控制部分
		{
			cmd_vel_Pub.publish(cmd_vel_msg);    //将速度指令发送给机器人

			if(cmd_vel_msg.linear.x ==0 && cmd_vel_msg.angular.z == 0)
			{
				std_msgs::Int8 cmd_vel_flag_msg;
        			cmd_vel_flag_msg.data = 0;
       				cmd_vel_flag_pub.publish(cmd_vel_flag_msg);
			}

			if(distance_judgment() && dis_angleX_judgment())    //判断障碍物的距离和方向
			{
				temp_count++;
				if(temp_count > radar_count)    //连续计数5️次停止运动防止碰撞，避免雷达有噪点
				{
					if(turn_fin_flag == 0)    //非寻找声源控制遇到障碍物则向用户界面发送提醒
					{
						std_msgs::String count_msg;
						count_msg.data = str1;
						control1.publish(count_msg);
						cmd_vel_msg.angular.z = 0;
						cmd_vel_msg.linear.x = 0;    //速度置零
						cmd_vel_Pub.publish(geometry_msgs::Twist());
						cmd_vel_flag = 0;    //各标志位置零
						turn_fin_flag = 0;
						temp_count = 0;
					}
					else
					{
					cmd_vel_msg.angular.z = 0;
					cmd_vel_msg.linear.x = 0;    //速度置零
					cmd_vel_Pub.publish(geometry_msgs::Twist());
					cmd_vel_flag = 0;    //各标志位置零
					turn_fin_flag = 0;
					temp_count = 0;
					system("aplay -D plughw:CARD=Device,DEV=0 ~/wheeltec_robot/src/xf_mic_asr_offline/feedback_voice/find.wav");
					}
				}
			}
			else temp_count = 0;    //排除雷达噪点
		}


		//printf("distance =%f\n",distance1);
		//printf("-----temp_count =%d\n",temp_count);
		//printf("-----radar_range =%f\n",radar_range);
		//cout<<"reach: "<<reach<<endl;
		ros::spinOnce();
		loopRate2.sleep();
	} 
	//return 0;
}

