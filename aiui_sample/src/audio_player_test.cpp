#include <AudioPlayer.h>

int main(){
    AudioPlayer player;
    int buf_len = 128;
	unsigned char buf[buf_len];
    int i =0;
    // while (i<100)
    // {
    //    player.Write(buf);
    //     i++;
    // }
    player.WriteTest();
    
}