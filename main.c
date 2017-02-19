#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <math.h>
#include <time.h>
#include "lingkaran.h"
#include "matrix.h"
#include "point.h"
#include "object.h"
#include <stdio.h>
#include <pthread.h>
#include <termios.h>

static struct termios oldt;
char c;
int end = 1;
int nBullets = 0;
Object bullets[20];
int yBullet[20];
int xBullet[20];
 
char *fbp = 0;
int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
long location;
int dx=0;
int dy=0;


int xMeriam = 600;

float mul;

#define RED 1
#define BLACK 2
#define WHITE 3
#define GREEN 4
#define BLUE 5

void fillColor(int color) {
	if(color == RED) {
		*(fbp + location) = 0; 
		*(fbp + location + 1) = 0;   
		*(fbp + location + 2) = 255;   
		*(fbp + location + 3) = 0;  
	}
	else if(color == BLACK) {
		*(fbp + location) = 0; 
		*(fbp + location + 1) = 0; 
		*(fbp + location + 2) = 0; 
		*(fbp + location + 3) = 0;   					
	}
	else if(color == GREEN) {
		*(fbp + location) = 0;    
		*(fbp + location + 1) = 255; 
		*(fbp + location + 2) = 0; 
		*(fbp + location + 3) = 0; 
	} 
	else if(color == WHITE) {
		*(fbp + location) = 255; 
		*(fbp + location + 1) = 255;  
		*(fbp + location + 2) = 255;
		*(fbp + location + 3) = 0; 
	}
	else if(color == BLUE) {
		*(fbp + location) = 255; 
		*(fbp + location + 1) = 0;  
		*(fbp + location + 2) = 0;
		*(fbp + location + 3) = 50; 
	}
}
 
void *get_keypress(void *x_void_ptr)
{
	while (end == 1) {
		c=getchar();
	}
}

void *make_bullets(void *x_void_ptr) {
	while (end == 1) {
		if (c == '\n') {
			bullets[nBullets] = makePeluru(xMeriam,500);
			yBullet[nBullets] = 510;
			xBullet[nBullets] = xMeriam;
			++nBullets;
			if (nBullets >= 19) {
				nBullets = 0;
			}
			c = 'p';
		}
	}
}

void *geser(void *x_void_ptr) {
	while (end==1) {
		if (c == 'a') {
			dx=10;
			c = 'p';
		}
		else if (c == 'd') {
			dx=-10;
			c = 'p';
		}
		else if (c == 'w') {
			dy=10;
			c = 'p';
		}
		else if (c == 's') {
			dy=-10;
			c = 'p';
		}
	}
}

void restore_terminal_settings(void)
{
    tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
}

void disable_waiting_for_enter(void)
{
    struct termios newt;

    /* Make terminal read 1 char at a time */
    tcgetattr(0, &oldt);  /* Save terminal settings */
    newt = oldt;  /* Init new settings */
    newt.c_lflag &= ~(ICANON | ECHO);  /* Change settings */
    tcsetattr(0, TCSANOW, &newt);  /* Apply settings */
    atexit(restore_terminal_settings); /* Make sure settings will be restored when program ends  */
}

int main(){
   	int x = 0, y = 0;

   	printf("Enter Zoom: ");
   	scanf("%f",&mul);

	// Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fbfd, 0);
    if ((int)*fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");

	pthread_t thread_keypress, thread_bullet, thread_seekCannon;

	if(pthread_create(&thread_keypress, NULL, get_keypress, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	
	if(pthread_create(&thread_bullet, NULL, make_bullets, NULL)) {
		fprintf(stderr, "Error creating thread 2\n");
		return 1;
	}

	if (pthread_create(&thread_seekCannon, NULL, geser, NULL)) {
		fprintf(stderr, "Error creating thread 3\n");
		return 1;
	}

	Matrix M;
	Point P1, P2;
	Lingkaran L;
	char c1, c2, c3, c4;
	c1 = '1';
	c2 = '2';
	c3 = '3';
	c4 = 240;
	initMatrix(&M, 3000, 3000);
	Object pesawat = makePesawat(550,300);
	int xPesawat = 650;
	int yPesawat = 300;

	do{
		disable_waiting_for_enter();


		moveHorizontal(&pesawat, dx);
		moveVertical(&pesawat, dy);		
		xPesawat +=dx;
		yPesawat +=dy;

		dx = 0;		
		dy = 0;

		resetMatrix(&M);
		gambarObject(pesawat, &M, c1,mul);

		fillMatrix(&M, xPesawat, yPesawat, BLUE);
	    
		for (y = 200; y < 600; y++) {
	        for (x = 400; x < 800; x++) {
				if (M.M[y][x] == GREEN || M.M[y][x] == BLUE || M.M[y][x] == RED || M.M[y][x] == WHITE) {
					fillColor(M.M[y][x]);
				}
	            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
	                       (y+vinfo.yoffset) * finfo.line_length;

	            if (vinfo.bits_per_pixel == 32) {
	                *(fbp + location) = M.M[y][x];
	                *(fbp + location + 1) = M.M[y][x];
	                *(fbp + location + 2) = M.M[y][x];
	                *(fbp + location + 3) = 0;
	            } else  {
	                int b = 10;
	                int g = (x-100)/6;
	                int r = 31-(y-100)/16;
	                unsigned short int t = r<<11 | g << 5 | b;
	                *((unsigned short int*)(fbp + location)) = t;
	            }
	        }
	    }
	} while (1);
	
	end = 0;
    munmap(fbp, screensize);
    close(fbfd);

	if(pthread_join(thread_keypress, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
	if(pthread_join(thread_bullet, NULL)) {
		fprintf(stderr, "Error joining thread 2\n");
		return 2;
	}
	if (pthread_join(thread_seekCannon,NULL)) {
		fprintf(stderr, "Error joining thread 3\n");
	}
    return 0;
}
