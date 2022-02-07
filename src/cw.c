#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <string.h>
#define RED {0, 0, 255}
#define WHITE {255, 255, 255}
#define GREEN {0, 255, 0}

#pragma pack (push, 1)
typedef struct
{
	unsigned short signature;
	unsigned int filesize;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int pixelArrOffset;
} BitmapFileHeader;

typedef struct tagCIEXYZ1 {
  unsigned int x; 
  unsigned int y; 
  unsigned int z; 
} CIEXYZ1;

typedef struct tagCIEXYZTRIPLE1 {
  CIEXYZ1  ciexyzRed; 
  CIEXYZ1  ciexyzGreen; 
  CIEXYZ1  ciexyzBlue; 
} CIEXYZTRIPLE1;

typedef struct
{
	unsigned int headerSize;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bitsPerPixel;
	unsigned int compression;
	unsigned int imageSize;
	unsigned int xPixelsPerMeter;
	unsigned int yPixelsPerMeter;
	unsigned int colorsInColorTable;
	unsigned int importantColorCount;
	unsigned int bV4RedMask;
  	unsigned int bV4GreenMask;
  	unsigned int bV4BlueMask;
  	unsigned int bV4AlphaMask;
  	unsigned int bV4CSType;
  	CIEXYZTRIPLE1 bV4Endpoints;
  	unsigned int bV4GammaRed;
	unsigned int bV4GammaGreen;
  	unsigned int bV4GammaBlue;
	unsigned int bV5Intent; 
  	unsigned int bV5ProfileData; 
  	unsigned int bV5ProfileSize; 
  	unsigned int bV5Reserved; 
} BitmapInfoHeaderV5;

typedef struct
{
	unsigned int headerSize;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bitsPerPixel;
	unsigned int compression;
	unsigned int imageSize;
	unsigned int xPixelsPerMeter;
	unsigned int yPixelsPerMeter;
	unsigned int colorsInColorTable;
	unsigned int importantColorCount;
	unsigned int bV4RedMask;
  	unsigned int bV4GreenMask;
  	unsigned int bV4BlueMask;
  	unsigned int bV4AlphaMask;
  	unsigned int bV4CSType;
  	CIEXYZTRIPLE1  bV4Endpoints;
  	unsigned int bV4GammaRed;
	unsigned int bV4GammaGreen;
  	unsigned int bV4GammaBlue;
} BitmapInfoHeaderV4;

typedef struct
{
	unsigned int headerSize;
	unsigned short width;
	unsigned short height;
	unsigned short planes;
	unsigned short bitsPerPixel;
} BitmapInfoHeaderCORE;

typedef struct
{
	unsigned int headerSize;
	unsigned int width;
	unsigned int height;
	unsigned short planes;
	unsigned short bitsPerPixel;
	unsigned int compression;
	unsigned int imageSize;
	unsigned int xPixelsPerMeter;
	unsigned int yPixelsPerMeter;
	unsigned int colorsInColorTable;
	unsigned int importantColorCount;
} BitmapInfoHeader;

typedef struct
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
} Rgb;

typedef struct{
	int h;
	int w;
} coordinates;

#pragma pack(pop)

void printHelp(){
	printf("This is help:\n");
	printf("-c --change <color> <color> - change color\n");
	printf("-d --draw <kind> <color> <size> - draw frame\n");
	printf("-r --replicate <left up x> <left up y> <right down x>...\n...<right down y> <place x> <place y> - replicate part\n");
	printf("-s --six <way> <left up x/center x> <left up y/center y>...\n...<left down x/radius> <left down y?> <size> <color> <fill?> <colorfill> - draw hexagon\n");
	printf("-p --place <place> - change place for save\n");
	printf("-a --about - print info about file\n");
	printf("-h --help - print help\n");
	printf("color: red/white/green/blue. kind: 1/2/3. size: big/small...\n...way: square/circle. fill: yes/no\n");
}


