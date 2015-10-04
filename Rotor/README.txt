
Commands:

CalCurr;	->	OK;			(Calibrate Current (set Zero))
SetDir:\d+;	->	OK;			(Calibrate Direction 0-N)
SetMax:\d+;	->	OK;			(Set MAX turning)
SetMin:\d+;	->	OK;			(Set MIN turning)
Go:\d+; 	-> 	OK;			(Goto Dir)
Stop;		->	OK;			(Stop)
Status;		->	OK:\d+:\d+:\d+;		(Return Current Pos, Aim & Current)
