#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Check command-line arguments
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    // Open forensic image file
    FILE *cardImage = fopen(argv[1], "r");
    if (cardImage == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    // Buffer for storing memory blocks (FAT file system)
    BYTE BLOCK[512];

    int imgNumber = 0;
    FILE *img = NULL;

    // Read each block until the end of file

NextBlock:
    while (fread(BLOCK, sizeof(BYTE), 512, cardImage) == 512)
    {
        // Checking first 4 bytes of the block to check for jpeg signature
        BYTE bytes[4];
        bytes[0] = BLOCK[0];
        bytes[1] = BLOCK[1];
        bytes[2] = BLOCK[2];
        bytes[3] = BLOCK[3];

        if (imgNumber == 0)
        {
            if (bytes[0] != 0xff || bytes[1] != 0xd8 || bytes[2] != 0xff || (bytes[3] & 0xf0) != 0xe0)
            {
                goto NextBlock;
            }
        }

        // Checking if the memory block has jpeg signature
        if (bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff && (bytes[3] & 0xf0) == 0xe0)    // If the block has jpeg signature
        {
            char filename[10];

            if (imgNumber > 0)
            {
                // Closing previous jpeg file (if a jpeg file is already open)
                fclose(img);
            }

            // Decide the names of the jpeg files found on the forensic image
            sprintf(filename, "%03i.jpg", imgNumber);

            // Open jpeg file
            img = fopen(filename, "w");

            if (imgNumber == 0)                    // For the first jpeg
            {
                fwrite(BLOCK, sizeof(BYTE), 512, img);
            }

            if (imgNumber > 0)                  // For subsequent jpegs
            {
                fwrite(BLOCK, sizeof(BYTE), 512, img);
            }
            imgNumber++;
        }
        else                                                                                              // If the block has no jpeg signature
        {
            fwrite(BLOCK, sizeof(BYTE), 512, img);
        }
    }

    // Close files
    fclose(img);
    fclose(cardImage);

    return 0;
}
