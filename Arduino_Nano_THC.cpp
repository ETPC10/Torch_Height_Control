//NOT TO BE USED BY FUCKWITS
//Sketch requires no Z movement from CNC-Controller during cutting cycle. DIR from CNC-Controller must be 0.
//Use of "Pierce Delay" and "Pause at the end of cut" may be a good Idea
//Because it only works when ArcOK is true, it is save to use a Laser on the Plasma Table.



float pos_old = 0; //Stores hight of torch. 


void setup() {
	pinMode(2, INPUT);   //Torch to low
	pinMode(3, INPUT);   //Torch to hight
	pinMode(4, INPUT);   //Arc OK
	pinMode(11, OUTPUT); //Step Pin
	pinMode(12, OUTPUT); //DIR Pin
} //END of setup()

void movetorch(bool dir, int steps) {
	//1=up 0=down
	int StepPin = 11;
	int DirPin = 12;
	int i;

	if (dir = 1) {                  //Change if direction is reversed
		digitalWrite(DirPin, HIGH);	//UP		
	}
	else {
		digitalWrite(DirPin, LOW);  //DOWN
	}


	for (i = 0; i <= steps; i++) {        
			digitalWrite(DirPin, HIGH);
			digitalWrite(DirPin, LOW);
			delay(1); //slows down movement of torch, may depend on stepper driver 
	}
}// END of movetorch() 

void mmts( int pos_new) { //function for math stuff. 
	float stepsmm = 273;  //Machine Specific 
	float way;
	int steps, softlimit = 5; //Maximum Way torch will travel up. Starts in the middel of it. i.e. if its set to 5 it will travel 2.5mm up 
	bool dir;

	way = pos_new - pos_old;  //Calcuates the way to travel 
	
	if (way > 0) {   //checks if move is up or down
		dir = 1;
	}

	if (way < 0) {
		dir = 0;
		way = way * (-1);
	}

	if (pos_new < (softlimit*0.5)) {	//checks if move is legal
		steps = way * stepsmm;
		movetorch(dir, steps);
		pos_old = pos_new;
	}
	else {
	//May add warning LED
	}

} //End of mmts()


void loop() {
	int lowPin = 2;
	int highPin = 3;
	int okPin = 4;
	int i;
	bool lp, hp, okp;
	float delta = 0.05; //Way torch travels per run. Must be bigger than 1/(Steps per mm of machine)

	//delay(20); //Slows down everything to avoid too fast actions, probably unnecessary.

	okp = digitalRead(okPin);//debounce
	delay(5);
	if (digitalRead((okPin) == okp) && (okp == 1)) { //True if not bouncing and pin on high
	 //If Arc is OK

		
								
					  //reads low-pin 
		lp = digitalRead(lowPin);//debounce
		delay(5);

			if (digitalRead((lowPin) == lp) && (lp == 1)) { //True if not bouncing and pin on high
				mmts((pos_old+delta));  //lifts torch
			}

		

						//reads hight-pin  
		hp = digitalRead(highPin);//debounce
		delay(5);

			if (digitalRead((highPin) == hp) && (hp == 1)) { //True if not bouncing and pin on highs
				mmts((pos_old - delta));  //lowers torch
			}
		}
	}
	else {
		//if arc is not ok, torch is off (should be at least)
		if (pos_old != 0) {
			mmts(0); //Moves torche back to 0;
		}

	}

}// END of loop()

// There's always one more bug. 