#include "ssu_mntr.h"

int deleteOpt=0;
int sizeOpt=0;
int recoverOpt=0;
int treeOpt=0;
int exitOpt=0;
int helpOpt=0;
int invalidOpt=0;

int deloptI=0;
int deloptR=0;

int optldt_cmp(Node *a, Node *b) {
    return a->optldt - b->optldt;
}

void ssu_mntr_play(void){
    char cmdbuf[BUFFER_SIZE];
    int cmd;


    while(1){
	memset((char*)cmdbuf,0,BUFFER_SIZE);

	//printf("d:%d,s:%d,r:%d,t:%d,e:%d,h:%d,inv:%d",deleteOpt,sizeOpt,recoverOpt,treeOpt,exitOpt,helpOpt,invalidOpt);//debug
	printf("20162969>");
	fgets(cmdbuf,BUFFER_SIZE,stdin);

	if(cmdbuf[0]=='\n'){//엔터만 입력시 프롬프트 재출력
	    printf("entered");
	    continue;
	}
	cmdbuf[strlen(cmdbuf)-1]=0;//개행문자제거 
	check_opt(cmdbuf);
	if(deleteOpt&!sizeOpt&!recoverOpt&!treeOpt&!exitOpt&!helpOpt&!invalidOpt){
	    deleteOpt=0;
	    get_deleteOpt(cmdbuf);
	}
	else if(!deleteOpt&sizeOpt&!recoverOpt&!treeOpt&!exitOpt&!helpOpt&!invalidOpt){
	    sizeOpt=0;
	    do_sizeOpt(cmdbuf);
	}
	else if(!deleteOpt&!sizeOpt&recoverOpt&!treeOpt&!exitOpt&!helpOpt&!invalidOpt){
	    recoverOpt=0;
	    do_recoverOpt(cmdbuf);
	}
	else if(!deleteOpt&!sizeOpt&!recoverOpt&treeOpt&!exitOpt&!helpOpt&!invalidOpt){
	    treeOpt=0;
	    do_treeOpt(cmdbuf);
	}
	else if(!deleteOpt&!sizeOpt&!recoverOpt&!treeOpt&exitOpt&!helpOpt&!invalidOpt){
	    exit(0);
	}
	else if(!deleteOpt&!sizeOpt&!recoverOpt&!treeOpt&!exitOpt&helpOpt&!invalidOpt){
	    helpOpt=0;
	    do_helpOpt(cmdbuf);
	}
	else if(!deleteOpt&!sizeOpt&!recoverOpt&!treeOpt&!exitOpt&!helpOpt&invalidOpt){
	    invalidOpt=0;
	    do_helpOpt(cmdbuf);
	    continue;
	}
    }
}
int check_opt(const char *str){
    char trim[31];
    char cmdbuf[BUFFER_SIZE];
    memset((char*)trim,0,sizeof((char*)trim));
    for(int i=0;i<strlen(str)&&str[i]!=' ';i++){
	trim[i]=str[i];
    }
    while(1){
	if(!strcmp(trim,"delete")){
	    deleteOpt=1;
	    break;
	}
	if(!strcmp(trim,"size")){
	    sizeOpt=1;
	    break;
	}
	if(!strcmp(trim,"recover")){
	    recoverOpt=1;
	    break;
	}
	if(!strcmp(trim,"tree")){
	    treeOpt=1;
	    break;
	}
	if(!strcmp(trim,"exit")){
	    exitOpt=1;
	    break;
	}
	if(!strcmp(trim,"help")){
	    helpOpt=1;
	    break;
	}
	else{
	    invalidOpt=1;
	    return -1;
	}
    }
    return 1;

}

