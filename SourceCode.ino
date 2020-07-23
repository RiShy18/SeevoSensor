#include <LiquidCrystal.h>
#include <IRremote.h>
#include <Servo.h>

// Variables del Servo y Ultrasónico
int cm_in = 0;
int cm_out = 0;
Servo servo_6;
const int Sensor_IN = 7;
const int Sensor_OUT = 8;

// Códigos del IRSensor
int C[12] = {16593103, 16582903, 16615543, 16599223, 16591063,
                 16623703, 16607383, 16586983, 16619623, 16603303,
             16597183, 16601263};

// Pines del LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// IR Sensor
int RECV_PIN = 10;
IRrecv irrecv(RECV_PIN);
decode_results results;

// Columna actual
int column = 0;
// Valor mayor de personas
int MAXValue = 1;
// Cantidad de personas dentro
int personas_cont = 0;


void setup() {
  // Servo
  pinMode(Sensor_IN, INPUT);
  pinMode(Sensor_OUT, INPUT);
  Serial.begin(9600);
  servo_6.attach(6);
  
  // LCD
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("#Personas");
  irrecv.enableIRIn(); 

}

void loop() {
  // Distancias de los sensores
  cm_in = 0.01723 * readUltrasonicDistance(Sensor_IN);
  cm_out = 0.01723 * readUltrasonicDistance(Sensor_OUT);
  
  // Si la distancia del sensor de entrada está en el rango
  // entonces se incrementa el contador
  if (cm_in < 250){
    personas_cont++;
    delay(500);
  }
  // Si la distancia del sensor de salida está en el rango
  // entonces se decrementa el contador
  if(cm_out < 250){
    if (personas_cont > 0){
    personas_cont--;
    }
    delay(500);
  }
  
  // Imprimir la cantidad de personas que hay
  // Corrección de pantalla
  if ((personas_cont == 9) || (personas_cont == 99)){
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("#Personas ");
    lcd.setCursor(0, 1);
    lcd.print(personas_cont);
  }else{
    lcd.setCursor(0, 1);
    lcd.print(personas_cont);
  }
  // Si se llega al valor mayor, se cierra la puerta
  if (personas_cont == MAXValue){
    servo_6.write(100);
  }
  // Si no está en el valor mayor, la puerta se mantiene
  // abierta
  else if (personas_cont < MAXValue){
    
    servo_6.write(0);
  }
  // Si se modifica el maximo con mas personas dentro
  // la puerta se mantiene cerrada.
  else if (personas_cont >  MAXValue){
    
    servo_6.write(100);
  }
 
  // Detectar si un botón del control se presiona
  if (irrecv.decode(&results)) {
    
    // Se lee el valor, del botón pulsado
    int read = value_read(results.value);
    // Se modifica el la segunda columna
    lcd.setCursor(0, 1);
    
    // Modo de edición de la cantidad máxima de personas
    if (read == 10){
     lcd.setCursor(0, 0);
     lcd.clear();
     lcd.print("#PersonasMAX ");
     
     lcd.print(MAXValue);
     MAXValue = 0;
     // Ciclo que espera que se presione flecha arriba
     // para setear el nuevo valor maximo.
     while (true){
       	
        if (irrecv.decode(&results)) {
          read = value_read(results.value);
          
          // Si se presiona flecha arriba
          if (read == 11){
            lcd.clear();
            lcd.print("#Personas");
            lcd.setCursor(0, 1);
            column= 0;
            break;
          }
          // Mientras no sea la tecla de función
          else if ((read != 10)){
            
            lcd.setCursor(column, 1);
            column++;
            MAXValue = read + MAXValue * 10;
            lcd.print(read);
          }
          irrecv.resume();
      	}
      }
    }
    
    irrecv.resume(); // Receive thxt value
  }
  delay(100);
  
 
}
// Devuelve el equivalente numerico del código del Sensor IR
int value_read (int code){
  for (int i = 0; i<12; i++){
    if (code == C[i]){
      return i; }
  }
}

// Lee la distancia del sensor ultrasónico
long readUltrasonicDistance(int pin)
{
  pinMode(pin, OUTPUT);  
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  return pulseIn(pin, HIGH);
}