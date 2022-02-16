#!/usr/bin/env python
# encoding: utf-8

from visualization_msgs.msg import Marker
from visualization_msgs.msg import MarkerArray
import rospy
import math
from geometry_msgs.msg import PointStamped, PoseStamped
from std_msgs.msg import Int8
import actionlib
from move_base_msgs.msg import *

#到达目标点成功或失败的回调函数，输入参数：[3：成功， 其它：失败](4：ABORTED)
def pose_callback(msg):
    global try_again, index, try_again, index
    if msg.status.status == 3 and count>0 :  #成功到达任意目标点，前往下一目标点
        try_again = 1 #允许再次尝试前往尚未抵达的该目标点
       
        #count表示当前目标点计数，index表示已完成的目标点计数
        if index == count:                   #当index等于count时，表示所有目标点完成，重新开始巡航
            print ('Reach the target point '+str(index-1)+'.')
            if count>1: print 'Complete instructions!' #只有一个目标点不算巡航
            index = 0;
            pose = PoseStamped()
            pose.header.frame_id = 'map'
            pose.header.stamp = rospy.Time.now()
            pose.pose.position.x = markerArray.markers[index].pose.position.x
            pose.pose.position.y = markerArray.markers[index].pose.position.y
            pose.pose.orientation.z = 0
            pose.pose.orientation.w = 1
            goal_pub.publish(pose)
            index += 1 #下一次要发布的目标点序号
        elif index < count:                   #当index小于count时，表示还未完成所有目标点，目标巡航未完成
            print ('Reach the target point '+str(index-1)+'.')           
            pose = PoseStamped()
            pose.header.frame_id = 'map'
            pose.header.stamp = rospy.Time.now()
            pose.pose.position.x = markerArray.markers[index].pose.position.x
            pose.pose.position.y = markerArray.markers[index].pose.position.y
            pose.pose.orientation.z = 0
            pose.pose.orientation.w = 1
            goal_pub.publish(pose)
            index += 1 #下一次要发布的目标点序号
        
    elif count>0: #未抵达设定的目标点    
        rospy.logwarn('Can not reach the target point '+str(index-1)+'.')                             

        #如果未尝试过前往尚未抵达的目标点，则尝试前往尚未抵达的目标点
        if try_again == 1:
            rospy.logwarn('trying reach the target point '+str(index-1)+' again!')
            pose = PoseStamped()
            pose.header.frame_id = 'map'
            pose.header.stamp = rospy.Time.now()
            pose.pose.position.x = markerArray.markers[index - 1].pose.position.x           
            pose.pose.position.y = markerArray.markers[index - 1].pose.position.y
            pose.pose.orientation.z = 0
            pose.pose.orientation.w = 1
            goal_pub.publish(pose)
            try_again = 0 #不允许再次尝试前往尚未抵达的该目标点

        #如果已经尝试过前往尚未抵达的目标点，则前往下一个目标点
        elif index < len(markerArray.markers):      #若还未完成目标点
            rospy.logwarn('try reach the target point '+str(index-1)+' failed! reach next point.')
            if index==count: index=0 #如果下一个目标点序号为count，说明当前目标点为最后一个目标点，下一个目标点序号应该设置为0
            pose = PoseStamped()
            pose.header.frame_id = 'map'
            pose.header.stamp = rospy.Time.now()
            pose.pose.position.x = markerArray.markers[index].pose.position.x      
            pose.pose.position.y = markerArray.markers[index].pose.position.y
            pose.pose.orientation.z = 0
            pose.pose.orientation.w = 1
            goal_pub.publish(pose)
            index += 1 #下一次要发布的目标点序号
            try_again = 1 #允许再次尝试前往尚未抵达的该目标点

#rviz内标记按下的回调函数，输入参数：按下的位置[x, y, z=0]
def press_callback(msg):           
    global index, count

    print('Add a new target point '+str(count)+'.')
    marker = Marker()      #创建marker对象
    marker.header.frame_id = 'map' #以哪一个TF坐标为原点
    marker.type = marker.TEXT_VIEW_FACING #一直面向屏幕的字符格式
    marker.action = marker.ADD #添加marker
    marker.scale.x = 1 #marker大小
    marker.scale.y = 1 #marker大小
    marker.scale.z = 1 #marker大小，对于字符只有z起作用
    marker.color.a = 1 #字符透明度
    marker.color.r = 1 #字符颜色R(红色)通道
    marker.color.g = 0 #字符颜色G(绿色)通道
    marker.color.b = 0 #字符颜色B(蓝色)通道
    marker.pose.position.x = msg.point.x #字符位置
    marker.pose.position.y = msg.point.y #字符位置
    marker.pose.position.z = msg.point.z #字符位置
    marker.text = str(count) #字符内容
    markerArray.markers.append(marker) #添加元素进数组

    #markers的id不能一样，否则rviz只会识别最后一个元素
    id = 0
    for m in markerArray.markers:    #遍历marker分别给id赋值
        m.id = id
        id += 1
    
    mark_pub.publish(markerArray) #发布markerArray，rviz订阅并进行可视化

    #第一次添加marker时直接发布目标点
    if count == 0:
        pose = PoseStamped() #创建目标点对象
        pose.header.frame_id = 'map' #以哪一个TF坐标为原点
        pose.header.stamp = rospy.Time.now()
        pose.pose.position.x = msg.point.x #目标点位置
        pose.pose.position.y = msg.point.y #目标点位置
        pose.pose.orientation.z = 0 #四元数，到达目标点后小车的方向，z=sin(angle/2)
        pose.pose.orientation.w = 1 #四元数，到达目标点后小车的方向，w=cos(angle/2)
        goal_pub.publish(pose)
        index += 1 #下一次要发布的目标点序号

    count += 1 #有几个目标点

def send_mark():
    global markerArray, count, index, try_again, mark_pub, goal_pub
    markerArray = MarkerArray() #目标点标记数组
    count = 0
    index = 0
    try_again = 1
    sendflagPublisher = rospy.Publisher('/send_flag', Int8, queue_size =1)
    rospy.init_node('path_point_demo') #初始化节点
    mark_pub = rospy.Publisher('/path_point', MarkerArray, queue_size = 100) #用于发布目标点标记
    click_sub = rospy.Subscriber('/clicked_point', PointStamped, press_callback) #订阅rviz内标记按下的位置
    goal_pub = rospy.Publisher('/move_base_simple/goal', PoseStamped, queue_size = 1) #用于发布目标点
    send_flag=Int8()
    send_flag.data=1
    sendflagPublisher.publish(send_flag)
    rospy.sleep(1.)
    sendflagPublisher.publish(send_flag)
    rospy.loginfo('a=%d',send_flag.data)
    print("11111111111111111111111111")
    goal_status_sub = rospy.Subscriber('/move_base/result', MoveBaseActionResult, pose_callback) #用于订阅是否到达目标点状态
    rospy.spin()

if __name__ == '__main__':
    send_mark()


