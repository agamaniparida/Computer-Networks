Name : Agamani Parida

Assignment name : PA2

Date : 03/09/2017

High-level description of the assignment and what your program(s) does - 

It is a socket programming where one client connects to one server using customized protocol on top of UDP protocol fro sending information from client to server.
The client requests specific information from server; the server will verify the validity of request and then will respond accordingly. Communication uses one ack timer which is set to 3 seconds for 3 times. If ack is not received by client within that timer, client will resend the packet. If ack is not received after 3 times, client will generate “server not responding” error. Below are the cases that are handled in this program.
1) if subscriber is present in database and paid, it will get access permission.
2) if subscriber is present in database and not pain, it will generate subscriber not paid error.
3) if subscriber is not present in database, it will generate subscriber does not exist error.

All outputs are displayed on terminal.


A list of submitted files - input files, output files, source code, snapshot of output, readme file

Instructions for running your program: Run from command line -
s
1) go to the directory where source code is present. I have created two folders for client and server.
2) compile both client.c and server.c using gcc command.
3) execute using ./a.out.
4) all the acknowledgement and rejection packets are displayed on terminal.