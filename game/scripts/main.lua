-- Internal Revenue Service



-- DATA AND FILES, INIT

INIT { function() 
	FONT { "files/BodoniModa.ttf", 16, "BodoniModa" }
	VARIABLE { "INTEGER", "VarTest", 1 }

	PRINT("Init test 1.")
end }

INIT { function() 
	SaintMatthew = IMAGE { "SaintMatthew",		"files/images/SaintMatthew.png" }
	TitleMusic = AUDIO { "TitleMusic",		"files/audio/TitleMusic.mp3" }
	Static = MUSIC { "Static",			"files/audio/Static.mp3" }

	PRINT("Init test 2.")
end }



-- GAME 

---- Events
--		Trigger, opt.; Name;	   Execute;
--EVENT { "CONTINUOUS", "TestEvent"; function() 
--	PRINT("Called continuous event.");
--	Pause(5);
--end }
--
--EVENT { "EVENT_DISABLED", "DeleteEventHook"; function(name)
--	PRINT("The event '"..name.."' has just been disabled.");
--end }
--
EVENT { "START", "StartEvent"; function() 
	PRINT("Called event which is hooked to 'start.'");
end }

---- States
LABEL { "SPLASHSCREEN"; function()
	PRINT("Splashscreen called.");
	ShowImage("SaintMatthew");
	--ShowImage(SaintMatthew);
	PlayMusic("Static");
	Pause(2);

end }

LABEL { "MAIN_MENU"; function()
	PRINT("Main called.");
	--PlayAudio("TitleMusic");
	Pause(3);
	Quit();

end }