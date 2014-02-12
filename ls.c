#include <sys/stat.h> /* stat */
#include <sys/types.h> /* types */
#include <pwd.h> /* pwd */
#include <dirent.h> /* dirent */
#include <time.h> /* time */
#include <unistd.h> /* unistd */
#include <string.h> /*string */
#include <stdio.h>/*stdio*/


struct data_info /* struktura przechowujaca informacje o nazwie pliku i czasu jego modyfikacji */
{
	char *filename;
	int mktime;
};

void sw(struct data_info *data1, struct data_info *data2) /* funkcja swap dzialajaca na strukturze */
{
        struct data_info tmp;
        tmp.filename = data1->filename;
        tmp.mktime = data1->mktime;
        data1->filename = data2->filename;
        data1->mktime = data2->mktime;
        data2->filename = tmp.filename;
        data2->mktime = tmp.mktime;
}

void sort_t(const char *name) /* sortowanie po dacie dostepu */
{


	DIR *dir;
	struct dirent *dp;
	struct stat statbuf;

	if(name == NULL) /* jezei przekazana nazwa jest rowna null to bierzmy bierzacy katalog */
	{
		name = ".";
	}
	if((dir = opendir(name)) == NULL){
		perror("blad");
	}

	int ile=0;

	while((dp = readdir(dir))!=NULL)
	{
	    ile++; /* zliczamy ilosc plikow w katalogu */
	}
    printf("%d", ile);
	struct data_info tab[ile+4];

	int k=0;

	while((dp=readdir(dir))!=NULL)
	{
		stat(dp->d_name,&statbuf);

	        tab[k].mktime = statbuf.st_mtime; /* zapisywanie danych do struktury */
	        tab[k].filename = dp->d_name;
		k++;
	}

	int j,i;

	for(j=ile; j>0;j--) /* sortowanie */
	{
		int p=0;
		for(i=0;i<j;i++)
		{
			if(tab[i].mktime > tab[i+1].mktime)
			{
				sw(&tab[i],&tab[i+1]);
				p=0;
			}
		}
		if(p) break;
	}

	int l;
	for(l=0;l<ile;l++) /* wyswietlanie wynikow */
	{
		printf("%s\n",tab[l].filename);
	}


}


void show_normal(char *name) /* wyswietlanie normalnego widoku ls */
{

	DIR *dir;
	struct dirent *dp;
	struct stat   statbuf;

	printf("%s", name);

	if(name == 0)
	{
		name = ".";
	}

	if((dir = opendir(name)) == NULL)
	{
		perror("Blad");
	}
        while((dp = readdir(dir)) != NULL)
	{
		if(stat(dp->d_name,&statbuf) == -1)
			continue;

		 if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) /* pominiecie plikow . i .. */
		                 continue;



		printf("%s\n", dp->d_name);

	}
	closedir(dir);
	printf("dziala");
}

void show_l(const char *name) /* pokazanie szczegolowego widoku z prawami dostepu */
{

	DIR *d;
	struct dirent *de;
	struct stat buf;
	int i,j;
	char P[10]="rwxrwxrwx",AP[10]=" ";

	struct tm *t;
	char time[26];

	if(name == NULL)
	{
		name = ".";
	}

         if (!(d  = opendir(name)))
                    return;
         if (!(de = readdir(d)))
                    return;

	while((de=readdir(d)))
	{
	stat(de->d_name,&buf);

	if(S_ISDIR(buf.st_mode)) /* poszczegolne ify odpowiedzialne za wyluskanie istniejacych praw dla danego pliku */
		printf("d");
	else if(S_ISREG(buf.st_mode))
		printf("-");
	else if(S_ISCHR(buf.st_mode))
		printf("c");
	else if(S_ISBLK(buf.st_mode))
		printf("b");
	else if(S_ISLNK(buf.st_mode))
		printf("l");
	else if(S_ISFIFO(buf.st_mode))
		printf("p");
	else if(S_ISSOCK(buf.st_mode))
		printf("s");

	 for(i=0,j=(1<<8);i<9;i++,j>>=1)
     {
		 AP[i]= (buf.st_mode & j ) ? P[i] : '-' ;
	 }
	       printf("%s",AP);
	       printf("%5d",buf.st_nlink);
	       printf(" %8d",buf.st_size);
	       t=localtime(&buf.st_mtime);
	       strftime(time,sizeof(time),"%b %d %H:%M",t);
   	       printf(" %s",time);
	       printf(" %s\n",de->d_name);
    }
}
void show_rl(const char *name, int level, int option) /*rekurencyjne wywolanie wraz z prawami do plikow */
{
    DIR *dir;
    struct dirent *entry;
    struct stat buf;
    int i,j;
	char P[10]="rwxrwxrwx",AP[10]=" ";
	char time[26];
	struct tm *t;

	if(name == NULL)
	{
		name = ".";
	}
    if (!(dir = opendir(name)))
	           return;
	if (!(entry = readdir(dir)))
	           return;

	do {
    char path[1024];
	int len = snprintf(path, sizeof(path)-1, "%s/%s", name, entry->d_name);
	path[len] = 0;
    stat(entry->d_name,&buf);

		    if(S_ISDIR(buf.st_mode))  /*lista praw dostepu */
          		 printf("d");
        	else if(S_ISREG(buf.st_mode))
                    printf("-");
                else if(S_ISCHR(buf.st_mode))
                    printf("c");
                else if(S_ISBLK(buf.st_mode))
                    printf("b");
                else if(S_ISLNK(buf.st_mode))
                    printf("l");
                else if(S_ISFIFO(buf.st_mode))
                    printf("p");
                else if(S_ISSOCK(buf.st_mode))
                    printf("s");
                 for(i=0,j=(1<<8);i<9;i++,j>>=1)
                 {
                     AP[i]= (buf.st_mode & j ) ? P[i] : '-' ;
                 }
                    printf("%s",AP);
                   printf("%5d",buf.st_nlink);
                   printf(" %8d",buf.st_size);
                   t=localtime(&buf.st_mtime);
                   strftime(time,sizeof(time),"%b %d %H:%M",t);
                   printf(" %s",time);

		   printf(" %s\n",entry->d_name);


	  	       if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
	               continue;


            printf("%*s[%s]\n", level*2, "", entry->d_name);
			show_rl(path, level + 1,0); /* wejscie glebiej */


    }
	   while (entry = readdir(dir));
           closedir(dir);
}

