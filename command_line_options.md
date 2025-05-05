# Command line options

Your program should accept the command line options detailed below. The first option should always be `-i <input_image>` or `-d <data_volume>` to 
specify whether the program is going to be processing an image or volume, and to specify where to find the input. The last option should be the output image name (this does not require a flag). The program should then process the input image or volume according to the options provided, and save the output image to the specified location.

Note that each option (other than the -i/-d flags and the output name) can use either the long or short flag format: e.g. `--greyscale` or `-g`.

## Image Processing Options

- Input Image: `-i <input_image>`
- Output Image: `<output_image>` (must have extension .png or .jpg)

### Filters
- Greyscale: `--greyscale` or `-g`
- Brightness: `--brightness <value>` or `-b <value>`
- Histogram Equalisation: `--histogram <type>` or `-h <type>` (e.g., HSV, HSL)
- Blur: `--blur <type> <size> [<stdev>]` or `-r <type> <size> [<stdev>]` (e.g., Gaussian 5 2.0, Box 7, Median 3; note `<stdev>` is only required for Gaussian)
- Edge Detection: `--edge <type>` or `-e <type>` (e.g., Sobel, Prewitt, Scharr, RobertsCross)
- Laplacian Sharpening: `--sharpen` or `-p`
- Salt and Pepper Noise: `--saltpepper <amount>` or `-n <amount>`
- Threshold: `--threshold <value> <type>` or `-t <value> <type>` (e.g., 128 HSV, 64 HSL)

You can specify one or multiple filters, by chaining the options together (e.g. `-g -r Median 3` will convert to greyscale and then apply a median blur filter).

## Volume Processing Options

- Input Volume: `-d <data_volume>`
- Output Image: `<output_image>` (must have extension .png or .jpg)

### Volume Reading
- First Index: `--first <index>` or `-f <index>` (optional)
- Last Index: `--last <index>` or `-l <index>` (optional)
- Extension: `--extension <ext>` or `-x <ext>` (optional; default is png).

If first and last index are not defined, all images in volume should be read.

### Volume Blur Filter
- Blur: `--blur <type> <size> [<stdev>]` or `-r <type> <size> [<stdev>]` (e.g., `Gaussian 3 2.0, Median 3`; note `<stdev>` is only required for Gaussian)

Note that the blur filter is optional in volume processing; if it is spcified, the subsequent slice or projection will be applied to the blurred volume, otherwise it will be applied to the original volume.

### Volume Processing Options
- Slice: `--slice <plane> <constant>` or `-s <plane> <constant>` (e.g., `XZ 16`, `YZ 16`)
- Projection: `--projection <type>` or `-p <type>` (e.g., MIP, MinIP, meanAIP, medianAIP)

## Example Commands

- Brightness: `./APImageFilters -i input.png -b 100 output.png`
- Greyscale: `./APImageFilters -i input.png -g output.png`
- Histogram Equalisation: `./APImageFilters -i input.png -h HSV output.png`
- Blur (Gaussian): `./APImageFilters -i input.png -r Gaussian 5 2.0 output.png`
- Edge Detection (Sobel): `./APImageFilters -i input.png --edge Sobel output.png`
- Laplacian Sharpening: `./APImageFilters -i input.png --sharpen output.png`
- Salt and Pepper Noise: `./APImageFilters -i input.png --saltpepper 5 output.png`
- Threshold: `./APImageFilters -i input.png --threshold 128 HSV output.png`
- Volume Slice (XZ): `./APImageFilters -d volume -s XZ 16 output.png`
- Volume Projection (MIP): `./APImageFilters -d volume -p MIP output.png`
