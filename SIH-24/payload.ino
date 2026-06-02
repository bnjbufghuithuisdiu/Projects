#include "HX711.h";
HX711 scale;
const int threshold_val=1.5;
float data_bucket[10];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  scale.begin(15,2); //da=2;sck=3
  
  Serial.println("Initial readings");
  scale.read_average(20);
  Serial.println("place initial weight");
  delay(5000);
  Serial.println("taring started....");
  scale.set_scale(-25000);
  Serial.println("taring over....");
  scale.tare();
  scale.get_value(10);
  
;}
bool error_checker(float data_bucket[]){
  
  bool result=true;
  for(int i=0;i<10;i++){
    
    if(-(threshold_val)>(data_bucket[i]-data_bucket[i+1]) || (data_bucket[i]-data_bucket[i+1])>threshold_val){

      result=false;

    }
  }
 
  return result;
}
float* stor_val_array(float data_bucket[]){

 
  for(int i=0;i<10;i++){
   
    int data_load_cell=scale.get_units(5);
    data_bucket[i]=data_load_cell;
    
    delay(100);
  }
  return data_bucket;
}
float normalized_val(float data_bucket[]){
  float sum=0;
  for(int i=0;i<10;i++){
    sum+=data_bucket[i];
  }
  return (sum/10);
}
void loop() {
  // put your main code here, to run repeatedly:
  
  // stor_val_array(data_bucket);
  // if(error_checker(data_bucket)){
    
    
  // }
  float num=scale.get_units(10);
    if(num>=0){
      Serial.println(num);
    }
    else {
      Serial.println(-num);
    }
  
  
  
  delay(800);

}
