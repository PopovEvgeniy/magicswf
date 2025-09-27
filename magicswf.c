#include "magicswf.h"

void show_intro();
FILE *open_input_file(const char *name);
FILE *create_output_file(const char *name);
char *get_memory(const size_t length);
void fast_data_dump(FILE *input,FILE *output,const size_t length);
void data_dump(FILE *input,FILE *output,const size_t length);
unsigned long int get_file_size(FILE *file);
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
  puts("The work has been finished");
 }
 else
 {
  puts("You must give the Flash Player Projector file name and the Flash movie file name as the command-line arguments!");
 }
 return 0;
}

void show_intro()
{
 putchar('\n');
 puts("Magic swf. Version 1.5.5");
 puts("A simple tool for converting an Adobe Flash movie to a self-played movie");
 puts("This sofware was made by Popov Evgeniy Alekseyevich,2011-2025 years");
 puts("This software is distributed under the GNU GENERAL PUBLIC LICENSE");
 putchar('\n');
}

FILE *open_input_file(const char *name)
{
 FILE *target;
 target=fopen(name,"rb");
 if (target==NULL)
 {
  puts("Can't open the input file");
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
  puts("Can't create the ouput file");
  exit(2);
 }
 return target;
}

char *get_memory(const size_t length)
{
 char *memory=NULL;
 memory=(char*)calloc(length,sizeof(char));
 if(memory==NULL)
 {
  puts("Can't allocate memory");
  exit(3);
 }
 return memory;
}

void data_dump(FILE *input,FILE *output,const size_t length)
{
 char *buffer;
 size_t current,elapsed,block;
 current=0;
 elapsed=0;
 block=4096;
 buffer=get_memory(block);
 while (current<length)
 {
  elapsed=length-current;
  if (elapsed<block)
  {
   block=elapsed;
  }
  fread(buffer,sizeof(char),block,input);
  fwrite(buffer,sizeof(char),block,output);
  current+=block;
 }
 free(buffer);
}

void fast_data_dump(FILE *input,FILE *output,const size_t length)
{
 char *buffer;
 buffer=(char*)malloc(length);
 if (buffer==NULL)
 {
  data_dump(input,output,length);
 }
 else
 {
  fread(buffer,sizeof(char),length,input);
  fwrite(buffer,sizeof(char),length,output);
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
 result=get_memory(length+1);
 return strncpy(result,name,length);
}

char* get_name(const char *name,const char *ext)
{
  char *result=NULL;
  char *output=NULL;
  size_t length;
  output=get_short_name(name);
  length=strlen(output)+strlen(ext);
  result=get_memory(length+1);
  sprintf(result,"%s%s",output,ext);
  free(output);
  return result;
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
  puts("The executable file of the Flash Player projector was corrupted");
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
   puts("The Flash movie was corrupted");
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