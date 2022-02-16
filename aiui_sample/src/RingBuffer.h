 class RingBuffer
{

public:
    RingBuffer(int buff_size);
    ~RingBuffer();
    

public:
    int RingBuff_Rx(char *buf, int buf_len);
    int RingBuff_Tx(char *buf, int buf_len);
    int RingBuff_Rx_Byte(char data);  //按字节入队
    int RingBuff_Tx_Byte(char *data); //按字节出队
    int get_head();
    int get_tail(); //队列入口
    int get_length();
private:
    unsigned int Head; //队列出口
    unsigned int Tail;
    unsigned int Length; //数据长度
    unsigned int Size;   //队列长度
    char *Ring_Buf;
};




RingBuffer::RingBuffer(int buff_size) : Head(0), Tail(0), Length(0)
{
    Size = buff_size;

    Ring_Buf = new char[buff_size];
}

RingBuffer::~RingBuffer()
{
    delete[] Ring_Buf;
}
int RingBuffer::get_head()
{
    return Head;
}
int RingBuffer::get_tail()
{
    return Tail;
}

int RingBuffer::get_length()
{
    return Length;
}

int RingBuffer::RingBuff_Rx(char *buf, int buf_len)
{
    Tail = Tail+100;
    int realSize = 0;
    int reLength = 0;
    //循环队列剩下的长度
    reLength = Size - Length;

    //循环队列剩下的长度，如果剩余不够，返回－１
    if (reLength <= 0)
    {
        return -1;
    }
    //如果剩下的空间不够存储,那么有效的长度
    if (buf_len > reLength)
    {
        realSize = reLength; //可存储的大小就是剩余大小
    }
    else if (buf_len <= reLength)
    {
        realSize = buf_len; //可存储大小就是传过来的所有大小
    }

    for (int i = 0; i < realSize; i++)
    {
        Ring_Buf[Tail] = buf[i]; //指针依次指向buf内容
        Tail = (Tail + 1) % Size;
        Length++; //有效长度也加１
    }

    //返回实际存储了多少
    return realSize;
}

int RingBuffer::RingBuff_Tx(char *buf, int buf_len)
{ 
    
    Head = Head+100;
    int realSize = 0;

    if (Length <= 0)
    {
        return -1;
    }

    if (buf_len > Length)
    {
        realSize = Length;
    }
    else if (buf_len <= Length)
    {
        realSize = buf_len;
    }

    for (int i = 0; i < realSize; i++)
    {
	
        buf[i] = Ring_Buf[Head];
        Head = (Head + 1) % Size;
        Length--;
    }
    //返回实际拿走了多少
    return realSize;
}
int RingBuffer::RingBuff_Rx_Byte(char data)
{
    if (Length >= Size)
    {
        return -1;
    }

    Ring_Buf[Tail] = data;
    Tail = (Tail + 1) % Size;
    Length++;

    return 1;
}

int RingBuffer::RingBuff_Tx_Byte(char *data)
{
    if (Length <= 0)
    {
        return -1;
    }

    *data = Ring_Buf[Head];
    Head = (Head + 1) % Size;
    Length--;

    return 1;
}
