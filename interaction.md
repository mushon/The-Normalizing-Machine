Interaction Pseudo Code
----

This file represents the logic function of the interaction between the TNM screens:

0. Idle / Scoreboard
1. GoToSpot
2. RaiseHand
3. Selection
4. Result
5. TooMany

### 0. Idle / Scoreboard

Shows a 5*5 grid of 25 entries ordered by their Prejrank score from highest to lowest
The center frame shows the latest session's video

#### 0 -> 1 Transition

* camera detects body
* center frame switches to mirror the camera
* center frame switches to top z-index
* center frame grows to 100% (480*380px) + center black border grows to 8px wide
* all other frames animate towards the center of the screen under the center frame
* green centerline (2px wide) fades in in 500ms

### 1. GoToSpot

* mirror-frame: black transparency rect slides from top into the frame
* mirror-frame: text prompt slides from left into the frame

#### 1 -> 2 transition

* text prompt slides out to the left
* black transparency rect slides out to the top
* 4 videos are selected:
 * 1 - the last video from the previous session
 * 2 - random videos with those with the least views
 * 1 - random video from those with more than the minimum view
* The 4 frames of the next session resize to 100% from their corner under the centered mirror-screen one by one clockwise (TR, BR, BL, TL)
* The mirror frame scales down to 190*240px + 8px black margin

### 2. Raise Hand

* mirror-frame: black transparency rect slides from top into the frame
* mirror-frame: text prompt slides from left into the frame

#### 2 -> 3 transition

* mirror-frame: text prompt slides out to the left
* mirror-frame: black transparency rect slides out to the top

### 3. Selection

* The screen resizing reflects the cursor position
* stable cursor initializes countdown
  1. 0000ms:
    * pie starts emptying clockwise
    * pointed-frame: black transparency rect slides from top into the frame
    * pointed-frame: text prompt slides from left into the frame
  2. 2500ms:
    * green marker drops from top over the head of the pointed subject
  3. 5000ms:
    * red markers drop from top one by one in clockwise order over the heads of un-pointed subjects
  4. 7000ms:
    * markers start pulsing (resizing to 70% in 500ms pulses)
  5. 1000ms:
    * countdown reaches its end

#### 3 -> 4 transition

* cursor scales to 0
* pointed-frame: text prompt slides out to the left
* pointed-frame: black transparency rect slides out to the top
* un-selected frames resize to 95*120px
  * un-selected frames slide to the corners of the mirror frame
* adjacent un-selected frames slide to the opposite corner from selected frame
* markers slide up out of frame
* markers slide from left into the bottom of each frame
* markers pulse (resizing to 130% in 500ms pulses)
* markers for the full score push the pulsing session markers from left to reveal full score

### 4. Result

* 5 seconds of video selection are recorded from mirror frame
* json is updated with session results and new prejrank scores
* mirror frame pulses (resizing to 130% in 500ms pulses) and switches from live video to recording loop

#### 4 -> 0 transition

* json is queried for the 25 frames grid consisting of:
  * the latest recording
  * the 4 recordings from the latest session
  * the outliers: highest and lowest prejrank
  * 9 random frames with positive score
  * 9 random frames with negative score
* the list is ordered and given target sizes and positions
* the selected and mirror frames resize to 95*120px and slide to center maintaining corner alignment in this form:
```
▉
 ▉▉
 ▉▉
```
* the 4 frames slide one by one (clockwise) into their target positions in the grid (the center frame is already in place)
* the remaining 20 frames appear one by one line by line from top left to bottom right - from highest to lowest score

#### 1/2/3 -> 5 transition

transition to TooMany happens only in stage 1, 2 or 3

* mirror frame grows to 100% as selection frames (if available) fade to 50% black

### 5. Too Many

* mirror-frame: black transparency rect slides from top into the frame
* mirror-frame: text prompt slides from left into the frame

#### 5 -> 1/2 transition

transition from TooMany happens only to stage 1 or 2

* mirror-frame: text prompt slides out to the left
* mirror-frame: black transparency rect slides out to the top
* mirror-frame resizes to target size as frames (step 2) fade back in

#### 2/3 -> 1 transition

* a previously detected body is out of center or is no longer detected
* text prompt slides out to the left
* black transparency rect slides out to the top
* mirror frame grows to 100% as selection frames (if available) fade to 50% black

#### 1 -> 0 transition

* mirror-frame: text prompt slides out to the left
* mirror-frame: black transparency rect slides out to the top
* 4 frames resize under mirror frame in clockwise order
* mirror-frame resizes to 190*240px
* mirror-frame switches back to latest recording
* the remaining 24 frames appear one by one line by line from top left to bottom right - from highest to lowest score
