/*=========================================================================

  Program:   Mouse Sensor OpenIGTLink -- Based on Example for Tracker Server Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: 5/29/2012$
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"

// ---------------------
// 5/29/2012 ayamada
// for linux input subsystem
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
// for multi threading
#include <pthread.h>
#include <semaphore.h>
// definitions
#define NUM_THREADS 1
#define COEFFICIENT 0.025
// ---------------------

// thread 
void* thread_getMouseCounts(void *p);

pthread_mutex_t work_mutex;
pthread_t a_thread[NUM_THREADS];

int nthreads = 1;
int  threadFinishFlag =0;

// thread number
int tNumber[NUM_THREADS];
void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

// global variables
long movementX = 0;
long movementY = 0;

int main(/*int argc, char* argv[]*/)
{
  
  // 5/29/2012 ayamada
  // make pthread
  int res;
  void *thread_result;
  int lots_of_threads;
  
  res = pthread_mutex_init(&work_mutex, NULL);
  if (res != 0) {
    std::cerr << "Mutex initialization failed\n" <<std::endl;    
    exit(EXIT_FAILURE);
  }
  
  pthread_mutex_lock(&work_mutex);
  // create a thread for getting mouse counts
  res = pthread_create(&(a_thread[0]), NULL, thread_getMouseCounts, (void *)&lots_of_threads);
  if (res != 0) {
    std::cerr << "Thread 0 (intelligent suction tube) creation failed.\n" <<std::endl;    
    exit(EXIT_FAILURE);
  }else{
    std::cerr << "Thread 0 (intelligent suction tube) creation succeeded.\n" <<std::endl;            
  }
  sleep(1);
  pthread_mutex_unlock(&work_mutex);
  
  int x = 0;
  int y = 0;
  
  //------------------------------------------------------------
  // Parse Arguments

/*
  if (argc != 3) // check number of arguments
    {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <port> <fps>"    << std::endl;
    std::cerr << "    <port>     : Port # (18944 in Slicer default)"   << std::endl;
    std::cerr << "    <fps>      : Frequency (fps) to send coordinate" << std::endl;
    exit(0);
    }
*/

      for (;;) {

        struct input_event event;
          
        if (read(0, &event, sizeof(event)) != sizeof(event)) {
          exit(EXIT_FAILURE);
        }
        
        pthread_mutex_lock(&work_mutex);
        switch(event.type) {
          case EV_REL:
            switch(event.code) {
              case REL_X:
                movementX += event.value;
                break;
              case REL_Y:
                movementY += event.value;
                break;
              default:
                break;
            }
            break;
          case EV_KEY:
            switch(event.code) {
                //case KEY_SPACE:
              case BTN_LEFT:
                //case KEY_ESC:
                movementX = 0;
                movementY = 0;
                printf("Space key was pushed.\n");
                break;
              default:
                break;
            }
          default:
            break;
        }
        pthread_mutex_unlock(&work_mutex);
                
      }

}


void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  pthread_mutex_lock(&work_mutex);
  matrix[0][3] = float(movementX)*COEFFICIENT;
  matrix[1][3] = 0.0;
  matrix[2][3] = -float(movementY)*COEFFICIENT;
  pthread_mutex_unlock(&work_mutex);

  for(int i = 0; i<3; i++)
  {
   matrix[0][i] = 0.0;
   matrix[1][i] = 0.0;
   matrix[2][i] = 0.0;
  }
  matrix[0][0] = 1.0;
  matrix[1][1] = 1.0;
  matrix[2][2] = 1.0;

  igtl::PrintMatrix(matrix);
  printf("movementX:%d\n",movementX);
  printf("movementY:%d\n",movementY);

}

void* thread_getMouseCounts(void *p)
{
  
  int    port     = 18900;//atoi(argv[1]);
  double fps      = 15;//atof(argv[2]);
  int    interval = (int) (1000.0 / fps);
  
  igtl::TransformMessage::Pointer transMsg;
  transMsg = igtl::TransformMessage::New();
  //transMsg->SetDeviceName("Tracker");
  transMsg->SetDeviceName("MarkersPosition2");
  
  igtl::ServerSocket::Pointer serverSocket;
  serverSocket = igtl::ServerSocket::New();
  int r = serverSocket->CreateServer(port);
  
  if (r < 0)
  {
    std::cerr << "Cannot create a server socket." << std::endl;
    exit(0);
  }
  
  igtl::Socket::Pointer socket;
  
  while (1)
  {
    //------------------------------------------------------------
    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);
    
    if (socket.IsNotNull()) // if client connected
    {
      //------------------------------------------------------------
      // loop
      
      for (;;) {
        
        igtl::Matrix4x4 matrix;
        GetRandomTestMatrix(matrix);
        transMsg->SetMatrix(matrix);
        transMsg->Pack();
        socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
        igtl::Sleep(interval); // wait
                
      }
    }
  }
  
  //------------------------------------------------------------
  // Close connection (The example code never reachs to this section ...)
  
  socket->CloseSocket();
  
}

