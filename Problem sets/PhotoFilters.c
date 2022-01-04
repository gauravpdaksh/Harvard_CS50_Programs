#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Getting a shade of gray for the current pixel
            float GrayShade = ((float)image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3;

            int RGrayShade = round(GrayShade);

            // Asssigning the shade of gray to the current pixel
            image[i][j].rgbtBlue = (BYTE)RGrayShade;
            image[i][j].rgbtGreen = (BYTE)RGrayShade;
            image[i][j].rgbtRed = (BYTE)RGrayShade;
        }
    }
    return;
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Sepia filter formula
            float sepiaRed = .393 * (float)image[i][j].rgbtRed + .769 * image[i][j].rgbtGreen + .189 * image[i][j].rgbtBlue;
            float sepiaGreen = .349 * (float)image[i][j].rgbtRed + .686 * image[i][j].rgbtGreen + .168 * image[i][j].rgbtBlue;
            float sepiaBlue = .272 * (float)image[i][j].rgbtRed + .534 * image[i][j].rgbtGreen + .131 * image[i][j].rgbtBlue;

            // Putting a cap on maximum values of sepiaRed, sepiaGreen, and sepiaBlue
            if (sepiaRed > 255)
            sepiaRed = 255;

            if (sepiaGreen > 255)
            sepiaGreen = 255;

            if (sepiaBlue > 255)
            sepiaBlue = 255;

            // Rounding off to the nearest whole number
            int RsepiaRed = round(sepiaRed);
            int RsepiaGreen = round(sepiaGreen);
            int RsepiaBlue = round(sepiaBlue);

            // Asssigning the final sepia color values to the current pixel
            image[i][j].rgbtRed = (BYTE)RsepiaRed;
            image[i][j].rgbtGreen = (BYTE)RsepiaGreen;
            image[i][j].rgbtBlue = (BYTE)RsepiaBlue;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        // Reverse values horizontally in each row
        int tmp;

        // For an image with a width of one pixel
        if (width == 1)
        {
            return;

        }


        // For even number of pixels in each row
        else if ((width % 2) == 0)
        {
            for (int j = 0; j <= ((width / 2) - 1); j++)
            {
                tmp = image[i][width -j -1].rgbtBlue;
                image[i][width -j -1].rgbtBlue = image[i][j].rgbtBlue;
                image[i][j].rgbtBlue = tmp;

                tmp = image[i][width -j -1].rgbtGreen;
                image[i][width -j -1].rgbtGreen = image[i][j].rgbtGreen;
                image[i][j].rgbtGreen = tmp;

                tmp = image[i][width -j -1].rgbtRed;
                image[i][width -j -1].rgbtRed = image[i][j].rgbtRed;
                image[i][j].rgbtRed = tmp;

            }

        }

        // For odd number of pixels in each row (except when width = 1 pixel)
        else
        {
            for (int j = 0; j <= (width - 3) / 2; j++)
            {
                tmp = image[i][width -j -1].rgbtBlue;
                image[i][width -j -1].rgbtBlue = image[i][j].rgbtBlue;
                image[i][j].rgbtBlue = tmp;

                tmp = image[i][width -j -1].rgbtGreen;
                image[i][width -j -1].rgbtGreen = image[i][j].rgbtGreen;
                image[i][j].rgbtGreen = tmp;

                tmp = image[i][width -j -1].rgbtRed;
                image[i][width -j -1].rgbtRed = image[i][j].rgbtRed;
                image[i][j].rgbtRed = tmp;
            }
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    int i;
    int j;
    float tmp;
    int Rtmp;

    BYTE tmpBlurR[height][width];
    BYTE tmpBlurG[height][width];
    BYTE tmpBlurB[height][width];


    // Blur effect on the whole image except the corners and edges
    for (i = 1; i < height; i++)
    {
        for (j = 1; j < width; j++)
        {
            // Blur effect on the current pixel

            // For red value
            tmp = ( (float)image[i - 1][j - 1].rgbtRed + image[i - 1][j].rgbtRed + image[i - 1][j + 1].rgbtRed
                  + image[i][j - 1].rgbtRed     + image[i][j].rgbtRed     + image[i][j + 1].rgbtRed
                  + image[i + 1][j - 1].rgbtRed + image[i + 1][j].rgbtRed + image[i + 1][j + 1].rgbtRed ) / 9;

            Rtmp = round(tmp);

            // Put a cap on the maximum value of tmp
            if (tmp > 255)
                tmp = 255;

            tmpBlurR[i][j] = (BYTE)Rtmp;

            // For green value
            tmp = ( (float)image[i - 1][j - 1].rgbtGreen + image[i - 1][j].rgbtGreen + image[i - 1][j + 1].rgbtGreen
                  + image[i][j - 1].rgbtGreen     + image[i][j].rgbtGreen     + image[i][j + 1].rgbtGreen
                  + image[i + 1][j - 1].rgbtGreen + image[i + 1][j].rgbtGreen + image[i + 1][j + 1].rgbtGreen ) / 9;

            Rtmp = round(tmp);

            // Put a cap on the maximum value of tmp
            if (tmp > 255)
                tmp = 255;

            tmpBlurG[i][j] = (BYTE)Rtmp;

            // For blue value
            tmp = ( (float)image[i - 1][j - 1].rgbtBlue + image[i - 1][j].rgbtBlue + image[i - 1][j + 1].rgbtBlue
                  + image[i][j - 1].rgbtBlue     + image[i][j].rgbtBlue     + image[i][j + 1].rgbtBlue
                  + image[i + 1][j - 1].rgbtBlue + image[i + 1][j].rgbtBlue + image[i + 1][j + 1].rgbtBlue ) / 9;

            Rtmp = round(tmp);

            // Put a cap on the maximum value of tmp
            if (tmp > 255)
                tmp = 255;

            tmpBlurB[i][j] = (BYTE)Rtmp;
        }
    }



    // Blur effect on the upper-left corner
    i = 0;
    j = 0;

    // For red value
    tmp = ( (float)image[i][j].rgbtRed     + image[i][j + 1].rgbtRed
          + image[i + 1][j].rgbtRed + image[i + 1][j + 1].rgbtRed ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurR[i][j] = (BYTE)Rtmp;

    // For red value
    tmp = ( (float)image[i][j].rgbtGreen     + image[i][j + 1].rgbtGreen
          + image[i + 1][j].rgbtGreen + image[i + 1][j + 1].rgbtGreen ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurG[i][j] = (BYTE)Rtmp;

    // For red value
    tmp = ( (float)image[i][j].rgbtBlue     + image[i][j + 1].rgbtBlue
          + image[i + 1][j].rgbtBlue + image[i + 1][j + 1].rgbtBlue ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurB[i][j] = (BYTE)Rtmp;



    // Blur effect on the upper-right corner
    i = 0;
    j = width;

    // For red value
    tmp = ( (float)image[i][j - 1].rgbtRed     + image[i][j].rgbtRed
          + image[i + 1][j - 1].rgbtRed + image[i + 1][j].rgbtRed ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurR[i][j] = (BYTE)Rtmp;

    // For red value
    tmp = ( (float)image[i][j - 1].rgbtGreen     + image[i][j].rgbtGreen
          + image[i + 1][j - 1].rgbtGreen + image[i + 1][j].rgbtGreen ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurG[i][j] = (BYTE)Rtmp;

    // For red value
    tmp = ( (float)image[i][j - 1].rgbtBlue     + image[i][j].rgbtBlue
          + image[i + 1][j - 1].rgbtBlue + image[i + 1][j].rgbtBlue ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurB[i][j] = (BYTE)Rtmp;



    // Blur effect on the lower-left corner
    i = height;
    j = 0;

    // For red value
    tmp = ( (float)image[i - 1][j].rgbtRed     + image[i - 1][j + 1].rgbtRed
          + image[i][j].rgbtRed + image[i][j + 1].rgbtRed ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurR[i][j] = (BYTE)Rtmp;

    // For red value
    tmp = ( (float)image[i - 1][j].rgbtGreen     + image[i - 1][j + 1].rgbtGreen
          + image[i][j].rgbtGreen + image[i][j + 1].rgbtGreen ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurG[i][j] = (BYTE)Rtmp;

    // For red value
    tmp = ( (float)image[i - 1][j].rgbtBlue     + image[i - 1][j + 1].rgbtBlue
          + image[i][j].rgbtBlue + image[i][j + 1].rgbtBlue ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurB[i][j] = (BYTE)Rtmp;



    // Blur effect on the lower-right corner
    i = height;
    j = width;

    // For red value
    tmp = ( (float)image[i - 1][j - 1].rgbtRed     + image[i - 1][j].rgbtRed
          + image[i][j - 1].rgbtRed + image[i][j].rgbtRed ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurR[i][j] = (BYTE)Rtmp;

    // For red value
    tmp = ( (float)image[i - 1][j - 1].rgbtGreen     + image[i - 1][j].rgbtGreen
          + image[i][j - 1].rgbtGreen + image[i][j].rgbtGreen ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurG[i][j] = (BYTE)Rtmp;

    // For red value
    tmp = ( (float)image[i - 1][j - 1].rgbtBlue     + image[i - 1][j].rgbtBlue
          + image[i][j - 1].rgbtBlue + image[i][j].rgbtBlue ) / 4;

    Rtmp = round(tmp);

    // Put a cap on the maximum value of tmp
    if (tmp > 255)
        tmp = 255;

    tmpBlurB[i][j] = (BYTE)Rtmp;



    // Blur effect on the left edge
    j = 0;

    for (i = 1; i < height; i++)
    {
        // Blur effect on the current pixel

        // For red value
        tmp = ( (float)image[i - 1][j].rgbtRed + image[i - 1][j + 1].rgbtRed
              + image[i][j].rgbtRed     + image[i][j + 1].rgbtRed
              + image[i + 1][j].rgbtRed + image[i + 1][j + 1].rgbtRed ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurR[i][j] = (BYTE)Rtmp;

        // For green value
        tmp = ( (float)image[i - 1][j].rgbtGreen + image[i - 1][j + 1].rgbtGreen
              + image[i][j].rgbtGreen    + image[i][j + 1].rgbtGreen
              + image[i + 1][j].rgbtGreen + image[i + 1][j + 1].rgbtGreen ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurG[i][j] = (BYTE)Rtmp;

        // For blue value
        tmp = ( (float)image[i - 1][j].rgbtBlue + image[i - 1][j + 1].rgbtBlue
              + image[i][j].rgbtBlue     + image[i][j + 1].rgbtBlue
              + image[i + 1][j].rgbtBlue + image[i + 1][j + 1].rgbtBlue ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurB[i][j] = (BYTE)Rtmp;
    }


    // Blur effect on the upper edge
    i = 0;

    for (j = 1; j < width; j++)
    {
        // Blur effect on the current pixel

        // For red value
        tmp = ( (float)image[i][j - 1].rgbtRed     + image[i][j].rgbtRed     + image[i][j + 1].rgbtRed
              + image[i + 1][j - 1].rgbtRed + image[i + 1][j].rgbtRed + image[i + 1][j + 1].rgbtRed ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurR[i][j] = (BYTE)Rtmp;

        // For green value
        tmp = ( (float)image[i][j - 1].rgbtGreen     + image[i][j].rgbtGreen     + image[i][j + 1].rgbtGreen
              + image[i + 1][j - 1].rgbtGreen + image[i + 1][j].rgbtGreen + image[i + 1][j + 1].rgbtGreen ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurG[i][j] = (BYTE)Rtmp;

        // For blue value
        tmp = ( (float)image[i][j - 1].rgbtBlue     + image[i][j].rgbtBlue     + image[i][j + 1].rgbtBlue
              + image[i + 1][j - 1].rgbtBlue     + image[i + 1][j].rgbtBlue + image[i + 1][j + 1].rgbtBlue ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurB[i][j] = (BYTE)Rtmp;
    }


    // Blur effect on the right edge
    j = width;

    for (i = 1; i < height; i++)
    {
        // Blur effect on the current pixel

        // For red value
        tmp = ( (float)image[i - 1][j - 1].rgbtRed + image[i - 1][j].rgbtRed
              + image[i][j - 1].rgbtRed     + image[i][j].rgbtRed
              + image[i + 1][j - 1].rgbtRed + image[i + 1][j].rgbtRed ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurR[i][j] = (BYTE)Rtmp;

        // For green value
        tmp = ( (float)image[i - 1][j - 1].rgbtGreen + image[i - 1][j].rgbtGreen
              + image[i][j - 1].rgbtGreen     + image[i][j].rgbtGreen
              + image[i + 1][j - 1].rgbtGreen + image[i + 1][j].rgbtGreen ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurG[i][j] = (BYTE)Rtmp;

        // For blue value
        tmp = ( (float)image[i - 1][j - 1].rgbtBlue + image[i - 1][j].rgbtBlue
              + image[i][j - 1].rgbtBlue     + image[i][j].rgbtBlue
              + image[i + 1][j - 1].rgbtBlue + image[i + 1][j].rgbtBlue ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurB[i][j] = (BYTE)Rtmp;
    }


    // Blur effect on the lower edge
    i = height;

    for (j = 1; j < width; j++)
    {
        // Blur effect on the current pixel

        // For red value
        tmp = ( (float)image[i - 1][j - 1].rgbtRed + image[i - 1][j].rgbtRed + image[i - 1][j + 1].rgbtRed
              + image[i][j - 1].rgbtRed     + image[i][j].rgbtRed     + image[i][j + 1].rgbtRed ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurR[i][j] = (BYTE)Rtmp;

        // For green value
        tmp = ( (float)image[i - 1][j - 1].rgbtGreen + image[i - 1][j].rgbtGreen + image[i - 1][j + 1].rgbtGreen
              + image[i][j - 1].rgbtGreen     + image[i][j].rgbtGreen     + image[i][j + 1].rgbtGreen ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

        tmpBlurG[i][j] = (BYTE)Rtmp;

        // For blue value
        tmp = ( (float)image[i - 1][j - 1].rgbtBlue + image[i - 1][j].rgbtBlue + image[i - 1][j + 1].rgbtBlue
              + image[i][j - 1].rgbtBlue     + image[i][j].rgbtBlue     + image[i][j + 1].rgbtBlue ) / 6;

        Rtmp = round(tmp);

        // Put a cap on the maximum value of tmp
        if (tmp > 255)
            tmp = 255;

      tmpBlurB[i][j] = (BYTE)Rtmp;
    }


    // Asssign the corresponding blurred pixel to the current pixel
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = tmpBlurR[i][j];
            image[i][j].rgbtGreen = tmpBlurG[i][j];
            image[i][j].rgbtBlue = tmpBlurB[i][j];

        }
    }
    return;
}
