
#define inMat A0
#define midMat A2
#define outMat A4

int processing_delay = 250;
int units_per_person = 25;
int inValue = 0, midValue = 0, outValue = 0;
int count = 0;
long start_time, stop_time;
int threshold_inMat, threshold_midMat, threshold_outMat;
int step_array[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int nine_sequence_array[46][10] = {
            {1, 2, 3, 1, 2, 3, 1, 2, 3, 3},
            {1, 2, 3, 1, 2, 3, 3, 2, 1, 1},
            {1, 2, 3, 3, 2, 1, 1, 2, 3, 1},
            {1, 2, 3, 3, 2, 1, 3, 2, 1, -1},
            {3, 2, 1, 1, 2, 3, 1, 2, 3, 1},
            {3, 2, 1, 1, 2, 3, 3, 2, 1, -1},
            {3, 2, 1, 3, 2, 1, 1, 2, 3, -1},
            {3, 2, 1, 3, 2, 1, 3, 2, 1, -3},

            {3, 1, 2, 3, 1, 2, 3, 2, 1, 1},  // person at 3 waits for 2 others to come or go before going out
            {3, 3, 2, 1, 3, 2, 1, 2, 1, -3},
            {3, 1, 2, 3, 3, 2, 1, 2, 1, -1},
            {3, 3, 2, 1, 1, 2, 3, 2, 1, -1},

            {3, 2, 1, 2, 3, 1, 2, 3, 1, 1},  // person at 2 waiting for 2 people to come or go before going out
            {3, 2, 3, 2, 1, 3, 2, 1, 1, -3},
            {3, 2, 1, 2, 3, 3, 2, 1, 1, -1},
            {3, 2, 3, 2, 1, 1, 2, 3, 1, -1},

            {1, 1, 2, 3, 1, 2, 3, 2, 3, 3},  // person at 1 waits for 2 others to come or go before going in
            {1, 3, 2, 1, 3, 2, 1, 2, 3, -1},
            {1, 1, 2, 3, 3, 2, 1, 2, 3, 1},
            {1, 3, 2, 1, 1, 2, 3, 2, 3, 1},

            {1, 2, 1, 2, 3, 1, 2, 3, 3, 1},  // person at 2 waiting for 2 people to come or go before going in
            {1, 2, 3, 2, 1, 3, 2, 1, 3, -3},
            {1, 2, 1, 2, 3, 3, 2, 1, 3, -1},
            {1, 2, 3, 2, 1, 1, 2, 3, 3, -1},

            {3, 1, 2, 3, 1, 2, 3, 0, 0, 2},  // person at 3 waits for 2 others to come or go before going back in
            {3, 3, 2, 1, 3, 2, 1, 0, 0, -2},
            {3, 1, 2, 3, 3, 2, 1, 0, 0, 0},
            {3, 3, 2, 1, 1, 2, 3, 0, 0, 0},

            {3, 2, 1, 2, 3, 1, 2, 3, 3, 2},  // person at 2 waiting for 2 people to come or go before going back in
            {3, 2, 3, 2, 1, 3, 2, 1, 3, -2},
            {3, 2, 1, 2, 3, 3, 2, 1, 3, 0},
            {3, 2, 3, 2, 1, 1, 2, 3, 3, 0},

            {1, 1, 2, 3, 1, 2, 3, 0, 0, 2},  // person at 1 waits for 2 others to come or go before going back out
            {1, 3, 2, 1, 3, 2, 1, 0, 0, -2},
            {1, 1, 2, 3, 3, 2, 1, 0, 0, 0},
            {1, 3, 2, 1, 1, 2, 3, 0, 0, 0},

            {1, 2, 1, 2, 3, 1, 2, 3, 1, 2},  // person at 2 waiting for 2 people to come or go before going back out
            {1, 2, 3, 2, 1, 3, 2, 1, 1, -2},
            {1, 2, 1, 2, 3, 3, 2, 1, 1, 0},
            {1, 2, 3, 2, 1, 1, 2, 3, 1, 0},

            {1, 2, 1, 3, 2, 1, 2, 3, 0, 0}, // person at 2 steps down to 1 to allow a person to go out before going in
            {1, 2, 3, 1, 2, 1, 2, 3, 0, 0},
            
            {1, 2, 1, 3, 2, 3, 2, 1, 0, 0}, // person at 3 waits for person at 1 (gone back from 2) to go in before going out
            {1, 2, 3, 1, 2, 3, 2, 1, 0, 0},
            
            {1, 2, 1, 3, 2, 1, 3, 2, 3, 3},  // people going in one after the other at each step
            {3, 2, 3, 1, 2, 3, 1, 2, 3, -3}
    };
    
    int six_sequence_array[4][7] = {
            {1, 1, 2, 2, 3, 3, 1},  // person stepping twice on each step
            {3, 3, 2, 2, 1, 1, -1},

            {1, 2, 1, 3, 2, 3, 2},
            {3, 2, 3, 1, 2, 1, -2}
      };
      
    int three_sequence_array[2][4] = {
            {1, 2, 3, 1},
            {3, 2, 1, -1}
      };
      
void setup() {
  threshold_inMat = analogRead(inMat) + units_per_person;
  threshold_midMat = analogRead(midMat) + units_per_person;
  threshold_outMat = analogRead(outMat) + units_per_person;
  start_time = millis();
  Serial.begin(9600);
}

void loop() {
  inValue = analogRead(inMat);
  midValue = analogRead(midMat);
  outValue = analogRead(outMat);

  on_step(1, inValue, &threshold_inMat);
  
  on_step(2, midValue, &threshold_midMat);
  
  on_step(3, outValue, &threshold_outMat);

  stop_time = millis();
  timeout_refresh();
}

void nine_sequence_count(){
    int match = 0;
    bool break_flag = false;
    for(int i = 0; i < 42 ; i++){
      match = 0;
        for(int j = 0; j < 9 ; j++){
            if(step_array[j] == nine_sequence_array[i][j]){
                match++;  
            }
            if(match == 9){
                count += nine_sequence_array[i][9];
                Serial.print("nine seq: ");
                Serial.println(nine_sequence_array[i][9]);
                memset(step_array, 0, sizeof(step_array));
                break_flag = true;
                break;  
            }
        }
        if(break_flag){
              break;
        }
    }
    start_time = millis();
    Serial.print("count: ");
    Serial.println(count);
}

void six_sequence_count(){
    int match = 0;
    bool break_flag = false;
    for(int i = 0; i < 4 ; i++){
      match = 0;
        for(int j = 0; j < 6 ; j++){
            if(step_array[j] == six_sequence_array[i][j]){
                match++;  
            }
            if(match == 6){
                count += six_sequence_array[i][6];
                Serial.print("six seq: ");
                Serial.println(six_sequence_array[i][6]);
                memset(step_array, 0, sizeof(step_array));
                break_flag = true;
                break;  
            }
        }
        if(break_flag){
          break;
        }
    }
    start_time = millis();
    Serial.print("count: ");
    Serial.println(count);
}

void three_sequence_count(){
    int match = 0;
    bool break_flag = false;
    for(int i = 0; i < 2 ; i++){
        match = 0;
        for(int j = 0; j < 3 ; j++){
            if(step_array[j] == three_sequence_array[i][j]){
                match++;  
            }
            if(match == 3){
                count += three_sequence_array[i][3];
                Serial.print("three seq: ");
                Serial.println(three_sequence_array[i][3]);
                memset(step_array, 0, sizeof(step_array));
                break_flag = true;
                break;  
            }
        }
        if(break_flag){
              break;
        }
    }
    start_time = millis();
    Serial.print("count: ");
    Serial.println(count);
}

void on_step(int number, int step_value, int *threshold){
  if(step_value > *threshold){
    *threshold = step_value + units_per_person;
    Serial.print("at step ");
    Serial.print(number);
    Serial.print("    threshold: ");
    Serial.println(*threshold);
    process_step(number); 
    check_length(); 
    start_time = millis();
  }    
}

void process_step(int number){
    delay(processing_delay);
    for(int i = 0; i < 9; i++){
        if(step_array[i] == 0){
            step_array[i] = number;
            break;
        }  
    }    
    Serial.print("step array: ");
    for(int i = 0; i < 9; i++){
        Serial.print(step_array[i]);
        Serial.print(" ");
    }
    Serial.println("");
}

void check_length(){
    int index = 9;
    for(int i = 0; i < 10; i++){
        if(step_array[i] == 0){
            index = i;
            break;
        }  
    }
    Serial.print("index: ");
    Serial.println(index);
//    if(index == 3){
//      three_sequence_count();  
//    }   
    if(index == 6){
        six_sequence_count();
    }
    else if(index == 9 || index == 8){
        nine_sequence_count();
        memset(step_array, 0, sizeof(step_array));
    }
}

void timeout_refresh(){
  if((stop_time - start_time) >= 5000){
    int index = 9;
    for(int i = 0; i < 10; i++){
      if(step_array[i] == 0){
          index = i;
          break;
      }  
    }
    if(index == 3){
      three_sequence_count();
    }
  }
  if((stop_time - start_time) >= 15000){
    Serial.println("timeout, resetting values");
    start_time = millis();
    memset(step_array, 0, sizeof(step_array));
    threshold_inMat = analogRead(inMat) + units_per_person;
    threshold_midMat = analogRead(midMat) + units_per_person;
    threshold_outMat = analogRead(outMat) + units_per_person;
  }  
}
