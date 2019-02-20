#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
	unsigned char format[3];
	unsigned short width, height;
	unsigned short maxval;
	unsigned char *gray;
	unsigned char *red;
	unsigned char *green;
	unsigned char *blue;
}image;

struct arg_struct {
	image *in;
	image *out;
	int thread_id;
};

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

void* threadFunction(void *arguments);

#endif /* HOMEWORK_H */