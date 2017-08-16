/*
 Simple udp client
 */
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <stdint.h>
#include <sys/time.h>
#define clear() printf("\033[H\033[J")
#define SERVER "127.0.0.1"
#define BUFLEN 255  //Max length of buffer
#define PORT 8888

int i=0;
int counter =0;
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
    int ms = mseconds * 100000;
    clock_t goal = ms + clock();
    while (goal > clock());
}
void sendPacket(struct Packet Packet1)
{
    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);
    char buf[BUFLEN];
    char sendline[BUFLEN]; 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    
    if (inet_aton(SERVER , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    
    printf("Messge Transmitted: \n--------------------------\n");
    printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
    printf("--------------------------\n");
    if (sendto(s,&Packet1, sizeof(Packet1)+25, 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("sendto()");
    }
    fd_set select_fds;                
    struct timeval timeout;           
    FD_ZERO(&select_fds);             
    FD_SET(s, &select_fds);                                                              
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 )
    {
        printf("Timed occured for the Attempt #1 ... Sending Once again\n");  
        printf("Message Transmitted: \n--------------------------\n");
        printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
        printf("--------------------------\n");
        if (sendto(s,&Packet1, sizeof(Packet1)+25, 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
        FD_ZERO(&select_fds);             
        FD_SET(s, &select_fds);  
        if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 )
        {
            printf("Timed occured for the Attempt #2 ... Sending Once again\n");
            printf("Message Transmitted: \n--------------------------\n");
            printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
            printf("--------------------------\n");
            if (sendto(s,&Packet1, sizeof(Packet1)+25, 0 , (struct sockaddr *) &si_other, slen)==-1)
            {
                die("sendto()");
            }
            FD_ZERO(&select_fds);             
            FD_SET(s, &select_fds);
            if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 )
            {
                printf("Timed occured for the Attempt #3 ... Sending Once again\n");
                printf("Message Transmitted: \n--------------------------\n");
                printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
                printf("--------------------------\n");
                if (sendto(s,&Packet1, sizeof(Packet1)+25, 0 , (struct sockaddr *) &si_other, slen)==-1)
                {
                    die("sendto()");
                }
                FD_ZERO(&select_fds);            
                FD_SET(s, &select_fds); 
                if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 )
                {
                        printf("Timed occured for the Attempt #4 \n");
                        printf("Server Not Responding\n");
                        exit(0);
                } 
                else
                {
                    if (recvfrom(s, &Packet1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                    {
                        die("recvfrom()");
                    }
                    if(strcmp(Packet1.acc_Per,"P") ==0)
                    {
                        printf("Acknowledgement Block : Subscriber is permitted to access\n");
                    }
                    else if(strcmp(Packet1.acc_Per,"Q") ==0)
                    {
                        printf("Rejection Block : Subscriber has not paid\n");
                    }
                    else if(strcmp(Packet1.acc_Per,"R") ==0)
                    {
                        printf("Rejection Block : Subscriber does not exist\n");
                    }
                    printf("Message Received: \n--------------------------\n");
                    printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
                    printf("--------------------------\n");
                }
            }
            else
            {
                if (recvfrom(s, &Packet1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                if(strcmp(Packet1.acc_Per,"P") ==0)
                {
                    printf("Acknowledgement Block : Subscriber is permitted to access\n");
                }
                else if(strcmp(Packet1.acc_Per,"Q") ==0)
                {
                    printf("Rejection Block : Subscriber has not paid\n");
                }
                else if(strcmp(Packet1.acc_Per,"R") ==0)
                {
                    printf("Rejection Block : Subscriber does not exist\n");
                }
                printf("Message Received: \n--------------------------\n");
                printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
                printf("--------------------------\n");
            }
        }
        else
        {
            if (recvfrom(s, &Packet1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
            {
                die("recvfrom()");
            }
            if(strcmp(Packet1.acc_Per,"P") ==0)
            {
                printf("Acknowledgement Block : Subscriber is permitted to access\n");
            }
            else if(strcmp(Packet1.acc_Per,"Q") ==0)
            {
                printf("Rejection Block : Subscriber has not paid\n");
            }
            else if(strcmp(Packet1.acc_Per,"R") ==0)
            {
                printf("Rejection Block : Subscriber does not exist\n");
            }
            printf("Message Received: \n--------------------------\n");
            printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
            printf("--------------------------\n");
        }
    }
    else
    {
        if (recvfrom(s, &Packet1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            die("recvfrom()");
        }
        if(strcmp(Packet1.acc_Per,"P") ==0)
        {
            printf("Acknowledgement Block : Subscriber is permitted to access\n");
        }
        else if(strcmp(Packet1.acc_Per,"Q") ==0)
        {
            printf("Rejection Block : Subscriber has not paid\n");
        }
        else if(strcmp(Packet1.acc_Per,"R") ==0)
        {
            printf("Rejection Block : Subscriber do not exist\n");
        }
        printf("Message Received: \n--------------------------\n");
        printf("|%s|%d|%s|%d|%d|%d|%u|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.acc_Per,Packet1.segNo,Packet1.length,Packet1.technology,Packet1.src_Sub_num,Packet1.endID);
        printf("--------------------------\n");
    }
    close(s);
    s=-1;
    i++;
}
int main(void)
{
    clear();

    struct Packet Packets[4];
    char lyne[264];
    char message[BUFLEN];   
    int reccount = 0;
    char *item;

    FILE *fp;   
    fp = fopen("TST.txt","r"); // read mode

    if( fp == NULL )
    {
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
    }   
    //int messageSize = sizeof(message);
    
    while (fgets(lyne,264,fp) && reccount <4)
    {
        char *ptr;
        //printf("Current size %d\n", sizeof(Packets[reccount]));
        item = strtok(lyne,",");
        strcpy(Packets[reccount].sPackID,item);

        item = strtok(NULL,",");
        Packets[reccount].clID=atoi(item);

        item = strtok(NULL,",");
        strcpy(Packets[reccount].acc_Per,item);

        item = strtok(NULL,",");
        Packets[reccount].segNo = atoi(item);

        item = strtok(NULL,",");
        //Assign from the data packet
        Packets[reccount].technology = atoi(item);

        item = strtok(NULL,",");
        Packets[reccount].src_Sub_num=atoi(item);
        

        item = strtok(NULL,"\n");
        strcpy(Packets[reccount].endID,item);

        //Assign the actual length
        Packets[reccount].length = sizeof(Packets[reccount].src_Sub_num);
        reccount++;
    }
    /* Close file */
    fclose(fp);
    while(counter<4)
    {
        printf("Counter: %d\n", counter);
        sendPacket(Packets[counter]);
        counter++;
    }

}
