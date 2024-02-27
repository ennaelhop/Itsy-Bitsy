# Itsy Bitsy

## Background ##
This was a course project made (over the course of 13 weeks) in a team of 6 for CPSC 427: Video Game Programming at UBC. It is written in C++ and OpenGL. The OpenGL libraries used are glfw, gl3w, glm. 
The project was written with the ECS (Entity-Component-System) design pattern. 

---
## Inspiration ##
The game idea was inspired by the arcade game - Rampage that I played when I was young. 

---

## Story ##
Gerald is a mutated spider and his webs are poisonous enough to destroy ecosystems. 
The goal of the game is to destroy everything that crosses his path, and along the way, he will meet formidable enemies like the bumblebees and the butterflies. 
He will also meet powerless creatures like ants that he can eat for bonus points. 
Gerald can make webs to navigate across obstacles. 
Once he successfully destroys part of the environment, he will move on to the next and stop at nothing. 

---
## How To Run ##
- Use CMake to build project

--- 
 
## How To Play ##
- use WASD keyboard control to move spider
- avoid getting hit by bees
- eat ants to increase health
- use cursor to shoot webs at bees and trees

---
## Features ##
- Default is regular gameplay, to enter zoom mode press Z and to return to regular gameplay press X
- Delay agnostic: press Z to enter zoom mode where only when the spider (player) moves, so do the ants/bees. To increase gameplay, when spider throws acid or swings from web, the duration of time acid is present, the movement for the ants/bees will resume.
- Mouse gestures (right-click and drag to enable web swing)
- To skip levels (for evaluation purposes): ',' [comma] will go back to previous level whereas '.' [period] will go forward to next level
- Use "u" to save and "i" to reload

Note:
- Please see "MilestoneSubmissionForm" pdf for full features description.
- Please see 'Balance Pass' pdf for more details on the game balance feature.

---

# Credits #
## Citations ##
- Background for level 2: https://edermunizz.itch.io/free-pixel-art-hill
- Glow effect for spider: https://www.shadertoy.com/view/lsXGWn
- Precise collision (determining if a point is in a polygon): https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
- Collision detection (AABB): https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection

## Audio credits ##
- Background music: https://www.chosic.com/free-music/all/ 
- Spider falling off tree, bee stinger impact, web swoosh, bee hit impact, spider dead: https://mixkit.co/license/#sfxFree
- Spider eating: https://www.fiftysounds.com/royalty-free-music/eating-sounds.html

## Background credits ##
- Artwork created by Eder Muniz: https://edermunizz.itch.io/free-pixel-art-forest?download
 








