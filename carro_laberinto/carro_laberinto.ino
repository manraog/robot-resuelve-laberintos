#include <Ultrasonic.h>

/*////////////////////////////
  Conexión de pines
//////////////////////////////*/

// mapeo pines puente H lm293D
#define ENGINE_LEFT_L1 0 // pin 2
#define ENGINE_LEFT_L2 1 // pin 7
#define ENGINE_RIGHT_R1 2 // pin 10
#define ENGINE_RIGHT_R2 3 // pin 15

// Se usan pines analógicos (PWM) para poder controlar la velocidad (voltaje) de los motores
#define ENGINE_ENABLE_1 A4 // pin 1
#define ENGINE_ENABLE_2 A5 // pin 9
#define LED_SENSOR_I 11 // led indicador
#define LED_SENSOR_C 12 // led indicador
#define LED_SENSOR_D 13 // led indicador

// Foto transistores. Señal de HIGH = BLANCO, LOW = NEGRO. Se utiliza el circuito CNY70 (diodo emisor infrarrojo y foto transistor receptor)
#define SENSOR_I 6
#define SENSOR_FI 5
#define SENSOR_FD 4
#define SENSOR_D 7

// Control
#define VELOCIDAD 128 //128
#define ADELANTE 1
#define ATRAS 0
#define IZQUIERDA 0
#define DERECHA 1

//Ultrasonico
#define TRIGGER 8
#define ECHO 9
#define DISTANCIA 8
Ultrasonic ultrasonico(TRIGGER,ECHO);


//----------- FUNCIONES DEFINIDAS ---------------------

/*////////////////////////////
  Controlar Motores individuales
//////////////////////////////*/
void girarMotorIzquierdo(byte sentido, byte velocidad){
  if(sentido == ADELANTE){
    digitalWrite(ENGINE_LEFT_L1, HIGH);
    digitalWrite(ENGINE_LEFT_L2, LOW); 
    analogWrite(ENGINE_ENABLE_1, velocidad);
  }
  else if ( sentido == ATRAS ){
    digitalWrite(ENGINE_LEFT_L1, LOW);
    digitalWrite(ENGINE_LEFT_L2, HIGH); 
    analogWrite(ENGINE_ENABLE_1, velocidad);
  }
}

void girarMotorDerecho(byte sentido, byte velocidad){
  if(sentido == ADELANTE){
    digitalWrite(ENGINE_RIGHT_R1, HIGH);
    digitalWrite(ENGINE_RIGHT_R2, LOW); 
    analogWrite(ENGINE_ENABLE_2, velocidad);
  }
  else if ( sentido == ATRAS ){
    digitalWrite(ENGINE_RIGHT_R1, LOW);
    digitalWrite(ENGINE_RIGHT_R2, HIGH); 
    analogWrite(ENGINE_ENABLE_2, velocidad);
  }
}
void pararMotorIzquierdo( void ){
    digitalWrite(ENGINE_LEFT_L1, LOW);
    digitalWrite(ENGINE_LEFT_L2, LOW); 
    analogWrite(ENGINE_ENABLE_1, 0);
}
void pararMotorDerecho( void ){
    digitalWrite(ENGINE_RIGHT_R1, LOW);
    digitalWrite(ENGINE_RIGHT_R2, LOW); 
    analogWrite(ENGINE_ENABLE_2, 0);
}


/*////////////////////////////
  Avanzar Adelante
//////////////////////////////*/
void avanzar(byte velocidad){
  if ( digitalRead(SENSOR_FI) == LOW && digitalRead(SENSOR_FD) == LOW )
  {
    girarMotorDerecho(ADELANTE, velocidad);
    girarMotorIzquierdo(ADELANTE, velocidad);
  }
  else if ( digitalRead(SENSOR_FI) == LOW && digitalRead(SENSOR_FD) == HIGH )
  {
    girarMotorDerecho(ADELANTE, velocidad);
    girarMotorIzquierdo(ATRAS, velocidad);
  }
  else if ( digitalRead(SENSOR_FI) == HIGH && digitalRead(SENSOR_FD) == LOW )
  {
    girarMotorIzquierdo(ADELANTE, velocidad);
    girarMotorDerecho(ATRAS, velocidad);
  }
}


