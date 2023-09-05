# StudyingVulkan - compute__BucketSortVisualization

   This branch implements a compute shader that calculates the frequency distribution of values sorted into buckets representing various contiguous ranges of integers, and than a standard graphics pipeline renders it.
   
Note:

      - The distribution rendered as a bar graph. 
      
      - The specific quantity and height of the bars in the graph will differ from run to run.
      
      - The quantity of numbers used in the input list will be randomly chosen within the range 64-128.

      - The bucket size is 4 by default, but can a custom bucket size may be specified from the command line by passing an integer argument.


 Issues:
 
        - Didnt feel like porting the keyboard code from another branch, so instead of running till a key is pressed,  the application renderes 5 frames and than exits (although the compute shader only runs once, prior to any frames being rendered)


Example Frame: 
![bucketSortVisualization](https://raw.githubusercontent.com/Shalnon/StudyingVulkan/compute__BucketSortVisualization/Assets/Screenshots/compute__BucketSortVisualization.bmp)
