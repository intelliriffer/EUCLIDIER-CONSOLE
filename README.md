# Euclidier - 8 Channel Midi Euclidean Note Sequencer for Force/MPC Beta 0.1.4


Features:

1. 8 Independent Channels/Tracks
2. Custom Midi Channel Per Track
3. Upto 4 - 32 Steps / Channel
4. Custom Note Pitch / Channel
5. Velocity + Velocity Humanization / Channel  (Adds/Subtracts values up defined (max 32) from Base Velocity).
6. Sequence Shifting (Offset)
7. Different Note/Time Divisions per channel.
8. Will run on Force and Raspberry Pi
9. ** v 0.1.4 Add Loop Parameter, Loop Parameter Sets the Sequence Restart Point. Values > Steps Creates Polythythms 
10. New Midi Mapping Layout with Loop Controls added



## Installation: 
1. If Using Mockba Mod and my NodeJs App Server, Copy All the Files into your 662522/AddOns/nodeServer/modules Folder and use web to  start/stop  it.

2. If not using my NodeApp server, you can copy the euclidier file anywhere on your device and run using ssh shell (perhaps someone can better explain the process).
or if using Mockba/Kick Gen Mod, you can put in a launch script in their respective autolaunch directors to launch this in background on Startup.

## Notes:
1. If you change Steps, Pulses, Shift parameters or enable a channel , you need to start and stop Force playback to sync the sequences. (I may update this later on to auto sync once I figure out the best way)

2. There is No GUI (I may create web Gui later on ), but a Force Track Template is provided Will all parameters named and Mapped.
4: The Last 4 channels are Defaulted to ch 10,  with notes and simple pattern set for a Drum Kit.
5: The Sequencer might crash (itself  and not Force) sometimes for some values (I need to look into that)
6: It might consume 2-3% cpu as internal resolution is set to 100 microseconds (1/10 ms);


## Setup:
1. Load the Provided Track in Force
2. Set its midi output to Mockba Euclidier, out ch to 16 (all channels work right now),
3. Set Input to none for Now.
4. Create your instrument track(s) set input to ch: 1 for example: and port to Mockba Euclidier.
5. Start play on Force, it should start playing 1 channel sequence.
6. Repeat for other Channels. (go to Eculider track midi control to edit Settings : shift +  clip)
7. for Drum testing load as Kit, Set input to Mockba Euclidier, and input channel to 10. set monitoring to in or merge.
8. On Control track enable channels 5-8 (pages3-4) (5 = kick)

## Midi Mapping :
    1. CC: 1,11,21,31,41,51,61,71 : Enable Channels (1-8 Respectively) value > 64 = on

    2. CC: 2,12,22,32,42,52,62,72 :  Set Pitch / Note for Channels (1-8) Values 0-127

    3. CC: 3,13,23,33,43,53,63,73 : Set Time Divisions for Channels Values : 1 = 1/16, 2 = 1/12 (dotted eight),3 = 1/8 , 4 = 1/4, 5=1/2, 6 = 1 (whole).

    4. CC: 4,14,24,34,44,54,69,74 : Set Sequence Length in steps Values (4-32)

    5. CC: 5,15,25,35,45,55,65,75 : Set Pulse (2-32) Pulse are The Number of Filled Steps that will be spread over the Sequence. if Pulse > Steps , All steps will be enabled.

    6. CC: 6,16,26,36,46,56,66,76 : Set Sequence Shift Values 0-32 . Sets the Sequence Offset, values greater and sequence length Keep Rotating. for example Shift of 3 will move all steps 3 steps towards right and out of bounds will wrap around. 

    7. CC: 9,17,27,37,47,57,67,77: Note Gate Length (10-100), Sets the Duration of the Note based on Time Division in the Sequence Default is 65%.

    8. CC:8,18,28,38,48,58,68,78 : Sequence Output Midi Channel (1-16)

    9. CC: 81,82,83,84,85,86,87,88 : Set Base Velocity of the Channel, default 96

    10. CC: 91,92,93,94,95,96,97,98 : Set Velocity Humanization value 0-36 a Random Amount up to the value gets added or subtracted to Base Velocity. Default:0
    10. CC: 101,102,103,104,105,106,107,108 : Set LOOP Steps (Restart Point) 0=off, the Sequence Restarts at its Number of steps. 1-64 : The sequnce will play cyclickly until Loop Point number of steps have been played and then will restart. Values < steps , will shorten the playing sequence pattern.

## Building from Source
** To use on MPC/Force you must have SSH Access (Firmware Mod)

1. To Build from Source you can use use g++ (with alsa support) on Raspberry pi for Akai Force/MPC
copy the generated bin/euclidier file to your Force/MPC and run from ssh 

2. Compile Scripts are Provided for Compilation on a Raspberry Pi system and Mac OSX.

3. This Can also be run on a Raspberry Pi box or a Mac. It will create two midi Ports that you can use for communication. (you wll need to create own Midi control surface or use Provided Force Track Template to control the parameters.
   










