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
    int ms = mseconds * 1000000;
    clock_t goal = ms + clock();
    while (goal > clock());
}
int main(void)
{
    clear();
    struct sockaddr_in si_me, si_other;
    struct Packet Packet1;
    struct Acknowlegemnt ack1;
    struct Rejection rej1;
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
        printf("\n****************************\n");
        printf("\nCounter %d\n",counter);
        printf("\n****************************\n");

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, &Packet1, sizeof(Packet1)-1, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
        printf("Message Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        //printf("Data: %s\n" , Packet1.actMessage);
        if(counter==4)
        {
            delay(1);
        }
        printf("---------------------------");
        printf("\n|%s|%c|%s|%d|%d|%s|%s|\n", Packet1.sPackID,Packet1.clID,Packet1.dataTP,Packet1.segNo,Packet1.length,Packet1.actMessage,Packet1.endID);
        printf("---------------------------\n");

        if(Packet1.length != strlen(Packet1.actMessage))
        {
            strcpy( rej1.rej_sPackID, Packet1.sPackID);
            rej1.rej_clID= Packet1.clID;
            strcpy( rej1.rej_dataTP, "R"); 
            strcpy( rej1.rej_subcode, "O");
            printf("Rejecttion Sub Code %s\n", rej1.rej_subcode);
            rej1.rej_segNo= Packet1.segNo;
            strcpy(rej1.rej_endID,Packet1.endID);
            if (sendto(s, &rej1, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            {
                die("sendto()");
            }
            printf("Sending Rejection : Due to Mismatch Payload length\n--------------\n");
            printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
            printf("--------------\n");
        }
        else if(counter == Packet1.segNo)
        {
            if (!Packet1.endID || (!*Packet1.endID) == 0)
            {
                strcpy( ack1.ack_sPackID, Packet1.sPackID);
                ack1.ack_clID=Packet1.clID;
                strcpy( ack1.ack_dataTP, "A"); 
                ack1.ack_segNo= Packet1.segNo;
                strcpy(ack1.ack_endID,Packet1.endID);
                if (sendto(s, &ack1, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
                {
                    die("sendto()");
                }
                printf("Sending Acknowledgement : \n------------\n");
                printf("|%s|%c|%s|%d|%s|\n", ack1.ack_sPackID,ack1.ack_clID,ack1.ack_dataTP,ack1.ack_segNo,ack1.ack_endID);
                printf("------------\n");
            }
            else
            {
                //Create Rejection
                //printf("Sending Rejection : Due to End ID Missing\n");
                strcpy( rej1.rej_sPackID, Packet1.sPackID);
                rej1.rej_clID= Packet1.clID;
                strcpy( rej1.rej_dataTP, "R"); 
                strcpy( rej1.rej_subcode, "L");
                rej1.rej_segNo= Packet1.segNo;
                strcpy(rej1.rej_endID,Packet1.endID);
                if (sendto(s, &rej1, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
                {
                    die("sendto()");
                }
                printf("Sending Rejection : Due to Missing End ID\n--------------\n");
                printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
                printf("--------------\n");
            } 
        }
        else if(counter < Packet1.segNo)
        {
            //printf("Sending Rejection : Due to Skipping One of the segment\n");
            strcpy( rej1.rej_sPackID, Packet1.sPackID);
            rej1.rej_clID= Packet1.clID;
            strcpy( rej1.rej_dataTP, "R"); 
            strcpy( rej1.rej_subcode, "M");
            rej1.rej_segNo= Packet1.segNo;
            strcpy(rej1.rej_endID,Packet1.endID);
            printf("Rejecttion Sub Code %s\n", rej1.rej_subcode);
            if (sendto(s, &rej1, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            {
                die("sendto()");
            }
            printf("Sending Rejection : Due to Sequence Miss Match\n--------------\n");
            printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
            printf("--------------\n");
        }
        else if(counter > Packet1.segNo)
        {
            //printf("Sending Rejection : Due to Duplicate Sequence %c\n", Packet1.segNo);
            strcpy( rej1.rej_sPackID, Packet1.sPackID);
            rej1.rej_clID= Packet1.clID;
            strcpy( rej1.rej_dataTP, "R"); 
            strcpy( rej1.rej_subcode, "N");
            printf("Rejecttion Sub Code %s\n", rej1.rej_subcode);
            rej1.rej_segNo= Packet1.segNo;
            strcpy(rej1.rej_endID,Packet1.endID);
            if (sendto(s, &rej1, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
            {
                die("sendto()");
            }
            printf("Sending Rejection : Due to Duplicate Sequence\n--------------\n");
            printf("|%s|%c|%s|%s|%d|%s|\n", rej1.rej_sPackID,rej1.rej_clID,rej1.rej_dataTP,rej1.rej_subcode,rej1.rej_segNo,rej1.rej_endID);
            printf("--------------\n");
        }
        
        counter++;  
    }
    close(s);
    return 0;
}
