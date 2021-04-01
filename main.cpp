//============================================================================
// Name        : main.cpp
// Author      : Florian Neugebauer
// Version     : 1.0
// Copyright   : -
// Description : Opens 4 Windows. 1. RGB 2. Canny Edge detection on RGB 3. Depth 4. Canny Edge detection on Depth
//				 With ESC you can stop filming. Also three files are generatad:
//				 1. final_depth_img.txt 
//				 2. final_depth_img.png
//				 3. final_rgb_img.png
//               Parts of the code are from  jayrambhia from github.
//               link: https://gist.github.com/jayrambhia/5677608
//============================================================================

#include <iostream>
#include <fstream>
#include <math.h>
#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <libfreenect.h>

#define FREENECTOPENCV_WINDOW_D "Depthimage"
#define FREENECTOPENCV_WINDOW_N "Normalimage"
#define FREENECTOPENCV_RGB_DEPTH 3
#define FREENECTOPENCV_DEPTH_DEPTH 1
#define FREENECTOPENCV_RGB_WIDTH 640
#define FREENECTOPENCV_RGB_HEIGHT 480
#define FREENECTOPENCV_DEPTH_WIDTH 640
#define FREENECTOPENCV_DEPTH_HEIGHT 480


using namespace std;
using namespace cv;

Mat depthimg, rgbimg, tempimg_depth, tempimg_rgb, canny_temp, canny_img;

Mat final_depth_img, final_rgb_img;

pthread_mutex_t mutex_depth = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_rgb = PTHREAD_MUTEX_INITIALIZER;
pthread_t cv_thread;

// callback for depthimage, called by libfreenect
void depth_cb(freenect_device *dev, void *depth, uint32_t timestamp)
 
{
    Mat depth8;
    Mat mydepth = Mat( FREENECTOPENCV_DEPTH_WIDTH,FREENECTOPENCV_DEPTH_HEIGHT, CV_16UC1, depth);
    // lock mutex for opencv depth image
    mydepth.convertTo(depth8, CV_8UC1, 1.0/4.0);
    pthread_mutex_lock( &mutex_depth );
    memcpy(depthimg.data, depth8.data, 640*480);
    // unlock mutex
    pthread_mutex_unlock( &mutex_depth );
 
}

// callback for rgbimage, called by libfreenect
 
void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
 
    // lock mutex for opencv rgb image
    pthread_mutex_lock( &mutex_rgb );
    memcpy(rgbimg.data, rgb, (FREENECTOPENCV_RGB_WIDTH+0)*(FREENECTOPENCV_RGB_HEIGHT+950));
    // unlock mutex
    pthread_mutex_unlock( &mutex_rgb );
}
/*
 * thread for displaying the opencv content
 */
void *cv_threadfunc (void *ptr) {
    depthimg = Mat(FREENECTOPENCV_DEPTH_HEIGHT, FREENECTOPENCV_DEPTH_WIDTH, CV_8UC1);
    rgbimg = Mat(FREENECTOPENCV_RGB_HEIGHT, FREENECTOPENCV_RGB_WIDTH, CV_8UC3);
    tempimg_rgb = Mat(FREENECTOPENCV_RGB_HEIGHT, FREENECTOPENCV_RGB_WIDTH, CV_8UC3);
    tempimg_depth = Mat(FREENECTOPENCV_RGB_HEIGHT, FREENECTOPENCV_RGB_WIDTH, CV_8UC3);
    canny_img = Mat(FREENECTOPENCV_RGB_HEIGHT, FREENECTOPENCV_RGB_WIDTH, CV_8UC1);
    canny_temp = Mat(FREENECTOPENCV_DEPTH_HEIGHT, FREENECTOPENCV_DEPTH_WIDTH, CV_8UC3);

    final_rgb_img = Mat(FREENECTOPENCV_RGB_HEIGHT, FREENECTOPENCV_RGB_WIDTH, CV_8UC3);
    final_depth_img = Mat(FREENECTOPENCV_RGB_HEIGHT, FREENECTOPENCV_RGB_WIDTH, CV_8UC3);

    ofstream myfile;
  	myfile.open ("final_depth_img.txt");
    // use image polling
    while (1)
    {
        //lock mutex for depth image
        pthread_mutex_lock( &mutex_depth );
        Canny(depthimg, canny_temp, 50.0, 200.0, 3);
        cvtColor(depthimg,tempimg_depth,COLOR_GRAY2BGR);
        cvtColor(tempimg_depth,tempimg_depth,COLOR_HSV2BGR);

        imshow(FREENECTOPENCV_WINDOW_D,tempimg_depth);
        imshow("Depth Canny", canny_temp);
        //unlock mutex for depth image
        pthread_mutex_unlock( &mutex_depth );

        //lock mutex for rgb image
        pthread_mutex_lock( &mutex_rgb );

        cvtColor(rgbimg,tempimg_rgb,COLOR_BGR2RGB);
        cvtColor(tempimg_rgb, canny_img, COLOR_BGR2GRAY);

        imshow(FREENECTOPENCV_WINDOW_N, tempimg_rgb);

        Canny(canny_img, canny_img, 50.0, 200.0, 3);

        imshow("Canny Image", canny_img);
        //unlock mutex
        pthread_mutex_unlock( &mutex_rgb );

        // wait for quit key
        if(waitKey(15) == 27){
            
        	final_depth_img = tempimg_depth;
        	final_rgb_img = tempimg_rgb;

        	myfile << depthimg;
 	 		myfile.close();
        	imwrite("final_depth_img.png",final_depth_img );
        	imwrite("final_rgb_img.png", final_rgb_img);

            break;
        }
    }
    pthread_exit(NULL);

    return NULL;

}
int main(int argc, char** argv) {


    int res = 0;
    int die = 0;

    freenect_context *f_ctx;
    freenect_device *f_dev;
    printf("Kinect camera test\n");
    if (freenect_init(&f_ctx, NULL) < 0)
    {
        printf("freenect_init() failed\n");
        return 1;
    }
    if (freenect_open_device(f_ctx, &f_dev, 0) < 0)
    {
        printf("Could not open device\n");
        return 1;
    }

    freenect_set_depth_callback(f_dev, depth_cb);
    freenect_set_video_callback(f_dev, rgb_cb);

    // create opencv display thread
    res = pthread_create(&cv_thread, NULL, cv_threadfunc, NULL);
    if (res)
    {
        printf("pthread_create failed\n");
        return 1;
    }

    freenect_start_depth(f_dev);
    freenect_start_video(f_dev);

    while(!die && freenect_process_events(f_ctx) >= 0 );


    return 0;

}
