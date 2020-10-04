
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include<sys/types.h>

#define DEVICE_PATH "/dev/simple_character_device"
#define BUFFER_SIZE 1024

int main()
{
int bytes;
int offset;
int whence;
char* buffer;

int filedes=open(DEVICE_PATH,O_RDWR);
if(!filedes)
	printf("Error opening file.\n");
char input='a';
while(input!='e')
{
	printf("Press 'r' to read from device\n");
	printf("Press 'w' to write to device\n");
	printf("Press 's' to seek into the device\n");
	printf("Press 'e' to exit from the device\n");
	printf("Enter command: ");
	fgets(&input,10,stdin);

switch(input) {
	case 'r':
		printf("Enter the number of bytes to read: ");
		scanf("%d",&bytes);
		getchar();

		buffer=(char*)malloc(bytes);
		read(filedes,buffer,bytes);
		printf("Data read: %s\n",buffer);
		memset(buffer,'\0',bytes);
		free(buffer);
		break;

	case 'w':
		buffer=(char*)malloc(BUFFER_SIZE);
		printf("Enter data to write to the device: ");
		fgets(buffer,BUFFER_SIZE,stdin);
		write(filedes,buffer,strlen(buffer));
		memset(buffer,'\0',BUFFER_SIZE);
		free(buffer);
		break;

	case 's':
		printf("Enter offset value: ");
		scanf("%d",&offset);
		getchar();
		printf("Enter whence: ");
		scanf("%d",&whence);
		getchar();
		lseek(filedes,offset,whence);
		break;
}

}
close(filedes);

return 0;
}
