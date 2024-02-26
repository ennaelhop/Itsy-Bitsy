Game: Itsy Bitsy

Story:
Gerald is a mutated spider and his webs are poisonous enough to destroy ecosystems. 
The goal of the game is to destroy everything that crosses his path, and along the way, he will meet formidable enemies like the bumblebees and the butterflies. 
He will also meet powerless creatures like ants that he can eat for bonus points. 
Gerald can make webs to navigate across obstacles. 
Once he successfully destroys part of the environment, he will move on to the next and stop at nothing. 


******************************* Milestone 4 *******************************

key gameplay additions (for full features implemented please see MilestoneSubmissionForm):
- tutorial
- default is regular gameplay, to enter zoom mode press Z and to return to regular gameplay press X
- delay agnostic: press Z to enter zoom mode where only when the spider (player) moves, so do the ants/bees. To increase gameplay, when spider throws acid or swings from web, the duration of time acid is present, the movement for the ants/bees will resume. 
- timer and score (with best time + score saved and displayed per level)
- particle system when trees/bees get destroyed
- light
- added level 2:
    -  swarm behavior
    -  additional acid power: by changing acid modes using TAB and holding LEFT MOUSE BUTTON, player can build up a bigger ball of acid with higher damage
    -  new type of enemy (snail)
- mouse gestures (right-click and drag to enable web swing)
- to skip levels (for evaluation purposes): ',' [comma] will go back to previous level whereas '.' [period] will go forward to next level


Please see 'Balance Pass' pdf for more details on the game balance feature.


citations:
- Background for level 2: https://edermunizz.itch.io/free-pixel-art-hill

******************************* Milestone 3 *******************************

key gameplay additions (for full features implemented please see MilestoneSubmissionForm):
- introduced 5 new ants that give spider special power-ups when eaten
- spider glow effect when eats Immune Ant to indicate to player the power-up is enabled
- added story to motivate player (intro animation with story description + end of game battle with Boss Bee text) 
- spider can swing from web (right-click and drag mouse to where you want spider to swing)
- trees reacting to the poisonous web 
- once all trees have been destroyed, Boss Bee battles with spider
- use "u" to save and "i" to reload
- precise collision between spider and bee 

citations:
- glow effect for spider: https://www.shadertoy.com/view/lsXGWn
- precise collision (determining if a point is in a polygon): https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon

******************************* Milestone 2 *******************************

Goal:
- destroy all the trees with poisonous webs while avoiding getting attacked and killed by the bees

To play: 
- use WASD keyboard control to move spider
- avoid getting hit by bees
- eat ants to increase health
- use cursor to shoot webs at bees and trees 


key gameplay additions (for full features please check the MilestoneSubmissionForm):
- added ants that spider can eat for points 
- spider can climb on/fall from the trees
- health bar is introduced: 
    - ants will give points
    - bees will remove points
    - once health is empty, spider dies
- bees will drop stingers which will have a pathfinding implementation to find spiders
- spider can shoot webs to destroy trees and to kill bees

notes:
- when first run, level 0 is displayed. To work level loader, select SHIFT and PERIOD to get to level 1.
- to enter debug mode press the O key.
    - it is enabled when the stinger is approaching the spider to visualize the direction the stinger will take.
- to access 'help' feature: hover the cursor over the '?' asset at the bottom right corner of game.

audio credits: 
- background music: https://www.chosic.com/free-music/all/ 
- spider falling off tree, bee stinger impact, web swoosh, bee hit impact, spider dead: https://mixkit.co/license/#sfxFree
- spider eating: https://www.fiftysounds.com/royalty-free-music/eating-sounds.html



******************************* Milestone 1 *******************************

To play: use WASD keybord control to move spider and try to avoid getting killed by the bees. 
     
REFERENCES:
- collision detection (AABB): https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection

Background credits: 
 Artwork created by Eder Muniz
 Downloaded from https://edermunizz.itch.io/free-pixel-art-forest?download


