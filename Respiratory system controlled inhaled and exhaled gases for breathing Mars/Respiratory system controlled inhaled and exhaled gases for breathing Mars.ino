float VoutArray[] =  { 0.30769 ,20.00000, 40.00000 ,60.00000 ,120.61538 ,186.76923};
float  O2ConArray[] =  { 0.00018, 2.66129, 5.32258, 8.05300, 16.19851, 25.15367};
float VoutArray1[] =  { 0.30769 ,20.00000, 40.00000 ,60.00000 ,120.61538 ,186.76923};
float  O2ConArray1[] =  { 0.00018, 2.66129, 5.32258, 8.05300, 16.19851, 25.15367};

int mq135_ex = 0;
int mq135_in = 0;

volatile int pulsos; // Cantidad de pulsos del sensor. Como se usa dentro de una interrupcion debe ser volatile
unsigned int litrosPorHora; // Calculated litres/hour
unsigned int militrosPorMinuto;
unsigned char sensorDeFlujo = 2; // Pin al que esta conectado el sensor
unsigned long tiempoAnterior; // Para calcular el tiempo
unsigned long pulsosAcumulados; // Pulsos acumulados
float litros; // Litros acumulados

void flujo () // Funcion de interrupcion
{
  pulsos++; // Simplemente sumar el numero de pulsos
}

void setup()
{
  pinMode(sensorDeFlujo, INPUT);
  Serial.begin(9600);
  attachInterrupt(0, flujo, RISING); // Setup Interrupt
  interrupts(); // Habilitar interrupciones
}


void loop() {
  tiempoAnterior = millis(); // Updates cloopTime
  pulsosAcumulados += pulsos;
  litrosPorHora = (pulsos * 60 / 6.67); // (Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour
  militrosPorMinuto = (litrosPorHora / 60) * 1000;
  pulsos = 0; // Reset Counter

  mq135_ex = analogRead(2);
  mq135_in = analogRead(3);
  mq135_ex = map(mq135_ex, 0, 1023, 0, 1000) / 100;
  mq135_in = map(mq135_in, 0, 1023, 0, 1000) / 100;

  Serial.print("CO2 Exhalado: ");
  Serial.print(mq135_ex);
  Serial.print("%   ");
  Serial.print("CO2 Inhalado: ");
  Serial.print(mq135_in);
  Serial.print("%   ");
  Serial.print("O2 Exhalado: ");
  Serial.print(readConcentration(A0));
  Serial.print("%   ");
  Serial.print("O2 Inhalado: ");
  Serial.print(readConcentration1(A1));
  Serial.print("%   ");
  Serial.print(militrosPorMinuto, DEC); // Print litres/hour
  Serial.print(" mL/min.  ");
  float consumo_oxigeno=(readConcentration1(A1)-readConcentration(A0))*militrosPorMinuto;
  float produccion_co2=(mq135_ex-mq135_in)*militrosPorMinuto;
  float coeficiente=(mq135_ex-mq135_in)/(readConcentration1(A1)-readConcentration(A0));
  Serial.print("VO2: ");
  Serial.print(consumo_oxigeno);
  Serial.print(" mL/min.  ");
  Serial.print("DO2: ");
  Serial.print(produccion_co2);
  Serial.print(" mL/min.  ");
  Serial.print("Coeficiente ");
  Serial.println(coeficiente); 
  delay(1000);
}

//ESTA FUNCION ES PARA LOS CALCULOS DEL GROVE 1
float readConcentration(uint8_t analogpin)
{
    float MeasuredVout = analogRead(A0) * (3.3 / 1023.0);   
    float Concentration = FmultiMap(MeasuredVout, VoutArray,O2ConArray, 6);
    float Concentration_Percentage=Concentration*100;
    return Concentration_Percentage;
}

float FmultiMap(float val, float * _in, float * _out, uint8_t size){
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];
 
  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while(val > _in[pos]) pos++;
 
  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];
 
  // interpolate in the right segment for the rest
  return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}

//ESTA FUNCION ES PARA LOS CALCULOS DEL GROVE 2
float readConcentration1(uint8_t analogpin1)
{
    float MeasuredVout1 = analogRead(A1) * (3.3 / 1023.0);   
    float Concentration1 = FmultiMap1(MeasuredVout1, VoutArray1,O2ConArray1, 6);
    float Concentration_Percentage1=Concentration1*100;
    return Concentration_Percentage1;
}

float FmultiMap1(float val1, float * _in1, float * _out1, uint8_t size){
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val1 <= _in1[0]) return _out1[0];
  if (val1 >= _in1[size-1]) return _out1[size-1];
 
  // search right interval
  uint8_t pos1 = 1;  // _in[0] allready tested
  while(val1 > _in1[pos1]) pos1++;
 
  // this will handle all exact "points" in the _in array
  if (val1 == _in1[pos1]) return _out1[pos1];
 
  // interpolate in the right segment for the rest
  return (val1 - _in1[pos1-1]) * (_out1[pos1] - _out1[pos1-1]) / (_in1[pos1] - _in1[pos1-1]) + _out1[pos1-1];
}

