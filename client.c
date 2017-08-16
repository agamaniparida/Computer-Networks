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
#define PORT 8888   //The port on which to send data

int i=0;
int counter =0;
struct Packet 
{
   char  sPackID[2];
   char  clID;
   char  dataTP[2];
   uint8_t  segNo;
   uint8_t length;
   char  actMessage[BUFLEN];
   char  endID[2];
};

struct Acknowlegemnt 
{
   char  ack_sPackID[2];
   char  ack_clID;
   char  ack_dataTP[2];
   uint8_t  ack_segNo;
   char  ack_endID[2];
};
struct Rejection 
{
   char  rej_sPackID[2];
   char  rej_clID;
   char  rej_dataTP[2];
   char  rej_subcode[3];
   uint8_t  rej_segNo;
   char  rej_endID[2];
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
    struct Acknowlegemnt ack1;
    struct Rejection rej1;
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
    printf("\nMessage Transmitted: \n---------------------------");
    printf("\n|%s|%c|%s|%d|%d|%s|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.dataTP,Packet1.segNo,Packet1.length,Packet1.actMessage,Packet1.endID);
    printf("---------------------------\n");
    if (sendto(s,&Packet1, sizeof(Packet1)+25, 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        die("sendto()");
    }
    //i++;
    fd_set select_fds;                
    struct timeval timeout;           
    FD_ZERO(&select_fds);             
    FD_SET(s, &select_fds);                                                              
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 )
    {
        printf("Timed occured for the Attempt #1 ... Sending Once again\n");
        if (sendto(s,&Packet1, sizeof(Packet1), 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sendto()");
        }
        FD_ZERO(&select_fds);             
        FD_SET(s, &select_fds);  
        if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 )
        {
            printf("Timed occured for the Attempt #2 ... Sending Once again\n");
            if (sendto(s,&Packet1, sizeof(Packet1), 0 , (struct sockaddr *) &si_other, slen)==-1)
            {
                die("sendto()");
            }
            FD_ZERO(&select_fds);            
            FD_SET(s, &select_fds);  
            if ( select(32, &select_fds, NULL, NULL, &timeout) == 0 )
            {
                printf("Timed occured for the Attempt #3 \n");
                printf("Server Not Responding\n");
                exit(0);
            }
            else
            {
                /////This is the vblock where the code will check for the THIRD time
                memset(buf,'\0', BUFLEN);
                if(strlen(Packet1.actMessage) != Packet1.length)
                {
                    if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                    {
                        die("recvfrom()");
                    }
                    printf("Rejection Received: Due to Mismatch in Payload length\n--------------\n");
                    printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
                    printf("--------------\n"); 
                }
                else if(counter == Packet1.segNo)
                {
                    if ((!*Packet1.endID) == 0)
                    {
                        //This block shows the end packet id is not blank
                        if (recvfrom(s, &ack1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                        {
                            die("recvfrom()");
                        }
                        printf("Acknowlegemnt Received: \n------------\n");
                        printf("|%s|%c|%s|%d|%s|\n", ack1.ack_sPackID,ack1.ack_clID,ack1.ack_dataTP,ack1.ack_segNo,ack1.ack_endID);
                        printf("------------\n");
                    }
                    else
                    {
                        if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                        {
                            die("recvfrom()");
                        }
                        printf("Rejection Received: Due to Missing End ID\n--------------\n");
                        printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
                        printf("--------------\n");
                    }
                }
                else if(counter != Packet1.segNo)
                {
                    if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                    {
                        die("recvfrom()");
                    }
                    if(strcmp(rej1.rej_subcode,"M")==0)
                    {
                    printf("Rejection Received: Due to Sequence Miss Match\n--------------\n");
                    }
                    else if(strcmp(rej1.rej_subcode,"N")==0)
                    {
                    printf("Rejection Received: Due to Duplicate Sequence\n--------------\n");
                    }
                    printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
                    printf("--------------\n"); 
                }   
            }
        }
        else
        {
            /////This is the vblock where the code will check for the second time
            memset(buf,'\0', BUFLEN);
            if(strlen(Packet1.actMessage) != Packet1.length)
            {
                if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                printf("Rejection Received: Due to Mismatch in Payload length\n--------------\n");
                printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
                printf("--------------\n"); 
            }
            else if(counter == Packet1.segNo)
            {
                if ((!*Packet1.endID) == 0)
                {
                    //This block shows the end packet id is not blank
                    if (recvfrom(s, &ack1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                    {
                        die("recvfrom()");
                    }
                    printf("Acknowlegemnt Received: \n------------\n");
                    printf("|%s|%c|%s|%d|%s|\n", ack1.ack_sPackID,ack1.ack_clID,ack1.ack_dataTP,ack1.ack_segNo,ack1.ack_endID);
                    printf("------------\n");
                }
                else
                {
                    //This block shows the end packet id is blank
                    if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                    {
                        die("recvfrom()");
                    }
                    printf("Rejection Received: Due to Missing End ID\n--------------\n");
                    printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
                    printf("--------------\n");
                }
            }
            else if(counter != Packet1.segNo)
            {
                if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                if(strcmp(rej1.rej_subcode,"M")==0)
                {
                printf("Rejection Received: Due to Sequence Miss Match\n--------------\n");
                }
                else if(strcmp(rej1.rej_subcode,"N")==0)
                {
                printf("Rejection Received: Due to Duplicate Sequence\n--------------\n");
                }
                printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
                printf("--------------\n"); 
            }
        }
    }
    else
    {
        /////This is the block where the code will check for the first time
        memset(buf,'\0', BUFLEN);
        if(strlen(Packet1.actMessage) != Packet1.length)
        {
            if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
            {
                die("recvfrom()");
            }
            printf("Rejection Received: Due to Mismatch in Payload length\n--------------\n");
            printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
            printf("--------------\n"); 
        }
        else if(counter == Packet1.segNo)
        {
            if ((!*Packet1.endID) == 0)
            {
                //This block shows the end packet id is not blank
                if (recvfrom(s, &ack1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                printf("Acknowlegemnt Received: \n------------\n");
                printf("|%s|%c|%s|%d|%s|\n", ack1.ack_sPackID,ack1.ack_clID,ack1.ack_dataTP,ack1.ack_segNo,ack1.ack_endID);
                printf("------------\n");
            }
            else
            {
                //This block shows the end packet id is blank
                if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }
                printf("Rejection Received: Due to Missing End ID\n--------------\n");
                printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
                printf("--------------\n");
            }
        }
        else if(counter != Packet1.segNo)
        {
            if (recvfrom(s, &rej1, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
            {
                die("recvfrom()");
            }
            if(strcmp(rej1.rej_subcode,"M")==0)
            {
            printf("Rejection Received: Due to Sequence Miss Match\n--------------\n");
            printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
            printf("--------------\n");
            exit(0);
            }
            else if(strcmp(rej1.rej_subcode,"N")==0)
            {
            printf("Rejection Received: Due to Duplicate Sequence\n--------------\n");
            printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
            printf("--------------\n");
            }
            
        }
    } 
    close(s);
    s=-1;
    i++;
}
int main(void)
{
    clear();
    struct Packet Packets[5];
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

    while (fgets(lyne,264,fp) && reccount <5) 
    {
        item = strtok(lyne,",");
        strcpy(Packets[reccount].sPackID,item);

        item = strtok(NULL,",");
        Packets[reccount].clID='D';

        item = strtok(NULL,",");
        strcpy(Packets[reccount].dataTP,item);

        item = strtok(NULL,",");
        Packets[reccount].segNo = atoi(item);

        item = strtok(NULL,",");
        //Assign from the data packet
        Packets[reccount].length = atoi(item);

        item = strtok(NULL,",");
        strcpy(Packets[reccount].actMessage,item);

        //Assign the actual length
        Packets[reccount].length = strlen(Packets[reccount].actMessage);
        /*if(reccount ==2)
        {
            Packets[reccount].length = 2;
        }*/

        item = strtok(NULL,"\n");
        if(item != NULL)
        {
          strcpy(Packets[reccount].endID,item);
        }
        else
        {
          strcpy(Packets[reccount].endID,"");
        }
        
        reccount++;
    }
    /* Close file */
    fclose(fp);
    
    while(counter<5)
    {
        printf("Counter %d\n", counter);
        sendPacket(Packets[counter]);
        counter++;
    }

}
