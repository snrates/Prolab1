#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct Dot{
	float x;
	float y;
	float z;
	int n;
};
struct DotBinaryRgb{
	float x,y,z;
	int r,g,b;
};
struct File{
	FILE *readFile;
	FILE *outFile;
	char readFileName[50];//  okunacak DOSYALAR�n adlar�
	char outFileName[50]; // ��kt� dosyalar�n�n adlar�
	int version;// dosyalar�n version tipleri
	bool type;// dosyalarda verilen alanlar tipi true ise xyz false ise xyzrgb
	float distance;//noktalar aras� uzakl�k ortalamas�
	int dotSize;//Dosyalarda verilen nokta say�s�
	bool dataType;//Dosyalarda verilen data tipi true ise asci false ise binary
	bool flag;//Dosyalar�n verilen formata uyum kontrol�
	struct File * next;	//ba�l� listedeki bir sonraki file i�aret eder;

};
struct File* headFile=NULL;
struct File* tempFile=NULL;
struct Dot near1;
struct Dot near2;
struct Dot remote1;
struct Dot remote2;
bool forced=false;
void Menu();
void FindFiles();
void AddFile(char fileName[50]);
void FilesOpen();
void FilesClose();
void Control(struct File *files);
void NearAndRemote(struct File *files);
void Cube(struct File *files);
void Sphere(struct File *files, float x, float y, float z, float r);
void DotDistance(struct File *files);

void getWord(int target, char buffer[200] , char word[25]);
int getWordSize(char buffer[200]);
void getDots(struct File *files, float dots[files->dotSize][3]);

