#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 4096
//copy text file to text file function
void mycp(const char* from, const char* to)
{
  char buffer[SIZE];
  int in, out;
  ssize_t bytes;
  
  in = open(from, O_RDONLY);
  if(!in){
	fprintf(stderr, "cannot open file\n"); 
	exit(1);
  }
  out = open(to, O_WRONLY | O_CREAT | O_TRUNC,
               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

  while((bytes = read(in, buffer, SIZE)) > 0){
	if(write(out, buffer, SIZE) < bytes){
		fprintf(stderr, "error writing to a text file..\n");
		exit(1);
	}
  }

  if(bytes<0){
	fprintf(stderr, "error reading text file..\n"); 
	exit(1);
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
    fprintf(stderr, "Destination must be directory or created with -R flag set.\n");
    exit(1);
  }

  //open source directory
  DIR *d = opendir(from);
  struct dirent *de = readdir(d);

  while (de) {
    //set new paths
    char *newSource = ispath(from,de->d_name);
    char *newTarget = ispath(to, de->d_name);

    //get file characteristics
    if (lstat(newSource, &mode) < 0)
      perror("mycp cannot open file");

    //if directory, that's not . or .., recurse
    if (S_ISDIR(mode.st_mode) && strcmp(de->d_name, ".") != 0 
         && strcmp(de->d_name, "..") != 0)
      cpdir(newSource, newTarget);

    //if file, copy it
    if (S_ISREG(mode.st_mode))
      mycp(newSource, newTarget);

    de=readdir(d);
    free(newSource);
    free(newTarget);
  }
  closedir(d);
}

void cpfiledir(char* from, char*to){
 
  /*struct stat mode;
  //open source directory
  DIR *d = opendir(from);
  struct dirent *de = readdir(d);

  while (de) {
    //set new paths
    char *newSource = ispath(from,de->d_name);
    char *newTarget = ispath(to, de->d_name);

    //get file characteristics
    if (lstat(newSource, &mode) < 0)
      perror("mycp cannot open file");

    //if directory, that's not . or .., recurse
    if (S_ISDIR(mode.st_mode) && strcmp(de->d_name, ".") != 0 
         && strcmp(de->d_name, "..") != 0)
      cpdir(newSource, newTarget);

    //if file, copy it
    if (S_ISREG(mode.st_mode))
      mycp(newSource, newTarget);

    de=readdir(d);
    free(newSource);
    free(newTarget);
  }
  closedir(d);*/
  DIR *dir = opendir(from);
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

    }

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
        	fprintf (stderr, "mycp file1 file2 OR mycp -R dir1 dir2..\n");
        	exit(1);
    }
  }

  //check arguments
  if (argc < 3) {
    fprintf(stderr, "mycp file1 file2 OR mycp -R dir1 dir2..\n");
    exit(1);
  }

  //set readable names
  from = argv[optind];
  to = argv[optind+1];

  //from non-existant or incorrect format
  if ((lstat(from, &buff)) < 0) {
    fprintf(stderr, "no file or directory..\n");
    exit(1);
  }
  //file && -r
  else if (flag == 1 && !S_ISDIR(buff.st_mode)) {
    fprintf(stderr, "don't forget flag -R..\n");
    exit(1);
  }

  //just file
  else if (flag == 0 && S_ISREG(buff.st_mode))
    mycp(from, to);
  //file to dir
   else if (flag == 1 && S_ISDIR(buff.st_mode))
    cpfiledir(from, to);

  //directory && -r
  else if (flag == 1 && S_ISDIR(buff.st_mode))
    cpdir(from, to);

  else {
    fprintf(stderr, "mycp file1 file2 OR mycp -R dir1 dir2..\n");
    exit(1);
  }

  return 0;
}
