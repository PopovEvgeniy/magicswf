#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void show_intro();
FILE *open_input_file(const char *name);
FILE *create_output_file(const char *name);
void fast_data_dump(FILE *input,FILE *output,const size_t length);
void data_dump(FILE *input,FILE *output,const size_t length);
unsigned long int get_file_size(FILE *file);
char *get_string_memory(const size_t length);
size_t get_extension_position(const char *source);
char *get_short_name(const char *name);
char* get_name(const char *name,const char *ext);
unsigned long int copy_file(FILE *input,FILE *output);
void check_executable(FILE *input);
void check_signature(FILE *input);
void write_service_information(FILE *output,const unsigned long int length);
void compile_flash(const char *player,const char *flash,const char *result);
void work(const char *player,const char *flash);

int main(int argc, char *argv[])
{
 show_intro();
 if(argc==3)
 {
  puts("Working... Please wait");
  work(argv[1],argv[2]);
  puts("Work finish");
 }
 else
 {
  puts("You must give file name of Flash Player Projector and Flash movie file name as command line argument!");
 }
 return 0;
}

void show_intro()
{
 putchar('\n');
 puts("Magic swf. Version 1.4.7");
 puts("A simple tool for converting Adobe flash movie to self-played movie");
 puts("This sofware made by Popov Evgeniy Alekseyevich,2011-2022 years");
 puts("This software distributed under GNU GENERAL PUBLIC LICENSE");
 putchar('\n');
}

FILE *open_input_file(const char *name)
{
 FILE *target;
 target=fopen(name,"rb");
 if (target==NULL)
 {
  puts("Can't open input file");
  exit(1);
 }
 return target;
}

FILE *create_output_file(const char *name)
{
 FILE *target;
 target=fopen(name,"wb");
 if (target==NULL)
 {
  puts("Can't create ouput file");
  exit(2);
 }
 return target;
}

void data_dump(FILE *input,FILE *output,const size_t length)
{
 unsigned char data;
 size_t index;
 data=0;
 for (index=0;index<length;++index)
 {
  fread(&data,sizeof(unsigned char),1,input);
  fwrite(&data,sizeof(unsigned char),1,output);
 }

}

void fast_data_dump(FILE *input,FILE *output,const size_t length)
{
 unsigned char *buffer=NULL;
 buffer=(unsigned char*)calloc(length,sizeof(unsigned char));
 if (buffer==NULL)
 {
  data_dump(input,output,length);
 }
 else
 {
  fread(buffer,sizeof(unsigned char),length,input);
  fwrite(buffer,sizeof(unsigned char),length,output);
  free(buffer);
 }

}

unsigned long int get_file_size(FILE *file)
{
 unsigned long int length;
 fseek(file,0,SEEK_END);
 length=ftell(file);
 rewind(file);
 return length;
}

char *get_string_memory(const size_t length)
{
 char *memory=NULL;
 memory=(char*)calloc(length+1,sizeof(char));
 if(memory==NULL)
 {
  puts("Can't allocate memory");
  exit(3);
 }
 return memory;
}

size_t get_extension_position(const char *source)
{
 size_t index;
 for(index=strlen(source);index>0;--index)
 {
  if(source[index]=='.')
  {
   break;
  }

 }
 if (index==0) index=strlen(source);
 return index;
}

char *get_short_name(const char *name)
{
 size_t length;
 char *result=NULL;
 length=get_extension_position(name);
 result=get_string_memory(length);
 strncpy(result,name,length);
 return result;
}

char* get_name(const char *name,const char *ext)
{
  char *result=NULL;
  char *output=NULL;
  size_t length;
  output=get_short_name(name);
  length=strlen(output)+strlen(ext);
  result=get_string_memory(length);
  strcpy(result,output);
  free(output);
  return strcat(result,ext);
}

unsigned long int copy_file(FILE *input,FILE *output)
{
 unsigned long int length;
 length=get_file_size(input);
 fast_data_dump(input,output,(size_t)length);
 return length;
}

void check_executable(FILE *input)
{
 char signature[2];
 fread(signature,sizeof(char),2,input);
 if (strncmp(signature,"MZ",2)!=0)
 {
  puts("Executable file of Flash Player Projector corrupted");
  exit(4);
 }

}

void check_signature(FILE *input)
{
 char signature[3];
 fread(signature,sizeof(char),3,input);
 if (strncmp(signature,"FWS",3)!=0)
 {
  if (strncmp(signature,"CWS",3)!=0)
  {
   puts("Flash movie corrupted");
   exit(5);
  }

 }

}

void write_service_information(FILE *output,const unsigned long int length)
{
 unsigned long int flag;
 flag=0xFA123456;
 fwrite(&flag,sizeof(unsigned long int),1,output);
 fwrite(&length,sizeof(unsigned long int),1,output);
}

void compile_flash(const char *player,const char *flash,const char *result)
{
 unsigned long int length;
 FILE *projector;
 FILE *swf;
 FILE *output;
 projector=open_input_file(player);
 swf=open_input_file(flash);
 check_executable(projector);
 check_signature(swf);
 output=create_output_file(result);
 copy_file(projector,output);
 length=copy_file(swf,output);
 write_service_information(output,length);
 fclose(projector);
 fclose(swf);
 fclose(output);
}

void work(const char *player,const char *flash)
{
 char *output=NULL;
 output=get_name(flash,".exe");
 compile_flash(player,flash,output);
 free(output);
}