void changepixel(unsigned char* a, unsigned char color){
	*a = color;
}
void changecolor(Rgb** arr, int H, int W, Rgb colorforchange, Rgb colorchange){
	for (int i = 0; i < H; i++){
		for (int j = 0; j < W; j++){
			if (arr[i][j].b==colorforchange.b && arr[i][j].g==colorforchange.g && arr[i][j].r==colorforchange.r){
				//changepixel(&(arr[i][j].r), colorchange.r);
				//changepixel(&(arr[i][j].g), colorchange.g);
				//changepixel(&(arr[i][j].b), colorchange.b);
				arr[i][j] = colorchange;
			}
		}
	}
}

void diagonal(Rgb** arr, Rgb colorhex, int xu1, int xm1, int yu1, int ym1){
	int dx = abs(xu1 - xm1);
    int dy = abs(yu1 - ym1);
    int signX = ((xm1 < xu1) ? 1 : -1);
    int signY = ((ym1 < yu1) ? 1 : -1);

    int error = dx - dy;

	arr[yu1][xu1]=colorhex;
    while (xm1 != xu1 || ym1 != yu1)
    {
		arr[ym1][xm1]=colorhex;
    	int error2 = error * 2;
        if (error2 > -dy){
        	error -= dy;
        	xm1 += signX;
    	}
        if (error2 < dx){
            error += dx;
            ym1 += signY;
        }
    }
}

void drawframe(Rgb** arr, int H, int W, int kind, Rgb color, int thickness){
	// first kind
	if (kind == 1){
		for (int i = 0; i < thickness; i++){
			for (int j = 0; j < W; j++){
				arr[i][j] = color;
			}
		}
		for (int i = H-thickness; i < H; i++){
			for (int j = 0; j < W; j++){
				arr[i][j] = color;
			}
		}
		for (int i = 0; i < H; i++){
			for (int j = 5; j < 5+thickness; j++){
				arr[i][j] = color;
			}
		}
		for (int i = 0; i < H; i++){
			for (int j = W-5-thickness; j < W-5; j++){
				arr[i][j] = color;
			}
		}
	}
	//second kind
	if (kind == 2){
		if (thickness == 10){
			//left
			for (int i = 0; i < H; i = i+5){
				diagonal(arr, color, 0, floor(0.02*W), i, 0);
			}
			//right
			for (int i = 0; i < H; i = i+5){
				diagonal(arr, color, W-1, floor(0.98*W), i, 0);
			}
			//up
			for (int i = 0; i < W; i = i+5){
				diagonal(arr, color, i, 0, H-1, floor(0.98*H));
			}
			//down
			for (int i = 0; i < W; i = i+5){
				diagonal(arr, color, i, 0, 0, floor(0.02*H));
			}
		}
		if (thickness == 20){
			//left
			for (int i = 0; i < H; i = i+5){
				diagonal(arr, color, 0, floor(0.04*W), i, 0);
			}
			//right
			for (int i = 0; i < H; i = i+5){
				diagonal(arr, color, W-1, floor(0.96*W), i, 0);
			}
			//up
			for (int i = 0; i < W; i = i+5){
				diagonal(arr, color, i, 0, H-1, floor(0.96*H));
			}
			//down
			for (int i = 0; i < W; i = i+5){
				diagonal(arr, color, i, 0, 0, floor(0.04*H));
			}
		}
	}
	//third kind
	if (kind == 3){
		if (thickness == 10){
			//leftdown
			for (int i = 0; i < floor(H/8); i = i + 5){
				diagonal(arr, color, 0, floor(W/2), i, 0);
			}
			//leftup
			for (int i = H-1; i > floor(H-H/8); i = i - 5){
				diagonal(arr, color, 0, floor(W/2), i, H-1);
			}
			//rightup
			for (int i = H-1; i > floor(H-H/8); i = i - 5){
				diagonal(arr, color, W-1, floor(W/2), i, H-1);
			}
			//rightdown
			for (int i = 0; i < floor(H/8); i = i + 5){
				diagonal(arr, color, W-1, floor(W/2), i, 0);
			}
			//leftup
			for (int i = 0; i < floor(W/10); i = i + 5){
				diagonal(arr, color, i, 0, H-1, floor(H/2));
			}
			//rightup
			for (int i = floor(W-W/10); i < W; i = i + 5){
				diagonal(arr, color, i, W-1, H-1, floor(H/2));
			}
			//leftdown
			for (int i = 0; i < floor(W/10); i = i + 5){
				diagonal(arr, color, i, 0, 0, floor(H/2));
			}
			//rightdown
			for (int i = floor(W-W/10); i < W; i = i + 5){
				diagonal(arr, color, i, W-1, 0, floor(H/2));
			}
		}
		if (thickness == 20){
			//leftdown
			for (int i = 0; i < floor(H/7); i = i + 4){
				diagonal(arr, color, 0, floor(W/2), i, 0);
			}
			//leftup
			for (int i = H-1; i > floor(H-H/7); i = i - 4){
				diagonal(arr, color, 0, floor(W/2), i, H-1);
			}
			//rightup
			for (int i = H-1; i > floor(H-H/7); i = i - 4){
				diagonal(arr, color, W-1, floor(W/2), i, H-1);
			}
			//rightdown
			for (int i = 0; i < floor(H/7); i = i + 4){
				diagonal(arr, color, W-1, floor(W/2), i, 0);
			}
			//leftup
			for (int i = 0; i < floor(W/10); i = i + 4){
				diagonal(arr, color, i, 0, H-1, floor(H/2));
			}
			//rightup
			for (int i = floor(W-W/10); i < W; i = i + 4){
				diagonal(arr, color, i, W-1, H-1, floor(H/2));
			}
			//leftdown
			for (int i = 0; i < floor(W/10); i = i + 4){
				diagonal(arr, color, i, 0, 0, floor(H/2));
			}
			//rightdown
			for (int i = floor(W-W/10); i < W; i = i + 4){
				diagonal(arr, color, i, W-1, 0, floor(H/2));
			}
		}
	}

}

