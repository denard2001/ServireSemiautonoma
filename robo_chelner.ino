#include <EEPROM.h>
#include <IRremote.h>

#define ENABLE_A 6
#define FRONT_A 4
#define REAR_A 5
#define ENABLE_B 9
#define FRONT_B 7
#define REAR_B 8

#define TOP_LEFT_SENSOR 10
#define LEFT_SENSOR 11
#define RIGHT_SENSOR 12
#define TOP_RIGHT_SENSOR 13

#define IR_RECIEVER 3

#define TRIG_PIN A0
#define HCSR_OUTPUT_PIN A1 

#define NOT_CONFIGURED -1
#define KITCHEN 0
#define ROAD_TO_TABLE 1 
#define ROAD_TO_KITCHEN 2 
#define TABLE 3
#define OBSTACLE_FOUND 4


#define RECIEVER_0 0xE916FF0
#define RECIEVER_1 0xF30CFF00
#define RECIEVER_2 0xE718FF00
#define RECIEVER_3 0xA15EFF00
#define RECIEVER_4 0xF708FF00
#define RECIEVER_5 0xE31CFF00
#define RECIEVER_6 0xA55AFF00
#define RECIEVER_7 0xBD42FF00
#define RECIEVER_8 0xAD52FF00
#define RECIEVER_9 0xB54AFF00
#define RECIEVER_OK 0xF609FF00

#define LM_SPEED 60
#define RM_SPEED 60
#define LINE_HOLDING_SPEED 200
#define DISTANCE_TO_AVOID_OBSTACLE 20
 
  


int8_t restaurant_map[20][20];
uint8_t curr_intersection,intersection_decision[20],intersection_counter, n, m, i_buc , j_buc, robot_status, table_number, old_status;
char start_dir;

IRrecv reciever(IR_RECIEVER);
decode_results results;

 void setup() {
  Serial.begin(115200);
  pinMode(FRONT_A,OUTPUT);
  pinMode(REAR_A,OUTPUT);
  pinMode(FRONT_B,OUTPUT);
  pinMode(REAR_B,OUTPUT);

  pinMode(ENABLE_A,OUTPUT);
  pinMode(ENABLE_B,OUTPUT);

  pinMode(LEFT_SENSOR, INPUT); 
  pinMode(RIGHT_SENSOR, INPUT); 
  pinMode(TOP_LEFT_SENSOR, INPUT); 
  pinMode(TOP_RIGHT_SENSOR, INPUT); 

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(HCSR_OUTPUT_PIN, INPUT);

  initialize_intersection_vector();
  
  if(read_from_eeprom())
  {
    robot_status = KITCHEN;
    get_kitchen_position_and_direction();
    
  }
  else
    robot_status = NOT_CONFIGURED;

  reciever.enableIRIn();
}