char *rozmiar(double size, char *buf)
{
	int i = 0;
	const char* jednostki[] = {"B","kB","MB","GB"}; /* tablica jednostek */
	while(size > 1024)
	{
		size /= 1024; /* zamiana jednostek */
		i++;
	}
	sprintf(buf, "%.*f %s", i, size, jednostki[i]);
	return buf;
}

void show_size(const char * name)
{
	DIR *dir;
	struct dirent *dp;
	struct stat statbuf;
	char buf[10];
    /* standardowe procedury czytania katalogow */

	if(name == NULL)
	{
		name = ".";
	}

	if((dir = opendir(name)) == NULL)
	{
		perror("Blad");
	}
	while((dp=readdir(dir))!=NULL)
	{
		if(stat(dp->d_name, &statbuf) == -1)
			continue;

 	if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
	                        continue;

		lstat(dp->d_name, &statbuf);
		printf("%s %s\n", dp->d_name, rozmiar(statbuf.st_size,buf)); /* wywolanie funkcji wyswietlajacej jednostki */
        }
}

void  information(int a)
{
	//a == 1 - help
	//a == 0 - version

	if(a==0)
	{
	    printf("\n v.0.95b \n\n");
	}
	if(a==1)
	{
		printf("\n Opcje \n");
		printf(" -l szczegolowy wydok \n -r rekursyjne przegladanie \n -t sortowanie po dacie modyfikacji \n -h rozmiar plikow \n --version wersjai\n\n");
	}
}

int main(int argc,char **argv)
{
	if(argc == 0)
	{
		argv[1] == ".";
	}
	    int l_flag=0,r_flag=0,t_flag=0,h_flag=0,help_flag=0,version_flag=0,hf_flag=0;

	    int opt;


	    if(strcmp(argv[1],"--help") == 0)
	    {
	            help_flag= 1;
		    information(1);
	    }
	    else if(strcmp(argv[1], "--version") == 0)
	    {
		    version_flag = 1;
		    information(0);
	    }
	    if(strcmp(argv[1],"--help")!=0 && strcmp(argv[1],"--version")!=0)
	    {

                while((opt = getopt(argc,argv,"lRlthhelpversion"))!=-1)
                {
                    switch(opt)
                    {
                        case 'l':
                              l_flag = 1;
                            break;
                        case 'r':
                             r_flag = 1;
                                break;
                        case 't':
                         t_flag = 1;
                        break;
                        case 'h':
                            hf_flag = 1;
                        break;
                        }

                }

                if(l_flag==1)
                {
                    show_l(argv[2]);
                }
                if(l_flag == 1 && r_flag==1)
                {
                    show_rl(argv[2],0,1);
                }
                if(t_flag == 1)
                {
                    sort_t(argv[2]);
                }
                if(hf_flag ==1)
                {
                    show_size(argv[2]);
                }
        }

        if(hf_flag == 0 && l_flag == 0 && r_flag == 0 && t_flag == 0 && h_flag == 0 && help_flag == 0 && version_flag == 0)
	    {
		    char *katalog;

		    if(argc == 0)
		    {
		      katalog = ".";

		    }
		    else
		    {
			   katalog = argv[1];
		    }

		    show_normal(katalog);
		}

    return 0;

}