void copy(Rgb** arr, int H, int W, coordinates partleft, coordinates partright, coordinates placeforcopy){
	Rgb** site = malloc(((-partleft.h+partright.h)+1)*sizeof(Rgb*));
	for (int i = 0; i < (partright.h-partleft.h+1); i++){
		site[i] = malloc((partright.w-partleft.w+1) * sizeof(Rgb) + ((partright.w-partleft.w+1)*3)%4);
	}
	int i1 = 0;
	int j1 = 0;
	for (int i = (H-partleft.h-1); i > ((H-partright.h-1)-1); i--){
		for (int j = (partleft.w); j < (partright.w+1); j++){
			//changepixel(&(arr[i][j].r), colorc.r);
			//changepixel(&(arr[i][j].g), colorc.g);
			//changepixel(&(arr[i][j].b), colorc.b);
			site[i1][j1]=arr[i][j];
			j1++;
		}
		j1=0;
		i1++;
	}
	i1 = 0;
	j1 = 0;
	for (int i = (H-placeforcopy.h-1); i > ((H-placeforcopy.h-1-(partright.h-partleft.h+1))) & i > -1; i--){
		for (int j = placeforcopy.w; j < (placeforcopy.w+(partright.w-partleft.w+1)) && j < W; j++){
			arr[i][j]=site[i1][j1];
			j1++;
		}
		j1=0;
		i1++;
	}
	for (int i = 0; i < (partright.h-partleft.h+1); i++){
		free(site[i]);
	}
	free(site);
}

void fillBackground(Rgb** arr, Rgb colorhexfill, Rgb colorhex, int x, int y){
	if ((arr[y][x].r == colorhex.r && arr[y][x].g == colorhex.g && arr[y][x].b == colorhex.b) || (arr[y][x].r == colorhexfill.r 
		&& arr[y][x].g == colorhexfill.g && arr[y][x].b == colorhexfill.b)){
	}
	else{
		arr[y][x] = colorhexfill;
		fillBackground(arr, colorhexfill, colorhex, x+1, y);
		fillBackground(arr, colorhexfill, colorhex, x-1, y);
		fillBackground(arr, colorhexfill, colorhex, x, y+1);
		fillBackground(arr, colorhexfill, colorhex, x, y-1);
	}
}

