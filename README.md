#Sketchy

Sketchy is an audio-visual sketch pad for users of all ages. Through the use of graphically mapped granular synthesis, 
Sketchy expands the creative potential of drawing and sketching into the auditory and compositional realms. 

![Demo Screenshot](https://github.com/graham-davis/sketchy/blob/master/bin/screenshots/demo.png "Sketchy")

##Visual Features
The settings menu (which can be toggled with the settings icon) allows users to customize their drawing brush and
add other items to the canvas.
<ul>
<li>5 brush colors</li>
<li>Variable brush radius and opacity</li>
<li>Stickers</li>
<li>Grid mode</li>
</ul>

##Granular Synthesis Mappings
As the user draws, characteristics of their brush stroke determine the auditory feedback produced by the STK 
granular synthesis algorithm. All stroke characteristics are maintained and recreated during playback.
<ul>
<li>Granulated audio file : Brush color</li>
<li>Grain size : Brush radius</li>
<li>Gain : Brush opacity</li>
<li>Grain position : X coordinate of stroke</li>
<li>Pitch shift : Y coordinate (higher on canvas = higher pitch)</li>
</ul>

##Stickers
A number of stickers can be added to the canvas, each influencing stroke playback parameters.
<ul>
<li>Sun : Repeats stroke</li>
<li>Lightning : Adds noise to the stroke</li>
<li>Rain cloud : Slows stroke playback</li>
<li>Wind : Delays stroke start</li>
<li>Spiral : Plays stroke backwards</li>
</ul>

##Interaction and Playback
Playback of the current canvas, deletion of strokes and saving functionality are controlled through the keyboard. 
<ul>
<li>Spacebar : Playback of current drawing</li>
<li>Delete (back-arrow icon) : Removes most recent stroke from the canvas</li>
<li>Shift+Delete (trash icon) : Remove all strokes from the canvas</li>
<li>Command+[1-9] : Save current canvas</li>
<li>[1-9] : Load saved canvas
</ul>
