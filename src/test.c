
int stackTest(int argc, char** argv){
	printf("Voy %d\n", argc);
	sleep(100);
	stackTest(argc+1,0);
	return 1;
}

int heapTest(int argc, char** argcv){
	int i=1;
	void* mem, *aux;
	while(i<20){
		if(i%3==0){
			mem=malloc(100);
			printf("malloc de 100 dio %d\n", (int)mem);
		}else if(i%3==1){
			mem= malloc(250);
			printf("malloc de 250 dio %d\n", (int)mem);
		}else if(i%3==2){
			mem= malloc(600);
			printf("malloc de 600 dio %d\n", (int)mem);
		}
		if(i%5==0){
			printf("libere %d\n", (int)aux);
			free(aux);
		}
		aux=mem;
		i++;
		sleep(500);
	}
	printHeap();
	return 0;
}

int test(int a, char** v){


	/* la forma de ejecutar es qemu -kernel bin/kernel.bin -hda disk.img */

	char buffer2[10],buffer[10];
	int i;
	for(i=0;i<10;i++){
		buffer2[i]='h';
	}
	printf("buffer tiene eso\n");
	for(i=0 ; i<10 ;i++){
		printf("%c", buffer2[i]);
	}
	printf("\n");

	ata_read(ATA0, buffer2, 10, 1, 0);
	printf("lei esto\n");
	for(i=0 ; i<10 ;i++){
		printf("%c", buffer2[i]);
	}
	printf("\n");

	printf("escribo: llllllllll\n");
	for(i=0 ; i<10 ; i++){
		buffer[i] = 'l';
	}
	ata_write(ATA0, buffer, 10, 1, 0);
	printf("buffer tiene eso\n");
	for(i=0;i<10;i++){
		buffer2[i]='d';
	}
	for(i=0 ; i<10 ;i++){
		printf("%c", buffer2[i]);
	}
	printf("\n");
	ata_read(ATA0, buffer2, 10, 1, 0);
	printf("lei esto\n");
	for(i=0 ; i<10 ;i++){
		printf("%c", buffer2[i]);
	}
	return 0;
}