void hexagon(Rgb** arr, int H, int W, coordinates partlefthex, coordinates partrighthex, int signal, int thickness, Rgb colorhex, int fill, Rgb colorhexfill){
	float radius;
	coordinates center;
	int x;
	int y;
	if (signal == 1){
		radius = (partrighthex.h - partlefthex.h)/2;
		center.h = floor((partlefthex.h + partrighthex.h)/2);
		center.w = floor((partlefthex.w + partrighthex.w)/2);
		x = center.w;
		y = center.h;
	}
	else {
		center = partlefthex;
		x = center.w;
		y = center.h;
		radius = partrighthex.h;
	}
	//printf("%d %d %f\n", x, y, radius);
	int xu1 = floor(x - (0.5*radius));
	int yu1 = H-floor(y + (radius*sin(-M_PI/3)))-1;
	int xu2 = ceil(x + (0.5*radius));
	int yu2 = yu1;

	int xm1 = floor(x - radius);
	int ym1 = H-y-1;
	int xm2 = ceil(x + radius);
	int ym2 = ym1;

	int xd1 = xu1;
	int yd1 = H-floor(y + (radius*sin(M_PI/3)))-1;
	int xd2 = xu2;
	int yd2 = yd1;
	
	//printf("%d %d\n", xu1, yu1);
	//printf("%d %d\n", xu2, yu2);
	//printf("%d %d\n", xm1, ym1);
	//printf("%d %d\n", xm2, ym2);
	//printf("%d %d\n", xd1, yd1);
	//printf("%d %d\n", xd2, yd2);


	//upper
	for (int i = yu1; i > (yu1-1-thickness); i--){
		for (int j = xu1; j < xu2+1; j++){
			changepixel(&(arr[i][j].r), colorhex.r);
			changepixel(&(arr[i][j].g), colorhex.g);
			changepixel(&(arr[i][j].b), colorhex.b);
		}
	}
	//down
	for (int i = yd1; i < (yd1+1+thickness); i++){
		for (int j = xd1; j < xd2+1; j++){
			changepixel(&(arr[i][j].r), colorhex.r);
			changepixel(&(arr[i][j].g), colorhex.g);
			changepixel(&(arr[i][j].b), colorhex.b);
		}
	}
	//leftupper
	diagonal(arr, colorhex, xu1, xm1, yu1, ym1);
	//leftdown
	diagonal(arr, colorhex, xm1, xd1, ym1, yd1);
	//rightupper
	diagonal(arr, colorhex, xu2, xm2, yu2, ym2);
	//rightdown
	diagonal(arr, colorhex, xm2, xd2, ym2, yd2);
	if (thickness != 0){
		int i = 1;
		while(i != thickness+1){
		//leftupper
		diagonal(arr, colorhex, xu1+i, xm1+i, yu1, ym1);
		//leftdown
		diagonal(arr, colorhex, xm1+i, xd1+i, ym1, yd1);
		//rightupper
		diagonal(arr, colorhex, xu2-i, xm2-i, yu2, ym2);
		//rightdown
		diagonal(arr, colorhex, xm2-i, xd2-i, ym2, yd2);
		i++;
		}
	}
	if (fill == 1){
		if (arr[H-y-1][x].r == colorhexfill.r && arr[H-y-1][x].g == colorhexfill.g && arr[H-y-1][x].b == colorhexfill.b){
			arr[H-y-1][x].r = 5;
		}
		fillBackground(arr, colorhexfill, colorhex, x, H-y-1);
	}
}

