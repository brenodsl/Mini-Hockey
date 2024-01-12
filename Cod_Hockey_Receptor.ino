#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define MOT_A1_PIN 27
#define MOT_A2_PIN 14
#define MOT_B1_PIN 12
#define MOT_B2_PIN 13
#define SLEEP 25
#define FAULT 26
 
RF24 radio(4, 5);

const uint64_t address[6] = {0x7878787878LL, 
0XB3B4B5B6F1LL,
0xB3B4B5B6CDLL,
0XB3B4B5B6A3LL,
0XB3B4B5B60FLL,
0xB3B4B5B605LL
};

 
struct MyData
{ 
  int leftX;
  int leftY;
  int rightX;
  int rightY;
};
MyData data;

int joytickLeft = 0;
int joytickRight = 0;

void setup() 
{
  // Set all the motor control inputs to OUTPUT
  pinMode(MOT_A1_PIN, OUTPUT);
  pinMode(MOT_A2_PIN, OUTPUT);
  pinMode(MOT_B1_PIN, OUTPUT);
  pinMode(MOT_B2_PIN, OUTPUT);
  pinMode(SLEEP, OUTPUT);
  pinMode(FAULT, INPUT_PULLUP);

  // Turn off motors - Initial state
  digitalWrite(MOT_A1_PIN, LOW);
  digitalWrite(MOT_A2_PIN, LOW);
  digitalWrite(MOT_B1_PIN, LOW);
  digitalWrite(MOT_B2_PIN, LOW);
  digitalWrite(SLEEP, HIGH);

  Serial.begin(115200);
  Serial.print("Aguardando resposta ");

  while(!radio.begin()){
    Serial.println(".");
    radio.begin();                  //Starting the Wireless communication
    delay(500);
  }
  radio.openReadingPipe(1, address[0]);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.startListening();              //This sets the module as receiver
}
 
int recvData()
{ 
  if (radio.available() ) 
  {
    radio.read(&data, sizeof(MyData));
    return 1;
  }else{
    return 0;
  }
}
 
void loop()
{
  
  if(recvData()){
    data.leftY = map(data.leftY, 0, 4095, 0, 255);
    data.rightY = map((data.rightY), 0, 4095, 0, 255); 
    
    //JOYSTICK ESQUERDO MAPEAMENTO PARA DRIVE MOTOR
    if(data.leftY > 170 && data.leftY < 190){
      joytickLeft = 0;
    }
    if(data.leftY > 190){
      joytickLeft = map((data.leftY), 190, 255, 0, 255);
    }
    if(data.leftY < 170){
      joytickLeft = map((data.leftY), 170, 0, 0, -255);
    }

    //JOYSTICK DIREITO MAPEAMENTO PARA DRIVE MOTOR
    if(data.rightY > 170 && data.rightY < 190){
      joytickRight = 0;
    }
    if(data.rightY > 190){
      joytickRight = map((data.rightY), 190, 255, 0, -255);
    }
    if(data.rightY < 150){
      joytickRight = map((data.rightY), 150, 0, 0, 255);
    }

    

    if(digitalRead(FAULT)){   //Alguma falha ocorrendo? Não...

      
    

      Serial.print("Rec leftY ");
      Serial.print(data.leftY);

      Serial.print("Alt leftY ");
      Serial.print(joytickLeft);

      Serial.print("Rec RightY ");
      Serial.print(data.rightY);

      Serial.print("// Alt RightY ");
      Serial.println(joytickRight);

      if(joytickRight == 0 && joytickLeft == 0){
        digitalWrite(SLEEP, LOW); //para o motor 
        spin_and_wait(0, 0);
      }else if(joytickRight == 0){
        digitalWrite(SLEEP, HIGH); //habilita
        spin_and_wait(joytickLeft, 0);
      }else if(joytickLeft == 0){
        digitalWrite(SLEEP, HIGH); //habilita
        spin_and_wait(0, joytickRight);
      }else{
        digitalWrite(SLEEP, HIGH); //habilita
        spin_and_wait(joytickLeft, joytickRight);
      }
      
      
    }else{   //Senão, existe falha
      digitalWrite(SLEEP, LOW); //para o motor 
      spin_and_wait(0, 0);
    }
    
  }else{
    digitalWrite(SLEEP, LOW); //para o motor 
    spin_and_wait(0, 0);
  }
  
    
}
/// Set the current on a motor channel using PWM and directional logic.
///
/// \param pwm    PWM duty cycle ranging from -255 full reverse to 255 full forward
/// \param IN1_PIN  pin number xIN1 for the given channel
/// \param IN2_PIN  pin number xIN2 for the given channel
void set_motor_pwm(int pwm, int IN1_PIN, int IN2_PIN)
{
  if (pwm < 0) {  // reverse speeds
    analogWrite(IN1_PIN, -pwm);
    digitalWrite(IN2_PIN, LOW);

  } else { // stop or forward
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, pwm);
  }
}

/// Set the current on both motors.
///
/// \param pwm_A  motor A PWM, -255 to 255
/// \param pwm_B  motor B PWM, -255 to 255
void set_motor_currents(int pwm_A, int pwm_B)
{
  set_motor_pwm(pwm_A, MOT_A1_PIN, MOT_A2_PIN);
  set_motor_pwm(pwm_B, MOT_B1_PIN, MOT_B2_PIN);
}

/// Simple primitive for the motion sequence to set a speed and wait for an interval.
///
/// \param pwm_A  motor A PWM, -255 to 255
/// \param pwm_B  motor B PWM, -255 to 255
/// \param duration delay in milliseconds
void spin_and_wait(int pwm_A, int pwm_B)
{
  set_motor_currents(pwm_A, pwm_B);
}
