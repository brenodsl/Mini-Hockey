#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>
 
RF24 radio(4, 5); // CE, CSN on Blue Pill 
const uint64_t address[6] = {0x7878787878LL, 
0XB3B4B5B6F1LL,
0xB3B4B5B6CDLL,
0XB3B4B5B6A3LL,
0XB3B4B5B60FLL,
0xB3B4B5B605LL
};
 
int vel1 = 0;
int vel2 = 0;
 
struct MyData 
{ 
  int leftX;
  int leftY;
  int rightX;
  int rightY;
};
MyData data;
 
void setup() 
{
  Serial.begin(115200);
  Serial.print("Aguardando resposta ");

  while(!radio.begin()){
    Serial.print(".");
    radio.begin();                  //Starting the Wireless communication
    delay(500);
  }

  radio.openWritingPipe(address[0]); //Setting the address where we will send the data
  radio.setPALevel(RF24_PA_MIN);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.stopListening();          //This sets the module as transmitter
}
 
void loop()
{
  data.leftX = analogRead(27);
  data.leftY = analogRead(34);
  data.rightX = analogRead(32);
  data.rightY = analogRead(14);
  
  //Serial.print("leftX ");
  //Serial.print(data.leftX);
  
  Serial.print("  //  leftY ");
  Serial.print((data.leftY));
  
 
  //Serial.print("  //  rightX ");
  //Serial.print(data.rightX);

  Serial.print("  //  rightY ");
  Serial.println((data.rightY));
  
  
  radio.write(&data, sizeof(MyData));
}
