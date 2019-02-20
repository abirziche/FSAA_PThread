#include "homework.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
int num_threads;
int resize_factor;

void* threadFunction(void *arguments)
{
	struct arg_struct *args = arguments;
	int start, end, offset, row, column;
	int i, j, k, l, s, sr, sg, sb;
	const int GK[3][3] = {  {1, 2, 1} , {2, 4, 2} , {1, 2, 1}  };
	start = ceil((double)args->out->height/(double)num_threads) * args->thread_id;
	end = fmin(ceil((double)args->out->height/(double)num_threads) * 
				(args->thread_id + 1), args->out->height);

	for (i = start; i < end; i++) {
		for (j = 0; j < args->out->width; j++) {
			s = 0;
			sr = 0;
			sg = 0;
			sb = 0;

			for (k = i * resize_factor; k < (i + 1) * resize_factor; k++) {
				for (l = j * resize_factor; l < (j + 1) * resize_factor; l++) {

					offset = k * args->in->width + l;
					row = k - i * resize_factor;
					column = l - j * resize_factor;
 
					if (args->out->format[1] - 48 == 5) {
						if (resize_factor == 3)
							s += *(args->in->gray + offset) * GK[row][column];
						else
							s += *(args->in->gray + offset);
					} else {
						if (resize_factor == 3) {
							sr += *(args->in->red + offset) * GK[row][column];
							sg += *(args->in->green + offset) * GK[row][column];
							sb += *(args->in->blue + offset) * GK[row][column];
						} else { 
							sr += *(args->in->red + offset);
							sg += *(args->in->green + offset);
							sb += *(args->in->blue + offset);
						}
					}
				}
			}

			offset = i * args->out->width + j;

			if (args->out->format[1] - 48 == 5) {
				if (resize_factor == 3)
					s = s / 16;
				else 
					s = s / (resize_factor * resize_factor);
				*(args->out->gray + offset) = (unsigned char)s;
			} else {
				
				if (resize_factor == 3) {
					sr = sr / 16;
					sg = sg / 16;
					sb = sb / 16;
				} else { 
					sr = sr / (resize_factor * resize_factor);
					sg = sg / (resize_factor * resize_factor);
					sb = sb / (resize_factor * resize_factor);
				}
				*(args->out->red + offset) = (unsigned char)sr;
				*(args->out->green + offset) = (unsigned char)sg;
				*(args->out->blue + offset) = (unsigned char)sb;
			}
		}
	}
	return NULL;
}

void readInput(const char * fileName, image *img) {

	FILE *fp;
	unsigned char currChar;
	fp = fopen(fileName, "rb");
	int i;
	fread(img->format, 3, 1, fp);

	img->width = 0;
	img->height = 0;
	img->maxval = 0;
	while((currChar = fgetc(fp)) != ' ') {
		img->width = img->width * 10 + currChar - 48;
	}
	while((currChar = fgetc(fp)) != '\n') {
		img->height = img->height * 10 + currChar - 48;
	}
	while((currChar = fgetc(fp)) != '\n') {
		img->maxval = img->maxval * 10 + currChar - 48;
	}

	if((img->format[1] - 48 ) == 5) {
		img->gray = (unsigned char *)malloc(img->height * img->width);
		fread(img->gray, img->height * img->width, 1, fp);
	}
	else if ((img->format[1] - 48) == 6) {
		img->red = (unsigned char *)malloc(img->height * img->width);
		img->green = (unsigned char *)malloc(img->height * img->width);
		img->blue = (unsigned char *)malloc(img->height * img->width);

		unsigned char *buffer;
		buffer = (unsigned char *)malloc(img->height * img->width * 3);
		fread(buffer, img->height * img->width * 3, 1, fp);
		for (i = 0; i < img->height * img->width * 3; i++) {
			if (i % 3 == 0)
				memcpy(img->red + i / 3, buffer + i, 1);
			if (i % 3 == 1)
				memcpy(img->green + i / 3, buffer + i, 1);
			if (i % 3 == 2)
				memcpy(img->blue + i / 3, buffer + i, 1);
		}
		free(buffer);
	}
	fclose(fp);
}

void writeData(const char * fileName, image *img) {

	FILE *fp;
	fp = fopen(fileName, "w+");
	int i;
	fwrite(img->format, 1, sizeof(img->format), fp);
	fprintf(fp, "%hu ", img->width);
	fprintf(fp, "%hu\n", img->height);
	fprintf(fp, "%hu\n", img->maxval);
	if (img->format[1] - 48 == 5) {
		fwrite(img->gray, 1, img->width * img->height, fp);
		free(img->gray);
	} else {

		unsigned char *buffer;
		buffer = (unsigned char *)malloc(img->height * img->width * 3);
		for (i = 0; i < img->height * img->width; i++) {
				memcpy(buffer + 3 * i, img->red + i, 1);
				memcpy(buffer + 3 * i + 1, img->green + i, 1);
				memcpy(buffer + 3 * i + 2, img->blue + i, 1);
		}
		fwrite(buffer, 1, img->width * img->height * 3, fp);
		free(buffer);
		free(img->red);
		free(img->green);
		free(img->blue);
	}
	fclose(fp);
}

void resize(image *in, image * out) {

	memcpy(out->format, in->format, 3);	
	out->width = in->width / resize_factor;
	out->height = in->height / resize_factor;
	out->maxval = in->maxval;

	if((in->format[1] - 48 ) == 5) {
		out->gray = (unsigned char *)malloc(out->height * out->width);
	} else {
		out->red = (unsigned char *)malloc(out->height * out->width);
		out->green = (unsigned char *)malloc(out->height * out->width);
		out->blue = (unsigned char *)malloc(out->height * out->width);
	}

	pthread_t tid[num_threads];
	int i;
	struct arg_struct args[num_threads];
	for(i = 0; i < num_threads; i++) {
		args[i].in = in;
		args[i].out = out;
		args[i].thread_id = i;
	}

	for(i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, (void *)&(args[i]));
	}
	
	for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

	if((in->format[1] - 48) == 5) {
		free(in->gray);
	} else {
		free(in->red);
		free(in->blue);
		free(in->green);
	}
}
