#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(const int argc, char *argv[])
{
    FILE *f;
    IplImage *image, *mask;
    int i, j;

    int red, green, blue;
    char class[20];

    if(argc == 2) {
        f = fopen(argv[1], "r");
    } else {
        return -1;
    }

    fscanf(f, "@RELATION carimage\n\n");
    fscanf(f, "@ATTRIBUTE image_red NUMERIC\n");
    fscanf(f, "@ATTRIBUTE image_green NUMERIC\n");
    fscanf(f, "@ATTRIBUTE image_blue NUMERIC\n");
    fscanf(f, "@ATTRIBUTE class  {NON_NAVIGABLE,NAVIGABLE,BORDER}\n");
    fscanf(f, "\n@DATA\n");

    image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    mask = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);

    for(i = 0; i < 640; i++)
        for(j = 0; j < 480; j++) {
            fscanf(f, "%d,%d,%d,%s\n", &blue, &green, &red, class);
            cvSet2D(image, j, i, CV_RGB(red, green, blue));
            if(strcmp(class, "NON_NAVIGABLE") == 0) {
                cvSet2D(mask, j, i, CV_RGB(255, 0, 0));
            } else if(strcmp(class, "NAVIGABLE") == 0) {
                cvSet2D(mask, j, i, CV_RGB(0, 255, 0));
            } else {
                cvSet2D(mask, j, i, CV_RGB(0, 0, 255));
            }

        }

    cvShowImage("Image", image);
    cvShowImage("Mask", mask);
    while((char) cvWaitKey(10) != 27);

    cvReleaseImage(&image);
    cvReleaseImage(&mask);
    fclose(f);
    return 1;
}