void Menu(){
	int parametre;
	printf("Bir parametre seciniz..\n1-) Dosya Kontrolu \n2-) En Yakin/Uzak Noktalar\n3-) Kup\n4-) Kure\n5-) Nokta Uzakliklari \nSecin=> ");
	scanf("%d",&parametre);
	switch(parametre){
		case 1:{
			forced=true;
			FilesOpen();
			tempFile=headFile;
			while(tempFile->next!=NULL){
				fprintf(tempFile->outFile,"SECIM 1\n");
				Control(tempFile);
				tempFile=tempFile->next;
			}
			fprintf(tempFile->outFile,"SECIM 1\n");
			Control(tempFile);
			FilesClose();
			Menu();
		} break;
		case 2:{
			if(forced){
				FilesOpen();
				tempFile=headFile;
				while(tempFile->next!=NULL){
					if(tempFile->flag){
						fprintf(tempFile->outFile,"SECIM 2\n");
						NearAndRemote(tempFile);
					}
					tempFile=tempFile->next;
				}
				if(tempFile->flag){
					fprintf(tempFile->outFile,"SECIM 2\n");
					NearAndRemote(tempFile);
				}
				FilesClose();
			}else{
				printf("Lutfen once dosya kontolu yapiniz\n");
			}
			Menu();
		}break;
		case 3:{
			if(forced){
				FilesOpen();
				tempFile=headFile;
				while(tempFile->next!=NULL){
					if(tempFile->flag){
					   fprintf(tempFile->outFile,"SECIM 3\n");
					   Cube(tempFile);
					}
					tempFile=tempFile->next;
				}
				if(tempFile->flag){
					fprintf(tempFile->outFile,"SECIM 3\n");
					Cube(tempFile);
				}
				FilesClose();
			}else{
				printf("Lutfen once dosya kontolu yapiniz\n");
			}
			Menu();
		} break;
		case 4:{
			if(forced){
				FilesOpen();
				float x,y,z,r;
				printf("Kurenin  x degerini giriniz\n");
				scanf("%f",&x);
				printf("Kurenin  y degerini giriniz\n");
				scanf("%f",&y);
				printf("Kurenin  z degerini giriniz\n");
				scanf("%f",&z);
				printf("Kurenin  yaricapini giriniz\n");
				scanf("%f",&r);
				tempFile=headFile;
				while(tempFile->next!=NULL){

					if(tempFile->flag){
						fprintf(tempFile->outFile,"SECIM 4\n");
						fprintf(tempFile->outFile, "cx= %f\ncy= %f\ncz= %f\ncr= %f\n",x,y,z,r);
						Sphere(tempFile,x,y,z,r);
					}
					tempFile=tempFile->next;
				}
				if(tempFile->flag){
					fprintf(tempFile->outFile,"SECIM 4\n");
					fprintf(tempFile->outFile, "cx= %f\ncy= %f\ncz= %f\ncr= %f\n",x,y,z,r);
					Sphere(tempFile,x,y,z,r);
				}
				FilesClose();
			}else{
				printf("Lutfen once dosya kontolu yapiniz\n");
			}
			Menu();
		} break;
		case 5:{
			if(forced){
				FilesOpen();
				tempFile=headFile;
				while(tempFile->next!=NULL){

					if(tempFile->flag){
						fprintf(tempFile->outFile,"SECIM 5\n");
						DotDistance(tempFile);
					}
					tempFile=tempFile->next;
				}
				if(tempFile->flag){

					fprintf(tempFile->outFile,"SECIM 5\n");
					DotDistance(tempFile);
				}
				FilesClose();
				printf("Butun islemler tamamlandi output dosyalarini kontrol edebilirsin....");
				exit(1);
			}else{
				printf("Lutfen once dosya kontolu yapiniz\n");
				Menu();
			}

		} break;
		default: {
			printf("!!! Lutfen Gecerli Bir Deger Giriniz !!!\n ");
			Menu();
		}
	}
}
void FindFiles(){
	struct dirent *de;
    DIR *dir = opendir(".");
    while ((de = readdir(dir)) != NULL){
    	int i =0;
    	char nkt[5];
		for(i=0;i<4;i++)
			nkt[i]=de->d_name[strlen(de->d_name)-(4-i)]; // bunun ile sadece  nkt uzant�s�n� alabiliriz
        nkt[4]='\0';
		if(!strcmp(nkt,".nkt")){
			AddFile(de->d_name);
		}
	}
    closedir(dir);
}
void AddFile(char fileName[50]){
	  char outName[50]="outs/output";
	  char readName[50]="";
	  strcat(outName,fileName);
	  strcat(readName,fileName);

	  struct File* newNode = (struct File*)malloc(sizeof(struct File));
	  newNode->readFile=NULL;
	  newNode->outFile=NULL;
	  strcpy(newNode->readFileName,fileName);
	  strcpy(newNode->outFileName,outName);
	  newNode->version=0;
	  newNode->type=true;
	  newNode->distance=0;
	  newNode->dotSize=0;
	  newNode->dataType=true;
	  newNode->flag=true;
	  newNode->next=NULL;

	  if(headFile==NULL) headFile=newNode;
	  else{
	  	tempFile=headFile;
	  	while(tempFile->next!=NULL){
	  		tempFile=tempFile->next;
		}
	  	tempFile->next=newNode;
	  }
}
void FilesOpen(){
	tempFile=headFile;
	while(tempFile->next!=NULL){
		tempFile->readFile=fopen(tempFile->readFileName,"r");
		tempFile->outFile=fopen(tempFile->outFileName,"a");
		tempFile=tempFile->next;
	}
	tempFile->readFile=fopen(tempFile->readFileName,"r");
	tempFile->outFile=fopen(tempFile->outFileName,"a");
}
void FilesClose(){
	tempFile=headFile;
	while(tempFile->next!=NULL){
		fclose(tempFile->readFile);
		fclose(tempFile->outFile);
		tempFile=tempFile->next;
	}
	fclose(tempFile->readFile);
	fclose(tempFile->outFile);
}
void Control(struct File *files){
	char buffer[200];
	int line=0;
	bool fileFormat=true;
	while(fgets(buffer,200,files->readFile) != NULL){
		line++;
	    if(line==2){
			char version[25];
			getWord(2,buffer,version);
			files->version=atoi(version);
			if(files->version!=1){
				fprintf(files->outFile,"Verilen versiyon numarasi hatalidir\n");
				files->flag=false;
			}
		}
		else if(line==3){
			 char a[25],b[25],c[25];
			 char free[25];
			 if(getWordSize(buffer)==4){
				 if(strcmp(buffer,"ALANLAR x y z\n")!=0){
				  	fprintf(files->outFile,"Boyle bir dosya format� yok\n");
				 	files->flag=false;
					fileFormat=false;
					break;
				}
			 }
			 else if(getWordSize(buffer)==7){
				files->type=false;
			 }
			 else{
			 	fprintf(files->outFile,"Boyle bir dosya format� yok\n");
			 	files->flag=false;
				fileFormat=false;
				break;
			 }
		}
		else if(line==4){
			char dotSize[25];
			getWord(2,buffer,dotSize);
			files->dotSize=atoi(dotSize);
		}
		else if(line==5){
			char dataType[25];
			char free[25];
			sscanf(buffer,"%s %s",&free,&dataType);
			if(strcmp(dataType,"ascii")!=0)	files->dataType=false;
			if(files->dataType==false){
				struct DotBinaryRgb dotBinaryRgb;
				while (1)   {
					size_t number = fread(&dotBinaryRgb, sizeof(struct DotBinaryRgb), 1, files->readFile);
					if (number < 1)  break;
					if(files->type){
						//printf("%d . nokta %f %f %f\n", line-4,dotBinaryRgb.x,dotBinaryRgb.y,dotBinaryRgb.z);
						line++;
					}
					else{
						if((dotBinaryRgb.r<0||dotBinaryRgb.r>255)||(dotBinaryRgb.g<0||dotBinaryRgb.g>255)||(dotBinaryRgb.b<0||dotBinaryRgb.b>255)){
			    			 fprintf(files->outFile,"%d. satir rgb araligi yanlistir\n",line-5);
			    			 files->flag=false;
						}
						line++;
					}
				}
			}
		}
		else if(line>5 && files->dataType){
			if(files->type){
			   if(getWordSize(buffer)!=3){
			    	fprintf(files->outFile,"%d. satir xyz formatina uygun degildir\n",line-5);
			   		files->flag=false;
		     	}
			}
			else{
			  if(getWordSize(buffer)!=6){
				  fprintf(files->outFile,"%d. satir xyzrgb formatina uygun degildir\n",line-5);
				  files->flag=false;
		    	}else{
		    		float x,y,z; int r,g,b;
		    		sscanf(buffer,"%f %f %f %d %d %d",&x,&y,&z,&r,&g,&b);
		    		if((r<0||r>255)||(g<0||g>255)||(b<0||b>255)){
		    			 fprintf(files->outFile,"%d. satir rgb araligi yanlistir\n",line-5);
		    			 files->flag=false;
					}
				}
	    	}
	}
}

	if(files->dotSize!=line-5 && fileFormat){
		fprintf(files->outFile,"Formatta verilen nokta sayisi (%d) dosyada verilen nokta sayisi (%d) ile esit degildir.\n",files->dotSize,line-5);
		files->flag=false;
	}
	if(files->flag) fprintf(files->outFile,"Dosya uygun formatta yazilmistir\n");
	else fprintf(files->outFile,"Dosya uygun formatta yazilmamistir\n");
}
void NearAndRemote(struct File *files){
	float dots[files->dotSize][3];
	  getDots(files,dots);
	  int i,j;
	  bool flag=true;
	  float nearNorm=0,remoteNorm=0;
	  double normTotal=0;
	  for(i=0;i<files->dotSize-1;i++){
		float x1=dots[i][0],y1=dots[i][1],z1=dots[i][2];
		 for(j=i+1;j<files->dotSize;j++){
			float x2=dots[j][0],y2=dots[j][1],z2=dots[j][2];
			float norm=sqrt(pow((x2-x1),2)+pow((y2-y1),2)+pow((z2-z1),2));
			normTotal+=norm;
			if(flag){
				nearNorm=norm;
				near1.x=x1; near1.y=y1; near1.z=z1; near1.n=i+1;
				near2.x=x2; near2.y=y2; near2.z=z2; near2.n=j+1;
				flag=false;
			}
			if(norm<nearNorm){
				nearNorm=norm;
				near1.x=x1; near1.y=y1; near1.z=z1; near1.n=i+1;
				near2.x=x2; near2.y=y2; near2.z=z2; near2.n=j+1;
			}
			else if(norm>remoteNorm){
		    	remoteNorm=norm;
				remote1.x=x1; remote1.y=y1; remote1.z=z1; remote1.n=i+1;
				remote2.x=x2; remote2.y=y2; remote2.z=z2; remote2.n=j+1;
			}
		  }
	   }
	   files->distance=(normTotal/(((double)files->dotSize*((double)files->dotSize-1))/2.0)) ;
	   if(files->dataType){
	   	   fprintf(files->outFile,"en yakin noktalar\n%d. nokta x=%f y=%f z=%f\n",near1.n,near1.x,near1.y,near1.z);
	 	   fprintf(files->outFile,"%d. nokta x=%f y=%f z=%f\n", near2.n,near2.x,near2.y,near2.z);
	       fprintf(files->outFile,"en uzak noktalar\n%d. nokta x=%f y=%f z=%f\n",remote1.n,remote1.x,remote1.y,remote1.z);
	 	   fprintf(files->outFile,"%d. nokta x=%f y=%f z=%f\n",remote2.n,remote2.x,remote2.y,remote2.z);
	   }else{
	   		fwrite(&near1, sizeof(struct Dot), 1, files->outFile);
	   		fwrite(&near2, sizeof(struct Dot), 1, files->outFile);
	   		fwrite(&remote1, sizeof(struct Dot), 1, files->outFile);
	   		fwrite(&remote2, sizeof(struct Dot), 1, files->outFile);
	   		fprintf(files->outFile,"\n");
	   }




}
void Cube(struct File *files){
	float dots[files->dotSize][3];
	  getDots(files,dots);
	  float min,max=0;
   	  int i ,j;
	  min = dots[0][0];
	  for(i=0;i<files->dotSize;i++){
		 for(j=0;j<3;j++){
			if(dots[i][j]<min){
		 		min=dots[i][j];
			}
			if(dots[i][j]>max){
				max=dots[i][j];
		    }
		}
	}
		if(files->dataType){
		 fprintf(files->outFile,"1.nokta= x=%f y=%f z=%f\n",min,min,min);
		 fprintf(files->outFile,"2.nokta= x=%f y=%f z=%f\n",min,min,max);
		 fprintf(files->outFile,"3.nokta= x=%f y=%f z=%f\n",min,max,min);
		 fprintf(files->outFile,"4.nokta= x=%f y=%f z=%f\n",min,max,max);
		 fprintf(files->outFile,"5.nokta= x=%f y=%f z=%f\n",max,min,min);
		 fprintf(files->outFile,"6.nokta= x=%f y=%f z=%f\n",max,min,max);
		 fprintf(files->outFile,"7.nokta= x=%f y=%f z=%f\n",max,max,min);
		 fprintf(files->outFile,"8.nokta= x=%f y=%f z=%f\n",max,max,max);
		}else{
			fwrite(&max, sizeof(float), 1, files->outFile);fwrite(&max, sizeof(float), 1, files->outFile);fwrite(&max, sizeof(float), 1, files->outFile);
			fwrite(&min, sizeof(float), 1, files->outFile);fwrite(&min, sizeof(float), 1, files->outFile);fwrite(&min, sizeof(float), 1, files->outFile);
			fwrite(&min, sizeof(float), 1, files->outFile);fwrite(&max, sizeof(float), 1, files->outFile);fwrite(&max, sizeof(float), 1, files->outFile);
			fwrite(&max, sizeof(float), 1, files->outFile);fwrite(&min, sizeof(float), 1, files->outFile);fwrite(&max, sizeof(float), 1, files->outFile);
			fwrite(&max, sizeof(float), 1, files->outFile);fwrite(&max, sizeof(float), 1, files->outFile);fwrite(&min, sizeof(float), 1, files->outFile);
			fwrite(&max, sizeof(float), 1, files->outFile);fwrite(&min, sizeof(float), 1, files->outFile);fwrite(&min, sizeof(float), 1, files->outFile);
			fwrite(&min, sizeof(float), 1, files->outFile);fwrite(&max, sizeof(float), 1, files->outFile);fwrite(&min, sizeof(float), 1, files->outFile);
			fwrite(&min, sizeof(float), 1, files->outFile);fwrite(&min, sizeof(float), 1, files->outFile);fwrite(&max, sizeof(float), 1, files->outFile);
			fprintf(files->outFile,"\n");
		}



}
void Sphere(struct File *files, float x, float y, float z, float r){
	int i;
	float a;
	float dots[files->dotSize][3];
	bool range;
      getDots(files,dots);
      for(i=0; i<files->dotSize;i++){
      	 range=false;
         a=sqrt(pow(x-dots[i][0],2)+pow(y-dots[i][1],2)+pow(z-dots[i][2],2));
         if(a<=r){
         	range=true;
		 }
        if(range){
			if(files->dataType){
				 fprintf(files->outFile,"Kure icindeki noktalarin bilgileri: x=%f y=%f z=%f\n",dots[i][0],dots[i][1],dots[i][2]);
			}else{
				fwrite(&dots[i][0], sizeof(float), 1, files->outFile);
                fwrite(&dots[i][1], sizeof(float), 1, files->outFile);
                fwrite(&dots[i][2], sizeof(float), 1, files->outFile);
                fprintf(files->outFile,"\n");
			}
        }
     }
    fprintf(files->outFile,"!!!! %s dosyas�na ait bilgiler !!!!\n",files->readFileName);
	fprintf(files->outFile,"Dosya versiyonu=> %d\n",files->version);
	if(files->type) fprintf(files->outFile,"Dosya alanlar� tipi => XYZ formatidir\n");
    else fprintf(files->outFile,"Dosya alanlar� tipi => XYZ RGB formatidir\n");
	fprintf(files->outFile,"Dosyada verilen nokta sayisi=> %d\n",files->dotSize);
	if(files->dataType)fprintf(files->outFile,"Dosya data tipi=> ASCII\n");
	else fprintf(files->outFile,"Dosya data tipi=> BINARY\n");
}
void DotDistance(struct File *files){
	if(files->dataType){
		fprintf(files->outFile,"%s dosyas�nda bulunan noktalar�n birbirine olan uzakl�klar�n�n ortalamas�= %f\n\n",files->readFileName,files->distance);
	}else{
		fwrite(&files->distance,sizeof(float),1,files->outFile);
		fprintf(files->outFile,"\n");
	}
}
void getWord(int target, char buffer[200] , char word[25]){
	char split[1]=" ";
		int location=0;
		char *ptr = strtok(buffer, split);
		while(ptr != NULL){
			location++;
			if(location==target){
				strcpy(word,ptr);
				break;
			}
			ptr = strtok(NULL, split);
		}
}
int getWordSize(char buffer[200]){
	int i=0;
	int wordSize=0;
	while (buffer[i]!='\0') {
		if(buffer[i]==' ') {
			wordSize++;
		}
		i++;
	}
	return wordSize+1;
}
void getDots(struct File *files, float dots[files->dotSize][3]){
	char buffer[200], xString[50],yString[50],zString[50];
	int line=0;
	float tempDots[files->dotSize][3];
	while(fgets(buffer,200,files->readFile)!=NULL){
		line++;
		if(files->dataType&&line>5){
				sscanf(buffer,"%s %s %s",&xString,&yString,&zString);
				tempDots[line-6][0]=atof(xString);
				tempDots[line-6][1]=atof(yString);
				tempDots[line-6][2]=atof(zString);
		}
		else if(files->dataType==false && line==5){
			struct DotBinaryRgb dotBinaryRgb;
			while (1)   {
				size_t number = fread(&dotBinaryRgb, sizeof(struct DotBinaryRgb), 1, files->readFile);
				if (number < 1)  break;
				tempDots[line-5][0]=(dotBinaryRgb.x);
				tempDots[line-5][1]=(dotBinaryRgb.y);
				tempDots[line-5][2]=(dotBinaryRgb.z);
				line++;
			}
		}
	}
	memcpy(dots,tempDots,files->dotSize*3*sizeof(float) );
}
int main(void) {
    mkdir("outs");
	FindFiles();
	if(headFile!=NULL){
		Menu();
	}else{
		printf(".......HICBIR NKT DOSYASI BULUNAMADI......");
	}

}