/*////////////////////////////
  Girar
//////////////////////////////*/
void girar(byte sentido, byte velocidad )
{
  if ( sentido == IZQUIERDA)
  {
    girarMotorDerecho(ADELANTE, velocidad);
    girarMotorIzquierdo(ATRAS, velocidad);
  }
  else
  {
    girarMotorDerecho(ATRAS, velocidad);
    girarMotorIzquierdo(ADELANTE, velocidad);
  }
}

/*////////////////////////////
  Parar
//////////////////////////////*/
void parar( void )
{
	pararMotorIzquierdo();
	pararMotorDerecho();
	delay(500);
}

/*///////////////////////////////////////////////7
  Muestra estado de sensores  IR mediante LEDs
/////////////////////////////////////////////////*/
void readSensors( void ) //Muestra en los LEDs rojos el estado de SENSOR_I y SENSOR_D
{
  if (digitalRead(SENSOR_I) == LOW)
    digitalWrite(LED_SENSOR_I,LOW);
  else
    digitalWrite(LED_SENSOR_I,HIGH);

  if (digitalRead(SENSOR_D) == LOW)
    digitalWrite(LED_SENSOR_D,LOW);
  else
    digitalWrite(LED_SENSOR_D,HIGH);

  if (digitalRead(SENSOR_FI) == LOW && digitalRead(SENSOR_FD) == LOW)
    digitalWrite(LED_SENSOR_C,LOW);
  else
    digitalWrite(LED_SENSOR_C,HIGH);
}


/*///////////////////////////////////////////////
  Resuelve Laberinto de forma recursiva
/////////////////////////////////////////////////*/

bool resuelve_laberinto( void )
{
	//Avanza hasta que termine linea
	while ( digitalRead(SENSOR_FI)==LOW || digitalRead(SENSOR_FD)==LOW )
	{
		avanzar(VELOCIDAD);
		readSensors();
	}  
	parar();
	
	//===> Revisa si es la salida, bifurcacion o fin de camino
	 //Es Salida
	if ( ultrasonico.distanceRead() < DISTANCIA )
		return true;
	
	 //Es Fin de camino
	else if ( digitalRead(SENSOR_I)==HIGH && digitalRead(SENSOR_D)==HIGH  )
	{
		//Gira 180°
		while ( digitalRead(SENSOR_FI)==HIGH && digitalRead(SENSOR_FD)==HIGH  )
		{
			girar(IZQUIERDA,VELOCIDAD);
		}
		parar();
		return false;
	}
	
	//Es Bifurcacion
	else
	{
		//Elige camino al azar
		int camino1, camino2;
		camino1 = int( random(2) );
        if (camino1 == IZQUIERDA)
            camino2 = DERECHA;
        else
            camino2 = IZQUIERDA;
		////////Camino 1
		//Gira
		while ( digitalRead(SENSOR_FI)==HIGH || digitalRead(SENSOR_FD)==HIGH  )
		{
			girar(camino1,VELOCIDAD);
		}
		parar();
		//Recursion
		if ( resuelve_laberinto() == true  )
			return true;
		//Avanza hasta inicio de siguiente camino
		while ( digitalRead(SENSOR_I)==HIGH && digitalRead(SENSOR_D)==HIGH )
		{
		  avanzar(VELOCIDAD);
		  readSensors();
		}
		parar();
		///////Camino 2
		//Recursion
		if ( resuelve_laberinto() == true  )
			return true;
		//Avanza hasta inicio de siguiente camino
		while ( digitalRead(SENSOR_I)==LOW || digitalRead(SENSOR_D)==LOW )
		{
		  avanzar(VELOCIDAD);
		  readSensors();
		}
		parar();
		while ( digitalRead(SENSOR_I)==HIGH && digitalRead(SENSOR_D)==HIGH )
		{
		  avanzar(VELOCIDAD);
		  readSensors();
		}
		parar();
			//Gira
		if ( digitalRead(SENSOR_I)==LOW && digitalRead(SENSOR_D)==HIGH )
		{
		  while ( digitalRead(SENSOR_I)==LOW || digitalRead(SENSOR_D)==LOW  )
		  {
			girar(IZQUIERDA,VELOCIDAD);
			readSensors();
		  }
		}
		else if ( digitalRead(SENSOR_I)==HIGH && digitalRead(SENSOR_D)==LOW )
		{
		  while ( digitalRead(SENSOR_I)==LOW || digitalRead(SENSOR_D)==LOW  )
		  {
			girar(DERECHA,VELOCIDAD);
			readSensors();
		  }
		} 
		parar();
			return false;
		}
	}


