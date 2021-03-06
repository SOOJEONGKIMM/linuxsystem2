#include"./ssu_mntr.h"


//void scanmondir(char *searchdir,int depth,int sizeoptflag,int indentinit,char *delcurdir);
int update;
int bfcnt, nfcnt;

int main(void){
    char wdir[PATH_SIZE];
    char mondir[PATH_SIZE];
    //    char logpath[PATH_SIZE];//log.txt path name
    pid_t pid;

    memset(wdir,0,PATH_SIZE);
    memset(mondir,0,PATH_SIZE);

    getcwd(wdir,PATH_SIZE);//***절대경로 상대경로 입력시 모두 동작하도록 수정해야함


    if(chdir(wdir)<0){//returns 0 if success
	fprintf(stderr,"DIR:%s can't be found.\n",curdir);
	//exit(1);
    }
    //백업디렉토리(서브디렉토리) 이름을 지정해서 생성.
    sprintf(mondir,"%s/check",wdir);
    //chdir(mondir);
    //    mkdir(checkdir,0744);
    // printf("checkdir:%s\n",mondir);
    scanmondirBASE(mondir,1);
    startdemon(wdir,mondir);
    //chdir(wdir);
    /*switch(pid=fork()){
      case 0:
    // printf("I'm child. My PID is %d\n",getpid());
    startdemon(wdir,mondir);
    default:
    ssu_mntr_play();
    }*/

    //옵션입력으로 넘어감.
    return 0;
}
void startdemon(char *curdir,char *checkdir){
    char wdir[PATH_SIZE];
    char mondir[PATH_SIZE];
    memset(wdir,0,PATH_SIZE);
    memset(mondir,0,PATH_SIZE);
    getcwd(wdir,PATH_SIZE);//***절대경로 상대경로 입력시 모두 동작하도록 수정해야함
    sprintf(mondir,"%s/check",wdir);
    int bfcnt,nfcnt;
    int fd, maxfd;

    pid_t pid;

    //printf("forlogtxt");
    //printf(file);//debug

    //fork()로 자식 프로세스를 생성한다. 
    //이 함수는 한 번 호출되나 두 개의 리턴값을 리턴하는 함수다.
    //자식에게 리턴하는값은 0, 부모에게는 새 자식프로세스의 ID. 
    if((pid=fork())<0){//프로세스 생성 실패시 -1리턴.
	fprintf(stderr,"pid fork error\n");
	exit(1);
    }
    else if(pid!=0){
	ssu_mntr_play();
	exit(0);
    }
    pid=getpid();
    // printf("process running as daemon..\n");
    //터미널 종료시 signal의 영향을 받지 않는다.
    signal(SIGHUP, SIG_IGN);
    close(0);//STDIN_FILENO
    close(1);//STDOUT_FILENO
    close(2);//STDERR_FILENO
    maxfd=getdtablesize();

    for(fd=0;fd<maxfd;fd++)
	close(fd);

    /*if(chdir("/")==-1){
      printf("chdir error\n");
      exit(1);
      }*/

    umask(0);
    //setsid로 새로운 세션만들고,
    //현재프로세스(자식)의 세션의 PID가 제어권을 가지도록 한다.
    if(setsid()==-1){
	fprintf(stderr,"set sid error\n");
	exit(0);
    }

    fd=open("/dev/null",O_RDWR);
    dup(0);
    dup(0);

    while(1){
	sleep(1);
	printf("check:%s\n",mondir);
	//	chdir(ckdir);
	scanmondirNEW(mondir,1);//1초 주기로 new스캔+base&new비교
	forlogtxt();
	//	chdir(curdir);

	if(update==1){//if update, scan base again.
	    update=0;//init
	    printf("update:%d\n",update);
	    scanmondirBASE(mondir,1);
	}


    }

}
void forlogtxt(void){//cmp base&new 

    printf("-------------------------COMPARE STARTS----------------------\n");

    int newfile,deleted; 

    if(bfcnt==nfcnt){//MODIFY LOG
	if(update!=1){
    MnNode *Nmodnode;
	    Nmodnode=mnhead;

	    while(Nmodnode!=NULL){
		if(is_modified(Nmodnode->mtime,Nmodnode->inum,Nmodnode->listfname)){//but mtime change
		    write_logtxt(Nmodnode->listfname,"modify",NULL);
		    update=1;
		    printf("MODIFY LOG!!!!!!!!!!!!!!!!\n");
		}

		Nmodnode=Nmodnode->next;
	    }
	    list_print1(1);
	    list_print1(0);
	    //free(Nmodnode);
	}
    }

    if(bfcnt<nfcnt){//CREATE LOG
	if(update!=1){
	MnNode *Ncrenode;
	Ncrenode=mnhead;
	while(Ncrenode!=NULL){
	    if(list_search(Ncrenode->listfname,1)==0){
		write_logtxt(Ncrenode->listfname,"create",NULL);
		update=1;
		printf("DELETE LOG!!!!!!!!!!!!!!!!\n");
	    }
	    Ncrenode=Ncrenode->next;
	}
    //free(Ncrenode);
    }
    }
    if(bfcnt>nfcnt){//DELETE LOG 
	if(update!=1){
	    MNode *delnode;
	    delnode=mhead;

	    //DELETE LOG: base 기준으로 
	    while(delnode!=NULL){

		if(list_search(delnode->listfname,0)==0){
		    write_logtxt(delnode->listfname,"delete",NULL);
		    update=1;
		    printf("DELETE LOG!!!!!!!!!!!!!!!!\n");
		}
		delnode=delnode->next;

	    }
	   // free(delnode);
    }
}


}


