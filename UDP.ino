#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <MycobotBasic.h>
#include <ParameterList.h>

#include <cstring>
#include <cstdlib>

//实例化函数：
MycobotBasic myCobot;
int parameters_numbers;  //参数个数
int isCoord;   //0表示角度，1表示坐标
int i_temp;   //1-6
float angle; //角度
float value;
int myspeed = 100;
Angles angles;
Coords coords;


#define N 1024
 
const char* ssid     = "iPhone";
const char* password = "88888888";
const int port = 5555;
 
// The udp library class
WiFiUDP udp;

 
void print_wifi_state(){
    M5.Lcd.clear(BLACK);  // clear LCD
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setCursor(3, 3);
    M5.Lcd.println("");
    M5.Lcd.println("WiFi connected.");
    M5.Lcd.print("IP address: ");
    M5.Lcd.println(WiFi.localIP());
    M5.Lcd.print("Port: ");
    M5.Lcd.println(port);
}
 
void setup_wifi(){
    M5.Lcd.setTextColor(RED);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(3, 10);
    M5.Lcd.print("Connecting to ");
    M5.Lcd.println(ssid);
 
    // setup wifi
    WiFi.mode(WIFI_STA);  // WIFI_AP, WIFI_STA, WIFI_AP_STA or WIFI_OFF
    // WiFi.begin();
    WiFi.begin(ssid, password);
        
 
    // Connecting ..
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        M5.Lcd.print(".");
    }
 
    // print state
    print_wifi_state();
 
    udp.begin(port);
}
 
void setup() {
    M5.begin();
    myCobot.setup(); 
    myCobot.powerOn(); 

    //吸泵
    Serial.begin(9600);//打开串口，波特率9600
    pinMode(2,OUTPUT); //设定引脚G2为输出状态
    pinMode(5,OUTPUT); //设定引脚G5为输出状态
    delay(100);
    digitalWrite(2,1);//将引脚2设为高电平，关闭电磁阀
    digitalWrite(5,1);//将引脚5设为高电平，关闭泵机
    
    robot_init();
    // setup wifi
    setup_wifi();
}
 
void loop() {
    char packetBuffer[N];
    int packetSize = udp.parsePacket();

    // get packet
    if (packetSize){
        int len = udp.read(packetBuffer, packetSize);
        if (len > 0){
                packetBuffer[len] = '\0'; // end
        }

        // print param
        M5.Lcd.clear(BLACK);
        M5.Lcd.setCursor(3, 3);
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.println(packetBuffer);

        //对接收到的数据进行处理
        //接受7个参数或者接收3个参数
        //第一个参数0表示角度，1表示坐标
        
        to_number(packetBuffer);
        myCobot.writeCoord((Axis)i_temp,value,50);         
    }
}


// 机器人初始化函数 
void robot_init(){
   Angles angles; //定义 angles
   for(auto &val : angles) //关节回到原点
    val = 0.0;
   myCobot.writeAngles(angles, 50);

   delay(5000);
}

