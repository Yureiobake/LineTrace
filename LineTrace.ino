//16
#include <Adafruit_MCP3008.h>
#include <Servo.h>
Adafruit_MCP3008 adc; //下部センサの定義 
Servo servoR;
Servo servoL;
int v[6]={0,0,0,0,0,0}; //6連センサの値を格納する配列
int b=400;//黒線上であるかとないかの閾値を仮に400とする
int rotate;//センサの値
int integral =0; //integralの初期値
int deri =0;//微分するための値格納
float k = 0.056; //PID制御の値
float ki =0.005;//Iの係数
float kd= 0.1;//Dの係数
void setup() {
servoR.attach(4);//右車輪のモータのピンが4に配線されている場合のアタッチ
servoL.attach(5);//○○○.(i)でi番目のピンのモータを関連づける

/*モーター初期化*/
//引数に90を入れると基本的には車輪のモータは停止する
servoR.write(90); //○○○.write(int)でモータの回転角（速度）変える
servoL.write(90); //値は0～180の間

adc.begin(); //センサを初期化
Serial.begin(9600); //シリアル通信の転送速度
}

//メインループ
void loop(){
  //センサの値を取得
  rotate = read();
  //センサの値をモニターに出力
  Serial.print(rotate);
  Serial.print(" ");
  //pidの値取得
  pid=road(rotate);
  //走行
  runRotate(pid);
  Serial.print(pid);
  Serial.println();
  lasterror=rotate;//前回の値を保持
}

void R_run(){
  servoL.write(180);
  servoR.write(90);
  delay(100);
}

void L_run(){
  servoL.write(90);
  servoR.write(0);
  delay(100);
}

//入力が大きい値であれば大きく曲がる関数
void runRotate(float angle){
  if (angle>0){
    /*L 180→180 R 0→180*/
    servoL.write(180);
    servoR.write(int(angle));
  }else{
    /*L 180→0 R 0→0*/
    servoL.write(180+int(angle));
    servoR.write(0);
  }
}

void run(){
  servoL.write(180);
  servoR.write(0);
  delay(100);
}

//センサの値を読んで変換する関数
int read(){
  int rotate = 0;
  //センサの値を格納
  for (i=0;i<6;i++) {v[i] = adc.readADC(i);}

  //各センサに重みをかける
  v[0] *= -2;
  v[1] *= -1;
  v[2] *= -1;
  v[3] *= 1;
  v[4] *= 1;
  v[5] *= 2;
  
  //各センサの値を足し合わせる
  for (i=0;i<6;i++){
    rotate += v[i];
  }
  
  return rotate;
  
}
int road (error){
  intergral=error+integral;//積分（誤差の足し算）
  Serial.print(integral);
  Serial.print(" ");
  if(integral>1000){ //1000以上であれば1000に
    integral=1000;
  }
  if(integral<-1000){  //-1000以下であれば-1000に固定
    integral=-1000;
  }
  deri=rotate-lasterror;//微分の値(前回との差)

  pid=error*k+integral*ki+deri*kd;//pid制御の値
}


