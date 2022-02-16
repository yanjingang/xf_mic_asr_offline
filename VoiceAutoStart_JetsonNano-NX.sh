#! /bin/bash

### BEGIN INIT
gnome-terminal -- bash -c "source /opt/ros/melodic/setup.bash;source /home/wheeltec/wheeltec_robot/devel/setup.bash;roslaunch xf_mic_asr_offline base.launch"
sleep 10

gnome-terminal -- bash -c "source /opt/ros/melodic/setup.bash;source /home/wheeltec/wheeltec_robot/devel/setup.bash;roslaunch xf_mic_asr_offline mic_init.launch"

wait
exit 0
