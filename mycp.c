#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 4096

void error_msg(char *s){
  printf("%s\n", s);
  exit(1);
}

//copy text file to text file function
void mycp(char* from, char* to)
{
  char buffer[SIZE];
  int in, out;
  ssize_t bytes;
  
  in = open(from, O_RDONLY);
  if(!in){
	error_msg("cannot open file\n"); 
  }
  if((out = open(to, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
	error_msg("error opening the file..\n");
   }

  while((bytes = read(in, buffer, SIZE)) > 0){
	if(write(out, buffer, SIZE) < bytes){
		error_msg("error writing to a text file..\n");
	}
  }

  if(bytes<0){
	error_msg("error reading text file..\n"); 
  }
}

//creating a path
char *ispath(char *file1, char *file2) {
  char *getPath, *str;
  getPath = ((char *) malloc((sizeof(file1)) + (sizeof(file2)) + 1));
  str = "/";
  strcpy(getPath, file1);
  strcat(getPath, str);
  strcat(getPath, file2);
  return getPath;
}


//copy directory to directory
void cpdir(char *from, char *to) {
  
  struct stat mode;
  //mkdir dir when not exist
  if ((lstat(to, &mode)) < 0)
    mkdir(to, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  //mkdir dir when duplicate
  else if (S_ISDIR(mode.st_mode)) {
    to = ispath(to, from);
    mkdir(to, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  //error
  else {
    error_msg("must be directory or created with -R or -r flag set.\n");
  }

  //open source directory
  DIR *dir = opendir(from);
  struct dirent *e = readdir(dir);

  while (e) {
    //set new paths
    char *newFrom = ispath(from,e->d_name);
    char *newTo = ispath(to, e->d_name);

    //check file
    if (lstat(newFrom, &mode) < 0)
      perror("mycp cannot open file");

    //directory, that's not . or ..
    if (S_ISDIR(mode.st_mode) && strcmp(e->d_name, ".") != 0 
         && strcmp(e->d_name, "..") != 0)
      cpdir(newFrom, newTo);

    //copy file only
    if (S_ISREG(mode.st_mode))
      mycp(newFrom, newTo);

    e=readdir(dir);
    free(newFrom);
    free(newTo);
  }
  closedir(dir);
}
//copy file to dir
void cpfiledir(char* from, char*to){
 
  struct stat mode;
  //open source directory
  DIR *dir = opendir(from);
  struct dirent *e = readdir(dir);

  while (e) {
    //set new paths
    char *newFrom = ispath(from,e->d_name);
    char *newTo = ispath(to, e->d_name);

    //get file
    if (lstat(newFrom, &mode) < 0)
      perror("mycp cannot open file");

    //if directory, that's not . or ..
    if (S_ISDIR(mode.st_mode) && strcmp(e->d_name, ".") != 0 
         && strcmp(e->d_name, "..") != 0)
      cpdir(newFrom, newTo);

    //copy fle
    if (S_ISREG(mode.st_mode))
      mycp(newFrom, newTo);

    e=readdir(dir);
    free(newFrom);
    free(newTo);
  }
  closedir(dir);
  /*DIR *dir = opendir(from);
  if(dir){
	struct dirent *e = readdir(dir);
	while(e){
		struct stat info;
		char* newS = ispath(from, e->d_name);
		char* newT = ispath(to, e->d_name);
		if(!stat(from, &info)){
			if(S_ISDIR(info.st_mode)){
      				cpdir(newS, newT);
			}	
			else if(S_ISREG(info.st_mode)){
      				mycp(newS, newT);
		}
	  
	   }

  	}

    } */

}

int main(int argc, char **argv) {

  int c, flag =0;
  struct stat buff;
  //extern int = must set this to 0
  opterr = 0;
  char *from, *to;
  
  //set rR flag
  //reference:
  //http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_lib_ref%2Fkeywords-O.html
  while ((c = getopt(argc, argv, "Rr")) != -1) {
    switch (c) {
      case 'R':
        	flag = 1;
        	break;
      case 'r':
        	flag = 1;
        	break;
      case '?':
        	error_msg("mycp file1 file2 OR mycp -R dir1 dir2..\n");
    }
  }

  //check arguments
  if (argc < 3) {
    error_msg("mycp file1 file2 OR mycp -R dir1 dir2..\n");
  }

  //set readable names
  from = argv[optind];
  to = argv[optind+1];

  //from format
  if ((lstat(from, &buff)) < 0) {
    error_msg("no file or directory..\n");
  }
  //file && -r or -R
  else if (flag == 1 && !S_ISDIR(buff.st_mode)) {
    error_msg("./mycp file1 file2 OR -R file1 file2..\n");
  }

  //just file
  else if (flag == 0 && S_ISREG(buff.st_mode))
    mycp(from, to);
	
  //file to dir without flag -R or -r
   else if (flag == 0 && S_ISDIR(buff.st_mode))
    cpfiledir(from, to);

  //directory && -r or -R
  else if (flag == 1 && S_ISDIR(buff.st_mode))
    cpdir(from, to);

  else {
    error_msg("mycp file1 file2 OR mycp -R dir1 dir2..\n");
  }

  return 0;
}
