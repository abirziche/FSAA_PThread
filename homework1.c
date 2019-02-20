#include "homework1.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

int num_threads;
int resolution;

void* threadFunction(void *arguments) {

	struct arg_struct *args = arguments;
	int start, end, i, j;
	double x, y;
	start = ceil((double)args->in->width/(double)num_threads) * args->thread_id;
	end = fmin(ceil((double)args->in->width/(double)num_threads) * 
				(args->thread_id + 1), args->in->width);

	for (i = start; i < end; i++) {
		for (j = 0; j < resolution; j++) {

			x = (100.f / resolution) * (i + 0.5);
			y = (100.f / resolution) * (resolution - j - 0.5); 
			if ((abs(-1 * x + 2 * y ) / sqrt(5.f)) <= 3)
				*(args->in->data + j * args->in->width + i) = 0;
			else
				*(args->in->data + j * args->in->width + i) = 255;
		}
	}
	return NULL;
}

void initialize(image *im) {
	strcpy(im->format, "P5\n");
	im->width = resolution;
	im->maxval = 255;
	im->data = (unsigned char *)malloc(im->width * im->width);
}

void render(image *im) {

	pthread_t tid[num_threads];
	int i;
	struct arg_struct args[num_threads];
	for(i = 0; i < num_threads; i++) {
		args[i].in = im;
		args[i].thread_id = i;
	}

	for(i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, (void *)&(args[i]));
	}
	
	for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}

}

void writeData(const char * fileName, image *img) {

	FILE *fp;
	fp = fopen(fileName, "w+");

	fwrite(img->format, 1, sizeof(img->format), fp);
	fprintf(fp, "%hu %hu\n", img->width, img->width);
	fprintf(fp, "%hu\n", img->maxval);
	fwrite(img->data, 1, img->width * img->width, fp);

	free(img->data);
	fclose(fp);
}
