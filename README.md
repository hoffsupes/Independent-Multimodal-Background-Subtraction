# Independent-Multimodal-Background-Subtraction
Implementation for the Independent Multimodal Background Subtraction based on a paper written by Bloisi and Iocchi

The basic infrastructure has only been created. Shadow detection and removal has been implemented but not used in the final code. If you do wish to use the code with it, then the function and it's usage (commented out) is present and you are free to do so. 

This is a skeletal implementation, for those who want to gain a basic understanding of the concept and feel that the original author's implementation is too complicated. This has been implemented according to the pseudocode presented in the paper, so it directly correlates to that. That being said, *all portions needed for generating a sufficient background model are present* so if you only need something till that stage, go ahead, do give credit to the people who wrote the paper and also link to here if you do end up using the code somewhere.

At it's current stage, it forces videos into 360 * 640 for ease of processing. You can change this value at the top of the code, although I would ask you not to as it can be intensive on slow systems. Also, any debugging that arises out of the changes you make to the code is your responsibility.

USAGE:

1.) Take the path of the video for which you want to learn the foreground for and put it in the cap.open("Video Path in here") in imbs()

2.) Run the code via the command line / however you are running it. If you are on windows, feel free to read up on a few guides on how to set OpenCV up with your local IDE. If you're on linux however and have OpenCV already installed, then:

g++ -g IMBS.cpp -o op1 `pkg-config --cflags --libs opencv`; ./op1

Will help.

PS: the '-g' if for debugging purposes, leave it out if you dont plan on using gdb

3.) The original video frame and foreground mask will be displayed live. If you want to see the shadow frame too, you can uncomment it out. 

4.) To understand the meaning of the parameters, read the paper.

All credits for the design of the algorithm and the idea as a whole go to Bloisi and Iocchi.
https://www.dis.uniroma1.it/~bloisi/papers/independent-multimodal-background-subtraction.pdf
