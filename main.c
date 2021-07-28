#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 5678
//Protos
char *concatWithColon(char input[]);
int isError(char* key,int curSocket);
int isAlphanumeric(char* s) ;
void sendResponse(char* command, char* key, char* value,int curSocket);
int extractNumber(char* s);
void ExtractKey(char* from,char* into);
void ExtractValue(char* from,char* into);
void getFileName(char* key, char* into);
void writeString2File(char* fileName, char* content);
void getContentOfFile(char* fileName, char* into);

int del(char* key, int curSocket);
int get(char* key, int curSocket);
int put(char* key, char* value, int curSocket);

void op(char* key, char* value);
void sub(char* key, int curSocket);void callSubs(char* key);

void getKey(char* buffer, char* key);
void fixInput(char* buffer);
void *connect_sockets(void* socketNum);





int isError(char* key,int curSocket){
	if(isAlphanumeric(key) == 0)
	{
		send(curSocket , "Key is not Alphanumeric!\n", strlen("Key is not Alphanumeric!\n") , 0 );

		return -1;
	}
	else return 0;
}

static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;		//Mutex to prevent race conditions
static pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;

int ReaderMode = 0;
int wait2Write = 0;
int soleSock = -1;
char latestAction[1024] = {0};
char latestSender = -1;
char constWho[256]={0};

char* concatWithColon(char input[]){

return strcat(input,":");
}

void sendResponse(char* command, char* key, char* value,int curSocket)
{
	char output[1024]={'\0'};//is a global or static declaration, it will be stored in the binary's data segment with its initialized data, thus increasing your binary size by about 1024 bytes (in this case)
    //,n other words make array full of empty

	strcat(output,command);
	concatWithColon(output);
	strcat(output,key);
	concatWithColon(output);
	strcat(output,value);

	strcpy(latestAction, output);
	latestSender = curSocket;

	send(curSocket , output , strlen(output) , 0 );
}