void loop() {
  if(robot_status == ROAD_TO_TABLE || robot_status == ROAD_TO_KITCHEN){

  
    uint8_t top_left_sensor = digitalRead(TOP_LEFT_SENSOR);
    uint8_t top_right_sensor = digitalRead(TOP_RIGHT_SENSOR);
    uint8_t left_sensor = digitalRead(LEFT_SENSOR);
    uint8_t right_sensor = digitalRead(RIGHT_SENSOR);
    uint8_t sum_sensors = top_left_sensor+ top_right_sensor+ left_sensor+ right_sensor;
  
    if(check_for_obstacles()){
      old_status = robot_status;
      robot_status = OBSTACLE_FOUND;
      Stop();
    }
  
    else{
      if(sum_sensors > 1){
        intersection_detected();
      }
      else{
        if(right_sensor==0 && left_sensor==0) {
            forward();
        }

          else if(right_sensor==0 && left_sensor==1) {
            left(); 
        }

          else if(right_sensor==1 && left_sensor==0) {
            right(); 
        }
      }
    }
  }
  else if(robot_status == TABLE || robot_status == KITCHEN){
    
      if(robot_status == KITCHEN)
        if(Serial.available()>0)
           config_mode();
    
    read_remote_command();
  }
  else if(robot_status == OBSTACLE_FOUND){
    if(!(check_for_obstacles()))
      robot_status = old_status;
  }
  else{
    
      if(Serial.available()>0)
        config_mode();
    
  }
  

}
bool check_for_obstacles(){
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  if(pulseIn(HCSR_OUTPUT_PIN, HIGH) *0.034 / 2 < DISTANCE_TO_AVOID_OBSTACLE)
    return true;

  return false;
}
void initialize_intersection_vector(){
  for(uint8_t i=0; i < intersection_counter; i++)
      intersection_decision[i] = 5; // initialize with 5 because we don't use this value 
}
void read_remote_command(){
  if (reciever.decode()) {

    if(robot_status == TABLE){
      if(reciever.decodedIRData.decodedRawData == RECIEVER_OK){
        curr_intersection =intersection_counter-2;
        
        robot_status = ROAD_TO_KITCHEN;
      }
    }
    else{
     switch(reciever.decodedIRData.decodedRawData){
        case RECIEVER_0:
            table_number = table_number * 10 + 0;
            break;
        case RECIEVER_1:
            table_number = table_number * 10 + 1;
            break;
        case RECIEVER_2:
            table_number = table_number * 10 + 2;
            break;
        case RECIEVER_3:
            table_number = table_number * 10 + 3;
            break;
        case RECIEVER_4:
            table_number = table_number * 10 + 4;
            break;
        case RECIEVER_5:
            table_number = table_number * 10 + 5;
            break;
        case RECIEVER_6:
            table_number = table_number * 10 + 6;
            break;
        case RECIEVER_7:
            table_number = table_number * 10 + 7;
            break;
        case RECIEVER_8:
            table_number = table_number * 10 + 8;
            break;
        case RECIEVER_9:
            table_number = table_number * 10 + 9;
            break;
        case RECIEVER_OK:
            intersection_counter = road_to_table(table_number);
            if(intersection_counter > 0){
              curr_intersection = 1; 
              robot_status = ROAD_TO_TABLE;
            }
            table_number = 0;
            break;
      }
    }
    
    delay(1500);
    reciever.resume();
  }

}
void intersection_detected()
{
  if(robot_status == ROAD_TO_TABLE){
    if(intersection_decision[curr_intersection] == 1){ // left
      digitalWrite(FRONT_A, HIGH);
      digitalWrite(REAR_A, LOW);
      digitalWrite(FRONT_B, LOW);
      digitalWrite(REAR_B, HIGH);
      analogWrite(ENABLE_A, 240);
      analogWrite(ENABLE_B, 150);
      delay(1000);
    }
    if(intersection_decision[curr_intersection] == 3){ //right
      digitalWrite(FRONT_A, LOW);
      digitalWrite(REAR_A, HIGH);
      digitalWrite(FRONT_B, HIGH);
      digitalWrite(REAR_B, LOW);
      analogWrite(ENABLE_A, 160);
      analogWrite(ENABLE_B, 250);
      delay(1000);
    }
    if(intersection_decision[curr_intersection] == 2){ //forward
      digitalWrite(FRONT_A, HIGH);
      digitalWrite(REAR_A, LOW);
      digitalWrite(FRONT_B, HIGH);
      digitalWrite(REAR_B, LOW);
      analogWrite(ENABLE_A, RM_SPEED);
      analogWrite(ENABLE_B, LM_SPEED);
      delay(500);
    }
    if(intersection_decision[curr_intersection] == 0){  //stop
      digitalWrite(FRONT_A, HIGH);
      digitalWrite(REAR_A, LOW);
      digitalWrite(FRONT_B, LOW);
      digitalWrite(REAR_B, HIGH);
      analogWrite(ENABLE_A, 200);
      analogWrite(ENABLE_B, 200);
      delay(2200);
      Stop();
      robot_status = TABLE;
    }
    curr_intersection ++;
    if(curr_intersection == intersection_counter){
      Stop();
      robot_status = TABLE;
    }
    
  }
  else{
    if(intersection_decision[curr_intersection] == 1){ //right
      digitalWrite(FRONT_A, LOW);
      digitalWrite(REAR_A, HIGH);
      digitalWrite(FRONT_B, HIGH);
      digitalWrite(REAR_B, LOW);
      analogWrite(ENABLE_A, 160);
      analogWrite(ENABLE_B, 250);
      delay(1000);
    }
    if(intersection_decision[curr_intersection] == 3){ //left
      digitalWrite(FRONT_A, HIGH);
      digitalWrite(REAR_A, LOW);
      digitalWrite(FRONT_B, LOW);
      digitalWrite(REAR_B, HIGH);
      analogWrite(ENABLE_A, 250);
      analogWrite(ENABLE_B, 160);
      delay(1000);
    }
    if(intersection_decision[curr_intersection] == 2){ // forward
      digitalWrite(FRONT_A, HIGH);
      digitalWrite(REAR_A, LOW);
      digitalWrite(FRONT_B, HIGH);
      digitalWrite(REAR_B, LOW);
      analogWrite(ENABLE_A, RM_SPEED);
      analogWrite(ENABLE_B, LM_SPEED);
      delay(500);
    }
    if(intersection_decision[curr_intersection] == 0){ // stop
      digitalWrite(FRONT_A, HIGH);
      digitalWrite(REAR_A, LOW);
      digitalWrite(FRONT_B, LOW);
      digitalWrite(REAR_B, HIGH);
      analogWrite(ENABLE_A, 200);
      analogWrite(ENABLE_B, 200);
      delay(2200);
      Stop();
      robot_status = KITCHEN;
      initialize_intersection_vector();
    }
    
    if(curr_intersection > 0)
      curr_intersection --;
    else{
      Stop();
      robot_status = KITCHEN;
      initialize_intersection_vector();
    }

  }
}