int get_deleteOpt(char *str){//DELETE [FILENAME] [ENDTIME] [OPTION]
    char trashdir[PATH_SIZE];
    char filesdir[PATH_SIZE];//backup deleting file
    char infodir[PATH_SIZE];//deleting file info dir
    char infotxt[PATH_SIZE];//onlyfname을 info dir 경로로 
    char curdir[PATH_SIZE];
    char checkdir[PATH_SIZE];
    char fnamepath[PATH_SIZE];//deleting file name path
    //    char onlyfname[PATH_SIZE];//deleting file name
    char newdir[PATH_SIZE];//newdir path buf for 'files'dir
    char endtimestr[TM_SIZE];
    char enddatestr[TM_SIZE];
    char delopt[OPT_SIZE];
    struct stat statbuf;
    struct stat infobuf;
    // struct timeval *renamet;
    struct tm delt;
    struct tm *t;
    int i;
    int endtimeExist=1;
    deloptI=deloptR=0;//init delopt
    FILE *fp;

    int len=strlen(str);
    //trim option cmd part
    for(i=0;i<strlen(str);i++){
	if(str[i]==' ')
	    break;  
    }
    while(i<len && str[i]==' ')
	i++;
    memset((char*)onlyfname,0,PATH_SIZE);
    //인자로 얻은 str에서 [FILENAME]만 추출
    int a=0;
    int j;
    for(j=i;j<len && str[j]!=' ';j++){
	onlyfname[a]=str[j];
	a++;
	i++;
    }
    printf("onlyfname:%s",onlyfname);
    i=j;
    //[ENDTIME] 추출 
    while(i<len && str[i]==' ')
	i++;
    if(i>=len){//[ENDTIME]주어지지않은경우
	endtimeExist=0;
	printf("\nendTE:%d\n",endtimeExist);
    }
    else{
	endtimeExist=1;
	a=0;
	for(j=i;j<len && str[j]!=' ';j++){//2020-05-05
	    if(str[j]=='-'&&str[j+1]=='i'){//[ENDTIME]없이 바로 -i만 들어온 경우
		endtimeExist=0;
		printf("came here\n");
		break;
	    }
	    enddatestr[a]=str[j];
	    //   printf("enddate:%c\n",enddatestr[a]);
	    a++;
	    i++;
	}
	if(endtimeExist==1){
	    i=j;
	    while(i<len && str[i]==' ')
		i++;
	    a=0;
	    for(j=i;j<len && str[j]!=' ';j++){//10:00
		endtimestr[a]=str[j];
		a++;
		i++;
	    }
	    i=j;
	    printf("enddatestr:%s\n",enddatestr);
	    printf("endtimestr:%s\n",endtimestr);
	}
    }
    //[OPTION] 추출 
    i=j;
    while(i<len && str[i]==' ')
	i++;
    a=0;
    memset(delopt,0,OPT_SIZE);
    for(j=i;j<len && str[j]!=' ';j++){//-r -i
	delopt[a]=str[j];
	a++;
	i++;
    }
    printf("delopt:%s\n",delopt);
    i=j;
    if(!strcmp(delopt,"-i")){
	deloptI=1;//전역변수로 선언하고, do_deleteOpt()에서 옵션수행할때 다시 플래그 0으로 초기화할것.
    }
    if(!strcmp(delopt,"-r")){
	deloptR=1;
    }

    printf("deloptR:%d\n",deloptR);
    printf("\nendTE:%d",endtimeExist);
    if(endtimeExist==0)
	do_deleteOpt();
    else{
	//	char alarmTmin[TM_SIZE];
	char endtimehour[TM_SIZE];
	char endtimemin[TM_SIZE];
	struct tm alarmsetT;
	int waitmin=0;
	printf("setting alarm\n");
	time_t alarmsett=time(NULL);
	alarmsetT=*localtime(&alarmsett);
	printf("alarmsetT:%d일 %d:%d:%d\n",alarmsetT.tm_mday,alarmsetT.tm_hour,alarmsetT.tm_min,alarmsetT.tm_sec);
	//sprintf(alarmTmin,"%d",alarmsetT.tm_min);
	j=0;
	int i=0;
	for(i=0;i<strlen(endtimestr)&&endtimestr[i]!=':';i++){//10:30 hour
	    endtimehour[i]=endtimestr[i];
	    j++;
	}
	printf("Pendtimehour:%s\n",endtimehour);
	while(i<strlen(endtimestr) && endtimestr[i]==':')
	    i++;
	//j=0;
	for(int i=0;i<strlen(endtimestr)&&endtimestr[i]!=' ';i++){//10:30 min
	    endtimemin[i]=endtimestr[j+1];
	    j++;
	}
	int endtimeminInt=atoi(endtimemin);
	waitmin=endtimeminInt-alarmsetT.tm_min;
	if(waitmin<0){
	    printf("[ENDTIME] is set wrong.\n");
	    ssu_mntr_play();
	}
	printf("waitSec:%d\n",waitmin*60);
	int k=3;
	signal(SIGALRM,deloptR_alarm);
	alarm(waitmin*60);//예약시간이 되면 alarm이 SIGALRM을 전송한다.


	printf("after alarm\n");
    }



    return 0;
}

//endtimestr[] ENDTIME예정시각에 
//do_deleteOpt()호출 
void deloptR_alarm(int k){
    pid_t pid;
    int status;
    char recheckbuf[OPT_SIZE];
    printf("alarm ringing\n");
    //vfork 생성 (자식/부모프로세스에서 삭제작업 진행)
    switch(pid=fork()){
	case 0:
	    printf("I'm child. My PID is %d\n",getpid());
	    if(deloptR==1){//do_deleteOpt()호출 전 재확인 문구
		while(1){
		    printf("Delete [y/n]?");
		    memset(recheckbuf,0,OPT_SIZE);
		    fgets(recheckbuf,OPT_SIZE,stdin);
		    recheckbuf[strlen(recheckbuf)-1]=0;//개행문자제거 
		    if(!strcmp(recheckbuf,"y")){
			do_deleteOpt();
			exit(0);//자식프로세스를 죽여야함.
		    }
		    else if(!strcmp(recheckbuf,"n"))
			ssu_mntr_play();
		    else
			continue;
		}
	    }
	    else{
		printf("no -r option, working on delete in ENDTIME..\n");
		do_deleteOpt();
		exit(0);//자식프로세스를 죽여야함.
	    }
	    //case -1:
	    //	fprintf(stderr,"vfork error\n");
	    /*	default: //parent process
		while(wait(&status)!=pid)
		continue;
		break;*/
    }


    //after Ropt, kill vfork (부모가 죽지 않게 하기 위해서)
    // kill(getpid(),SIGKILL);
}


