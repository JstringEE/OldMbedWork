#include "MMA8452.h"
#include "mbed.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
#include "Servo.h"
#include "PinDetect.h"
#include "SDFileSystem.h"
#include "Servo.h"
#include "wave_player.h"

AnalogIn photocell(p15);

Serial pc(USBTX,USBRX);
Servo myservo(p21);
// SD CARD & SOUND
SDFileSystem sd(p5, p6, p7, p8, "sd"); //SD card
AnalogOut DACout(p18);
//SD CARD & SOUND

uLCD_4DGL uLCD(p9,p10,p11); // serial tx, serial rx, reset pin;
PinDetect pb1(p16);
PinDetect pb2(p17);
PinDetect pb3(p26);

AnalogIn pot1(p19);
AnalogIn pot2(p20);

DigitalOut led1(p30);
DigitalOut led2(p29);
DigitalOut led3(p25);
DigitalOut led4(p24);
DigitalOut led5(p23);
DigitalOut led6(p22);

Mutex lcd_mutex;

MMA8452 acc(p28, p27, 100000);
wave_player waver(&DACout);

////////////////// GLOBAL VARIABLES /////////////////////////////
int game_state = 0;
int player_guess = 1;
float dice_color = 16777215;
float result_rolled;
double xg,yg,zg;
float dot_size;

FILE *audio;
/////////////////////////////////////////////////////////////////

void one_led(){
    led1 = 1;
    led2 = 0;
    led3 = 0;
    led4 = 0;
    led5 = 0;
    led6 = 0;
    }
    
void two_led(){
    led1 = 1;
    led2 = 1;
    led3 = 0;
    led4 = 0;
    led5 = 0;
    led6 = 0;
    }
void three_led(){
    led1 = 1;
    led2 = 1;
    led3 = 1;
    led4 = 0;
    led5 = 0;
    led6 = 0;
    }
void four_led(){
    led1 = 1;
    led2 = 1;
    led3 = 1;
    led4 = 1;
    led5 = 0;
    led6 = 0;
    }
void five_led(){
    led1 = 1;
    led2 = 1;
    led3 = 1;
    led4 = 1;
    led5 = 1;
    led6 = 0;
    }
void six_led(){
    led1 = 1;
    led2 = 1;
    led3 = 1;
    led4 = 1;
    led5 = 1;
    led6 = 1;
    }
    
void guess_up() {
    player_guess++;
    if (player_guess > 6) {player_guess = 6;}
    }
    
void guess_down() {
    player_guess--;
    if (player_guess < 1) {player_guess = 1;}
    }
    

void next_state() {
    game_state++;
    if (game_state == 3) {game_state = 0;}
    else if(game_state == 2) {game_state = 1;}
}

void dice1(){
    lcd_mutex.lock();
    uLCD.filled_rectangle(20, 20, 108, 108, dice_color);
    uLCD.filled_circle(64,64,dot_size,BLACK);
    Thread::wait(1000);
    uLCD.cls();
    lcd_mutex.unlock();
    Thread::wait(1000);
}
void dice2(){
    lcd_mutex.lock();
    uLCD.filled_rectangle(17, 17, 111, 111, dice_color);
    uLCD.filled_circle(92,36,dot_size,BLACK);
    uLCD.filled_circle(36,92,dot_size,BLACK);
    Thread::wait(1000);
    uLCD.cls();
    lcd_mutex.unlock();
    Thread::wait(1000);
}
void dice3(){
    lcd_mutex.lock();

    uLCD.filled_rectangle(17, 17, 111, 111, dice_color);
    uLCD.filled_circle(64,64,dot_size,BLACK);
    uLCD.filled_circle(92,36,dot_size,BLACK);
    uLCD.filled_circle(36,92,dot_size,BLACK);
    Thread::wait(1000);
    uLCD.cls();
    lcd_mutex.unlock();
    Thread::wait(1000);   
}
void dice4(){
    lcd_mutex.lock();

    uLCD.filled_rectangle(17, 17, 111, 111, dice_color);
    uLCD.filled_circle(92,36,dot_size,BLACK);
    uLCD.filled_circle(36,92,dot_size,BLACK);
    uLCD.filled_circle(36,36,dot_size,BLACK);
    uLCD.filled_circle(92,92,dot_size,BLACK);
    Thread::wait(1000);
    uLCD.cls();
    lcd_mutex.unlock();
    Thread::wait(1000);   
}
void dice5(){
    lcd_mutex.lock();

    uLCD.filled_rectangle(17, 17, 111, 111, dice_color);
    uLCD.filled_circle(64,64,dot_size,BLACK);
    uLCD.filled_circle(92,36,dot_size,BLACK);
    uLCD.filled_circle(36,92,dot_size,BLACK);
    uLCD.filled_circle(36,36,dot_size,BLACK);
    uLCD.filled_circle(92,92,dot_size,BLACK);
    Thread::wait(1000);
    uLCD.cls();
    lcd_mutex.unlock();
    Thread::wait(1000);   
}
void dice6(){
    lcd_mutex.lock();
    uLCD.filled_rectangle(17, 17, 111, 111, dice_color);
    uLCD.filled_circle(92,36,dot_size,BLACK);
    uLCD.filled_circle(36,92,dot_size,BLACK);
    uLCD.filled_circle(36,36,dot_size,BLACK);
    uLCD.filled_circle(92,92,dot_size,BLACK);
    uLCD.filled_circle(36,64,dot_size,BLACK);//Not finished yet
    uLCD.filled_circle(92,64,dot_size,BLACK);
    Thread::wait(1000);
    uLCD.cls();
    lcd_mutex.unlock();
    Thread::wait(1000);   
}