void forward()
{
  digitalWrite(FRONT_A, HIGH);
  digitalWrite(REAR_A, LOW);
  digitalWrite(FRONT_B, HIGH);
  digitalWrite(REAR_B, LOW);

  analogWrite(ENABLE_A, RM_SPEED);
  analogWrite(ENABLE_B, LM_SPEED);
}


void right()
{
  digitalWrite(FRONT_A, LOW);
  digitalWrite(REAR_A, HIGH);
  digitalWrite(FRONT_B, HIGH);
  digitalWrite(REAR_B, LOW);

  analogWrite(ENABLE_A, LINE_HOLDING_SPEED);
  analogWrite(ENABLE_B, LINE_HOLDING_SPEED);
}

void left()
{
  digitalWrite(FRONT_A, HIGH);
  digitalWrite(REAR_A, LOW);
  digitalWrite(FRONT_B, LOW);
  digitalWrite(REAR_B, HIGH);

  analogWrite(ENABLE_A, LINE_HOLDING_SPEED);
  analogWrite(ENABLE_B, LINE_HOLDING_SPEED);
}

void Stop()
{
  digitalWrite(FRONT_A, LOW);
  digitalWrite(REAR_A, LOW);
  digitalWrite(FRONT_B, LOW);
  digitalWrite(REAR_B, LOW);
}


void get_kitchen_position_and_direction(){
    for(uint8_t i = 0 ;i < n; i++)
        for(uint8_t j = 0; j < m; j++)
            if(restaurant_map[i][j] == 2){
                i_buc = i;
                j_buc = j;

                if(i != 0)
                    if(restaurant_map[i-1][j] == 1)
                        start_dir = 'N';
                if(i != n)
                    if (restaurant_map[i+1][j] == 1)
                        start_dir = 'S';
                if(j != 0)
                    if(restaurant_map[i][j-1] == 1)
                        start_dir = 'V';
                if(j != m)
                    if (restaurant_map[i][j+1] == 1)
                        start_dir = 'E';

                return;
            }
}

void get_table_position(uint8_t table_nr, uint8_t *i_table, uint8_t *j_table){
    for(uint8_t i = 0 ;i < n; i++)
        for(uint8_t j = 0; j < m; j++)
            if( -1 * restaurant_map[i][j] == table_nr)
                {
                    *i_table = i;
                    *j_table = j;
                    return;
                }
    *i_table = -1;
    *j_table = -1;
}

bool is_posible(uint8_t i, uint8_t j, uint8_t i_table, uint8_t j_table, uint8_t sol[20][20]){
    return (i >= 0 && i <n && j >= 0 && j < m && (restaurant_map[i][j] == 1 || restaurant_map[i][j] == 2) && sol[i][j] == 0 );
}

