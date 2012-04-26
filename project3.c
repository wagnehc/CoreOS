


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

   void *countBytes(void *arg);
   void *countWords(void *arg);
   void *countLines(void *arg);
   void barrier();
   void barrier2();
   void eat();
   void writefile();
   void ByteCounter();
   void readerWords();
   void readerBytes();
   void readerLines();
   void WordCounter();
   void LineCounter();
   void writer();

   char *data[4][2048];
   char buffer1[2050];
   char buffer2[2050];
   char buffer3[2050];
   char buffer4[2050];
   char buffer5[2050];
   sem_t x,x1, x2, x3, x4, x5, y, z, wsem, wsem1, wsem2, wsem3, wsem4, wsem5, rsem,rsemword1, rsemword2, rsemword3, rsemword4, rsemword5, rsembyte1, rsembyte2, rsembyte3, rsembyte4, rsembyte5, rsemline1, rsemline2, rsemline3, rsemline4, rsemline5, philosopher, barriersem;

   volatile int size = 0, filePointer = 0, bytes = 0, words = 0, lines = 0, barrier_count = 0, barrier_count2 = 0, i1 = 0, i2 = 0, i3 = 0;
   volatile int readcount = 0, readcount1 = 0, readcount2 = 0, readcount3 = 0, readcount4 = 0, readcount5 = 0, writecount = 0;
   volatile bool complete = false;
   int buffercounter = 1, linecounter = 1, wordcounter = 1, bytecounter = 1;
   FILE *file;
   volatile size_t result;
