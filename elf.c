/* INCLUDES */
#include <stdio.h>
#include <stdint.h>
#include <windows.h>


/* MACROS */
#define   ELF_FILE          "file.elf" //change the name of the elf file 
#define   ELF_TYPE_LOAD     1U
#define   OK                0U
#define   NOT_OK            1U


/* types */
typedef struct
{
    uint8_t  identity[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t ph_offset;
    uint32_t sh_offset;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_size;
    uint16_t ph_num;
    uint16_t sh_size;
    uint16_t sh_num;
    uint16_t strtab_index;

}Elf_Header_t;


typedef struct
{
    uint32_t type;
    uint32_t offset;
    uint32_t virtual_address;
    uint32_t physical_address;
    uint32_t file_size;
    uint32_t mem_size;
    uint32_t flags;
    uint32_t align;

}Elf_ProgramHeader_t;


/* global variables */
uint8_t TextBuffer[1024*1024];
uint8_t DataBuffer[1024];//*1024

uint32_t Text_physical_address;
uint32_t Data_physical_address;
uint32_t Bss_physical_address;

uint32_t Text_size;
uint32_t data_size;
uint32_t bss_size;

/* for usb */
HANDLE GetSerialPort(char *);


/* Protoypes */
uint8_t IS_File_Exist(void);
uint8_t IS_ElfFile(Elf_Header_t *ElfFile);
uint8_t ElfFile_Load(uint8_t* FileData , uint8_t ElfFileSize);
uint8_t Get_File_Size(uint8_t* ElfFileSize);
uint8_t ElfFile_LoadSegments(Elf_ProgramHeader_t* Elf_ProgramHeader , Elf_Header_t* Elf_Header );



/**************************** main function ****************************/

void main(void)
{
  uint8_t data[1024*1024]={0};
	uint8_t ElfFileSize ;

	Elf_ProgramHeader_t* Elf_ProgramHeader;
	Elf_Header_t* Elf_Header ;

  Elf_Header = (Elf_Header_t *)data;

  void* InfoFilePtr;
  if(IS_File_Exist() == OK )
	{
		/* Get the file Size */
		Get_File_Size(&ElfFileSize);
		printf("elf file size = %d \n" ,ElfFileSize);

		/* Load file to data buffer */
		ElfFile_Load(data, ElfFileSize ) ;
		printf("entery point address = %x\n",Elf_Header->entry);


		/* Check if the file is elf file */
		if(IS_ElfFile(Elf_Header) == OK)
	  {
      printf("It is elf file\n");

      Elf_ProgramHeader = (Elf_ProgramHeader_t*)&data[Elf_Header->ph_offset] ;
      ElfFile_LoadSegments(Elf_ProgramHeader, Elf_Header) ;

      printf("Text_physical_address = %x\n",Text_physical_address);
      printf("Data_physical_address = %x\n",Data_physical_address);
      printf("Bss_physical_address = %x\n",Bss_physical_address);

      printf("Text_size = %x\n",Text_size);
      printf("data_size = %x\n",data_size);
      printf("bss_size = %x\n",bss_size);

	  }

		uint32_t InfoBuffer [] = {Text_physical_address,Text_size+data_size,Elf_Header->entry,Data_physical_address,data_size};
		
    InfoFilePtr= fopen("INFO_FILE.txt" , "w");
		fwrite(&InfoBuffer, 5, 4, InfoFilePtr);
    fclose(InfoFilePtr);
	}


}



/**************************** functions definition ****************************/

/* Check If elf File is Exist or not */
uint8_t IS_File_Exist(void)
{
	void* FilePtr;
	uint8_t ElfFile_Status = OK;

	FilePtr = fopen(ELF_FILE , "rb");

	if(FilePtr == NULL)
  {
    ElfFile_Status = NOT_OK ;
		printf("file Not Exist\n");
  }
  else
  {
      fclose(FilePtr) ;
	}
  return 	ElfFile_Status;
}


/* check is it elf file */
uint8_t IS_ElfFile(Elf_Header_t *ElfFile)
{
	uint8_t ElfFile_Status = OK;

	if (!( (ElfFile->identity[0] == 0x7f) && (ElfFile->identity[1] == 'E')
      && (ElfFile->identity[2] == 'L')  && (ElfFile->identity[3] == 'F') ) )
	{
      ElfFile_Status = NOT_OK;
	    printf("Elf file Not Exist\n");
	}

  return ElfFile_Status;
}



/* Get Elf File Size */
uint8_t Get_File_Size(uint8_t* ElfFileSize)
{
	uint8_t ElfFile_Status = OK;
	void* FilePtr;
	FilePtr = fopen(ELF_FILE, "rb");

	if ( FilePtr == NULL)
  {
    ElfFile_Status = NOT_OK ;
  }
  else
  {
    fseek(FilePtr , 0 , SEEK_END);
	  *ElfFileSize = ftell(FilePtr);
	  fseek(FilePtr , 0 ,SEEK_SET);
    fclose(FilePtr) ;
	}
  return 	ElfFile_Status;
}


/* Load File */
uint8_t ElfFile_Load(uint8_t* ElfFileData ,	uint8_t ElfFileSize)
{
	void* FilePtr;
	uint8_t ElfFile_Status = OK;

	FilePtr = fopen(ELF_FILE , "rb");

	if(FilePtr == NULL)
  {
    ElfFile_Status = NOT_OK ;
  }
  else
  {
    fread(ElfFileData,1, ElfFileSize+1, FilePtr);
    fclose(FilePtr);
	}
  return 	ElfFile_Status;
}


/* Load Elf File Segments */
uint8_t ElfFile_LoadSegments(Elf_ProgramHeader_t* Elf_ProgramHeader , Elf_Header_t* Elf_Header)
{
	uint8_t ElfFile_Status = OK;
	uint8_t Ph_index ;
  uint16_t Data_index;
  void* ElfFilePtr;
  void* TextFilePtr;
  void* DataFilePtr;

  ElfFilePtr = fopen(ELF_FILE , "rb");

  Text_physical_address = Elf_ProgramHeader[0].physical_address;
  Data_physical_address = Elf_ProgramHeader[1].physical_address;
  Bss_physical_address = Elf_ProgramHeader[2].physical_address;

  printf("Elf_ProgramHeader[0].type = %x\n", Elf_ProgramHeader[0].type);// Size in memory
  printf("Elf_ProgramHeader[1].type = %x\n", Elf_ProgramHeader[1].type);// Size in memory
  printf("Elf_ProgramHeader[Ph_in2dex].type = %x\n", Elf_ProgramHeader[2].type);// Size in memory
  
  
  for(Ph_index = 0; Ph_index < Elf_Header->ph_num; Ph_index++)
  {
      if(Elf_ProgramHeader[Ph_index].type == ELF_TYPE_LOAD)
      {
        printf("memory size = %x\n", Elf_ProgramHeader[Ph_index].mem_size);// Size in memory
        printf("file size = %x\n", Elf_ProgramHeader[Ph_index].file_size);// Size in file
        printf("memory offset = %x\n", Elf_ProgramHeader[Ph_index].virtual_address); // Offset in memory
        printf("physical address = %x\n", Elf_ProgramHeader[Ph_index].physical_address); // Offset in memory
        printf("file offset = %x\n", Elf_ProgramHeader[Ph_index].offset);// Offset in file

        /* reading .text , .data , .bss bytes to be sent   */

        if(ElfFilePtr == NULL)
        {
          ElfFile_Status = NOT_OK ;
        }
        else
        {
                fseek(ElfFilePtr , Elf_ProgramHeader[Ph_index].offset, SEEK_SET);

                if(Ph_index==0)//text
                {        
                      Text_size = Elf_ProgramHeader[Ph_index].file_size;
                      fread(TextBuffer,1, Text_size + 1,ElfFilePtr);

                      TextFilePtr = fopen("TEXT_FILE.txt" , "wb");
                      fwrite(&TextBuffer, Text_size, 1, TextFilePtr);
                      fclose(TextFilePtr);
                }
                else if(Ph_index==1)//data
                {
                      data_size = Elf_ProgramHeader[Ph_index].file_size;
                      fread(DataBuffer,1, data_size + 1 , ElfFilePtr);

                      DataFilePtr = fopen("DATA_FILE.txt" , "wb");
                      fwrite(&DataBuffer, data_size , 1, DataFilePtr);
                      fclose(DataFilePtr);
                }
                else
                {
                    //.bss >> file size = zero
                }
                fseek(ElfFilePtr , 0 ,SEEK_SET);
        }

      }
	}

  fclose(ElfFilePtr);
	return 	ElfFile_Status;
}