//命令解释器
void to_number(char *str)
{
// Arduino 不支持c++ 11 无法使用STL::map
//    static map<std::string,Axis> const table = { 
//             {"x",Axis::x}, 
//             {"y",Axis::y},
//             {"z",Axis::z},
//             {"rx",Axis::rx},
//             {"ry",Axis::ry},
//             {"rz",Axis::rz}
//             }; 
             
    int i = 0;
    char *inst;
    parameters_numbers = 0;
    const char  *delim = " ";
    int sp;
        
    inst = std::strtok(str,delim);//解析出命令
    if(!strcmp(inst,"wa")){
      int joint = std::atoi(strtok(NULL,delim));//指定关节
      float value = std::atof(strtok(NULL,delim));//指定角度
      
      char *p = strtok(NULL,delim);
      if(p==0)
        sp = myspeed;
      else
        sp = std::atoi(p);
        
      myCobot.writeAngle(joint,value,sp);
      M5.Lcd.println("writeAngle Done");
      
    }
    else if(!strcmp(inst,"was")){
      float a = std::atof(strtok(NULL,delim));
      float b = std::atof(strtok(NULL,delim));
      float c = std::atof(strtok(NULL,delim));
      float d = std::atof(strtok(NULL,delim));
      float e = std::atof(strtok(NULL,delim));
      float f = std::atof(strtok(NULL,delim));
      Angles angles = {a,b,c,d,e,f};

      char *p = strtok(NULL,delim);
      if(p==0)
        sp = myspeed;
      else
        sp = std::atoi(p);

      myCobot.writeAngles(angles,sp); 
      M5.Lcd.println("writeAngles Done");
       
    }
    else if(!strcmp(inst,"wc")){
      char *p = strtok(NULL,delim);
      Axis axis;
      if(!strcmp(p,"x")) axis=Axis(1);
      if(!strcmp(p,"y")) axis=Axis(2);
      if(!strcmp(p,"z")) axis=Axis(3);
      if(!strcmp(p,"rx"))axis=Axis(4);
      if(!strcmp(p,"ry"))axis=Axis(5);
      if(!strcmp(p,"rz"))axis=Axis(6);
      
      float value = std::atof(strtok(NULL,delim));
      
      p = strtok(NULL,delim);
      if(p==0)
        sp = myspeed;
      else
        sp = std::atoi(p);
      
      myCobot.writeCoord(axis,value,sp);
      M5.Lcd.println("writeCoord Done");
            
    }
    else if(!strcmp(inst,"wcs")){
      float x = std::atof(strtok(NULL,delim));
      float y = std::atof(strtok(NULL,delim));
      float z = std::atof(strtok(NULL,delim));
      float rx = std::atof(strtok(NULL,delim));
      float ry = std::atof(strtok(NULL,delim));
      float rz = std::atof(strtok(NULL,delim));
      Coords coords={x,y,z,rx,ry,rz};

      char *p = strtok(NULL,delim);
      if(p==0)
        sp = myspeed;
      else
        sp = std::atoi(p);

      myCobot.writeCoords(coords,sp);
      M5.Lcd.println("writeCoords Done");
    }
    else if(!strcmp(inst,"sgp")){
      char *p = strtok(NULL,delim);
      int data = std::atoi(p);
      
      myCobot.setGripper(data);
      M5.Lcd.println("setGripper Done");
    }
    else if(!strcmp(inst,"spp")){
      // https://www.elephantrobotics.com/docs/myCobot/3-development/7-side_products/7.1.1-gripper_ag.html
      char *p = strtok(NULL,delim);
      int data = std::atoi(p);
      if(data==1){
        digitalWrite(5,0);//将引脚5设为低电平，打开泵机
        delay(200);//延时200ms
        digitalWrite(2,0);//将引脚2设为低电平，打开电磁阀
      }
      else if(data==0){
        digitalWrite(2,1);//将引脚2设为高电平，关闭电磁阀
        delay(200);//延时200ms
        digitalWrite(5,1);//将引脚5设为高电平，关闭泵机
        delay(200);//延时200ms
      }
      M5.Lcd.println("setPump Done");
      
    }
    else if(!strcmp(inst,"ckr")){
      if(myCobot.checkRunning())
         M5.Lcd.println("True");
      else
         M5.Lcd.println("False");
    }
    else if(!strcmp(inst,"gas")){
      Angles angles = myCobot.getAngles();
      for(int i=0;i<6;i++){
         M5.Lcd.print(angles[i]);
         M5.Lcd.print(delim);
      }
      M5.Lcd.println();
    }
    else if(!strcmp(inst,"gcs")){
      Coords coords = myCobot.getCoords();
      for(int i=0;i<6;i++){
         M5.Lcd.print(coords[i]);
         M5.Lcd.print(delim);
      }
      M5.Lcd.println();
    }
    else if(!strcmp(inst,"pau")){
      myCobot.pause();
      M5.Lcd.println("pause...");
    }
    else if(!strcmp(inst,"res")){
      myCobot.resume();
      M5.Lcd.println("resume...");
    }
    else if(!strcmp(inst,"stop")){
      myCobot.stop();
      M5.Lcd.println("stop!");
    }
    else if(!strcmp(inst,"ssp")){
      char *p = strtok(NULL,delim);
      sp = std::atoi(p);
      myspeed = sp;      
    }
    else if(!strcmp(inst,"gsp")){
      sp = myspeed;
      M5.Lcd.print("speed: ");
      M5.Lcd.println(sp);
    }
    else if(!strcmp(inst,"sfm")){
      myCobot.setFreeMove();
      M5.Lcd.println("setFreeMove Done");      
    }
    else if(!strcmp(inst,"blank")){
      //
    }
        

}
 
