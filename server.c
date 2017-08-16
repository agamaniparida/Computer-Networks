/*
 Simple udp server
 */
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#define clear() printf("\033[H\033[J")
#define BUFLEN 255  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
int counter=0;
struct Packet 
{
   char  sPackID[2];
   uint8_t  clID;
   char  acc_Per[2];
   uint8_t  segNo;
   uint8_t length;
   uint8_t technology;
   unsigned int src_Sub_num;
   char  endID[2];
};
void die(char *s)
{
    perror(s);
    exit(1);
}
void delay(unsigned int mseconds)
{
    int ms = mseconds * 1000000;
    clock_t goal = ms + clock();
    while (goal > clock());
}
int checkData(unsigned int sNum, int technology)
{
    char *item;
    int reccount = 0;
    char lyne[15];
    FILE *fp;   
    fp = fopen("TST.txt","r"); // read mode

    if( fp == NULL )
    {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
    }  
    unsigned int subNumVal = 0; 
    int technologyNUM =0; 
    int paidNUM =0;
    while(fgets(lyne,15,fp)) 
    {
        item = strtok(lyne,"|");
        //printf("Value Found %u - %u\n", sNum, atoi(item));
        if(sNum == atoi(item))
        {
          item = strtok(NULL,"|");
          if(technology == atoi(item))
          {
            item = strtok(NULL,"\n");
            if(atoi(item)==1)
            {
              //Subscriber paid
              return 1;
              break;
            }
            else
            {
              //Subscriber Not Paid
              return 2;
              break;
            }
          }
        }
        reccount++;
    }
    return 0;
}
int main(void)
{
    clear();
    struct sockaddr_in si_me, si_other;
    struct Packet Packet1;
    int s, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
    
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
        printf("\n---------\n");
        printf("Counter %d\n",counter);
        printf("---------\n");

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, &Packet1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }

        int paidStatus = checkData(Packet1.src_Sub_num, Packet1.technology);
       
        if(counter==4)
        {
            delay(6);
        }
        if(paidStatus ==1)
        {
            printf("Acknowledgement Block : Subscriber is permitted to access\n");
            strcpy(Packet1.acc_Per,"P");
            printf("--------------------------\n");
            printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
            printf("--------------------------\n");
            if (sendto(s, &Packet1, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            {
                die("sendto()");
            }
        }
        else if(paidStatus ==2)
        {
            printf("Rejection Block : Subscriber has not paid\n");
            strcpy(Packet1.acc_Per,"Q");
            printf("--------------------------\n");
            printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
            printf("--------------------------\n");
            if (sendto(s, &Packet1, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            {
                die("sendto()");
            }
        }
        else if(paidStatus ==0)
        {
            printf("Rejection Block : Subscriber does not exist\n");
            strcpy(Packet1.acc_Per,"R");
            printf("--------------------------\n");
            printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
            printf("--------------------------\n");
            if (sendto(s, &Packet1, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            {
                die("sendto()");
            }
        }

        counter++;  
    }
    close(s);
    return 0;
}
