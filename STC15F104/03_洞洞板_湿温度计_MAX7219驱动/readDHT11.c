#include <readDHT11.h>

uint usleepA,usleepB;

int debugCnt;

// 显示用温度数据
uchar wendu;
uchar shidu;

void usleep(int tt) {
  for (usleepA = 0; usleepA < tt; ++usleepA)
  {
    for (usleepB = 0; usleepB < 1000; ++usleepB)
    {
      ;
    }
  }
}

uchar readDHT11() {
  int i,j,cnt = 0;
  // 接收到的40位数据
  uchar bits[40];
  // 5个数据分别储存湿度整数数据，湿度小数数据，温度整数数据，温度小数数据，校验和数据
  uchar datas[5];

  debugCnt = 0;

  for (j = 0; j < RETRY; ++j)
  {
    for (i = 0; i < 5; ++i) {
      datas[i] = 0;
    }

    // 先拉高DATA一段时间，准备发送开始指令
    DATA_PIN = 1;
    usleep(500);  // 500 ms

    // 拉低DATA口，输出开始指令（至少持续18ms）
    DATA_PIN = 0;
    usleep(TIME_START);
    DATA_PIN = 1;

    // 开始指令输出完毕，切换到输入模式，等待DHT11输出信号。
    // 由于有上拉电阻的存在，所以DATA口会维持高电平。
    DATA_PIN = 1; // 51单片机io口作为输入用之前要先置1

    // 在DATA口被拉回至高电平通知DHT11主机已经准备好接受数据以后，
    // DHT11还会继续等待20-40us左右以后才会开始发送反馈信号，所以我们把这段时间跳过去
    // 如果长时间（1000us以上）没有低电平的反馈表示有问题，结束程序
    cnt=0;
    while (DATA_PIN == 1) {
      cnt++;
      if (cnt > MAXCNT)
      {
        // printf("DHT11未响应，请检查连线是否正确，元件是否正常工作。\n");
        debugCnt = cnt;
        return READ_DHT11_RET_TIMEOUT;
      }
    }

    // 这个反馈响应信号的低电平会持续80us左右，但我们不需要精确计算这个时间
    // 只要一直循环检查DATA口的电平有没有恢复成高电平即可
    cnt=0;
    while (DATA_PIN == 0) {
      cnt++;
      if (cnt > MAXCNT)
      {
        // printf("DHT11未响应，请检查连线是否正确，元件是否正常工作。\n");
        debugCnt = cnt;
        return READ_DHT11_RET_TIMEOUT;
      }
    }

// For Test 不同的cpu速度不同，这里计数的数字就是80us的时间
//return cnt;

    // 这个持续了80us左右的低电平的反馈信号结束以后，DHT11又会将DATA口拉回高电平并再次持续80us左右
    // 然后才会开始发送真正的数据。所以跟上面一样，我们再做一个循环来检测这一段高电平的结束。
    cnt=0;
    while (DATA_PIN == 1) {
      cnt++;
      if (cnt > MAXCNT)
      {
        // printf("DHT11未响应，请检查连线是否正确，元件是否正常工作。\n");
        debugCnt = cnt;
        return READ_DHT11_RET_TIMEOUT;
      }
    }

    // ##################### 40bit的数据传输开始 ######################

    for (i = 0; i < 40; i++)
    {
      // 每一个bit的数据（0或者1）总是由一段持续50us的低电平信号开始
      // 跟上面一样我们用循环检测的方式跳过这一段
      while (DATA_PIN == 0) {
      }

      // 接下来的高电平持续的时间是判断该bit是0还是1的关键。
      //    根据DHT11的说明文档，我们知道 这段高电平持续26us-28us左右的话表示这是数据0。
      //    如果这段高电平持续时间为70us左右表示这是数据1。
      // 方法1：在高电平开始的时候记下时间，在高电平结束的时候再记一个时间，
      //        通过计算两个时间的间隔就能得知是数据0还是数据1了。
      // 方法2：在高电平开始的以后我们延时40us，然后再次检测DATA口:
      //        (a) 如果此时DATA口是低电平，表示当前位的数据已经发送完并进入下一位数据的传输准备阶段（低电平50us）了。
      //        由于数据1的高电平持续时间是70us，所以如果是数据1，此时DATA口应该还是高电平才对，
      //        据此我们可以断言刚才传输的这一位数据是0。
      //        (b) 如果延时40us以后DATA口仍然是高电平，那么我们可以断言这一位数据一定是1了，因为数据0只会持续26us。
      // 方法3：循环检测电平状态并计数，每检查一次如果电平状态没变就让计数器加一，一直到电平状态变成低电平为止。
      //        数据0的高电平持续时间短，所以计数一定比数据1的计数少，由于微秒级别的延时太短，这个计数会有一定误差。
      //        我们需要先观察（输出到数码管或是串口）每一位数据计数的结果，然后观察计数结果来设定一个阈值，
      //        高于这个阈值的就认为的数据1，低于这个值的就认为是数据0。（设个阈值是头文件里的VAL）
      // 我们这里采用简单易行的方法3。
      cnt=0;
      while (DATA_PIN == 1) {
        // 当所有数据传输完以后，DHT11会放开总线，DATA口就会被上拉电阻一直拉高。
        // 所以如果超过一定时间电平还没有被拉低就表示所有的数据已经传输完毕，停止检测。
        cnt++;
        if (cnt > MAXCNT)
        {
          break;
        }
      }

      if (cnt > MAXCNT)
      {
        break;
      }
      // 将当前位的计数保存起来
      bits[i] = cnt;
    }

    // 整理数据，将位数据转成5个数字
    for (i = 0; i < 40; ++i) {
      datas[i/8] <<= 1;
      if (bits[i] > VAL) 
      {
        datas[i/8] |= 1;
      }
      //下面这句话就是用来测试自己的设备应该设定多少阈值的测试代码
      //printf("bits[%d] = %d (%d) \n", i, bits[i], bits[i]>200?1:0 );
    }

    // 用校验和来检查接收数据是否完整
    if (datas[4] == ((datas[0] + datas[1] + datas[2] + datas[3]) & 0xFF) ) {
      // printf("校验成功！ \n");

      // 将湿度，温度数据赋值给显示用的湿度，温度变量
      shidu = datas[0];
      wendu = datas[2];
      return READ_DHT11_RET_SUCCESS;
    } else {
      // printf("校验不成功，重新取值！ \n");
      // 校验不成功，重新取值，连续10次取值不成功就放弃。一般连线和逻辑正确的话连续10次取值出错是不可能的。
      continue;
    }
  }
  return READ_DHT11_RET_CHECKESUM_WRONG;
}

uchar getWendu() {
  return wendu;
}

uchar getShidu() {
  return shidu;
}

int getDebugCnt() {
  return debugCnt;
}