int do_deleteOpt(void){//DELETE [FILENAME] [ENDTIME] [OPTION]
    char trashdir[PATH_SIZE];
    char filesdir[PATH_SIZE];//backup deleting file
    char infodir[PATH_SIZE];//deleting file info dir
    char infotxt[PATH_SIZE];//onlyfname을 info dir 경로로 
    char curdir[PATH_SIZE];
    char checkdir[PATH_SIZE];
    char fnamepath[PATH_SIZE];//deleting file name path
    //    char onlyfname[PATH_SIZE];//deleting file name
    char newdir[PATH_SIZE];//newdir path buf for 'files'dir
    struct stat statbuf;
    struct stat infobuf;
    // struct timeval *renamet;
    struct tm delt;
    struct tm *t;
    int i;
    FILE *fp;
    memset(trashdir,0,PATH_SIZE);
    memset(filesdir,0,PATH_SIZE);
    memset(infodir,0,PATH_SIZE);
    memset(infotxt,0,PATH_SIZE);
    memset(curdir,0,PATH_SIZE);
    memset(checkdir,0,PATH_SIZE);
    memset(fnamepath,0,PATH_SIZE);
    memset(newdir,0,PATH_SIZE);
    getcwd(curdir,PATH_SIZE);
    //printf("onlyfname:%s\n",onlyfname);
    //strcpy(curdir,getcwd(NULL,0));//***절대경로 상대경로 입력시 모두 동작하도록 수정해야함
    sprintf(checkdir,"%s/check",curdir);
    //strcpy(checkdir,getcwd(NULL,0));
    //printf("curdir:%s\n",curdir);
    //printf("trashdir:%s\n",trashdir);
    //printf("deloptI:%d\n",deloptI);

    chdir(curdir);
    sprintf(infodir,"%s/trash/info",curdir);
    sprintf(filesdir,"%s/trash/files",curdir);

    scanningTdir(infodir);
    scanningTdir(filesdir);
    chdir(curdir);
    scanningCdir();

    if(chdir(checkdir)<0){//returns 0 if success
	fprintf(stderr,"KDIR:%s can't be found.\n",checkdir);
	perror("chdir");
	exit(1);
    }

    if(access(onlyfname,F_OK)!=0){
	printf("%s is not a existing file\n",onlyfname);
	//ssu_mntr_play();
    }
    //삭제파일의 절대경로 가져옴
    if(realpath(onlyfname,fnamepath)==NULL){
	fprintf(stderr,"realpath error for %s\n",onlyfname);
	//exit(1);
    }
    //입력받은 파일이 정상적으로 존재하는 파일인지 확인
    if(lstat(fnamepath,&statbuf)<0){
	fprintf(stderr,"lstat error for %s\n",fnamepath);
	//exit(1);
    }
    if(!S_ISREG(statbuf.st_mode)){
	fprintf(stderr,"stat error for %s, not a regular file\n",fnamepath);
	//exit(1);
    } 
    //중복 체크
    //trash dir스캔해서 입력파일과 동일한 이름이 있는지 확인
    Node *printdup=(Node*)malloc(sizeof(Node));
    memset(printdup,0,sizeof(printdup));
    printdup=head;
    int overlapped=0;

    int dupindex=0;

    //printf("checkoverlap:%s\n",checkoverlap);
    while(printdup){
	if(!strcmp(printdup->listfname,onlyfname)){
	    printf("overlap exists\n");
	    //if(strlen(printdup->dupped)!=0)
	    overlapped=1;
	    printf("overlapped:%d\n",overlapped);
	    //printf("%d.  %s\n    %s    %s\n",dupindex,onlyfname,printdup->dtime,printdup->mtime);
	    dupindex++;
	    printdup->dupindex=dupindex;//중복횟수 셈
	    printf("printdup->listfname:%s\n",printdup->listfname);
	    printf("printdup->dupindex:%d\n",printdup->dupindex);
	}
	printdup=printdup->next;
    }


    if(deloptI==1){
	int delI=remove(fnamepath);
	if(delI==0)
	    printf("%s deleted with i option.\n",fnamepath);
	else
	    printf("%s failed to delete with i option.\n",fnamepath);
    }
    else if(deloptI==0){
	//printf("entered here");
	memset(trashdir,0,PATH_SIZE);
	memset(filesdir,0,PATH_SIZE);
	memset(infodir,0,PATH_SIZE);
	//trash dir 이름을 지정해서 생성.
	if(getcwd(trashdir,PATH_SIZE)==0){
	    fprintf(stderr,"getcwd error to %s\n",trashdir);
	    exit(1);
	}
	//printf("trashdir:%s\n",trashdir);
	sprintf(trashdir,"%s/trash",curdir);
	mkdir(trashdir,0744);
	//trashdir로 이동
	if(chdir(trashdir)<0){//returns 0 if success
	    fprintf(stderr,"PDIR:%s can't be found.\n",trashdir);
	    perror("chdir");
	    exit(1);
	}
	//files dir 서브디렉토리로 생성 
	if(getcwd(filesdir,PATH_SIZE)==0){
	    fprintf(stderr,"getcwd error to %s\n",filesdir);
	    exit(1);
	}
	sprintf(filesdir,"%s/files",filesdir);
	mkdir(filesdir,0744);

	//files디렉토리로 DELETE명령어로 지운 파일 자체 저장
	if(overlapped==1){
	    for(int i=0;i<dupindex;i++){
		sprintf(newdir,"%s/dup*%d_%s",filesdir,dupindex,onlyfname);
	    }
	}
	else
	    sprintf(newdir,"%s/%s",filesdir,onlyfname);

	if(rename(fnamepath,newdir)<0){
	    fprintf(stderr,"rename error for %s to %s\n",fnamepath,newdir);
	}
	//파일 삭제 시간 저장.(info dir파일정보 출력시 사용)
	if(stat(fnamepath,&infobuf)<0){
	    time_t t=time(NULL);
	    delt=*localtime(&t);
	}
	//files dir에 작업 마치고 다시 trashdir로 이동
	if(chdir(trashdir)<0){//returns 0 if success
	    fprintf(stderr,"DIR:%s can't be found.\n",trashdir);
	    perror("chdir");
	    //exit(1);
	}
	//info dir 서브디렉토리로 생성  
	if(getcwd(infodir,PATH_SIZE)==0){
	    fprintf(stderr,"getcwd error to %s\n",infodir);
	    //exit(1);
	}
	sprintf(infodir,"%s/info",infodir);
	mkdir(infodir,0744);
	//info dir에 DELETE명령어로 지운 파일정보를 담은 파일 저장
	//(이름은 절대경로 제외 파일이름으로)
	if(overlapped==1){
	    for(int i=0;i<dupindex;i++){
		sprintf(infotxt,"%s/dup*%d_%s",infodir,dupindex,onlyfname);
	    }
	}
	else
	    sprintf(infotxt,"%s/%s",infodir,onlyfname);
	if((fp=fopen(infotxt,"a"))<0){
	    fprintf(stderr,"fopen error for %s\n",infotxt);
	    //exit(1);
	}
	if(stat(infotxt,&infobuf)<0){
	    fprintf(stderr,"lstat error for %s\n",infotxt);
	    //exit(1);
	}
	fprintf(fp,"[Trash info]\n");
	fprintf(fp,"%s\n",fnamepath);//지운 파일 절대경로 
	//파일 삭제 시간
	fprintf(fp,"D : %04d-%02d-%02d %02d:%02d:%02d\n", delt.tm_year+1900,delt.tm_mon+1,delt.tm_mday,delt.tm_hour,delt.tm_min,delt.tm_sec);
	//파일 최종 수정시간
	t=localtime(&infobuf.st_mtime);
	fprintf(fp,"M : %04d-%02d-%02d %02d:%02d:%02d\n", t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

	fclose(fp);
    }
    chdir(curdir);



    return 0;
}

int do_sizeOpt(char *str){//SIZE [FILENAME] [OPTION]

    printf("sizeopt");
    return 0;
}

int do_recoverOpt(char *str){
    char removeinfopath[PATH_SIZE];//backup deleting file
    char fnamepath[PATH_SIZE];//deleting file name path
    char temppath[PATH_SIZE];//for checking info dir file list
    char recopt[OPT_SIZE];
    //    char onlyfname[PATH_SIZE];//deleting file name 전역변수 쓰므로 필요없음.
    char newdir[PATH_SIZE];//newdir path buf for 'files'dir
    struct stat statbuf;
    struct stat infobuf;
    struct stat tempstat;
    // struct timeval *renamet;
    struct tm delt;
    struct tm *t;
    //Node *newNode;
    int i;
    char mtime[TM_SIZE];
    char dtime[TM_SIZE];
    char skip1[TM_SIZE];
    char skip2[PATH_SIZE];
    char listfname[PATH_SIZE];
    int recoptl=1;//init
    FILE *fp;//info dir의 파일들을 읽기 위해 
    memset(trashdir,0,PATH_SIZE);
    memset(filesdir,0,PATH_SIZE);
    memset(infodir,0,PATH_SIZE);
    memset(removeinfopath,0,PATH_SIZE);
    memset(curdir,0,PATH_SIZE);
    memset(checkdir,0,PATH_SIZE);
    memset(fnamepath,0,PATH_SIZE);
    memset(temppath,0,PATH_SIZE);
    memset(recopt,0,OPT_SIZE);
    memset(newdir,0,PATH_SIZE);
    // newNode=(Node*)malloc(sizeof(Node));

    int len=strlen(str);
    //trim option cmd part
    for(i=0;i<strlen(str);i++){
	if(str[i]==' ')
	    break;  
    }
    while(i<len && str[i]==' ')
	i++;
    memset((char*)onlyfname,0,PATH_SIZE);
    //인자로 얻은 str에서 [FILENAME]만 추출
    int a=0;
    int j;
    for(j=i;j<len && str[j]!=' ';j++){
	onlyfname[a]=str[j];
	a++;
	i++;
    }
    //      printf("onlyfname:%s",onlyfname);
    i=j;
    //[OPTION] 추출 
    while(i<len && str[i]==' ')
	i++;
    if(i>=len){//[OPTION]주어지지않은경우
	recoptl=0;
	//	printf("recoptl:%d\n",recoptl);
    }
    if(recoptl==1){
	recoptl=0;
	a=0;
	memset(recopt,0,OPT_SIZE);
	for(j=i;j<len && str[j]!=' ';j++){//-r -i
	    recopt[a]=str[j];
	    a++;
	    i++;
	}
	//	printf("recopt:%s\n",recopt);
	i=j;
	if(!strcmp(recopt,"-l")){
	    recoptl=1;
	}
    }
    memset(curdir,0,PATH_SIZE);
    memset(trashdir,0,PATH_SIZE);
    memset(checkdir,0,PATH_SIZE);
    memset(filesdir,0,PATH_SIZE);
    memset(infodir,0,PATH_SIZE);

    strcpy(curdir,getcwd(NULL,0));//***절대경로 상대경로 입력시 모두 동작하도록 수정해야함
    //printf("curdir:%s\n",curdir);
    sprintf(checkdir,"%s/check",curdir);
    sprintf(filesdir,"%s/trash/files",curdir);
    sprintf(infodir,"%s/trash/info",curdir);
    /*trash dir 이름을 지정해서 생성.
      if(getcwd(filesdir,PATH_SIZE)==0){
      fprintf(stderr,"getcwd error to %s\n",filesdir);
      exit(1);
      }*/
    chdir(curdir);
    scanningTdir(infodir);
    chdir(curdir);
    scanningCdir();

    //l option이 있다면 먼저 l option처리해줌.
    //"trash"디렉토리 밑에 있는 파일과 삭제 시간들을 삭제 시간이 오래된 순으로 출력후, 명령어진행.
    if(recoptl==1){
	printf("working on l option..");

	int sort[TM_SIZE];
	Node *printoptl=(Node*)malloc(sizeof(Node));
	memset(printoptl,0,sizeof(printoptl));
	printoptl=head;
	i=0;

	/*
	while(printoptl){
	    if(printoptl->optldt > printoptl->next->optldt){
		printf("SSS: %d %d\n",printoptl->optldt, printoptl->next->optldt);
		printoptl->next->optldt=printoptl->optldt;
		printf("SSS: %d %d\n",printoptl->optldt, printoptl->next->optldt);
	    }
	    printoptl=printoptl->next;
	}
	*/
	/*
	while(printoptl){
	    if(printoptl->optldt!=0){
		printf("OPTLDT:%d",printoptl->optldt);
		sort[i]=printoptl->optldt;
		i++;
		list_sort(printoptl->optldt);
	    }
	    printoptl=printoptl->next;
	}
	Node *cur,*newnode;
	cur=head;
	newnode->optldt=cur->optldt;
	if(cur==NULL)
	    cur=cur->next;
	else{
	    while(cur->next!=NULL){
		if(cur->next->optldt > cur->optldt){
		    newnode->next=cur->next;
		    cur->next=newnode;
		    printf("%d, %s %s",cur->optldt, cur->listfname, cur->dtime);
		}
		cur=cur->next;
	    }
	} 
	*/
	

	//list_print();

	list_sort(optldt_cmp);

	//list_print();


	/*
	   int temp=0;;
	   int k;
	   i=k;
	   while(i>0){
	   for(int j=0;j<i-1;j++){
	   if(sort[j]>sort[j+1]){
	   temp=sort[j];
	   sort[j]=sort[j+1];
	   printf("[%d]",sort[j]);
	   sort[j+1]=temp;
	   }
	   }
	   i--;
	   }
	   while(k>0){
	   printf("[%d]",sort[k]);
	   k--;
	   }
	   */
	list_print();

    }
    //trash dir스캔해서 입력파일과 동일한 이름이 있는지 확인
    //중복체크. 중복된 이름 여러개 있는 경우 파일이름, 삭제시간, 수정시간 표준출력.
    Node *printdup=(Node*)malloc(sizeof(Node));
    memset(printdup,0,sizeof(printdup));
    printdup=head;
    int overlapped=0;
    char duponlyfname[PATH_SIZE];
    char dupindexpick[OPT_SIZE];
    int dupindex=1;
    /*while(printdup){
      if(!strcmp(printdup->listfname,onlyfname)){
      printf("overlapped\n");
      overlapped=1;
    //printdup->dupindex=dupindex;//중복횟수 셈
    }
    printdup=printdup->next;
    }*/
    char* ptr;
    char* ptrdup;
    printdup=head;
    while(printdup){
	if(!strcmp(printdup->listfname,onlyfname)){
	    overlapped=1;
	    printdup->dupindex=dupindex;
	    printf("%d.  %s\n    %s    %s\n",printdup->dupindex,printdup->listfname,printdup->dtime,printdup->mtime);
	    dupindex++;
	}
	ptr=NULL;
	ptrdup=NULL;
	ptr=strstr(printdup->listfname,onlyfname);//strstr has return value
	ptrdup=strstr(printdup->listfname,"dup*");
	if(ptr!=NULL){
	    if(ptrdup!=NULL){
		overlapped=1;
		printdup->dupindex=dupindex;//중복횟수 셈
		printf("%d.  %s\n    %s    %s\n",printdup->dupindex,printdup->listfname,printdup->dtime,printdup->mtime);

		dupindex++;
	    }
	}
	ptr=NULL;
	ptrdup=NULL;
	printdup=printdup->next;
    }
    if(overlapped==1){
	printf("Choose: ");
	fgets(dupindexpick,OPT_SIZE,stdin);
	int pick=atoi(dupindexpick);
	printdup=head;
	while(printdup){
	    if(pick==printdup->dupindex)
		strcpy(duponlyfname,printdup->listfname);
	    printdup=printdup->next;
	}
    }


    //trashdir로 이동
    if(chdir(filesdir)<0){//returns 0 if success
	fprintf(stderr,"DIR:%s can't be found.\n",filesdir);
	perror("chdir");
    }
    //l option이 있다면 먼저 l option처리해줌.
    //해당파일이 없거나 파일의 복구 경로가 없다면 에러처리 후 프롬프트 전환 
    if(overlapped==0){
	if(access(onlyfname,F_OK)!=0){
	    printf("There is no %s in the 'trash' directory\n",onlyfname);
	    chdir(curdir);
	    ssu_mntr_play();
	}

	memset(fnamepath,0,PATH_SIZE);
	//복구파일의 절대경로 가져옴
	if(realpath(onlyfname,fnamepath)==NULL){
	    printf("There is no %s in the 'trash' directory\n",onlyfname);
	    exit(1);
	}
    }
    if(overlapped==1){
	if(access(duponlyfname,F_OK)!=0){
	    printf("There is no %s in the 'trash' directory\n",duponlyfname);
	    chdir(curdir);
	    ssu_mntr_play();
	}

	memset(fnamepath,0,PATH_SIZE);
	//복구파일의 절대경로 가져옴
	if(realpath(duponlyfname,fnamepath)==NULL){
	    printf("There is no %s in the 'trash' directory\n",duponlyfname);
	    exit(1);
	}
    }


    //입력받은 파일이 정상적으로 존재하는 파일인지 확인
    if(lstat(fnamepath,&statbuf)<0){
	fprintf(stderr,"lstat error for %s\n",fnamepath);
	exit(1);
    }
    if(!S_ISREG(statbuf.st_mode)){
	fprintf(stderr,"stat error for %s, not a regular file\n",fnamepath);
	exit(1);
    } 
    if(chdir(checkdir)<0){//returns 0 if success
	fprintf(stderr,"DIR:%s can't be found.\n",checkdir);
	perror("chdir");
	exit(1);
    }
    if(overlapped==1)
	sprintf(newdir,"%s/%s",checkdir,duponlyfname);
    else
	sprintf(newdir,"%s/%s",checkdir,onlyfname);
    //"trash"디렉토리 안에 있는 파일을 원래 경로로 복구.
    if(rename(fnamepath,newdir)<0){
	fprintf(stderr,"rename error for %s to %s\n",fnamepath,checkdir);
    }
    //"info"디렉토리 안에 있는 파일은 삭제해줌.
    if(chdir(infodir)<0){
	fprintf(stderr,"infodir:%s can't be found.\n",infodir);
	perror("chdir");
    }
    if(overlapped==1)
	sprintf(removeinfopath,"%s/%s",infodir,duponlyfname);
    else
	sprintf(removeinfopath,"%s/%s",infodir,onlyfname);
    int delinfo=remove(removeinfopath);
    if(delinfo==0)
	printf("infodir:%s deleted.\n",removeinfopath);
    else
	printf("%s failed to delete .\n",removeinfopath);


    ////////////////////////////////////////////////////////////////////


    /*
       fprintf(fp,"%s\n",fnamepath);//지운 파일 절대경로 
    //파일 삭제 시간
    fprintf(fp,"D : %04d-%02d-%02d %02d:%02d:%02d\n", delt.tm_year+1900,delt.tm_mon+1,delt.tm_mday,delt.tm_hour,delt.tm_min,delt.tm_sec);
    //파일 최종 수정시간
    t=localtime(&infobuf.st_mtime);
    fprintf(fp,"M : %04d-%02d-%02d %02d:%02d:%02d\n", t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);

    fclose(fp);*/

    printf("Hcurdir:%s\n",curdir);

    chdir(curdir);
    return 0;
}

int do_treeOpt(char *str){
    printf("treeopt");

    return 0;
}

int do_helpOpt(char *str){

    printf("helpopt");
    return 0;
}
void scanningTdir(char *searchdir){
    head=NULL;//init first
    char trashdir[PATH_SIZE];
    char filesdir[PATH_SIZE];//backup deleting file
    char infodir[PATH_SIZE];//backup deleting file
    char removeinfopath[PATH_SIZE];//backup deleting file
    char curdir[PATH_SIZE];
    char checkdir[PATH_SIZE];
    char fnamepath[PATH_SIZE];//deleting file name path
    char temppath[PATH_SIZE];//for checking info dir file list
    char recopt[OPT_SIZE];
    //    char onlyfname[PATH_SIZE];//deleting file name 전역변수 쓰므로 필요없음.
    char newdir[PATH_SIZE];//newdir path buf for 'files'dir
    struct stat statbuf;
    struct stat infobuf;
    struct stat tempstat;
    // struct timeval *renamet;
    struct tm delt;
    struct tm *t;
    Node *newNode;
    pthread_t t_id;// thread id
    int i;
    char mtime[TM_SIZE];
    char dtime[TM_SIZE];
    char skip1[TM_SIZE];
    char skip2[PATH_SIZE];
    char listfname[PATH_SIZE];
    int dupindex;
    int recoptl=1;//init
    FILE *fp;//info dir의 파일들을 읽기 위해 
    memset(trashdir,0,PATH_SIZE);
    memset(filesdir,0,PATH_SIZE);
    memset(infodir,0,PATH_SIZE);
    memset(removeinfopath,0,PATH_SIZE);
    memset(curdir,0,PATH_SIZE);
    memset(checkdir,0,PATH_SIZE);
    memset(fnamepath,0,PATH_SIZE);
    memset(temppath,0,PATH_SIZE);
    memset(recopt,0,OPT_SIZE);
    memset(newdir,0,PATH_SIZE);
    newNode=(Node*)malloc(sizeof(Node));
    printf("-----------------------scanning trash/info dir------\n");
    strcpy(curdir,getcwd(NULL,0));//***절대경로 상대경로 입력시 모두 동작하도록 수정해야함
    //printf("curdir:%s\n",curdir);
    sprintf(checkdir,"%s/check",curdir);
    sprintf(filesdir,"%s/trash/files",curdir);
    sprintf(infodir,"%s/trash/info",curdir);
    int countdirp=0;
    struct dirent **flist;
    int l=0;int k=0;
    char optldtime[TM_SIZE];
    int optldtimeInt;
    int optldt;
    //infodir로 이동
    if(chdir(searchdir)<0){//returns 0 if success
	fprintf(stderr,"DIR:%s can't be found.\n",searchdir);
	perror("chdir");
    }
    //printf("infodir:%s\n",infodir);
    //trash/info dir에 있는 파일 정보들을 저장.
    if((countdirp=scandir(searchdir,&flist,0,alphasort))<0){
	fprintf(stderr,"scandir error for %s\n",filesdir);
	exit(1);
    }
    //printf("COUNT:%d\n",countdirp);
    i=0;
    //info dir의 파일이름, 삭제시간, 수정시간 각 노드 만들어주기
    while(i<countdirp){
	printf("counting file num in info dir..\n");
	if(!strcmp(flist[i]->d_name,".")||!strcmp(flist[i]->d_name,"..")){
	    i++;
	    continue;
	}
	//pthread_t new_id;
	memset(temppath,0,PATH_SIZE);
	if(realpath(flist[i]->d_name,temppath)==0){
	    fprintf(stderr,"real path error for %s\n",flist[i]->d_name);
	    exit(1);
	}
	if(lstat(temppath,&tempstat)<0){
	    fprintf(stderr,"lstat error for %s\n",temppath);
	    exit(1);
	}

	if(S_ISREG(tempstat.st_mode)){
	    Node *node=(Node*)malloc(sizeof(Node));
	    memset(node,0,sizeof(node));
	    memset(newNode->listfpath,0,PATH_SIZE);
	    strcpy(node->listfpath,temppath);
	    if((fp=fopen(node->listfpath,"r+w"))<0){
		fprintf(stderr,"fopen error for %s's %s\n",searchdir,node->listfpath);
	    }
	    memset(dtime,0,TM_SIZE);
	    memset(mtime,0,TM_SIZE);
	    while(1){
		fgets(skip1,80,fp);//[trashinfo]
		fgets(skip2,80,fp);//path
		fgets(dtime,80,fp);
		fgets(mtime,80,fp);
		if(feof(fp))
		    break;
		/*else{
		  fgets(dupped,80,fp);//중복파일이 있다면 내용이 들어온다.[trashinfo]
		  break;
		  }*/
	    }
	    fclose(fp);
	    ///////////////////////////////////////////////
	    l=k=0;
	    optldtimeInt=0;
	    optldt=0;
	    memset(optldtime,0,TM_SIZE);

	    while(l<strlen(dtime) && dtime[l]==' ')
		l++;
	    while(l<strlen(dtime) && dtime[l]=='D')
		l++;
	    while(l<strlen(dtime) && dtime[l]==' ')
		l++;
	    while(l<strlen(dtime) && dtime[l]==':')
		l++;
	    while(l<strlen(dtime) && dtime[l]==' ')
		l++;
	    l+=10;//2020-05-05 skip 
	    while(l<strlen(dtime) && dtime[l]==' ')
		l++;
	    for(k=0;l<strlen(dtime) && dtime[l]!=':';l++){
		optldtime[k]=dtime[l];
		//printf("optldtimeInt:%d\n",optldtimeInt[k]);
		k++;
	    }
	    while(l<strlen(dtime) && dtime[l]==':')
		l++;
	    for(;l<strlen(dtime) && dtime[l]!=':';l++){
		optldtime[k]=dtime[l];
		k++;
	    }
	    while(l<strlen(dtime) && dtime[l]==':')
		l++;
	    for(;l<strlen(dtime) && dtime[l]!=':';l++){
		optldtime[k]=dtime[l];
		k++;
	    }
	    optldtimeInt=atoi(optldtime);
	    ///////////////////////////////////////////////
	    optldt=0;
	    memset(node->listfname,0,PATH_SIZE);
	    memset(node->mtime,0,TM_SIZE);
	    memset(node->dtime,0,TM_SIZE);
	    node->optldt=optldtimeInt;
	    strcpy(node->listfname,flist[i]->d_name);//filename only
	    strcpy(node->mtime,mtime);//newNode->mtime;
	    strcpy(node->dtime,dtime);
	    //중복체크 
	    dupindex=0;
	    char dup1onlyfname[PATH_SIZE];

	    if(!strcmp(node->listfname,onlyfname)){
		dupindex++;
		node->dupindex=dupindex;//중복횟수 셈
		printf("node->dupindex:%d\n",node->dupindex);
		//sprintf(dup1onlyfname,"%d_%s",dupindex,onlyfname);
		//rename(onlyfname,dup1onlyfname);
	    }

	    list_insert(node);
	    printf("flist[i]->d_name:%s\n",flist[i]->d_name);
	    printf("listfpath:%s\n",node->listfpath);
	    printf("optldt:%d\n",node->optldt);
	    //printf("mtime:%s\n",node->mtime);
	    //printf("dtime:%s\n",node->dtime);
	    printf("dupindex:%d\n",node->dupindex);



	}
	else{
	    fprintf(stderr,"%s is not a regular file.\n",flist[i]->d_name);
	    exit(1);
	}
	i++;
    }
    printf("-----------------------scanning trash/info dir ends------\n");
}
void list_insert(Node *newNode){//list에 node추가
    newNode->next=NULL;
    if(head==NULL)
	head=newNode;
    else{
	Node *listF;
	listF=head;
	while(listF->next!=NULL)
	    listF=listF->next;
	listF->next=newNode;
    }
}
void scanningCdir(void){
    chead=NULL;//init first!
    char trashdir[PATH_SIZE];
    char filesdir[PATH_SIZE];//backup deleting file
    char infodir[PATH_SIZE];//backup deleting file
    char removeinfopath[PATH_SIZE];//backup deleting file
    char curdir[PATH_SIZE];
    char checkdir[PATH_SIZE];
    char fnamepath[PATH_SIZE];//deleting file name path
    char temppath[PATH_SIZE];//for checking info dir file list
    char recopt[OPT_SIZE];
    //    char onlyfname[PATH_SIZE];//deleting file name 전역변수 쓰므로 필요없음.
    char newdir[PATH_SIZE];//newdir path buf for 'files'dir
    struct stat statbuf;
    struct stat infobuf;
    struct stat tempstat;
    // struct timeval *renamet;
    struct tm delt;
    struct tm *t;
    CNode *newNode;
    pthread_t t_id;// thread id
    int i;
    char mtime[TM_SIZE];
    char dtime[TM_SIZE];
    char skip1[TM_SIZE];
    char skip2[PATH_SIZE];
    char listfname[PATH_SIZE];
    int recoptl=1;//init
    FILE *fp;//info dir의 파일들을 읽기 위해 
    memset(trashdir,0,PATH_SIZE);
    memset(filesdir,0,PATH_SIZE);
    memset(infodir,0,PATH_SIZE);
    memset(removeinfopath,0,PATH_SIZE);
    memset(curdir,0,PATH_SIZE);
    memset(checkdir,0,PATH_SIZE);
    memset(fnamepath,0,PATH_SIZE);
    memset(temppath,0,PATH_SIZE);
    memset(recopt,0,OPT_SIZE);
    memset(newdir,0,PATH_SIZE);
    newNode=(CNode*)malloc(sizeof(CNode));

    strcpy(curdir,getcwd(NULL,0));//***절대경로 상대경로 입력시 모두 동작하도록 수정해야함
    printf("-------------------------scanning check dir\n");
    printf("curdir:%s\n",curdir);
    sprintf(checkdir,"%s/check",curdir);
    sprintf(filesdir,"%s/trash/files",curdir);
    sprintf(infodir,"%s/trash/info",curdir);
    int countdirp=0;
    struct dirent **flist;
    //infodir로 이동
    if(chdir(checkdir)<0){//returns 0 if success
	fprintf(stderr,"DIR:%s can't be found.\n",filesdir);
	perror("chdir");
    }
    printf("checkdir:%s\n",checkdir);
    //trash/info dir에 있는 파일 정보들을 저장.
    if((countdirp=scandir(checkdir,&flist,0,alphasort))<0){
	fprintf(stderr,"scandir error for %s\n",filesdir);
	exit(1);
    }
    printf("COUNT:%d\n",countdirp);
    i=0;
    //info dir의 파일이름, 삭제시간, 수정시간 각 노드 만들어주기
    while(i<countdirp){
	printf("counting file num in check dir..\n");
	if(!strcmp(flist[i]->d_name,".")||!strcmp(flist[i]->d_name,"..")){
	    i++;
	    continue;
	}
	//pthread_t new_id;
	memset(temppath,0,PATH_SIZE);
	if(realpath(flist[i]->d_name,temppath)==0){
	    fprintf(stderr,"real path error for %s\n",flist[i]->d_name);
	    exit(1);
	}
	if(lstat(temppath,&tempstat)<0){
	    fprintf(stderr,"lstat error for %s\n",temppath);
	    exit(1);
	}

	if(S_ISREG(tempstat.st_mode)){
	    CNode *node=(CNode*)malloc(sizeof(CNode));
	    memset(node,0,sizeof(node));
	    memset(newNode->listfpath,0,PATH_SIZE);
	    strcpy(node->listfpath,temppath);
	    if((fp=fopen(node->listfpath,"r+w"))<0){
		fprintf(stderr,"fopen error for checkdir's %s\n",node->listfpath);
	    }
	    memset(dtime,0,TM_SIZE);
	    memset(mtime,0,TM_SIZE);
	    while(1){
		fgets(skip1,80,fp);//[trashinfo]
		fgets(skip2,80,fp);//path
		fgets(dtime,80,fp);
		fgets(mtime,80,fp);
		if(feof(fp))
		    break;
		/*else{
		  fgets(dupped,80,fp);//중복파일이 있다면 내용이 들어온다.[trashinfo]
		  break;
		  }*/
	    }
	    fclose(fp);
	    memset(node->mtime,0,TM_SIZE);
	    memset(node->dtime,0,TM_SIZE);
	    memset(node->listfname,0,PATH_SIZE);
	    strcpy(node->mtime,mtime);//newNode->mtime;
	    strcpy(node->dtime,dtime);
	    strcpy(node->listfname,flist[i]->d_name);
	    Clist_insert(node);
	    printf("flist[i]->d_name:%s\n",flist[i]->d_name);
	    printf("listfname:%s\n",listfname);
	    printf("listfpath:%s\n",node->listfpath);
	    printf("mtime:%s\n",node->mtime);
	    printf("dtime:%s\n",node->dtime);
	}
	else{
	    fprintf(stderr,"%s is not a regular file.\n",flist[i]->d_name);
	    exit(1);
	}
	i++;
    }
    printf("-------------------------scanning check dir ends\n");
}
void Clist_insert(CNode *newNode){//list에 node추가
    newNode->next=NULL;
    if(chead==NULL)
	chead=newNode;
    else{
	CNode *listF;
	listF=chead;
	while(listF->next!=NULL)
	    listF=listF->next;
	listF->next=newNode;
    }
}

void swap_node_data(Node *list1, Node *list2) {

    //char filesdir[PATH_SIZE];
    char listfpath[PATH_SIZE];
    char listfname[PATH_SIZE];
    char dtime[TM_SIZE];
    char mtime[TM_SIZE];
    int dupindex;
    char dupped[TM_SIZE];//info정보에 그다음내용이 있다면 중복파일이라는 걸 확인 위해
    int optldt;
    List *head;


    //tmp
    strcpy(listfpath, list2->listfpath);
    strcpy(listfname, list2->listfname);
    strcpy(dtime, list2->dtime);
    strcpy(mtime, list2->mtime);
    strcpy(dupped, list2->dupped);
    dupindex = list2->dupindex;
    optldt = list2->optldt;
    head = list2->head;


    strcpy(list2->listfpath, list1->listfpath);
    strcpy(list2->listfname, list1->listfname);
    strcpy(list2->dtime, list1->dtime);
    strcpy(list2->mtime, list1->mtime);
    strcpy(list2->dupped, list1->dupped);
    list2->dupindex = list1->dupindex;
    list2->optldt = list1->optldt;
    list2->head = list1->head;


    strcpy(list1->listfpath, listfpath);
    strcpy(list1->listfname, listfname);
    strcpy(list1->dtime, dtime);
    strcpy(list1->mtime, mtime);
    strcpy(list1->dupped, dupped);
    list1->dupindex = dupindex;
    list1->optldt = optldt;
    list1->head = head;
}

/*
int str_cmp(Node *a, Node *b) {
    return strcmp(a->str, b->str)
}
*/

void list_sort(int (*cmp)()){

	int is_swapped;

	Node *list_ptr;
	Node *last_ptr;

	last_ptr = 0;

	if (head != NULL) {
	    while(1) {
		is_swapped = 0;
		list_ptr = head;
		while (list_ptr->next != last_ptr) {

		    if (cmp(list_ptr, list_ptr->next) > 0) {
			swap_node_data(list_ptr, list_ptr->next);
		    }
		    list_ptr = list_ptr->next;
		}
		if (!is_swapped)
		    break;
		last_ptr = list_ptr;
	    }
	}
}

void list_print(){
    Node *cur;
    cur=head;
    int i=0;
    while(cur->next!=NULL){
	if(cur->listfname!=NULL&&cur->dtime!=NULL && cur->optldt!=0){
	    printf("%d, %s %s",cur->optldt, cur->listfname, cur->dtime);
	}
	cur=cur->next;
    }
     printf("%d, %s %s\n",cur->optldt,cur->listfname, cur->dtime);
}
int list_compare(char *onlyfname){



}

