#ifndef HOMEWORK_H1
#define HOMEWORK_H1

typedef struct {
	char format[3];
	unsigned short width;
	unsigned short maxval;
	unsigned char *data;
}image;

struct arg_struct {
	image *in;
	int thread_id;
};

void initialize(image *im);
void render(image *im);
void writeData(const char * fileName, image *img);
void* threadFunction(void *arguments);

#endif /* HOMEWORK_H1 */