int scanmondirBASE(char *searchdir,int inityes){
    if(inityes==1){

	free_list(mhead);	

	mhead=NULL;////init first!   
	bfcnt=0;
    }

    char *dirptr;
    dirptr=searchdir+strlen(searchdir);
    *dirptr++='/';
    *dirptr='\0';
    struct tm *t;//시간값 표현하기 위한 구조체

    char dirpath[PATH_SIZE];
    char recurdirpath[PATH_SIZE];

    struct stat tempstat;
    int i;   
    char listfname[FILE_SIZE];
    int fsize;//SIZE
    int fcnt=0;

    struct stat buf;//SIZE   

    int countdirp=0;
    struct dirent **flist;

    if((countdirp=scandir(searchdir,&flist,0,alphasort))<0){
	fprintf(stderr,"scandir error for %s\n",searchdir);
	exit(1);
    }
    i=0;

    while(i<countdirp){
	MNode *node=(MNode*)malloc(sizeof(MNode));
	memset(node,0,sizeof(node));
	if(!strcmp(flist[i]->d_name,".")||!strcmp(flist[i]->d_name,"..")){
	    i++;
	    continue;
	}
	if(!strcmp(flist[i]->d_name,".git")){
	    i++;
	    continue;
	}
	if(!strcmp(flist[i]->d_name,"Makefile")){
	    i++;
	    continue;
	}
	if(!strcmp(flist[i]->d_name,"a.out")){
	    i++;
	    continue;
	}
	if(!strcmp(flist[i]->d_name,"log.txt")){
	    i++;
	    continue;
	}
	strcpy(dirptr,flist[i]->d_name);
	bfcnt+=1;
	fsize=0;
	fsize=stat(searchdir,&buf);//SIZE 
	if(S_ISREG(buf.st_mode)){
	    memset(node->listfname,0,PATH_SIZE);
	    strcpy(node->listfname,flist[i]->d_name);

	    memset(node->listfpath,0,PATH_SIZE);
	    strcpy(node->listfpath,searchdir);

	    node->fsize=0;
	    node->fsize=buf.st_size;//SIZE

	    memset(node->ctime,0,TM_SIZE);
	    strftime(node->ctime,TM_SIZE,"%Y-%m-%d %H:%M:%S",localtime(&(buf.st_ctime)));

	    memset(node->mtime,0,TM_SIZE);
	    strftime(node->mtime,TM_SIZE,"%Y-%m-%d %H:%M:%S",localtime(&(buf.st_mtime)));

	    node->inum=buf.st_ino;//inode num

	    Mlist_insert(node);
	}

	if((buf.st_mode&S_IFDIR)==S_IFDIR){

	    scanmondirBASE(searchdir,0);

	}
	free(flist[i]);

	i++;
    }
    /* int j;
       for(j=0;j<countdirp;j++){
       free(flist[j]);
       }
       free(flist[j]);*/
    // indent--;
    // chdir("..");
    dirptr[-1]=0;
    //printf("-------------------------scanning dir BASE ENDS\n");
    return 0;
    }
    int scanmondirNEW(char *searchdir,int inityes){
	if(inityes==1){
	    mnhead=NULL;////init first!    
	    nfcnt=0;
	}

	// printf("-------------------------scanning dir NEW STARTS\n");
	char *dirptr;
	dirptr=searchdir+strlen(searchdir);
	*dirptr++='/';
	*dirptr='\0';
	struct tm *t;//시간값 표현하기 위한 구조체

	char dirpath[PATH_SIZE];
	char recurdirpath[PATH_SIZE];

	struct stat tempstat;
	// struct timeval *renamet;    
	int i;   
	char listfname[FILE_SIZE];
	int fsize;//SIZE
	int fcnt=0;

	struct stat buf;//SIZE   

	int countdirp=0;
	struct dirent **flist;

	if((countdirp=scandir(searchdir,&flist,NULL,alphasort))<0){
	    fprintf(stderr,"scandir error for %s\n",searchdir);
	    //exit(1);
	}
	i=0;
	while(i<countdirp){
	    MnNode *node=(MnNode*)malloc(sizeof(MnNode));
	    memset(node,0,sizeof(node));
	    if(!strcmp(flist[i]->d_name,".")||!strcmp(flist[i]->d_name,"..")){
		i++;
		continue;
	    }
	    if(!strcmp(flist[i]->d_name,".git")){
		i++;
		continue;
	    }
	    if(!strcmp(flist[i]->d_name,"Makefile")){
		i++;
		continue;
	    }
	    if(!strcmp(flist[i]->d_name,"a.out")){
		i++;
		continue;
	    }
	    if(!strcmp(flist[i]->d_name,"log.txt")){
		i++;
		continue;
	    }
	    strcpy(dirptr,flist[i]->d_name);
	    nfcnt+=1;
	    fsize=0;
	    fsize=stat(searchdir,&buf);//SIZE
	    if(S_ISREG(buf.st_mode)){
		memset(node->listfname,0,PATH_SIZE);
		strcpy(node->listfname,flist[i]->d_name);

		memset(node->listfpath,0,PATH_SIZE);
		strcpy(node->listfpath,searchdir);

		node->fsize=0;
		node->fsize=buf.st_size;//SIZE


		memset(node->ctime,0,TM_SIZE);
		strftime(node->ctime,TM_SIZE,"%Y-%m-%d %H:%M:%S",localtime(&(buf.st_ctime)));

		memset(node->mtime,0,TM_SIZE);
		strftime(node->mtime,TM_SIZE,"%Y-%m-%d %H:%M:%S",localtime(&(buf.st_mtime)));

		node->inum=buf.st_ino;//inode num

		Mnlist_insert(node);
	    }

	    if((buf.st_mode&S_IFDIR)==S_IFDIR){

		scanmondirNEW(searchdir,0);

	    }
	    free(flist[i]);

	    i++;
	}
	/*int j;
	  for(j=0;j<countdirp;j++){
	  free(flist[j]);
	  }
	  free(flist[j]);*/
	// indent--;
	// chdir("..");
	dirptr[-1]=0;
	return 0;
    }

    void Mlist_insert(MNode *newNode){//list에 node추가
	newNode->next=NULL;
	if(mhead==NULL)
	    mhead=newNode;
	else{
	    MNode *listF;
	    listF=mhead;
	    while(listF->next!=NULL)
		listF=listF->next;
	    listF->next=newNode;
	}
    }
    void Mnlist_insert(MnNode *newNode){//list에 node추가
	newNode->next=NULL;
	if(mnhead==NULL)
	    mnhead=newNode;
	else{
	    MnNode *listF;
	    listF=mnhead;
	    while(listF->next!=NULL)
		listF=listF->next;
	    listF->next=newNode;
	}
    }
    void get_time(char *str,char *status){
	char timestr[TM_SIZE];
	time_t timer=time(NULL);
	struct tm *t=localtime(&timer);

	memset(timestr,0,TM_SIZE);
	memset(str,0,TM_SIZE);

	sprintf(timestr,"[%04d-%02d-%02d %02d:%02d:%02d]",t->tm_year + 1900, t->tm_mon+1,t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	strcpy(str,timestr);
    }
    void write_logtxt(char *fname, char *status,char *mtimeifmod){
	//함수호출전에 반드시 curdir로, 함수호출후에 원래workingdir으로 반드시 바꿔놓을것.
	char timestr[TM_SIZE];
	char wdir[PATH_SIZE];
	char mondir[PATH_SIZE];
	memset(wdir,0,PATH_SIZE);
	memset(mondir,0,PATH_SIZE);
	getcwd(wdir,PATH_SIZE);
	sprintf(mondir,"%s/check",wdir);
	FILE *fp;
	char *logfname="log.txt";
	if((fp=fopen(logfname,"a"))<0){
	    fprintf(stderr,"fopen %s error",fname);
	    exit(1);
	}
	get_time(timestr,status);//current time
	fseek(fp,0,SEEK_END);
	fprintf(fp,"%s [%s _%s]\n",timestr,status,fname);
	fclose(fp);
    }
    int list_search(char *cmpfname,int searchisbase){
	if(searchisbase==1){//compare with base list 
	    MNode *searchnode=mhead;//(create log)
	    while(searchnode){
		if(!strcmp(searchnode->listfname,cmpfname))
		    return 1;//existing file
		searchnode=searchnode->next;
	    }
	    return 0;//new file(CREATE LOG)
	}
	if(searchisbase==0){//compare with new list 
	    MnNode *searchnode=mnhead;//(delete log)
	    while(searchnode){
		if(!strcmp(searchnode->listfname,cmpfname))
		    return 1;
		searchnode=searchnode->next;
	    }
	    return 0;
	}
    }
    int is_modified(char *cmpmtime,int cmpinum,char *cmpfname){
	MNode *searchnode=mhead;
	while(searchnode){
	    if(searchnode->inum==cmpinum){
		if(strcmp(searchnode->mtime,cmpmtime) || strcmp(searchnode->listfname, cmpfname)){
		    printf("IS_MODIFY%s\n", searchnode->listfname);
		    return 1; //modified.
		}
	    }
	    searchnode=searchnode->next;
	}
	return 0;//not existing modtime//is modified! (MODIFY LOG) 
    }
    void list_print1(int bnodeyes){
	if(bnodeyes==1){
	    MNode *printnode=mhead;
	    while(printnode){
		printf("inum:%d,fname:%s,mod:%s\n",printnode->inum,printnode->listfname,printnode->mtime);
		printnode=printnode->next;
	    }
	}
	if(bnodeyes==0){
	    MnNode *rintnode=mnhead;
	    while(rintnode){
		printf("inum:%d,fname:%s,mod:%s\n",rintnode->inum,rintnode->listfname,rintnode->mtime);
		rintnode=rintnode->next;
	    }
	}
    }
    void free_list(MNode* mhead){
	MNode *tmp;
	while(mhead!=NULL){
	    tmp=mhead;
	    mhead=mhead->next;
	    free(tmp);
	}
    }
