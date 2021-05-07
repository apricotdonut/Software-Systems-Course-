#include  <stdio.h>
#include  <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>



//part 1

static void displayStat (const struct stat *sb)
{		
	struct group *grp;
	struct passwd *pwd;
	
	printf("1. Mode: %lo\n", (unsigned long) sb->st_mode);
	if (sb->st_mode & (S_ISUID|S_ISGID|S_ISVTX))
		printf("    special bits set:	%s%s%s\n",
			(sb->st_mode & S_ISUID) ? "set-UID " : "",
			(sb->st_mode & S_ISGID) ? "set-GID " : "",
			(sb->st_mode & S_ISVTX) ? "sticky " : "");
	//2. number of links
	printf("2. Number of (hard) links: %ld\n", (long) sb->st_nlink);
	//3. owner's name (string)
	if ((pwd = getpwuid(sb->st_uid)) != NULL)
		printf("3. Owner Name: %-8.8s\n", pwd->pw_name);
	else
		printf(" %-8d\n", sb->st_uid);
	//4. group name (string)
	if ((grp = getgrgid(sb->st_gid)) != NULL)
		printf("4. Group Name: %-8.8s\n", grp->gr_name);
	else
		printf(" %-8d\n", sb->st_gid);
	//5. size in bytes
	printf("5. File size: %lld bytes\n", (long long) sb->st_size);
	//6. size in blocks
	printf("6. 512B blocks allocated: %lld\n", (long long) sb->st_blocks);
	//7. last modification time
	printf("7. Last file modification: %s", ctime(&sb->st_mtime));
	
	
	
}

//part 2
static void dirTraversal(const char * dirname)
{
    struct dirent * dir_entries;
    DIR * directory;
    char fullpath[256];

    if ((directory = opendir(dirname)) == NULL) {
    	//error
		
		if (errno == EACCES)
			printf ("Permission denied.\n");

    	return;
    }
     //read for entries
    while ((dir_entries = readdir(directory)) != NULL){
    	//ignoring . and ..
        if(strcmp(dir_entries->d_name,".") != 0 && strcmp(dir_entries->d_name,"..") != 0){
        	//check if it is directory
            if(dir_entries->d_type == DT_DIR){
            	//cd into directory and run recursively
                chdir(dirname);
                dirTraversal(dir_entries->d_name);
                chdir("..");
            }
            else
            //must be a file so print out file name
            {	    				
				char file[4096]; 
				getcwd(file, 4096);
				printf("\nCurrent working dir: %s\n", file);
				
				strcpy(fullpath, dirname);
				strcat(fullpath, "/");
				strcat(fullpath, dir_entries->d_name);
				printf("Path: %s\n", fullpath);

		
				const char * myfile;
				myfile = fullpath;
				struct stat sb;

				if (lstat(myfile, &sb) == -1){// lstat does not follow symbolic link
						perror("lstat");
				} else {
					displayStat(&sb);
					//8. Name
					printf("8. Name: %s\n", dir_entries->d_name);

				}
				

            }
        }
        
    }
    closedir(directory);
    return;
}

int main(int argc, char *argv[]){
	
	struct stat buf;
    if (lstat(argv[1], &buf) == -1){// lstat does not follow symbolic link
		perror("lstat");
	} else {
		if(S_ISDIR(buf.st_mode)){ //directory
			printf("It's a directoy\n");
			dirTraversal(argv[1]);
			
		} else{ //file
			printf("It's a file\n");
			displayStat(&buf);
			//8. Name
			printf("8. Name: %s\n", argv[1]);
		}
		

	}


	
}