int isAlphanumeric(char* s) {		//return == 1 => is		return == 0 => is not

	int index = 0;
	while(s[index] != '\0' && s[index] != '\n' && s[index] != ' ')
	{
		if (s[index] < 48 || s[index] > 122)return 0;

        	else if (s[index] < 65 && s[index] > 57) return 0;

       	 else if (s[index] < 97 && s[index] > 90) return 0;

       	 if(index == 100)return 0;

       	index++;
	}
	return 1;
}
int extractNumber(char* s) {

	int index = 0;
	while(s[index] != '\0' && s[index] != '\n' && s[index] != ' ')
	{
		if (s[index] < 48 || s[index] > 57)return 0;

       	index++;
	}
	char stringNumb[1024];
	memcpy( stringNumb, s, index);
	return atoi(stringNumb); //We may use strtol too but it convert to long int and that may be cause consistency
}
void ExtractKey(char* from,char* into)
{
	char buffer[1024];
	strcpy(buffer, from);
    	char * ptr;
  	ptr = strchr( buffer, ' ' );	//String from first whitespace(nullchraacter)
    	int wid =0;

    	while(ptr[wid + 1] != ' ' && ptr[wid + 1] != '\n' && ptr[wid + 1] != '\0')
    	{
    		//printf( "%c\n",ptr[index + width + 1] );
    		wid++;
    	}
	memcpy( into, ptr + 1, wid);
	into[wid] = '\0';
    	//printf( "KEY:%s\n",into);
}
void ExtractValue(char* from,char* into)
{
	char buffer[1024];
	char bS[1024];
	strcpy(buffer, from);
	strcpy(bS, buffer);
    	char * ptr;
  	ptr = strchr( buffer, ' ' );   //String from first whitespace(nullchraacter)
  	ptr = strchr( ptr+1, ' ' );	//String from second whitespace(nullchraacter)
  	//printf( "The first  %c in '%s' is '%s'\n",' ', buffer, ptr );
    	int wid =0;
    	while(ptr[wid + 1] != ' '&& ptr[wid + 1] != '\n' && ptr[wid + 1] != '\0')
    	{
    		//printf( "%c\n",ptr[index + width + 1] );
    		wid++;
    	}
	memcpy( into, ptr + 1, wid);
	into[wid] = '\0';

    	//printf( "VALUE: %s\n\n",into);
    strcpy(from, bS);
}
void getFileName(char* key, char* into)
{
	strcpy(into, key);
	strcat(into,".txt");
}
void writeString2File(char* fileName, char* content)
{
	FILE *file;

	file = fopen(fileName, "w+");

	if(file != NULL) {
		fputs(content, file);
		//fputc(10, file);
		fclose(file);

	}else {
		printf("File couldnt be opened!\n");
		exit(-1);
	}
}
void getContentOfFile(char* fileName, char* into)
{
   FILE * file;
   file = fopen (fileName , "r+");
   if (file != NULL)
   {
	   if ( fgets (into , 1000 , file) != NULL )
        {
        	puts (into);
    		fclose (file);
    	}

   }
   else printf("File couldnt be opened!! or Subbedfile can be empty\n");
}
int put(char* key, char* value, int curSocket)
{
	if(isError(key,curSocket)== -1)return -1;
	if(isAlphanumeric(value) == 0)
	{
		send(curSocket , "Value is not Alphanumeric!\n", strlen("Value is not Alphanumeric!\n") , 0 );
		return -1;
	}

	char fileName[1024]={'\0'};	//key + ".txt" ending
	getFileName(key,fileName);

	//printf("adding to filename => %s\n",fileName);
    	FILE * file;

   	/* open the file for writing*/
   	file = fopen (fileName,"w+");
	//can be writen w instead of w+
    	//Write
    	fprintf (file, "%s\n",value);

   	/* close the file*/
   	fclose (file);
   	strcat(value,"\n");

    	//printf("Key added\n");
    	sendResponse("PUT", key, value,curSocket);
	return 1;
}
int get(char* key, int curSocket){
	if(isError(key,curSocket)== -1)return -1;

	char fileName[1024]={'\0'};	//key + .txt ending
	getFileName(key,fileName);


	FILE *file;
	char readed[1024];

   	file = fopen (fileName, "r+");
	//can be writen r instead of r+
	 if (file != NULL)
	 {
	    	while(fscanf(file, "%s", readed)!=EOF) ///EOF=End Of File
	    	{
   			printf("%s (From GET)\n", readed );
   		}
		fclose (file);
		strcat(readed,"\n");
		sendResponse("GET", key, readed,curSocket);
    	}
    	else
    	{
    	
    		sendResponse("GET", key, "key_nonexistent\n",curSocket);
    		return 1;
    	}
    	return -1;
}
int del(char* key, int curSocket)
{
	if(isError(key,curSocket)== -1)return -1;

	char fileName[1024]={'\0'};	//key + .txt
	getFileName(key,fileName);

   	if (0==remove(fileName))
   	        sendResponse("DEL", key, "key_deleted\n",curSocket);
      		///printf("The file is Deleted \n");
      		
   	else
   		sendResponse("DEL", key, "key_nonexistent\n",curSocket);
      		///printf("the file is NOT Deleted\n");
}
void op(char* key, char* value) 
{
	/**
	int fd[2];

//	fd[0]=read
//  fd[1]=write

	*/
	//Father process continue to run server.
	int id = fork();
	if(id==0 &&  strcmp(value,"who")==0){ 
	//printf("vliiii");
		execlp("who","who",NULL);
	}
	if(id==0 && strcmp(value,"uptime")==0){ 
	//printf("vliiii");
		execlp("uptime","uptime",NULL);
	}
	if(id==0 &&strcmp(value,"date")==0){
	//printf("vliiii");
		execlp("date","date",NULL);
	}


}