volatile bool wordlock1, wordlock2, wordlock3, wordlock4, wordlock5, linelock = false;


   int main()
   {
   
   
      pthread_t bytethread, wordthread, linethread;
      sem_init(&x,0,1);
      sem_init(&x1, 0, 1);
      sem_init(&x2, 0, 1);
      sem_init(&x3, 0, 1);
      sem_init(&x4, 0, 1);
      sem_init(&x5, 0, 1);
      sem_init(&rsem,0,1);
      sem_init(&rsemword1, 0, 1);
      sem_init(&rsemword2, 0, 1);
      sem_init(&rsemword3, 0, 1);
      sem_init(&rsemword4, 0, 1);
      sem_init(&rsemword5, 0, 1);
      sem_init(&rsembyte1, 0, 1);
      sem_init(&rsembyte2, 0, 1);
      sem_init(&rsembyte3, 0, 1);
      sem_init(&rsembyte4, 0, 1);
      sem_init(&rsembyte5, 0, 1);
      sem_init(&rsemline1, 0, 1);
      sem_init(&rsemline2, 0, 1);
      sem_init(&rsemline3, 0, 1);
      sem_init(&rsemline4, 0, 1);
      sem_init(&rsemline5, 0, 1);
      sem_init(&wsem, 0, 1);
      sem_init(&wsem1, 0, 1);
      sem_init(&wsem2, 0, 1);
      sem_init(&wsem3, 0, 1);
      sem_init(&wsem4, 0, 1);
      sem_init(&wsem5, 0, 1);
      sem_init(&y,0,1);
      sem_init(&z,0,1);
      sem_init(&philosopher, 0, 2);
      sem_init(&barriersem, 0, 1);
   
   
      file = fopen("testfile.txt", "r");
      fseek(file, 0L, SEEK_END);
      size = ftell(file);
      rewind (file);
   
   
      if(pthread_create( &bytethread, NULL, countBytes, NULL))
      {
         printf("error creating thread.");
         abort();
      }
      if(pthread_create( &wordthread, NULL, countWords, NULL))
      {
         printf("error creating thread.");
         abort();
      } 
      if(pthread_create( &linethread, NULL, countLines, NULL))
      {
         printf("error creating thread.");
         abort();
      }
   
      barrier();
      eat();
      barrier2();  
      writer();
   
    
   //complete = true;
      if (pthread_join(bytethread, NULL)) {
         printf("error join thread");
         abort();
      }
      if (pthread_join(wordthread, NULL)) {
         printf("error join thread");
         abort();
      }
      if (pthread_join(linethread, NULL)) {
         printf("error join thread");
         abort();
      }
      return 0;
   }

   void *countWords(void *arg)
   {
      barrier();
      eat();
      barrier2();
    readerWords();
      printf("Total number of Words is: %i\n", words);
   }
   void *countBytes(void *arg)
   {
      barrier();
      eat();
      barrier2();
      readerBytes();
   
      printf("Total number of bytes is:  %i\n", bytes);
   }
   void *countLines(void *arg)
   {
      barrier();
      eat();
      barrier2();
   // readerLines();
      printf("total number of lines is: %i\n", lines);
   }

   void barrier()
   {
      sem_wait(&barriersem); 
      barrier_count++;
      sem_post(&barriersem);
   
      while (barrier_count != 4)
      {
      
      }
      printf("Thread%i leaving barrier1.\n", ++i1);
   }

   void barrier2()
   {
      sem_wait(&barriersem);
      barrier_count2++;
      sem_post(&barriersem);
    
      while (barrier_count2 != 4)
      {
      }
      printf("Thread%i leaving barrier2.\n", ++i3);    
   }

   void eat()
   {
      sem_wait(&philosopher);
      ;/* eating */
      sem_post(&philosopher);
      printf("Thread%i ate.\n", ++i2);
   }

   void writefile()
   {
      bool ableToWrite = false;
   //Reading from the file, write to five 2KB buffers
      switch (buffercounter)
      {
         case 1:
            sem_wait(&wsem1);sem_wait(&rsembyte1); sem_wait(&rsemword1); sem_wait(&rsemline1);
            printf("%c", buffer1[2049]);
            if (buffer1[2049] == NULL || (buffer1[2049] == '\0' && !wordlock1))
            {
               printf("buffer 2049 = null\n");
               buffer1[2049] = '1';
               result = fread(buffer1,1,2048,file);
               buffer1[result] = '\0';
wordlock1 = true;
               if (ferror (file))
                  printf ("Error Writing to myfile.txt\n");
               ableToWrite = true;
            }
            sem_post(&rsembyte1);sem_post(&rsemword1);sem_post(&rsemline1);sem_post(&wsem1);
            break;
         case 2:
            sem_wait(&wsem2);sem_wait(&rsembyte2); sem_wait(&rsemline2); sem_wait(&rsemword2);
            if (buffer2[2049] == NULL || (buffer2[2049] == '\0' && !wordlock2))
            {
               buffer2[2049] = '1';
               result = fread(buffer2,1,2048,file);
               buffer2[result] = '\0';
wordlock2 = true;
               if (ferror (file))
                  printf ("Error Writing to myfile.txt\n");
               ableToWrite == true;
            }
            sem_post(&rsembyte2);sem_post(&rsemword2);sem_post(&rsemline2);sem_post(&wsem2);
         
            break;
         case 3:
            sem_wait(&wsem3);sem_wait(&rsembyte3);sem_wait(&rsemline3);sem_wait(&rsemword3);
            if (buffer3[2049] == NULL || (buffer3[2049] == '\0' && !wordlock3))
            {
               buffer3[2049] = '1';
               result = fread(buffer3,1,2048,file);
               buffer3[result] = '\0';
wordlock3 = true;
               if (ferror (file))
                  printf ("Error Writing to myfile.txt\n");
               ableToWrite = true;
            }
            sem_post(&rsembyte3);sem_post(&rsemword3);sem_post(&rsemline3);sem_post(&wsem3);
         
            break;
         case 4:
            if (buffer4[2049] == NULL || (buffer4[2049] == '\0' && !wordlock4))
            {
               sem_wait(&wsem4);sem_wait(&rsembyte4);sem_wait(&rsemline4);sem_wait(&rsemword4);
               buffer4[2049] = '1';
               result = fread(buffer4,1,2048,file);
               buffer4[result] = '\0';
wordlock4 = true;
               if (ferror (file))
                  printf ("Error Writing to myfile.txt\n");
               ableToWrite = true;
            }
            sem_post(&rsembyte4);sem_post(&rsemword4);sem_post(&rsemline4);sem_post(&wsem4);
            break;
         case 5:
            if (buffer5[2049] == NULL || (buffer5[2049] == '\0' && !wordlock5))
            {
               sem_wait(&wsem5);sem_wait(&rsembyte5);sem_wait(&rsemword5);sem_wait(&rsemline5);
               buffer5[2049] = '1';
               result = fread(buffer5,1,2048,file);
               buffer5[result] = '\0';
wordlock5 = true;
               if (ferror (file))
                  printf ("Error Writing to myfile.txt\n");
               ableToWrite = true;
            }
            sem_post(&rsembyte5);sem_post(&rsemword5);sem_post(&rsemline5);sem_post(&wsem5);
            break;
         default:;
      
      }
      if (ableToWrite)
      {
         buffercounter++;
         if (buffercounter == 6)
            buffercounter = 1;
         ableToWrite = false;
      }
   }

   void ByteCounter()
   {
      int  i;
      char* c;
   
      switch (bytecounter)
      {
         case 1:
            sem_wait(&x1);
            readcount1++;
            if (readcount1 == 1)
               sem_wait(&wsem1);
            sem_post(&x1);
            sem_wait(&rsembyte1);
            c = buffer1;
            if (buffer1[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer1[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x1);
            readcount1--;
            if (readcount1 == 0)
               sem_post(&wsem1);
            sem_post(&x1);
            sem_post(&rsembyte1);
            break;
         case 2:
            sem_wait(&x2);
            readcount2++;
            if (readcount2 == 1)
               sem_wait(&wsem2);
            sem_post(&x2);
            sem_wait(&rsembyte2);
            c = buffer2;
            if (buffer2[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer2[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x2);
            readcount2--;
            if (readcount2 == 0)
               sem_post(&wsem2);
            sem_post(&x2);
            sem_post(&rsembyte2);
            break;
         case 3:
            sem_wait(&x3);
            readcount3++;
            if (readcount3 == 1)
               sem_wait(&wsem3);
            sem_post(&x3);
            sem_wait(&rsembyte3);
            c = buffer3;
            if (buffer3[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer3[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x3);
            readcount3--;
            if (readcount3 == 0)
               sem_post(&wsem3);
            sem_post(&x3);
            sem_post(&rsembyte3);
            break;
         case 4:
            sem_wait(&x4);
            readcount4++;
            if (readcount4 == 1)
               sem_wait(&wsem4);
            sem_post(&x4);
            sem_wait(&rsembyte4);
            c = buffer4;
            if (buffer4[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer4[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x4);
            readcount4--;
            if (readcount4 == 0)
               sem_post(&wsem4);
            sem_post(&x4);
            sem_post(&rsembyte4);
            break;
         case 5:
            sem_wait(&x5);
            readcount5++;
            if (readcount5 == 1)
               sem_wait(&wsem5);
            sem_post(&x5);
            sem_wait(&rsembyte5);
            c = buffer5;
            if (buffer5[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer5[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x5);
            readcount5--;
            if (readcount5 == 0)
               sem_post(&wsem5);
            sem_post(&x5);
            sem_post(&rsembyte5);
            break;
         default:;
      
      }
      bytecounter++;
      if (bytecounter == 6)
         bytecounter = 1;
   }

   void readerBytes()
   {
      int i;
      while (!complete)
      {
         ByteCounter();
      }
    //  for (i=0;i<5;i++)
      //   ByteCounter();
   }
   void readerLines()
   {
      
      while(!complete)
      {
      
         LineCounter();
         sem_wait(&x);
         readcount--;
         if (readcount == 0)
            sem_post(&wsem);
         sem_post(&x);
      }
   }

   void readerWords()
   {
      
      while(!complete)
      {
        
         WordCounter();
        
      }
   }

   void LineCounter()
   {
   
      int  i;
      char* c;
   
      switch (bytecounter)
      {
         case 1:
            sem_wait(&x1);
            readcount1++;
            if (readcount1 == 1)
               sem_wait(&wsem1);
            sem_post(&x1);
            sem_wait(&rsembyte1);
            c = buffer1;
            if (buffer1[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer1[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x1);
            readcount1--;
            if (readcount1 == 0)
               sem_post(&wsem1);
            sem_post(&x1);
            sem_post(&rsembyte1);
            break;
         case 2:
            sem_wait(&x2);
            readcount2++;
            if (readcount2 == 1)
               sem_wait(&wsem2);
            sem_post(&x2);
            sem_wait(&rsembyte2);
            c = buffer2;
            if (buffer2[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer2[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x2);
            readcount2--;
            if (readcount2 == 0)
               sem_post(&wsem2);
            sem_post(&x2);
            sem_post(&rsembyte2);
            break;
         case 3:
            sem_wait(&x3);
            readcount3++;
            if (readcount3 == 1)
               sem_wait(&wsem3);
            sem_post(&x3);
            sem_wait(&rsembyte3);
            c = buffer3;
            if (buffer3[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer3[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x3);
            readcount3--;
            if (readcount3 == 0)
               sem_post(&wsem3);
            sem_post(&x3);
            sem_post(&rsembyte3);
            break;
         case 4:
            sem_wait(&x4);
            readcount4++;
            if (readcount4 == 1)
               sem_wait(&wsem4);
            sem_post(&x4);
            sem_wait(&rsembyte4);
            c = buffer4;
            if (buffer4[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer4[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x4);
            readcount4--;
            if (readcount4 == 0)
               sem_post(&wsem4);
            sem_post(&x4);
            sem_post(&rsembyte4);
            break;
         case 5:
            sem_wait(&x5);
            readcount5++;
            if (readcount5 == 1)
               sem_wait(&wsem5);
            sem_post(&x5);
            sem_wait(&rsembyte5);
            c = buffer5;
            if (buffer5[2049] != '\0')
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  else
                  {
                     bytes++;
                  }
                  c++;
               }
               buffer5[2049] = '\0';
               printf("%i\n", bytes);
            }
            sem_wait(&x5);
            readcount5--;
            if (readcount5 == 0)
               sem_post(&wsem5);
            sem_post(&x5);
            sem_post(&rsembyte5);
            break;
         default:;
      
      }
      bytecounter++;
      if (bytecounter == 6)
         bytecounter = 1;
   }

   void WordCounter()
   {
      bool wordFlag = false;
      int  i;
      char* c;
   
      switch (wordcounter)
      {
         case 1:
            sem_wait(&x1);
            readcount1++;
            if (readcount1 == 1)
               sem_wait(&wsem1);
            sem_post(&x1);
            sem_wait(&rsemword1);
            c = buffer1;
            if (wordlock1)
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  if (*c != ' ') //*c == ' ' || *c == '\n' || (*c == ' ' && *(c + 1) == ' '))
                  {
                     wordFlag = true;
                  }
                  else if (*c == ' ')
                  {
                     wordFlag = false;
                  }
                  if ((!wordFlag && *( c - 1) != ' ') || (!wordFlag && *( c - 1) != '\0'))
                  {
                     words++;
                  }
                  c++;
               }
              
		wordlock1 = false;
               printf("%i\n", words);
            }
         
            sem_wait(&x1);
            readcount1--;
            if (readcount1 == 0)
               sem_post(&wsem1);
            sem_post(&x1);
            sem_post(&rsemword1);
            break;
         case 2:
            sem_wait(&x2);
            readcount2++;
            if (readcount2 == 1)
               sem_wait(&wsem2);
            sem_post(&x2);
            sem_wait(&rsemword2);
            c = buffer2;
            if (wordlock2)
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  if (*c != ' ') //*c == ' ' || *c == '\n' || (*c == ' ' && *(c + 1) == ' '))
                  {
                     wordFlag = true;
                  }
                  else if (*c == ' ')
                  {
                     wordFlag = false;
                  }
                  if ((!wordFlag && *( c - 1) != ' ') || (!wordFlag && *( c - 1) != '\0'))
                  {
                     words++;
                  }
                  c++;
                  
                     
                  }
wordlock2 = false;
                     printf("%i\n", words);
               }
               sem_wait(&x2);
               readcount2--;
               if (readcount2 == 0)
                  sem_post(&wsem2);
               sem_post(&x2);
               sem_post(&rsemword2);
               break;
            case 3:
               sem_wait(&x3);
               readcount3++;
               if (readcount3 == 1)
                  sem_wait(&wsem3);
               sem_post(&x3);
               sem_wait(&rsemword3);
               c = buffer3;
               if (wordlock3)
               {
                  for (i = 0; i < 2048; i++)
                  {
                     if (*c == '\0')
                     {
                        printf("end of file\n");
                        break;
                     }
                     if (*c != ' ') //*c == ' ' || *c == '\n' || (*c == ' ' && *(c + 1) == ' '))
                     {
                        wordFlag = true;
                     }
                     else if (*c == ' ')
                     {
                        wordFlag = false;
                     }
                     if ((!wordFlag && *( c - 1) != ' ') || (!wordFlag && *( c - 1) != '\0'))
                     {
                        words++;
                     }
                     c++;
                  }
                  
               
wordlock3 = false;
                  printf("%i\n", words);
            }
            sem_wait(&x3);
            readcount3--;
            if (readcount3 == 0)
               sem_post(&wsem3);
            sem_post(&x3);
            sem_post(&rsemword3);
            break;
         case 4:
            sem_wait(&x4);
            readcount4++;
            if (readcount4 == 1)
               sem_wait(&wsem4);
            sem_post(&x4);
            sem_wait(&rsemword4);
            c = buffer4;
            if (wordlock4)
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  if (*c != ' ') //*c == ' ' || *c == '\n' || (*c == ' ' && *(c + 1) == ' '))
                  {
                     wordFlag = true;
                  }
                  else if (*c == ' ')
                  {
                     wordFlag = false;
                  }
                  if ((!wordFlag && *( c - 1) != ' ') || (!wordFlag && *( c - 1) != '\0'))
                  {
                     words++;
                  }
                  c++;
               }
wordlock4 = false;
                  printf("%i\n", words);
            }
            sem_wait(&x4);
            readcount4--;
            if (readcount4 == 0)
               sem_post(&wsem4);
            sem_post(&x4);
            sem_post(&rsemword4);
            break;
         case 5:
            sem_wait(&x5);
            readcount5++;
            if (readcount5 == 1)
               sem_wait(&wsem5);
            sem_post(&x5);
            sem_wait(&rsemword5);
            c = buffer5;
            if (wordlock5)
            {
               for (i = 0; i < 2048; i++)
               {
                  if (*c == '\0')
                  {
                     printf("end of file\n");
                     break;
                  }
                  if (*c != ' ') //*c == ' ' || *c == '\n' || (*c == ' ' && *(c + 1) == ' '))
                  {
                     wordFlag = true;
                  }
                  else if (*c == ' ')
                  {
                     wordFlag = false;
                  }
                  if ((!wordFlag && *( c - 1) != ' ') || (!wordFlag && *( c - 1) != '\0'))
                  {
                     words++;
                  }
                  c++;
               
               }
wordlock5 = false;
                  printf("%i\n", words);
}
               sem_wait(&x5);
               readcount5--;
               if (readcount5 == 0)
                  sem_post(&wsem5);
               sem_post(&x5);
               sem_post(&rsemword5);
               break;
            default:;
            
            }
      
      wordcounter++;
      if (wordcounter == 6)
         wordcounter = 1;
   }


   void writer()
   {
      while(!feof(file))
      {
      /*sem_wait(&y);
      writecount++;
      if (writecount == 1)
      sem_wait(&rsem);
      sem_post(&y);
      sem_wait(&wsem);*/
         writefile();
      /* sem_post(&wsem);
      sem_wait(&y);
      writecount--;
      if (writecount == 0) 
      sem_post(&rsem);
      sem_post(&y);*/
      }
      complete = true;
      fclose(file);
   }


