#! /bin/bash

### BEGIN INIT
sleep 10

gnome-terminal -- bash -c "source /opt/ros/melodic/setup.bash;source /home/work/catkin_ws/devel/setup.bash;roslaunch xf_mic_asr_offline base.launch"

sleep 15

gnome-terminal -- bash -c "source /opt/ros/melodic/setup.bash;source /home/work/catkin_ws/devel/setup.bash;roslaunch xf_mic_asr_offline mic_init.launch"

wait
exit 0
