#include <stdio.h>
 
int main()
{
   FILE* srcfile;
   FILE* destfile;
 
   char buffer[2]={0};                        
 
   srcfile = fopen("data.txt","rb");     
   destfile = fopen("out.txt","wb");
 
   while(1)                                     
   {
      fread(buffer,2,1,srcfile);               
     
      if(feof(srcfile)!=0)                      
      {         
         break;                                
      }
      else                                      
      {
         fwrite(buffer,2,1,destfile);           
      }
   }
 
   fclose(destfile);
   fclose(srcfile);
}
 

