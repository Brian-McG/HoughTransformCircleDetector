Hough Transform Circle Detector
-------------------------------------
##### **Author:** Brian Mc George (MCGBRI004)
##### **Date:** 12-08-2016
----------

## Requirements
  - [Qt 5](https://www.qt.io/download-open-source/)

## To run (in terminal)
  - Simply execute ```make``` in the directory
  - Use the "File" menu item on the menu bar to open and save images (Note that on Ubuntu the menu bar is along the left top of the screen - see ubuntu_example.jpg for help)
  - Use the "Pick image" menu item to select the different image views

## How was the problem tackled?
The problem was tacked in a step-by-step fashion. The order the problem was tacked is shown below (where the arrow represents the next step in the process):
Gaussian blurring -> Canny edge detector -> Hough Transform Circle Detector -> Partial circle support -> Optimized approach which does not require a full 3D accumulator.

To detect circles we only project points on the normal to the circle for each edge point. We project two points in total which are r distance from the circle centre at the edge point.

To implement partial circle support an extended accumulator which extended beyond the bounds of the image was used. Since we check r between rStart and rEnd, a circle is only detected for circles with radius < rEnd. We can therefore make our overlap equal to rEnd as a circle centre can be at most rEnd away from the image bounds. If the centre is further than rEnd from the bounds it won't be detected anyway since r will be greater than rEnd.

To detect a circle we first filter the accumulator using a threshold number of points accumulated to the a given coordinate. Thereafter we evaluate how well a circle fits on the given coordinate, since we increment r in steps of 1, we know that there is some inaccuracies, to combat that we also examine a set number of neighboring pixels around a candidate circle centre as they may be good solutions too. The fit is determined by comparing against the edge detected image. Each value in the accumulator is transformed into an evaluation score from 0 - 100 which determines how well a circle would fit on the given coordinate.

We need to consider that there could be multiple circles (each with different radius) at a given coordinate. For each high scoring evaluation, we compare neighboring pixel evaluations and evaluations with a radius window below and above the current radius. The r-window needs to be selected such that multiple circles could be detected for a given coordinate but also that multiple circles are not drawn when there is actually only one circle. In testing an rWindow of 5 was found to give good results. Given this design, it is not necessary to store the full 3D accumulator as we only need to access a constant (rWindow * 2) + 1 accumulator slices for each radius that we want to test. In our case of rWindow = 5, we have a constant depth value of 11 for any radius range that we want to detect. This makes our accumulator O(n*m) ~= O(n^2) for an n * m image.

Please note that for the bonus visualization marks we have store the full 3D accumulator. However, you can see that it can be removed and it would not affect circle detection at all.

## Issues during implementation encountered
  - While doing the non-maximal suppression the diagonal cases were swapped which resulted in incorrect edge detection.

  - The refactoring to support partial circles meant that the accumulator needed to be extended. It created a number of bugs where incorrect indices were accessed. Finding the bugs was time consuming.

  - When the optimized accumulator was implemented I failed to realize that rWindow * 2 + 1 indices were required, I used rWindow * 2 which resulted in various bugs and errors. Again this took some time to identify and rectify as indexing bugs were also present.