int main(int argc, char* argv[]){
	//change color
	Rgb colorforchange;
	Rgb colorchange;
	//changecolor(arr, H, W, colorforchange, colorchange);

	//draw frame
	int kind;
	Rgb color;
	int thick;
	//drawframe(arr, H, W, kind, color, thick);

	//copy
	coordinates partleft;
	//partleft.h = 50;
	//partleft.w = 100;
	coordinates partright;
	//partright.h = 70;
	//partright.w = 150;
	coordinates placeforcopy;
	//placeforcopy.h = 265;
	//placeforcopy.w = 420;
	Rgb colorc = RED;
	//copy(arr, H, W, partleft, partright, placeforcopy);

	//place
	char* place;
	int check = 0;

	char* opts = "c:d:r:p:s:ah?";
	struct option longOpts[]={
		{"change", 1, NULL, 'c'},
		{"help", 0, NULL, 'h'},
		{"draw", 1, NULL, 'd'},
		{"replicate", 1, NULL, 'r'},
		{"place", 1, NULL, 'p'},
		{"six", 1, NULL, 's'},
		{"about", 0, NULL, 'a'},
		{NULL, 0, NULL, 0},
	};
	int opt;
	int longIndex;
	opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
	char* placed;
	
	placed = malloc(strlen(argv[1])*sizeof(char));
	placed = argv[1];
	if (strstr(placed, "bmp")==NULL){
		printf("It's not BMP file.\n");
		return 0;
	}
	
	FILE *f = fopen(placed, "rb");
	BitmapFileHeader bmfh;
	BitmapInfoHeader bmif;
	fread(&bmfh,1,sizeof(BitmapFileHeader),f);
	fread(&bmif,1,sizeof(BitmapInfoHeader),f);
	
	FILE *f1;
	BitmapFileHeader bmfh5;
	BitmapInfoHeaderV5 bmif5;
	if (bmif.headerSize == 0x7c){
		fclose(f);
		f1 = fopen(placed, "rb");
		fread(&bmfh5,1,sizeof(BitmapFileHeader),f1);
		fread(&bmif5,1,sizeof(BitmapInfoHeaderV5),f1);
	}
	BitmapFileHeader bmfh4;
	BitmapInfoHeaderV4 bmif4;
	if (bmif.headerSize == 0x6c){
		fclose(f);
		f1 = fopen(placed, "rb");
		fread(&bmfh4,1,sizeof(BitmapFileHeader),f1);
		fread(&bmif4,1,sizeof(BitmapInfoHeaderV4),f1);
	}
	BitmapFileHeader bmfhc;
	BitmapInfoHeaderCORE bmifc;
	if (bmif.headerSize == 0xc){
		fclose(f);
		f1 = fopen(placed, "rb");
		fread(&bmfhc,1,sizeof(BitmapFileHeader),f1);
		fread(&bmfhc,1,sizeof(BitmapInfoHeaderCORE),f1);
	}

	if (bmfh.signature != 0x4D42 && bmfh.signature != 0x424D){
		printf("It's not BMP file.\n");
		return 0;
	}

	unsigned int H = bmif.height;
	unsigned int W = bmif.width;

	Rgb **arr = malloc(H*sizeof(Rgb*));
	for(int i=0; i<H; i++){
		arr[i] = malloc(W * sizeof(Rgb) + (W*3)%4);
		fread(arr[i],1,W * sizeof(Rgb) + (W*3)%4,f);
	}
	

	//change one test pixel
	//Rgb color1 = RED;
	//changepixel(&(arr[10][10].r), color1.r);
	//changepixel(&(arr[10][10].g), color1.g);
	//changepixel(&(arr[10][10].b), color1.b);
    
	//hexagon
	coordinates partlefthex;
	//partlefthex.h = 50;
	//partlefthex.w = 100;
	coordinates partrighthex;
	//partrighthex.h = 110;
	//partrighthex.w = 160;

	int signal;
	int thickness;
	Rgb colorhex;
	Rgb colorhexfill;
	int fill;
	//hexagon(arr, H, W, partlefthex, partrighthex, signal, thickness, colorhex, fill);
	
	while (opt!=-1){
		switch(opt){
			case 'c':
				if (strcmp(optarg, "red")==0) { colorforchange.r = 255; colorforchange.g = 0; colorforchange.b = 0; }
				else if (strcmp(optarg, "white")==0) { colorforchange.r = 255; colorforchange.g = 255; colorforchange.b = 255; }
				else if (strcmp(optarg, "green")==0) { colorforchange.r = 0; colorforchange.g = 255; colorforchange.b = 0; }
				else if (strcmp(optarg, "blue")==0) { colorforchange.r = 0; colorforchange.g = 0; colorforchange.b = 255; }
				else{
					printf("There is no argument.\n");
					printHelp();
					return 0;
				}
				if (optind < argc && argv[optind][0] != '-') {
					if (strcmp(argv[optind], "red")==0) { colorchange.r = 255; colorchange.g = 0; colorchange.b = 0; }
					else if (strcmp(argv[optind], "white")==0) { colorchange.r = 255; colorchange.g = 255; colorchange.b = 255; }
					else if (strcmp(argv[optind], "green")==0) { colorchange.r = 0; colorchange.g = 255; colorchange.b = 0; }
					else if (strcmp(argv[optind], "blue")==0) { colorchange.r = 0; colorchange.g = 0; colorchange.b = 255; }
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
       				optind++;
    			}
				else{
					printf("There is no argument.\n");
					printHelp();
					return 0;
				}
				changecolor(arr, H, W, colorforchange, colorchange);
				break;
			case 'd':
				if (strcmp(optarg, "1")==0) { kind = 1;}
				else if (strcmp(optarg, "2")==0) { kind = 2;}
				else if (strcmp(optarg, "3")==0) { kind = 3;}
				else{
					printf("There is no argument.\n");
					printHelp();
					return 0;
				}
				if (optind < argc && argv[optind][0] != '-') {
					if (strcmp(argv[optind], "red")==0) { color.r = 255; color.g = 0; color.b = 0; }
					else if (strcmp(argv[optind], "white")==0) { color.r = 255; color.g = 255; color.b = 255; }
					else if (strcmp(argv[optind], "green")==0) { color.r = 0; color.g = 255; color.b = 0; }
					else if (strcmp(argv[optind], "blue")==0) { color.r = 0; color.g = 0; color.b =255; }
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
       				optind++;
    			}
				if (optind < argc && argv[optind][0] != '-') {
					if (strcmp(argv[optind], "small")==0) thick = 10;
					else if (strcmp(argv[optind], "big")==0) thick = 20;
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
       				optind++;
    			}
				drawframe(arr, H, W, kind, color, thick);
				break;
			case 'r':
					partleft.w = atoi(optarg);
					if (optind < argc && argv[optind][0] != '-') {
						partleft.h = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						partright.w = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						partright.h = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (partright.h < partleft.h || partright.w < partleft.w){
						printf("It's incorrect coordinates.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						placeforcopy.w = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						placeforcopy.h = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
				copy(arr, H, W, partleft, partright, placeforcopy);
				break;
			case 's':
				if (strcmp(optarg, "square")==0){
					signal = 1;
					if (optind < argc && argv[optind][0] != '-') {
						partlefthex.w = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						partlefthex.h = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						partrighthex.w = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						partrighthex.h = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (partrighthex.h < partlefthex.h || partrighthex.w < partlefthex.w){
						printf("It's incorrect coordinates.\n");
						printHelp();
						return 0;
					}
				}
				else if (strcmp(optarg, "circle")==0){
					signal = 0;
					if (optind < argc && argv[optind][0] != '-') {
						partlefthex.w = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						partlefthex.h = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
					if (optind < argc && argv[optind][0] != '-') {
						partrighthex.h = atoi(argv[optind]);
						optind++;
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
				}
				else{
					printf("There is no argument.\n");
					printHelp();
					return 0;
				}
				if (optind < argc && argv[optind][0] != '-') {
					if (strcmp(argv[optind], "small")==0) thickness = 0;
					else if (strcmp(argv[optind], "big")==0) thickness = 3;
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
       				optind++;
    			}
				if (optind < argc && argv[optind][0] != '-') {
					if (strcmp(argv[optind], "red")==0) { colorhex.r = 255; colorhex.g = 0; colorhex.b = 0; }
					else if (strcmp(argv[optind], "white")==0) { colorhex.r = 255; colorhex.g = 255; colorhex.b = 255; }
					else if (strcmp(argv[optind], "green")==0) { colorhex.r = 0; colorhex.g = 255; colorhex.b = 0; }
					else if (strcmp(argv[optind], "blue")==0) { colorhex.r = 0; colorhex.g = 0; colorhex.b = 255; }
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
       				optind++;
    			}
				if (optind < argc && argv[optind][0] != '-') {
					if (strcmp(argv[optind], "yes")==0) {
						optind++;	
						fill = 1;
						if (strcmp(argv[optind], "red")==0) { colorhexfill.r = 255; colorhexfill.g = 0; colorhexfill.b = 0; }
						else if (strcmp(argv[optind], "white")==0) { colorhexfill.r = 255; colorhexfill.g = 255; colorhexfill.b = 255; }
						else if (strcmp(argv[optind], "green")==0) { colorhexfill.r = 0; colorhexfill.g = 255; colorhexfill.b = 0; }
						else if (strcmp(argv[optind], "blue")==0) { colorhexfill.r = 0; colorhexfill.g = 0; colorhexfill.b = 255; }
						else{
							printf("There is no argument.\n");
							printHelp();
							return 0;
						}
					}
					else if (strcmp(argv[optind], "no")==0){
						fill = 0;
						optind++;	
					}
					else{
						printf("There is no argument.\n");
						printHelp();
						return 0;
					}
    			}
				hexagon(arr, H, W, partlefthex, partrighthex, signal, thickness, colorhex, fill, colorhexfill);
				break;
			case 'p':
				place = malloc(strlen(optarg)*sizeof(char));
				place = optarg;
				check = 1;
				break;
			case 'a':
				printf("Height: %u pixels\n", bmif.height);
				printf("Width: %u pixels\n", bmif.width);
				printf("Size of file: %u bytes\n", bmfh.filesize);
				break;
			case 'h':
			case '?':
				printHelp();
				return 0;
		}
		opt = getopt_long(argc, argv, opts, longOpts, &longIndex);
	}
	
	if (check == 0){
	argc -= optind;
	argv += optind;	
		//for(int i=0; i<argc; i++)
			//printf("\n>>%s",argv[i]);
	place = malloc(strlen(argv[argc-1])*sizeof(char));
	place = argv[argc-1];
	}
	if (strstr(place, "bmp")==NULL){
		printf("Save not in BMP file.\n");
		return 0;
	}
	FILE *ff = fopen(place, "wb");

	bmif.height = H;
	bmif.width = W;
	if (bmif.headerSize == 0x28){
		fwrite(&bmfh, 1, sizeof(BitmapFileHeader),ff);
		fwrite(&bmif, 1, sizeof(BitmapInfoHeader),ff);
	}
	if (bmif.headerSize == 0x6c){
		fwrite(&bmfh4, 1, sizeof(BitmapFileHeader),ff);
		fwrite(&bmif4, 1, sizeof(BitmapInfoHeaderV4),ff);
	}
	if (bmif.headerSize == 0x7c){
		fwrite(&bmfh5, 1, sizeof(BitmapFileHeader),ff);
		fwrite(&bmif5, 1, sizeof(BitmapInfoHeaderV5),ff);
	}
	if (bmif.headerSize == 0xc){
		fwrite(&bmfhc, 1, sizeof(BitmapFileHeader),ff);
		fwrite(&bmifc, 1, sizeof(BitmapInfoHeaderCORE),ff);
	}
	unsigned int w = (W) * sizeof(Rgb) + ((W)*3)%4;
	for(int i=0; i<H; i++){
		fwrite(arr[i],1,w,ff);
	}
	fclose(ff);

	printf("\n");


return 0;
}