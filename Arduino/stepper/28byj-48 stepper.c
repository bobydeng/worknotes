


/***
Stepper stepper(STEPS, 8, 10, 9, 11);//引脚要互换

***/


//使用arduino IDE自带的Stepper.h库文件
//注意28BYJ-48为减速电机所以速度不会很快
//具体引脚不写了看图片吧
#include <Stepper.h>
// 这里设置步进电机旋转一圈是多少步
#define STEPS 100
//设置步进电机的步数和引脚（就是注意点2里面说的驱动板上IN1～IN4连接的四个数字口）。
Stepper stepper(STEPS, 8, 10, 9, 11);//引脚要互换

void setup()
{
  // 设置电机的转速：每分钟为90步
  stepper.setSpeed(90);
  // 初始化串口，用于调试输出信息
  Serial.begin(9600);
}

void loop()
{
    // 顺时针旋转一周
    Serial.println("shun");
    stepper.step(2048); //4步模式下旋转一周用2048 步。
    delay(500);

    // 逆时针旋转半周
    Serial.println("ni");
    stepper.step(-1024); //4步模式下旋转一周用2048 步。
    delay(500);
}

