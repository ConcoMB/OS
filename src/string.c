#include "../include/string.h"

int strcmp(char* str1, char* str2){
	int ans;
	int i;
	for(i=0; str1[i]||str2[i];i++){
		ans=str1[i]-str2[i];
		if(ans!=0){
			return ans;
		}
	}
	return 0;
}

int substr(char* str1, char* str2){
	int i;
	for(i=0; str1[i];i++){
		if(str1[i]!=str2[i]){
			return 0;
		}
	}
	return 1;
}

char* itoa(int val, char* buffer)
{	
	int pos=0, start;
	char c;
	if(val<0){
		buffer[pos++]='-';
		val=-val;
	}
	start=pos;
	if(val==0){
		buffer[pos++]='0';
	}
	while(val!=0){
		buffer[pos++]=val%10 + '0';
		val/=10;
	}
	buffer[pos--]=0;
	while(pos>start)
	{
		c=buffer[start];
		buffer[start]=buffer[pos];
		buffer[pos]=c;
		pos--;
		start++;
	}
	return buffer;
}

char* xtoa(int mem, char* buffer){
	int i;
	int tmp;
	buffer[0]='0';
	buffer[1]='x';
	for(i=0;i<8;i++){
		tmp=(mem>>(i*4))&0x000F;
		if(tmp<10){
			tmp+='0';
		}
		else{
			tmp+=-10+'A';
		}
		/*LITTLE ENDIAN*/
		buffer[7-i+2]=(char)tmp;
	}
	buffer[10]=0;
	return buffer;
}

int strlen(char* s){
	int count=0;
	int i;
	for(i=0;s[i];i++){
		count++;
	}
	return count;
}

void strcpy(char* dest, char* src){
	int i;
	for(i=0;src[i];i++){
		dest[i]=src[i];
	}
	dest[i]=0;
}

int atoi(char* a){
	int i;
	int number=0;
	int len=strlen(a);
	for(i=0;a[i];i++){
		number+=(a[i]-'0')*pow(10,len-i-1);
	}
	return number;
}