////////////// SECOND THREAD //////////////////////////////////////////////////
void input_guess_change_settings(void const *args){
    while(true){
        if (game_state == 1){
            lcd_mutex.lock();
            uLCD.printf("Input your guess\nof the dice roll\nwith the buttons\non the breadbaord.\n\n");
            //Thread::wait(3000);
            uLCD.printf("Pick a number 1-6\nthen roll the die.\n\n");
            //Thread::wait(2000);
            uLCD.printf("If you want to\nchange the dot\nsize or color,\nyou can with the\npots.");
            Thread::wait(6000);
            uLCD.cls();
            lcd_mutex.unlock();
            player_guess = 1;
            led1 = 1;
            while(game_state == 1) {
                
                // if accelerometer changes enough, go to next state
                acc.readXYZGravity(&xg,&yg,&zg);
                zg = abs(zg);
                if(zg>1.1f || zg < 0.75f){
                    game_state = 2;
                    }
                dice_color = (floor(100.0f * float(pot1))/100.0f);
                dice_color = dice_color * 16777215.0f;
                if (float(photocell) < 0.4f) {
                    dice_color = RED;
                    }
                
                dot_size = ((floor(100.0f * float(pot2))/100.0f)*10 )+2;
                
                lcd_mutex.lock();
                uLCD.filled_rectangle(20, 20, 108, 108, dice_color);
                uLCD.filled_circle(64,64,dot_size,BLACK);
                uLCD.filled_circle(88,40,dot_size,BLACK);
                uLCD.filled_circle(40,88,dot_size,BLACK);
                Thread::wait(1000);
                uLCD.cls();
                lcd_mutex.unlock();
                
                switch (player_guess){
                    case 1: one_led();
                        break;
                    case 2: two_led();
                        break;
                    case 3: three_led();
                        break;
                    case 4: four_led();
                        break;
                    case 5: five_led();
                        break;
                    case 6: six_led();
                        break;
                    }
                    
                }
        }
    Thread::wait(1000);
    }
}
////////////////////////////////////////////////////////////////////////////////

////////////// THIRD THREAD ////////////////////////////////////////////////////
void view_result_high_five(void const *args) {
    while(true){
        if(game_state == 2){
            
            audio = fopen("/sd/roll.wav","r");
            waver.play(audio);
            fclose(audio);
            
            result_rolled = rand()%6 + 1;
            switch(int(result_rolled)){
                case 1: dice1();
                    audio = fopen("/sd/one.wav", "r");
                    waver.play(audio);
                    fclose(audio);
                    break;
                case 2: dice2();
                    audio = fopen("/sd/two.wav", "r");
                    waver.play(audio);
                    fclose(audio);
                    break;
                case 3: dice3();
                    audio = fopen("/sd/three.wav", "r");
                    waver.play(audio);
                    fclose(audio);
                    break;
                case 4: dice4();
                    audio = fopen("/sd/four.wav", "r");
                    waver.play(audio);
                    fclose(audio);
                    break;
                case 5: dice5();
                    audio = fopen("/sd/five.wav", "r");
                    waver.play(audio);
                    fclose(audio);
                    break;
                case 6: dice6();
                    audio = fopen("/sd/six.wav", "r");
                    waver.play(audio);
                    fclose(audio);
                    break;}
            Thread::wait(1000);
            
            while(game_state == 2){
                if(player_guess == result_rolled){
                    audio = fopen("/sd/won.wav", "r");
                    waver.play(audio);
                    fclose(audio);
                    lcd_mutex.lock();
                    uLCD.printf("     You WIN!!\n\n     High five!");
                    Thread::wait(1000);
                    uLCD.cls();
                    lcd_mutex.unlock();
                    for(float p=0; p<1.0; p += 0.01) {
                        myservo = p;
                        Thread::wait(0.2);
                    }
                    
                    Thread::wait(4000);
                
                    for(float p=1; p>0.0; p -= 0.01) {
                        myservo = p;
                        Thread::wait(0.2);
                    }
                    ///////////////////////////////////////
                }
                else if(player_guess != result_rolled) {
                    audio = fopen("/sd/lost.wav", "r");
                    waver.play(audio);
                    fclose(audio);
                    lcd_mutex.lock();
                    uLCD.printf("    you lose.....");
                    Thread::wait(1000);
                    uLCD.cls();
                    lcd_mutex.unlock();
                }
                lcd_mutex.lock();
                uLCD.printf("Press the button\nto play again!");
                while(game_state == 2){Thread::wait(250);}
                uLCD.cls();
                lcd_mutex.unlock();
            }
        }  
    }
}
///////////////////////////////////////////////////////////////////////////////

int main(){
    pb1.mode(PullDown);
    pb1.attach_deasserted(&guess_down);
    pb1.setSampleFrequency();
    
    pb2.mode(PullDown);
    pb2.attach_deasserted(&guess_up);
    pb2.setSampleFrequency();
    
    pb3.mode(PullDown);
    pb3.attach_deasserted(&next_state);
    pb3.setSampleFrequency();
    
    Thread t2(input_guess_change_settings);
    Thread t3(view_result_high_five);
    uLCD.baudrate(3000000); 
    while(1){
       if (game_state == 0){
          myservo = 0;
          lcd_mutex.lock();
          uLCD.printf("\n\n      Welcome!\n\n\nPress the button\nto start.");
          Thread::wait(1000);
          while(game_state == 0){Thread::wait(250);}
          uLCD.cls();
          lcd_mutex.unlock();
        }
        Thread::wait(250);
   
    }
}
