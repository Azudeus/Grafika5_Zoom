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
int dxMeriam=0;
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

void *geserMeriam(void *x_void_ptr) {
	while (end==1) {
		if (c == 'a') {
			dxMeriam=-10;
			c = 'p';
		}
		else if (c == 'd') {
			dxMeriam=10;
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

	if (pthread_create(&thread_seekCannon, NULL, geserMeriam, NULL)) {
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
	initMatrix(&M, 1200, 700);
	resetMatrix(&M);

	Object pesawat = makePesawat(950,100);
	// Object ledakan;
	// Object ledakan1;
	// Object ledakan2;
	// Object ledakan3;
	Object meriam = makeMeriam(xMeriam,750);
	// Object wheel = makeWheel(980,102);
 //    Object lineBaling = makeLine(1185, 100);
 //    Object baling = makeBaling(1189, 100);
	
	gambarObject(meriam, &M, c3,mul);
	gambarObject(pesawat, &M, c1,mul);
    // gambarObject(wheel, &M, c4,mul);
    // gambarObject(lineBaling, &M, c4,mul);
    // gambarObject(baling, &M, c1,mul);
    // gambarObject(ledakan3,&M,c4,mul);

   	int x = 0, y = 0;
	int xPesawat = 1100;

	fillMatrix(&M, xPesawat, 100, BLUE);
	fillMatrix(&M, meriam.pointInit.x, 690, RED); 			// meriam bawah
	fillMatrix(&M, meriam.pointInit.x, 680, GREEN); 		// meriam atas
	fillMatrix(&M, meriam.pointInit.x, 620, BLUE); 		// meriam persegi panjang
    
	for (y = 0; y < 700; y++) {
        for (x = 0; x < 1200; x++) {
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


	// int collide = 0;
	// int xWheel = 980;
	// int yWheel=110;
	// int xBaling = 1189;
 //    int yBaling = 100;

	// do {
	// 	disable_waiting_for_enter();
	// 	moveHorizontal(&pesawat,-2);
	// 	moveHorizontal(&wheel, -2);
	// 	moveHorizontal(&baling, -2);
	// 	moveHorizontal(&lineBaling, -2);
	// 	rotateClockwise(&baling, 2);

	// 	for (int j = 0; j < nBullets; ++j) {
	// 		moveVertical(&bullets[j], -5);
	// 		yBullet[j] -=5;
	// 	}

	// 	resetMatrix(&M);
	// 	gambarObject(pesawat, &M, c1,mul);
	//    	gambarObject(meriam, &M, c3,mul);
	//    	gambarObject(wheel, &M, c4,mul);
	//    	gambarObject(lineBaling, &M, c4,mul);
	// 	gambarObject(baling, &M, c1,mul);

	// 	for (int j = 0; j < nBullets; ++j) {
	// 		gambarObject(bullets[j], &M, c2,mul);
	// 		// fillMatrix(&M, xBullet[j], yBullet[j], WHITE);
	// 	}
		
	// 	moveHorizontal(&meriam, dxMeriam);		
	// 	xWheel -= 2;
	// 	xBaling -= 2;
	// 	xPesawat -= 2;
	// 	xMeriam = meriam.pointInit.x;

	// 	// fillMatrix(&M, xBaling, yBaling, RED);
		// fillMatrix(&M, meriam.pointInit.x, 690, RED); 			// meriam bawah
		// fillMatrix(&M, meriam.pointInit.x, 680, GREEN); 		// meriam atas
		// fillMatrix(&M, meriam.pointInit.x, 620, BLUE); 		// meriam persegi panjang
		// fillMatrix(&M, xPesawat, 100, BLUE);
	// 	// fillMatrix(&M, xWheel, 125, WHITE);
		
	// 	dxMeriam = 0;

	//    	for (y = 0; y < 700; y++) {
	// 		for (x = 0; x < 1200; x++) {
	// 			location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
	// 			(y+vinfo.yoffset) * finfo.line_length;

	// 			if (vinfo.bits_per_pixel == 32) {
	// 				if (M.M[y][x] == GREEN || M.M[y][x] == BLUE || M.M[y][x] == RED || M.M[y][x] == WHITE) {
	// 					fillColor(M.M[y][x]);
	// 				}
	// 				else { 
	// 					*(fbp + location) = M.M[y][x];
	// 					*(fbp + location + 1) = M.M[y][x];
	// 					*(fbp + location + 2) = M.M[y][x];
	// 					*(fbp + location + 3) = 0;
	// 				}
	// 			} else  {
	// 				int b = 10;
	// 				int g = (x-100)/6;
	// 				int r = 31-(y-100)/16;
	// 				unsigned short int t = r<<11 | g << 5 | b;
	// 				*((unsigned short int*)(fbp + location)) = t;
	// 			}
	// 		}
	// 	}
		
	// 	//check if plane is out of screen
	// 	if (isOut(&pesawat,-300,0)){
 //    		moveHorizontal(&pesawat,1500);
 //    		moveHorizontal(&wheel, 1500);
 //    		moveHorizontal(&baling, 1500);
 //    		moveHorizontal(&lineBaling, 1500);
 //    		xPesawat = 1300;
 //    		xWheel = 1240;
 //    		xBaling = 1440;
 //    	}
		
	// 	//check collide condition
	// 	if (isObjectCollide(pesawat, &M, c1) == 1) {
	// 		collide = 1;

	// 		ledakan = makeLedakan(550,100);
	// 		pesawat = makePesawat(1500,100);
	// 		ledakan1 = makeLedakanPesawat1(550,100);
	// 		ledakan2 = makeLedakanPesawat2(550,100);
	// 		ledakan3 = makeLedakanPesawat3(550,110);
	// 		Object pilot = makePilot(xPesawat+50, 100);
	// 		Object parasut = makeParasut(xPesawat+50, 20);

	// 		Point l1;
	// 		Point l2;
	// 		Point l3;
	// 		l1.x = 547;
	// 		l1.y = 100;
	// 		l2.x = 550;
	// 		l2.y = 100;
	// 		l3.x = 550;
	// 		l3.y = 105;

	// 		int yParasut = 20;
	// 		int dx = 0;
	// 		int xtreamPoint = 550;
	// 		int isLanded=0;
	// 		int isBalingLanded=0;
	// 		int isBounced=0;
	// 		int isXtream;
			
	// 		while(xtreamPoint<670){
	// 			if (!isLanded) {
	// 				moveVertical(&wheel, 5);
	// 				rotateWheelClockwise(&wheel, 5);
	// 				yWheel+=5;
					
	// 				moveVertical(&baling, 5);
	// 				rotateClockwise(&baling, 5);
	// 				yBaling += 5;
	// 			}

	// 			if (isWheelOut(&wheel)) {
	// 				xtreamPoint+=14;
	// 				isLanded=1;
	// 				isBounced=1;
	// 				isXtream=0;
	// 			}
				
	// 			if (isBounced) {
	// 				wheelBounce(&wheel, xtreamPoint, &isXtream);
	// 				xWheel+=3;
	// 				if (!isXtream)
	// 					yWheel-=5;
	// 				else yWheel+=5;
					
	// 				moveHorizontal(&baling, 2);
	// 				rotateClockwise(&baling, 3);
	// 				xBaling += 2;
	// 			}
			
	// 			pilotTerjun(&pilot);
	// 			pilotTerjun(&parasut);
	// 			yParasut+=3;

	// 			dx++;
	// 			moveHorizontal(&ledakan1,-3);
	// 			moveVertical(&ledakan1,((dx*dx)+2*dx)/1000);
	// 			l1.x += -3;
	// 			l1.y +=((dx*dx)+2*dx)/1000;
	// 			rotateCounterClockwise(&ledakan1,15);

	// 			moveHorizontal(&ledakan2,2);
	// 			moveVertical(&ledakan2,((dx*dx)+2*dx)/1500);
	// 			l2.x += 2;
	// 			l2.y +=((dx*dx)+2*dx)/1500;
	// 			rotateClockwise(&ledakan2,5);

	// 			moveHorizontal(&ledakan3,5);
	// 			moveVertical(&ledakan3,((dx*dx)+2*dx)/1200);
	// 			l3.x += 5;
	// 			l3.y +=((dx*dx)+2*dx)/1200;
	// 			rotateClockwise(&ledakan3,10);

	// 			moveHorizontal(&lineBaling,5);
	// 			moveVertical(&lineBaling,((dx*dx)+2*dx)/1200);
	// 			rotateClockwise(&lineBaling, 5);

	// 			resetMatrix(&M);
	// 			gambarObject(lineBaling, &M, c4,mul);
	// 			gambarObject(baling, &M, c1,mul);
	// 			gambarObject(wheel, &M, c4,mul);
	// 			gambarObject(meriam, &M, c3,mul);
	// 			gambarObject(ledakan, &M, c3,mul);
	// 			gambarObject(ledakan1, &M, c4,mul);
	// 			gambarObject(ledakan2, &M, c4,mul);
	// 			gambarObject(ledakan3, &M, c4,mul);
	// 			gambarObject(pilot, &M, c4,mul);
	// 			gambarObject(parasut, &M, c1,mul);

	// 			// fillMatrix(&M, xBaling, yBaling, RED);
	// 			// fillMatrix(&M, xWheel, yWheel, WHITE);
	// 			// fillMatrix(&M, l1.x, l1.y, BLUE);
	// 			// fillMatrix(&M, l2.x, l2.y, BLUE);
	// 			// fillMatrix(&M, l3.x, l3.y, BLUE);
	// 			// fillMatrix(&M, 550, 170, RED); 		// ledakan
	// 			// fillMatrix(&M, xMeriam, 690, RED); 		// meriam bawah
	// 			// fillMatrix(&M, xMeriam, 680, GREEN); 	// meriam atas
	// 			// fillMatrix(&M, xMeriam, 620, BLUE); 	// meriam persegi panjang
	// 			// fillMatrix(&M, xWheel, yWheel, WHITE);
	// 			// fillMatrix(&M, xPesawat+100, yParasut, GREEN);

	// 			for (y = 0; y < 1200; y++) {
	// 				for (x = 0; x < 700; x++) {
	// 					location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
	// 							   (y+vinfo.yoffset) * finfo.line_length;

	// 					if (vinfo.bits_per_pixel == 32) {
	// 						if (M.M[y][x] == GREEN || M.M[y][x] == BLUE || M.M[y][x] == RED || M.M[y][x] == WHITE) {
	// 							fillColor(M.M[y][x]);
	// 						}
	// 						else { 
	// 							*(fbp + location) = M.M[y][x];
	// 							*(fbp + location + 1) = M.M[y][x];
	// 							*(fbp + location + 2) = M.M[y][x];
	// 							*(fbp + location + 3) = 0;
	// 						}
	// 					} else  {
	// 						int b = 10;
	// 						int g = (x-100)/6;
	// 						int r = 31-(y-100)/16;
	// 						unsigned short int t = r<<11 | g << 5 | b;
	// 						*((unsigned short int*)(fbp + location)) = t;
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// } while (collide == 0);
	
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
