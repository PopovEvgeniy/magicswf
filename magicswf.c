#include "magicswf.h"
#include "format.h"

void show_intro();
FILE *open_input_file(const char *name);
FILE *create_output_file(const char *name);
void read_data(void *data,const size_t length,FILE *input);
void write_data(const void *data,const size_t length,FILE *output);
char *get_memory(const size_t length);
void check_executable(FILE *input);
void check_signature(FILE *input);
unsigned long int get_file_size(FILE *target);
void fast_data_dump(FILE *input,FILE *output,const size_t length);
void data_dump(FILE *input,FILE *output,const size_t length);
size_t get_name_without_extension_length(const char *source);
char *get_name_without_extension(const char *name);
char *get_name(const char *name,const char *extension);
unsigned long int copy_file(FILE *input,FILE *output);
void write_service_information(FILE *output,const unsigned long int length);
void compile_flash(const char *player,const char *flash,const char *result);
void work(const char *player,const char *flash);

int main(int argc, char *argv[])
{
 show_intro();
 if(argc<3)
 {
  puts("You must give the Flash Player Projector file name and the Flash movie file name as the command-line arguments!");
 }
 else
 {
  puts("Working... Please wait");
  work(argv[1],argv[2]);
  puts("The work has been finished");
 }
 return 0;
}

void show_intro()
{
 putchar('\n');
 puts("Magic swf. Version 1.6.7");
 puts("A simple tool for converting an Adobe Flash movie to a standalone movie");
 puts("This sofware was made by Popov Evgeniy Alekseyevich,2011-2026 years");
 puts("This software is distributed under the GNU GENERAL PUBLIC LICENSE");
 putchar('\n');
}

FILE *open_input_file(const char *name)
{
 FILE *target=NULL;
 if (name==NULL)
 {
  puts("Can't open the input file");
  exit(1);
 }
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
 FILE *target=NULL;
 if (name==NULL)
 {
  puts("Can't create the ouput file");
  exit(2);
 }
 target=fopen(name,"wb");
 if (target==NULL)
 {
  puts("Can't create the ouput file");
  exit(2);
 }
 return target;
}

void read_data(void *data,const size_t length,FILE *input)
{
 fread(data,sizeof(char),length,input);
 if (ferror(input)!=0)
 {
  puts("Can't read data!");
  exit(3);
 }

}

void write_data(const void *data,const size_t length,FILE *output)
{
 fwrite(data,sizeof(char),length,output);
 if (ferror(output)!=0)
 {
  puts("Can't write data!");
  exit(4);
 }

}

char *get_memory(const size_t length)
{
 char *memory=NULL;
 memory=(char*)calloc(length,sizeof(char));
 if(memory==NULL)
 {
  puts("Can't allocate memory");
  exit(5);
 }
 return memory;
}

void check_executable(FILE *input)
{
 char signature[2];
 read_data(signature,2,input);
 if (strncmp(signature,"MZ",2)!=0)
 {
  puts("The executable file of the Flash Player projector was corrupted");
  exit(6);
 }

}

void check_signature(FILE *input)
{
 char signature[3];
 read_data(signature,3,input);
 if (strncmp(signature,"FWS",3)!=0)
 {
  if (strncmp(signature,"CWS",3)!=0)
  {
   puts("The Flash movie was corrupted");
   exit(7);
  }

 }

}

unsigned long int get_file_size(FILE *target)
{
 unsigned long int length=0;
 if (fseek(target,0,SEEK_END)!=0)
 {
  puts("Can't get the file size!");
  exit(8);
 }
 length=ftell(target);
 rewind(target);
 return length;
}

void data_dump(FILE *input,FILE *output,const size_t length)
{
 char *buffer=NULL;
 size_t current=0;
 size_t elapsed=0;
 size_t block=4096;
 buffer=get_memory(block);
 for (current=0;current<length;current+=block)
 {
  elapsed=length-current;
  if (elapsed<block)
  {
   block=elapsed;
  }
  read_data(buffer,block,input);
  write_data(buffer,block,output);
 }
 free(buffer);
}

void fast_data_dump(FILE *input,FILE *output,const size_t length)
{
 char *buffer=NULL;
 buffer=(char*)malloc(length);
 if (buffer==NULL)
 {
  data_dump(input,output,length);
 }
 else
 {
  read_data(buffer,length,input);
  write_data(buffer,length,output);
  free(buffer);
 }

}

size_t get_name_without_extension_length(const char *source)
{
 size_t index=0;
 size_t position=0;
 size_t length=0;
 if (source!=NULL)
 {
  length=strlen(source);
 }
 for (index=length;index>0;--index)
 {
  position=index-1;
  if (source[position]==DIRECTORY_SEPARATOR)
  {
   break;
  }
  if (source[position]=='.')
  {
   if (position>0)
   {
    if ((source[position-1]!=DIRECTORY_SEPARATOR) && (source[position-1]!='.'))
    {
     length=position;
     break;
    }

   }

  }

 }
 return length;
}

char *get_name_without_extension(const char *name)
{
 char *result=NULL;
 size_t length=0;
 length=get_name_without_extension_length(name);
 if (length>0)
 {
  result=get_memory(length+1);
  strncpy(result,name,length);
 }
 return result;
}

char *get_name(const char *name,const char *extension)
{
  char *result=NULL;
  char *name_without_extension=NULL;
  size_t name_length=0;
  size_t extension_length=0;
  name_without_extension=get_name_without_extension(name);
  if (name_without_extension!=NULL)
  {
   name_length=strlen(name_without_extension);
  }
  if (extension!=NULL)
  {
   extension_length=strlen(extension);
  }
  if ((name_length>0) && (extension_length>0))
  {
   result=get_memory(name_length+extension_length+1);
   strncpy(result,name_without_extension,name_length);
   strncat(result,extension,extension_length);
  }
  free(name_without_extension);
  return result;
}

unsigned long int copy_file(FILE *input,FILE *output)
{
 unsigned long int length=0;
 length=get_file_size(input);
 fast_data_dump(input,output,(size_t)length);
 return length;
}

void write_service_information(FILE *output,const unsigned long int length)
{
 service_information information;
 information.signature[0]='V';
 information.signature[1]='4';
 information.marker[0]=18;
 information.marker[1]=250;
 information.length=length;
 write_data(&information,sizeof(service_information),output);
}

void compile_flash(const char *player,const char *flash,const char *result)
{
 unsigned long int length=0;
 FILE *projector=NULL;
 FILE *swf=NULL;
 FILE *output=NULL;
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