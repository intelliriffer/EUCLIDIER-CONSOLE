# Euclidier - 8 Channel Midi Euclidean Note Sequencer for Akai Force/ Akai MPC / Raspberry pi / Mac OSX (intel) Beta 0.1.8

## Download Latest Binaries 0.1.8
1. [Akai Force, Akai MPC & Raspberry Pi](https://mega.nz/file/kkhTUILQ#6C5HmxVo5gBDOwf4-Ut8xqvhVdaRlgo0hiePp4hF1es)

2. [Max OSX (intel only)](https://mega.nz/file/ZoBCESBL#gpWRZelZHE7NXsq_5xJLHHsOyjKaipFtfltOx3XJIG4)


## Features:
1. 8 Independent Channels/Tracks
2. Custom Midi Channel Per Track
3. Upto 4 - 32 Steps / Channel
4. Custom Note Pitch / Channel
5. Velocity + Velocity Humanization / Channel  (Adds/Subtracts values up defined (max 32) from Base Velocity).
6. Sequence Shifting (Offset)
7. Different Note/Time Divisions per channel.
8. CC Sequencing : *See Below
9. *** New >>> Internal + External Clock Support.
10. Realtime Note Based Transpostion
11. Will run on Force and Raspberry Pi
12. ** v 0.1.4 Added Loop Parameter, Loop Parameter Sets the Sequence Restart Point. Values > Steps Creates Polythythms 
13. ** 0.1.5
    1. Added Realtime Note Triggered Transpostion - See below for details.
    2. Extended Steps to 2-64
    
14. New Midi Mapping Layout with Loop Controls added
15. *** 0.1.6 New Killer Features
    1.  Velocity Sense (Default On) : Note Octave will be Shifted based on Incoming Note Velocities
        1.  Velocity < 22 : Octave -1
        2.  Velocity 23 - 126 : octave 0 (As the Orginal Note)
        3.  Velocity 127 : octave +1, Notes will wrap if beyong range;
    2.  CC Sequencing : Any Track can Now be configured as Note Track or a CC.
        1.  Mode Value :1 => Note Track
        2.  Mode Value: 2 => CC Track For Each Trigger CC same as Note Parameter Will be Triggered with Value of "VALUE ALT" and after Gate Duration Default "BASE VALUE" will be trigged. in a Nutshell It will strigger two different CC values.
        3.  Mode Value: 3 A Random Value Between "BASE VALUE" and "VALUE ALT" will be Triggered, after Gate Duration Default "BASE VALUE" will be trigged.
        4.  By Enabling Control on Euclidier Midi Port and only Soloing the CC Track, you can midi learn the cc and then apply to any automatable parameter on Force tracks/fx/ or send to External MIDI/CV.
    3.  Custom Progresisons (Chord Pads) Support : I have hand Created Few Progression files that you can load on your Akai Force/MPC and can use to send some triad chords to Euclidier to create chord stabs or quick custom apreggios (Majors,Minors and Sus4 Chords.) Please Refer to included Readme.txt with the Chords.
    4.  Parameter Names have been changed on Midi Track Template to Make it Easier.
    5.  Some Memory Optimizations (more will come later).
    6.  Added Midi CC/ Params for :
        1.  Reset Octave and any Transpose Applied (All Tracks).
        2.  Vel Sense:
            1.  0: OFF, Will not change octaves based on incoming note velocity.
            2.  1: ON, Will Change octaves based on incoming note velocity.
        3.  RECEIVE NOTES : (0-1) : Switches Realtime Note Input Respose Off/On.

16.  **** **v 0.1.7 Features****
     1.   128 Presets Slots to Save / Load  ( Supports program change loading)
     2.   Master Sync (CC 50, 0-8): Parameter Update Quantization (default 1/4) : Changes to Steps, Fill, Shift etc are quantized to  master clock from none to up to 8 bars.
     3.   When using console, loaded preset values display.
     4.   Preset Feedback Update, If your midi control is set to receive from euclidier on chn16, Euclidier will send the respective cc back to your control surface (mpc/ akai force) to Update the UI parameters.
     5.   Assignable Channels Reduced to 1-15 (Ch:16 is now reserved for feedback msssages.)
     6.   Many Stability Optimizations.
     7.   Added Drum to Track Type. When Trac is set to Drum (2) , it will not transpose or shift octaves on incoming notes. Might be usefor for other purposes in future.
  
        



## Installation: 
1. If Using Mockba Mod and my NodeJs App Server, Copy All the Files into your 662522/AddOns/nodeServer/modules Folder and use web to  start/stop  it.

2. If not using my NodeApp server, you can copy the euclidier file anywhere on your device and run using ssh shell (perhaps someone can better explain the process).
or if using Mockba/Kick Gen Mod, you can put in a launch script in their respective autolaunch directors to launch this in background on Startup.

## Notes:
1. If you change Steps, Fills, Shift parameters, time division , you need to start and stop Force playback to sync the sequences. (I may update this later on to auto sync once I figure out the best way)

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

    1. CC: 1,11,21,31,41,51,61,71 : Enable Channels (1-8 Respectively) value > 63 = on

    2. CC: 2,12,22,32,42,52,62,72 :  Set Pitch / Note for Channels (1-8) Values 0-127

    3. CC: 3,13,23,33,43,53,63,73 : Set Time Divisions (Step Duration) for Channels Values
       1.  1/16
       2.  1/8
       3.  1/4
       4.  1/2
       5.  1
       6.  1/32
       7.  1/64
       8.  .1/16
       9.  .1/8
       10. .1/4 

    4. CC: 4,14,24,34,44,54,69,74 : Set Sequence Length in steps Values (2-64).

    5. CC: 5,15,25,35,45,55,65,75 : Set Fill (1-64) Fill are The Number of Filled Steps that will be spread over the Sequence. if Fill > Steps , All steps will be enabled.

    6. CC: 6,16,26,36,46,56,66,76 : Set Sequence Shift Values 0-64 . Sets the Sequence Offset, values greater and sequence length Keep Rotating. for example Shift of 3 will move all steps 3 steps towards right and out of bounds will wrap around. 

    7. CC: 9,17,27,37,47,57,67,77: Note Gate Length (10-95), Sets the Duration of the Note based on Time Division in the Sequence Default is 65%.

    8. CC:8,18,28,38,48,58,68,78 : Sequence Output Midi Channel (1-15)

    9.  CC: 81,82,83,84,85,86,87,88 : Set Base Value for (Velocity or CC) For the Channel, default 96

    10. CC: 91,92,93,94,95,96,97,98 : Set "VALUE ALT"  (For notes 0-50) (for CC : 0-100 0 is disabled)
        1.  In Note Mode (0-127) a Random Amount between (0-VALUE ALT) is Added to BASE VALUE.
        2.  In CC Mode 1: Each Active Step Sends CC with VALUE ALT and Reverts back to BASE VALUE after Step/Gate Duration.
        3.  in CC Mode 2: Each Active Step Sends the CC with a "Random Value" between BASE VALUE and BALUE ALT. It Reverts back to BASE VALUE after Step/Gate Duration.

    11. CC: 101,102,103,104,105,106,107,108 : Set LOOP Steps (Restart Point) 0=off, the Sequence Restarts at its Number of steps. 1-64 : The sequnce will play cyclickly until Loop Point number of steps have been played and then will restart. Values < steps , will shorten the playing sequence pattern.

    12. CC: 111,112,113,114,115,116,117,118 : Set Track Modes Between
        1.   1: Note
        2.   2: Drum
        3.   2: CC1
        4.   3: CC2
    
    13. Global Modifiers
        1.  CC 99:  (0-1): Receive Notes
        2.  CC 100: (any positive value) : Reset  Any Note transposition and Octave offsets from all Tracks.
        3.  CC 119: Disable Octave Switching on incoming Note Velocity (you dont want it with drums etc) 
        4.  CC 70: Sync All Tracks (sets the internal step position to 0)
        5.  CC 20: (0-127), Select a Preset Slot to Load from or to Save to.
        6.  CC 29: Load From Selected Preset Slot.
        7.  CC 30: Save Current to Selected Preset Slot.
    14. **Clock Control**
        1.  Use External Clock: CC: 59 (0-1) 
        2.  When 0 Internal Clock will be used, and also sent out. Clock Must be started using Start / Stop CC: 60 (0-1)
        3.  Stop / Start Internal Clock : CC:80 Values (0-1)
        4.  BPM1: CC:39 Values (30-127)
        5.  BPM2: CC:40 Values (0-127)
        6.  Internal Clock BPM is Set to Values of BPM1 + BPM2 
           1. So if BPM1 = 100 and BPM 2 = 40, The Actual BPM becomes: 140

    15. Program Change (0-127) : Load from Preset Slots 0-127.

    16. Track  Note Transposition (Transposition if ) (Adds to Track Note Value)
        1.  Notes 0-11 (c-2) : Transpose Track 1 by +(0-11)
        2.  Notes 12-23 (c-1): Transpose Track 2 by +(0-11)
        3.  Notes 24-35 (c0) : Transpose Track 3 by +(0-11)
        4.  Notes 36-47 (c1) : Transpose Track 4 by +(0-11)
        5.  Notes 48-59 (c2) : Transpose Track 5 by +(0-11)
        6.  Notes 60-71 (c3) : Transpose Track 6 by +(0-11)
        7.  Notes 72-83 (c4) : Transpose Track 7 by +(0-11)
        8.  Notes 84-95 (c5) : Transpose Track 8 by +(0-11)

    17.  Notes: 96-103 : Reset Octave shifts on Tracks 1-8 respectively
    18.  Notes: 108-115 : Apply +1 Octave Shift to tracks 1-8 respectively.
    19.  Notes: 120-127 : Apply -1 Octave Shift to tracks 1-8 respectively.



## Building from Source
** To use on MPC/Force you must have SSH Access (Firmware Mod)

1. To Build from Source you can use use g++ (with alsa support) on Raspberry pi for Akai Force/MPC
copy the generated bin/euclidier file to your Force/MPC and run from ssh 

2. Compile Scripts are Provided for Compilation on a Raspberry Pi system and Mac OSX.

3. This Can also be run on a Raspberry Pi box or a Mac. It will create two midi Ports that you can use for communication. (you wll need to create own Midi control surface or use Provided Force Track Template to control the parameters.
   

**V.0.1.5 Release Notes**
1. Added New time divisions
2. Number of Steps changes from 4-32 to 2-64
3. Number of Fills changed from 1-32 to 1-64
4. Added Realtime Note Tansposition (Midi Clips can be used to automate).
   1. You can use Realtime Transpostion to build Custom Arpeggios/Sequences.
   2. By setting each channel to same note (octaves can be different) You can send In scale notes (as transpose parameter) to each channel to  create pattern variations. 
   3. Setting notes and pattern to same but sending different transpose values, you can create custom chord stab patterns.

     

 


 **0.1.6 Release Notes**

     1.  Velocity Sense (Default On) : Note Octave will be Shifted based on Incoming Note Velocities
        1.  Velocity < 22 : Octave -1
        2.  Velocity 23 - 126 : octave 0 (As the Orginal Note)
        3.  Velocity 127 : octave +1, Notes will wrap if beyong range;
    2.  CC Sequencing : Any Track can Now be configured as Note Track or a CC.
        1.  Mode Value :1 => Note Track
        2.  Mode Value: 2 => CC Track For Each Trigger CC same as Note Parameter Will be Triggered with Value of "VALUE ALT" and after Gate Duration Default "BASE VALUE" will be trigged. in a Nutshell It will strigger two different CC values.
        3.  Mode Value: 3 A Random Value Between "BASE VALUE" and "VALUE ALT" will be Triggered, after Gate Duration Default "BASE VALUE" will be trigged.
        4.  By Enabling Control on Euclidier Midi Port and only Soloing the CC Track, you can midi learn the cc and then apply to any automatable parameter on Force tracks/fx/ or send to External MIDI/CV.
    3.  Custom Progresisons (Chord Pads) Support : I have hand Created Few Progression files that you can load on your Akai Force/MPC and can use to send some triad chords to Euclidier to create chord stabs or quick custom apreggios (Majors,Minors and Sus4 Chords.) Please Refer to included Readme.txt with the Chords.
    4.  Parameter Names have been changed on Midi Track Template to Make it Easier.
    5.  Some Memory Optimizations (more will come later).
    6.  Added Midi CC/ Params for :
        1.  Reset Octave and any Transpose Applied (All Tracks).
        2.  Vel Sense:
            1.  0: OFF, Will not change octaves based on incoming note velocity.
            2.  1: ON, Will Change octaves based on incoming note velocity.
        3.  RECEIVE NOTES : (0-1) : Switches Realtime Note Input Respose Off/On.
        


# Tips/How To's:
### Simple 3/4 against 4/4
1. Track 1
   1. Steps : 4
   2. Fill  : 1
2. Track 2
   1. Steps : 3
   2. Fill  : 1

3. 3.Things to Try,  For Longer, Just multiply the value the values. and try setiing up loop few steps more than total steps.

### Creating Accents:
1. Create CC1 Track : Set Type to : 3
2. Set Note to : 11 (expression) . (Expression increases or decreses channel volume between 0-100% of its volume. )
3. Set Base Value  to say 90 (Non accent value)
4. Set Value Alt to : 127
5. Assign Ch to same as the for the instrument you want to control.



### [<<< Reference & World Rhythms >>](http://cgm.cs.mcgill.ca/~godfried/publications/banff.pdf)








