
#include <ros/ros.h>
#include <signal.h>
#include <stdlib.h>
#include <std_msgs/Int8.h>
#include <std_msgs/String.h>
#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/PoseStamped.h>

using namespace std;

int rrt_flag1 = 0;	  //自主建图标志位
int send_flag = 0;		//导航目标点标志位
ros::Publisher pub_goal;  //自主建图区域点话题发布者
ros::Publisher navigation_pub;    //创建导航目标点话题发布者
geometry_msgs::PointStamped target1;  //自主建图区域点消息数据:5个
geometry_msgs::PointStamped target2;
geometry_msgs::PointStamped target3;
geometry_msgs::PointStamped target4;
geometry_msgs::PointStamped target5;

geometry_msgs::PointStamped targetA;    //导航目标点消息数据:3个
geometry_msgs::PointStamped targetB;
geometry_msgs::PointStamped targetC;

float A_position_x ;
float A_position_y ;
float A_position_z ;

float B_position_x ;
float B_position_y ;
float B_position_z ;

float C_position_x ;
float C_position_y ;
float C_position_z ;

float D_position_x ;
float D_position_y ;
float D_position_z ;

float E_position_x ;
float E_position_y ;
float E_position_z ;

float O_position_x ;
float O_position_y ;
float O_position_z ;

float P_position_x ;
float P_position_y ;
float P_position_z ;

float Q_position_x ;
float Q_position_y ;
float Q_position_z ;


void voice_choosecallback(std_msgs::Int8 msg)
{
	send_flag = msg.data;
	if(send_flag == 1) //如果flag标志位为1这发布3个导航目标点
	{
		targetA.header.frame_id = "base_footprint";
		targetA.header.stamp = ros::Time();
    	targetA.point.x = O_position_x;
    	targetA.point.y = O_position_y;
    	targetA.point.z = O_position_z;	
		navigation_pub.publish(targetA);

		sleep(2.0);

		targetB.header.frame_id = "base_footprint";
		targetB.header.stamp = ros::Time();
    	targetB.point.x = P_position_x;
    	targetB.point.y = P_position_y;
    	targetB.point.z = P_position_z;	 
		navigation_pub.publish(targetB);

		sleep(2.0);

		targetC.header.frame_id = "base_footprint";
		targetC.header.stamp = ros::Time();
    	targetC.point.x = Q_position_x;
    	targetC.point.y = Q_position_y;
   	 	targetC.point.z = Q_position_z;	
		navigation_pub.publish(targetC);

		cout<<"已开始导航"<<endl;
  	}
}
/**************************************************************************
函数功能：自主建图区域点的设置
入口参数：rrt_flag.msg  wait_for_fin.cpp
返回  值：无
**************************************************************************/
void rrt_flag_Callback(std_msgs::Int8 msg)
{
	rrt_flag1 = msg.data;
	if(rrt_flag1 == 1)//如果flag标志位为1这发布5个自主建图区域点
	{
		target1.header.frame_id = "base_footprint";
		target1.header.stamp = ros::Time();
        target1.point.x = A_position_x;
        target1.point.y = A_position_y;
        target1.point.z = A_position_z;	 
        pub_goal.publish(target1);

        sleep(2);

        target2.header.frame_id = "base_footprint";
		target2.header.stamp = ros::Time();
        target2.point.x = B_position_x;
        target2.point.y = B_position_y;
        target2.point.z = B_position_z;	 
        pub_goal.publish(target2);

        sleep(2);

        target3.header.frame_id = "base_footprint";
		target3.header.stamp = ros::Time();
        target3.point.x = C_position_x;
        target3.point.y = C_position_y;
        target3.point.z = C_position_z;	 
        pub_goal.publish(target3);

        sleep(2);

        target4.header.frame_id = "base_footprint";
		target4.header.stamp = ros::Time();
        target4.point.x = D_position_x;
        target4.point.y = D_position_y;
        target4.point.z = D_position_z;	 
        pub_goal.publish(target4);

        sleep(2);

        target5.header.frame_id = "base_footprint";
		target5.header.stamp = ros::Time();
        target5.point.x = E_position_x;
        target5.point.y = E_position_y;
        target5.point.z = E_position_z;	 
        pub_goal.publish(target5);


	}
	
		printf("%d\n",rrt_flag1);

}


int main(int argc, char** argv)
{

	ros::init(argc, argv, "point_navigation");    //初始化节点  

	ros::NodeHandle nha; //初始化句柄

	pub_goal = nha.advertise<geometry_msgs::PointStamped>("/clicked_point",1); //Rviz点击自主建图区域点的发布

	/***创建导航目标点标志位订阅***/
	ros::Subscriber rrt_flag1_sub = nha.subscribe("rrt_flag", 1, rrt_flag_Callback);

	/***创建自主建图区域点标志位订阅***/
	ros::Subscriber voice_choose1_sub = nha.subscribe("send_flag",1,voice_choosecallback);

	navigation_pub = nha.advertise<geometry_msgs::PointStamped>("/clicked_point",1);//Rviz点击导航点的发布

	nha.param<float>("/A_position_x", A_position_x, 1);
	nha.param<float>("/A_position_y", A_position_y, 1);
	nha.param<float>("/A_position_z", A_position_z, 0);

	nha.param<float>("/B_position_x", B_position_x, 1);
	nha.param<float>("/B_position_y", B_position_y, 1);
	nha.param<float>("/B_position_z", B_position_z, 0);

	nha.param<float>("/C_position_x", C_position_x, 1);
	nha.param<float>("/C_position_y", C_position_y, 1);
	nha.param<float>("/C_position_z", C_position_z, 0);

	nha.param<float>("/D_position_x", D_position_x, 1);
	nha.param<float>("/D_position_y", D_position_y, 1);
	nha.param<float>("/D_position_z", D_position_z, 0);

	nha.param<float>("/E_position_x", E_position_x, 1);
	nha.param<float>("/E_position_y", E_position_y, 1);
	nha.param<float>("/E_position_z", E_position_z, 0);

	nha.param<float>("/O_position_x", O_position_x, 1);
	nha.param<float>("/O_position_y", O_position_y, 1);
	nha.param<float>("/O_position_z", O_position_z, 0);

	nha.param<float>("/P_position_x", P_position_x, 1);
	nha.param<float>("/P_position_y", P_position_y, 1);
	nha.param<float>("/P_position_z", P_position_z, 0);

	nha.param<float>("/Q_position_x", Q_position_x, 1);
	nha.param<float>("/Q_position_y", Q_position_y, 1);
	nha.param<float>("/Q_position_z", Q_position_z, 0);


	ros::spin();

	return 0;

}

