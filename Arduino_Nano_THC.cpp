//NOT TO BE USED BY FUCKWITS
//Sketch requires no Z movement from CNC-Controller during cutting cycle. DIR from CNC-Controller must be 0.
//Use of "Pierce Delay" and "Pause at the end of cut" may be a good Idea
//Because it only works when ArcOK is true, it is save to use a Laser on the Plasma Table.

 

/*
 
How it should work:

If the torch has fired up, the comparator board sets ArcOk signal to hight

The micro controller sets the DIR and drives up the torch for a specific hight. Then checks the pin again. 

If the comparator signals that the torch is high enough (voltage inside death zone), nothing will be done.

If the voltage is too high, the comparator will send a Signal. Same process as up, but different direction of movement.

After turning off the torch, torch is moved back to 0.


*/


float mm_already_moved = 0; //Stores hight of torch. 
bool wasok=0; //true = torch was already moved, false = torch was not moved.

void setup() {
  pinMode(2, INPUT);   //Torch to low
  pinMode(3, INPUT);   //Torch to hight
  pinMode(4, INPUT);   //Arc OK
  pinMode(11, OUTPUT); //Step Pin
  pinMode(12, OUTPUT); //DIR Pin
} //END of setup()

void movetorch(bool dir){
  //1=up 0=down

  float WayToMove = 0.100; //Way Axis moves per controller run[mm];    
  float TravelMax_UP = 5; //Maximum way torch travels up from first Arc Ok [mm].
  float TravelMax_DOWN = 0; //Maximum way torch travels down from first Arc Ok [mm]. Should be <0;


  int StepsToMove; //Number of Steps Axis moves
  int StepPin = 11;
  int DirPin = 12;
  float StepsPermm = 200; //nachschauen //depends on your machine.
  int msBetweenSteps = 1; // milliseconds, more = slower
  int i;
  bool MoveIsAllowed = false;

   
  if (dir=1){ //  UP                                                  Change if direction is reversed (1 form 2 spots)
    if(mm_already_moved<=TravelMax_UP){ //Checks if Axis hasn't moved down to far.
      digitalWrite(DirPin, HIGH); 
      mm_already_moved+=WayToMove; //adds WayToMove to mm_already_moved; (Positive direction)
      MoveIsAllowed = true; //Gives clearance for moving torch
      } 
  }    

  
  if (dir=0){ //Down                                                  Change if direction is reversed (2 form 2 spots)
    if(mm_already_moved>=TravelMax_DOWN){ //Checks if Axis hasn't moved down to far.
      digitalWrite(DirPin, LOW); 
      mm_already_moved-=WayToMove; //adds WayToMove to mm_already_moved; (Negative direction)
      MoveIsAllowed = true; //Gives clearance for moving torch
      } 
  }  

  
  StepsToMove =  WayToMove * StepsPermm ; //calculates Steps required to move WayToMove mm. Result is int because it must be in N|.
  
  if(MoveIsAllowed==true){//only moves torch if clearance is given 

      for ( i = 0; i <= StepsToMove; i++){        //Moves Torch WayToMove mm / StepsToMove
        digitalWrite(DirPin, HIGH);
        digitalWrite(DirPin, LOW);
        delay(msBetweenSteps); //slows down movement of torch
      }

  }
}// END of movetorch() 

void loop() {
  int lowPin = 2;  
  int highPin = 3;    
  int okPin = 4;
  bool lp, hp, okp;
  float tolerance =0.05;  //tolerance for going back to 0mm hight; 
 
  //delay(20); //Slows down everything to avoid too fast actions, probably unnecessary.
  
  okp=digitalRead(okPin);//debounce
  delay (5);
  if(digitalRead((okPin)==okp)&&(okp==1)){ //True if not bouncing and pin on high
   //If Arc is OK

      wasok=true; //Sets wasok to true, required for later reseting hight to 0;
      
 
      //reads low-pin 
      lp=digitalRead(lowPin);//debounce
      delay (5);
      
      if(digitalRead((lowPin)==lp)&&(lp==1)){ //True if not bouncing and pin on high
        movetorch(1);  //lifts torch
      }
      
      else{ //Else avoids dual movement in one code run, maybe suppresses oscillations 
      
         //reads hight-pin  
         hp=digitalRead(highPin);//debounce
         delay (5);
         
         if(digitalRead((highPin)==hp)&&(hp==1)){ //True if not bouncing and pin on high
          movetorch(0);  //lowers torch
         }   
      }
  }
  else{
      //if arc is not ok, torch is off (should be at least)
      if(wasok==true){
        
        if(mm_already_moved > tolerance){movetorch(0);} //if torch is above tolerance hight, torch will be lowered;
        
        if(mm_already_moved < ((-1)*tolerance)){movetorch(1);}//if torch is lower then lower tolerance limit, torch will be raised
        
        if((mm_already_moved < tolerance)&&(mm_already_moved > ((-1)*tolerance))){wasok=false;}  //if torch is inside the tolerance, 
                                                                                                  //wasok will be set false. Cutting cycle completed        
          
      }   
 
  }
  
}// END of loop()

// There's always one more bug. 
