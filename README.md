# BSAndReduce

#This program is used to substract Backgroung of the input and quantize the image to 16 colors. 

The program features 2 important concepts in OpenCV. Background Substration can be used in detecting objects accurately and Image quantization can solve the issue of space and can also be used in Context Based Image Retrival Systems as the histogram of reduced images are more robust than normal image.

The program uses BackGroundSubstrator and Kmeans classes to estimate backgroung and generate clusters.

Pseudo Code:
<--Begin
	Load Video
	Process each frame of video
		Substract Background of each frame
		Generate K clusters using Kmeans OpenCV class
	Display each frame
End-->