uint8_t bk_find_road(uint8_t i, uint8_t j, uint8_t i_table, uint8_t j_table, uint8_t sol[20][20]){
    if(i== i_table && j == j_table){
        sol[i][j] = 1;
        return 1;
    }

    if( is_posible(i, j, i_table, j_table, sol)){
        sol[i][j] = 1;


        if(bk_find_road(i+1, j, i_table, j_table, sol))
            return 1;

        if(bk_find_road(i-1, j, i_table, j_table, sol))
            return 1;

        if(bk_find_road(i, j+1, i_table, j_table, sol))
            return 1;

        if(bk_find_road(i, j-1, i_table, j_table, sol))
            return 1;

        sol[i][j] = 0;

        return 0;

    }
    else
        return 0;
}
uint8_t get_intersection_choices(uint8_t i_table, uint8_t j_table, uint8_t table_road[20][20]){
    uint8_t count_intersections = 0;
    uint8_t i= i_buc, j= j_buc;
    char dir = start_dir;
    intersection_decision[count_intersections++] = 0; // at first and at last intersection robot needs to stop
    while(!(i == i_table && j == j_table)){
            if(i != 0){
                if(table_road[i-1][j] == 1){
                    if(dir == 'V')
                        intersection_decision[count_intersections++] = 3;
                    else if(dir == 'E')
                        intersection_decision[count_intersections++] = 1;
                    else
                    {
                        int already_treated =0;
                        if(j != 0)
                            if(restaurant_map[i][j-1] == 1){
                                already_treated++;
                                intersection_decision[count_intersections++] = 2;
                            }
                        if(j != m)
                            if(restaurant_map[i][j+1] == 1 && already_treated == 0)
                                intersection_decision[count_intersections++] = 2;

                    }
                    dir = 'N';
                    table_road[i][j] = 0;
                    i--;
                    continue;
                }
            }

            if(i != n){
                if(table_road[i+1][j] == 1){
                    if(dir == 'V')
                        intersection_decision[count_intersections++] = 1;
                    else if(dir == 'E')
                        intersection_decision[count_intersections++] = 3;
                    else
                    {
                        int already_treated =0;
                        if(j != 0)
                            if(restaurant_map[i][j-1] == 1){
                                already_treated++;
                                intersection_decision[count_intersections++] = 2;
                            }
                        if(j != m)
                            if(restaurant_map[i][j+1] == 1 && already_treated == 0)
                                intersection_decision[count_intersections++] = 2;

                    }
                    dir = 'S';
                    table_road[i][j] = 0;
                    i++;
                    continue;
                }
            }

            if(j != 0){
                if(table_road[i][j-1] == 1){
                    if(dir == 'N')
                        intersection_decision[count_intersections++] = 1;
                    else if(dir == 'S')
                        intersection_decision[count_intersections++] = 3;
                    else
                    {
                        int already_treated =0;
                        if(i != 0)
                            if(restaurant_map[i-1][j] == 1){
                                already_treated++;
                                intersection_decision[count_intersections++] = 2;
                            }
                        if(i != n)
                            if(restaurant_map[i+1][j] == 1 && already_treated == 0)
                                intersection_decision[count_intersections++] = 2;

                    }
                    dir = 'V';
                    table_road[i][j] = 0;
                    j--;
                    continue;
                }
            }

            if(j != m){
                if(table_road[i][j+1] == 1){
                    if(dir == 'N')
                        intersection_decision[count_intersections++] = 3;
                    else if(dir == 'S')
                        intersection_decision[count_intersections++] = 1;
                    else
                    {
                        int already_treated =0;
                        if(i != 0)
                            if(restaurant_map[i-1][j] == 1){
                                already_treated++;
                                intersection_decision[count_intersections++] = 2;
                            }
                        if(i != n)
                            if(restaurant_map[i+1][j] == 1 && already_treated == 0)
                                intersection_decision[count_intersections++] = 2;

                    }
                    dir = 'E';
                    table_road[i][j] = 0;
                    j++;
                    continue;
                }
            }

    }

    intersection_decision[count_intersections++] = 0;

    return(count_intersections);

}

uint8_t road_to_table(uint8_t table_nr){

    uint8_t i_table, j_table;
    uint8_t intersection_counter = 0;
    get_table_position(table_nr, &i_table, &j_table);
    uint8_t table_road[20][20];
    for(uint8_t i = 0; i < n; i++)
      for(uint8_t j = 0 ; j < m; j++)
        table_road[i][j] = 0;   

    if(i_table != -1 && j_table != -1)
      if (bk_find_road(i_buc, j_buc, i_table, j_table, table_road))
        intersection_counter = get_intersection_choices(i_table, j_table, table_road);

    return(intersection_counter);
}

bool read_from_eeprom()
{
  n = EEPROM.read(0);
  m = EEPROM.read(1);
  if(n > 0 && m > 0 && n < 21 && m < 21){
    for(uint8_t i = 0; i < n; i++)
      for(uint8_t j = 0; j < m; j++)
        restaurant_map[i][j] =(int8_t)EEPROM.read( m * i + j + 2);
    return true;
  }
  return false;
}

void config_mode(){

    char ranges[7] = "";
    Serial.readBytes(ranges, 6);
    n = (ranges[0] - '0') * 10 + (ranges[1] - '0');
    m = (ranges[2] - '0') * 10 + (ranges[3] - '0');
    uint8_t nr_of_tables = (ranges[4] - '0') * 10 + (ranges[5] - '0');
    
    EEPROM.write(0,n);
    EEPROM.write(1,m);
    const int bytes_to_read = (n*m) + (2 * nr_of_tables);
    char readingBuffer[bytes_to_read + 1] = "";
    Serial.readBytes(readingBuffer, bytes_to_read);

    uint16_t i = 0;
    uint8_t row_index = 0, col_index = 0;
    while (i < bytes_to_read)
    {
      if(readingBuffer[i] == 'M')
      {
        restaurant_map[row_index][col_index] = -1 * ((readingBuffer[i+1] - '0') * 10 + (readingBuffer[i+2] - '0'));
        i = i+2;
      }
      else
        restaurant_map[row_index][col_index] = readingBuffer[i] - '0';

      EEPROM.write( row_index * m + col_index + 2, restaurant_map[row_index][col_index]);
      if (col_index < m-1){
        col_index++;
      }
      else
        {
          row_index++;
          col_index = 0;
        }

      i++;
    }
    robot_status = KITCHEN;
}

