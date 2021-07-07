// Define stepper motor connections and buttons here:
#define dirPinSeeder 2
#define stepPinSeeder 3
#define dirPinGantry 4
#define stepPinGantry 5
#define bitZeroPin 11
#define bitOnePin 12
#define bitTwoPin 13
// Define stepper motor steps per revolution
#define seederStepsPerRevolution 4
#define gantryStepsPerRevolution 10
#define plantPulse 500
#define gantryPulse 8000
// Define how many rows your seeder have and the plant delay and other configs
#define plantDelay 20
#define rowConst 7
#define forwardDir 0
#define backwardDir 1
// globals
bool seedprocess_running;
bool manualprocess_running;
int rows;
int stopchecks;
int x,y,z;
int x_loc;
int y_loc;
void setup() {
  // Declare pins as output:
  x = 0;
  y = 0;
  z = 0;
  x_loc = 0;
  y_loc = 0;
  rows = 0;
  stopchecks = 0;
  manualprocess_running = false;
  seedprocess_running = false;
  Serial.begin(9600);
  pinMode(stepPinSeeder, OUTPUT);
  pinMode(dirPinSeeder, OUTPUT);
  pinMode(stepPinGantry, OUTPUT);
  pinMode(dirPinGantry, OUTPUT);
  
  pinMode(bitZeroPin,INPUT);
  pinMode(bitOnePin,INPUT);
  pinMode(bitTwoPin,INPUT);

}

void loop() {
  checkKeyPress();  
}

void checkKeyPress(){
  x = digitalRead(bitZeroPin);
  y = digitalRead(bitOnePin);
  z = digitalRead(bitTwoPin);
  if(x==0&&y==0&&z==1){
      seedprocess_running = true;
      seedProcess();
  }
   else if(x==0&&y==1&&z==0){
      if (x_loc>0 || y_loc>0){
        rollback();
      }
      delay(500);
  }
  else if(x==0&&y==1&&z==1){
      if (rows<=rowConst){
        manualprocess_running = true;
        moveGantry(forwardDir);
        manualprocess_running = false;
      }
  }
  else if(x==1&&y==0&&z==0){
      if (rows>=1){
        manualprocess_running = true;
        moveGantry(backwardDir);
        manualprocess_running = false;
      }
  }
  else if(x==1&&y==0&&z==1){
      manualprocess_running = true;
      plant();
      manualprocess_running = false;
  }
  delay(100);
  
}

void checkStop(){
  x = digitalRead(bitZeroPin);
  y = digitalRead(bitOnePin);
  z = digitalRead(bitTwoPin);
  if(x==0&&y==1&&z==0){
      if (seedprocess_running){
        if (stopchecks<2){
          stopchecks+=1;
        }else{
          seedprocess_running = false;
        }
      }
  }
}
void seedProcess(){
  for(int i =0;i<rowConst;i++){
    if (!seedprocess_running){
      break;
    }
    plant();
    delay(500);
    moveGantry(forwardDir);
    delay(200);
  
  }
  if (seedprocess_running)
    plant();
  seedprocess_running = false;
  rollback();
  rows=0;
}

void plant(){
  if (seedprocess_running || manualprocess_running){
    digitalWrite(dirPinSeeder, HIGH);
    digitalWrite(seederIndicator,HIGH);
    for (int i =0; i < seederStepsPerRevolution; i++) {
      digitalWrite(stepPinSeeder, HIGH);
      x_loc = x_loc+seederStepsPerRevolution;
      print();
      delayMicroseconds(plantPulse);
      digitalWrite(stepPinSeeder, LOW);
      delayMicroseconds(plantPulse);
    }
    digitalWrite(dirPinSeeder, LOW);
    digitalWrite(seederIndicator,LOW);
    delay(plantDelay);
    digitalWrite(seederIndicator,HIGH);

    for (int i =0; i < seederStepsPerRevolution; i++) {
      digitalWrite(stepPinSeeder, HIGH);
      x_loc = x_loc-seederStepsPerRevolution;
      print();
      delayMicroseconds(plantPulse);
      digitalWrite(stepPinSeeder, LOW);
      delayMicroseconds(plantPulse);

    }
    digitalWrite(seederIndicator,LOW);
    checkStop();
  }

}
void moveGantry(int dir){
  if (seedprocess_running || manualprocess_running){
    digitalWrite(dirPinGantry, dir);
    digitalWrite(gantryIndicator,HIGH);
    for (int i = 0; i < gantryStepsPerRevolution; i++) {
      digitalWrite(stepPinGantry, HIGH);
      if(dir==0){
        y_loc+=gantryStepsPerRevolution;
      }else{
        y_loc-=gantryStepsPerRevolution;
      }
      print();
      delayMicroseconds(gantryPulse);
      digitalWrite(stepPinGantry, LOW);
      delayMicroseconds(gantryPulse);
    }
    digitalWrite(gantryIndicator,LOW);
    if (dir==0){
      rows++;
    }else{
      rows--;
    }
    checkStop();
  }
   
   
}
void rollback(){
  digitalWrite(dirPinGantry, 1);
  while(y_loc!=0){
    digitalWrite(stepPinGantry, HIGH);
    y_loc-=gantryStepsPerRevolution;
    print();
    delayMicroseconds(gantryPulse);
    digitalWrite(stepPinGantry, LOW);
    delayMicroseconds(gantryPulse);
  }
}
void print(){
   Serial.print(x_loc,DEC);
   Serial.print(";");
   Serial.println(y_loc,DEC);
}