//--------------------- FUNCIONES ARDUINO  --------------------------------

/*////////////////////////////
  SETUP
//////////////////////////////*/
void setup()
{  
  pinMode(ENGINE_LEFT_L1, OUTPUT);
  pinMode(ENGINE_LEFT_L2, OUTPUT);
  pinMode(ENGINE_RIGHT_R1, OUTPUT);
  pinMode(ENGINE_RIGHT_R2, OUTPUT);
  pinMode(ENGINE_ENABLE_1, OUTPUT);
  pinMode(ENGINE_ENABLE_2, OUTPUT);
  
  pinMode(SENSOR_I, INPUT);
  pinMode(SENSOR_FI, INPUT);
  pinMode(SENSOR_FD, INPUT);
  pinMode(SENSOR_D, INPUT);

  pinMode(LED_SENSOR_I, OUTPUT);
  pinMode(LED_SENSOR_C, OUTPUT);
  pinMode(LED_SENSOR_D, OUTPUT);

  digitalWrite(ENGINE_LEFT_L1, HIGH);
  digitalWrite(ENGINE_LEFT_L2, LOW);
  digitalWrite(ENGINE_RIGHT_R1, HIGH);
  digitalWrite(ENGINE_RIGHT_R2, LOW);
  digitalWrite(ENGINE_LEFT_L1, HIGH);
  digitalWrite(ENGINE_LEFT_L2, LOW); 
  
  randomSeed(analogRead(0));
}
  

/*//////////////////
  Main Loop
///////////////////*/
void loop()
{
/*
    //Avanza hasta inicio de siguiente camino
    while ( digitalRead(SENSOR_I)==HIGH && digitalRead(SENSOR_D)==HIGH )
    {
      avanzar(VELOCIDAD);
      readSensors();
    }
    parar();
    //Gira
    if ( digitalRead(SENSOR_I)==LOW && digitalRead(SENSOR_D)==HIGH )
    {
      while ( digitalRead(SENSOR_I)==LOW || digitalRead(SENSOR_D)==LOW  )
      {
        girar(IZQUIERDA,VELOCIDAD);
        readSensors();
      }
    }
    else if ( digitalRead(SENSOR_I)==HIGH && digitalRead(SENSOR_D)==LOW )
    {
      while ( digitalRead(SENSOR_I)==LOW || digitalRead(SENSOR_D)==LOW  )
      {
        girar(DERECHA,VELOCIDAD);
        readSensors();
      }
    } 
    parar();
    //Avanza un poco
    while ( digitalRead(SENSOR_I)==HIGH || digitalRead(SENSOR_D)==HIGH )
    {
      avanzar(VELOCIDAD);
      readSensors();
    }
    parar();
    while ( digitalRead(SENSOR_I)==LOW || digitalRead(SENSOR_D)==LOW )
    {
      avanzar(VELOCIDAD);
      readSensors();
    }
    parar();
    while(1)
    {
      readSensors();
    }*/
  ///////////////
	delay(800);
	if ( resuelve_laberinto()==true )
	{
		pararMotorIzquierdo();
		pararMotorDerecho();
		while (1) //Detiene Loop
		{
			digitalWrite(LED_SENSOR_I, LOW);
			digitalWrite(LED_SENSOR_D, LOW);
			digitalWrite(LED_SENSOR_C, LOW);
			delay(400);
			digitalWrite(LED_SENSOR_C, HIGH);
			delay(400);
		}
	}
	else
	{
		while(1)
		{
			digitalWrite(LED_SENSOR_C, LOW);
			digitalWrite(LED_SENSOR_I, LOW);
			digitalWrite(LED_SENSOR_D, LOW);
			delay(400);
			digitalWrite(LED_SENSOR_I, HIGH);
			digitalWrite(LED_SENSOR_D, HIGH);
			delay(400);
		}	
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
}