void sub(char* key, int curSocket)
{
    if(isError(key,curSocket)== -1)return ;

	char fileName[1024]={'\0'};	//key + .txt
	getFileName(key,fileName);

	FILE *file;
   	file = fopen (fileName, "r");
	 if (file == NULL)			//Test the first key
	 {
	 	sendResponse("SUB", key, "key_nonexistent",curSocket);
	 	return;
	 }
    	fclose (file);

	char content[1024]={'\0'};
	getContentOfFile("SUB.txt",content);
	//printf("Content File :%s#\n",content);

	char number[1024]={'\0'};
	sprintf(number, "%d", curSocket); ///sends formatted output to a string pointed to, by str.
	strcat(content,number);
	strcat(content," ");
	strcat(content,key);
	strcat(content,"#");//printf("Content with added:%s#\n",content);

	writeString2File("SUB.txt",content);
	char value[1024]={'\0'};
	getContentOfFile(fileName,value);
	sendResponse("SUB", key, value,curSocket);
}
void callSubs(char* key)
{
	char content[1024]={'\0'};
	char subbedtoKey[1024]={'\0'};
	getContentOfFile("SUB.txt",content);
	int curStart=0, subLength=0;
	while(content[curStart] != '\0')
	{
		subLength=0;
		while(content[curStart+subLength] != '#')
		{
			subLength++;
		}
		char sub[1024]={'\0'};
		memcpy( sub, &content[curStart], subLength);
		///printf(" sub: %s|\n",sub);
		int curSocket = extractNumber(sub);
		ExtractKey(sub,subbedtoKey);


		if(strcmp(subbedtoKey, key)==0 && latestSender != curSocket) ///  subbedtoKey = key
		{
			printf("sended %i.Socket\n",curSocket);
			send(curSocket , latestAction, strlen(latestAction) , 0 );

		}
		curStart += subLength + 1;
	}
}
void getKey(char* buffer, char* key)
{
    	ExtractKey(buffer, key);
    	printf( "Keyyy: %s\n",key);

    	strcat(key,".txt");
   	printf( "concat:\n");
   	printf("concat: %s\n",key);
}
void fixInput(char* buffer)
{
	int index = 0;
	while(buffer[index] != '\0')
	{
		if(buffer[index] == '\n')
		{
			buffer[index-1] = ' ';
			buffer[index] = '\0';
		}
		index++;
	}
}
void *connect_sockets(void* socketNum)
{
	int *num =(int *) socketNum;
	int valread, curSocket = *num;
	char buffer[1024] = {0};
	while(1)
	{
		valread = read( curSocket , buffer, 1024);
	    	printf( "%i. Socket send a request!\n",curSocket);
	    	fixInput(buffer);	//Input need a space at the end

	    	char command[1024];
	    	strcpy(command, buffer);
	    	char key[1024]={'\0'};
	    	char value[1024];
	    	pthread_mutex_lock(&mutex4);
	    	if(strstr(command, "BEG") != NULL && strlen(command) == 4)
	    	{
	    		send(curSocket , "Sole access granted!", strlen("Sole access granted!") , 0 );
	    		soleSock = curSocket;
	    		printf( "Socket:%i has sole access!\n",curSocket);
	    	}
	    	if(strstr(command, "END") != NULL && strlen(command) == 4 && curSocket == soleSock)
	    	{
	    		send(curSocket , "Sole access removed!", strlen("Sole access removed!") , 0 );
	    		soleSock = -1;// you may write number whatever you want.but give the same number later in the code
	    		printf( "Socket:%i released sole access!\n",curSocket);
	    	}
	    	pthread_mutex_unlock(&mutex4);
	    	if(curSocket == soleSock || soleSock == -1)
	    	{
		    	if(strstr(command, "PUT") != NULL ||strstr(command, "DEL") != NULL || strstr(command, "SUB") != NULL ||
		    		strstr(command, "OP") != NULL)	//Write-mode
		    	{										// 5x Mutex solve Reading
		    											// Writing Problem
		    		pthread_mutex_lock(&mutex2);
		    		wait2Write ++;
		    		pthread_mutex_unlock(&mutex2);

		    		while(ReaderMode > 0)
		    		{
		    			sleep(1);	//waits 1 sec  
		    		}
		    		pthread_mutex_lock(&mutex1);		//locking to prevent rase conditions.for Zombie Problem?
			    	if (strstr(command, "PUT") != NULL)
			    	{
					ExtractKey(buffer,key);
			    		ExtractValue(buffer, value);
			    		put(key,value,curSocket);
			    		callSubs(key);
				}
				if (strstr(command, "DEL") != NULL) {
					ExtractKey(buffer,key);
				 	del(key, curSocket);
				 	callSubs(key);
				}
				if (strstr(command, "SUB") != NULL) {
					ExtractKey(buffer,key);
				 	sub(key, curSocket);
				}
				if (strstr(command, "OP") != NULL) {
					ExtractKey(buffer,key);
					ExtractValue(buffer, value);
				 	op(key, value);
				}

				pthread_mutex_unlock(&mutex1);		//unlock mutex
				pthread_mutex_lock(&mutex2);
		    		wait2Write--;
		    		pthread_mutex_unlock(&mutex2);
		    	}
				else				//Reader-mod
		    	{
		    		while(wait2Write > 0)
		    		{
		    			sleep(1);
		    		}
		    		pthread_mutex_lock(&mutex3);
		    		ReaderMode ++;
		    		pthread_mutex_unlock(&mutex3);
			    	if (strstr(command, "GET") != NULL)
				{
					ExtractKey(buffer,key);
					get(key,curSocket);
				}
				pthread_mutex_lock(&mutex3);
		    		ReaderMode --;
		    		pthread_mutex_unlock(&mutex3);
		    	}
		}else
		{
			send(curSocket , "A client has sole access, please try later.", strlen("A client has sole access, please try later.") , 0 );
		}

		if (strstr(command, "QUIT") != NULL) {
			printf("Socket %i disconnected\n", curSocket);
			send(curSocket , "CONNECTION LOST!", strlen("CONNECTION LOST!") , 0 );
		 	break;
		}
    	}
    	return 0;
}
int main()///int main(int argc, char const *argv[])
{
    int socketDescr, curSocket;
 ///   int socketConnections[5];
 int* socketConnections;
 socketConnections = (int*)malloc(sizeof(int)*4);
    if(socketConnections==NULL){
        printf("Allocation Error");
        return -1;
    }
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};///is a global or static declaration, it will be stored in the binary's data segment with its initialized data, thus increasing your binary size about 1024 bytes (in this case of course)


    remove("SUB.txt");

    // create server socket		SOCK_STREAM = enables tcp/ip Protocoll	0= InternetProtocol
    if ((socketDescr = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");//////Exatcly I don't know what perror does but Youtubers using it in their tutorials
        exit(EXIT_FAILURE);
    }

    // settings optional options for other connections
    if (setsockopt(socketDescr, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;		//Settings listening ip adress to pcs current ip adress
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );		//settings the port to 5678

    // bind the socket to our specified IP and port
    if (bind(socketDescr, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(socketDescr, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf( "Waiting for client to connect!\n");
	printf("*************************************************************\n");
        printf("***                SOCKET PROGRAMMING                    ****\n");
	printf("**        Commands which you may use:(with Client)         **\n");
	printf("*   PUT -->[ ] [ ] you may put KEY and VALUE                *\n");
	printf("*   GET -->[ ]  you may get KEY's value                     *\n");
	printf("*   DEL -->[ ]  you may delete KEY and Value with it        *\n");
	printf("*   BEG --> you may start exclusive access                  *\n");
	printf("*   END --> you may end exclusive access                    *\n");
	printf("*   SUB -->[ ] [ ] you may Subscribe functionality          *\n");
	printf("*   OP -->[ ] [ ] you may learn who the user is,date,uptime *\n");
	printf("*   QUIT--> you may end server                              *\n");
	printf("*************************************************************\n");
	printf("*************************************************************\n");
    //storing socketconnection into new_socket
    if ((curSocket = accept(socketDescr, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
     printf( "Connected to socket:%i\n",curSocket);

        char greeting[1024]="Connected to server! ";
   	char str[12];
	sprintf(str, "%d", curSocket); ///In Client's side

  	strcat(greeting,str);
  	strcat(greeting,"\n");
    	send(curSocket , greeting, strlen(greeting) , 0 );

    	pthread_t threads[20];
    	pthread_create(&threads[0],NULL,connect_sockets,&curSocket);
    	int threadIndex = 1;

	while(1)//infinite loop  to make it continuous
	{
		printf( "Waiting for client to connect!\n");
    		//storing socketconnection into new_socket
    		if ((curSocket = accept(socketDescr, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
    		{
       		perror("accept");
        		exit(EXIT_FAILURE);
    		}
     		printf( "Connected to socket:%i\n",curSocket);

     		pthread_create(&threads[threadIndex++],NULL,connect_sockets,&curSocket);

     		strcpy(greeting, "Connected to server! ");
     		sprintf(str, "%d", curSocket);
  		strcat(greeting,str);
  		strcat(greeting,"\n");
    		send(curSocket , greeting, strlen(greeting) , 0 );
	}

    return 0;
}