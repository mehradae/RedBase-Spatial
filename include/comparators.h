#pragma once


#include <unistd.h>
#include <sys/types.h>
#include "pf.h"
#include <stdlib.h>
#include <cstdio>
#include <cstring>

static int compare_string(void *value1, void* value2, int attrLength){
  return strncmp((char *) value1, (char *) value2, attrLength);
}

static int compare_int(void *value1, void* value2, int attrLength){
  if((*(int *)value1 < *(int *)value2))
    return -1;
  else if((*(int *)value1 > *(int *)value2))
    return 1;
  else
    return 0;
}

static int compare_float(void *value1, void* value2, int attrLength){
  if((*(float *)value1 < *(float *)value2))
    return -1;
  else if((*(float *)value1 > *(float *)value2))
    return 1;
  else
    return 0;
}

//Adding MBR 
static int compare_mbr(void *value1, void* value2, int attrLength){
  
  mbr MBRTuple = *(mbr *)value1; //existing MBR
  mbr MBRQuery = *(mbr *)value2; //query MBR

         //Checking MBRs
         // It covers all cases of intersection including equality and inclusion.
    if ((MBRTuple.top_left_x > MBRQuery.top_left_x ? MBRTuple.top_left_x : MBRQuery.top_left_x) <=
    (MBRTuple.bottom_right_x < MBRQuery.bottom_right_x ? MBRTuple.bottom_right_x : MBRQuery.bottom_right_x) &&
    (MBRTuple.top_left_y > MBRQuery.top_left_y ? MBRTuple.top_left_y : MBRQuery.top_left_y) >=
    (MBRTuple.bottom_right_y < MBRQuery.bottom_right_y ? MBRTuple.bottom_right_y : MBRQuery.bottom_right_y)) {
        return 0;
    } else return -1;


//  if(MBRQuery.top_left_x == MBRTuple.top_left_x && MBRQuery.top_left_y == MBRTuple.top_left_y && MBRQuery.bottom_right_x == MBRTuple.bottom_right_x && MBRQuery.bottom_right_y == MBRTuple.bottom_right_y)
//    return 0;
//  else if(MBRQuery.top_left_x < MBRTuple.top_left_x && MBRQuery.top_left_y < MBRTuple.top_left_y && MBRQuery.bottom_right_x > MBRTuple.bottom_right_x && MBRQuery.bottom_right_y > MBRTuple.bottom_right_y)
//    return -1;
//  else
//  {
//    //Calculate original mbr area
//    int area = (MBRQuery.bottom_right_x - MBRQuery.top_left_x)*(MBRQuery.bottom_right_y - MBRQuery.top_left_y);
//    mbr newRect;
//
//    newRect.top_left_x = MBRTuple.top_left_x > MBRQuery.top_left_x ? MBRTuple.top_left_x : MBRQuery.top_left_x;
//    newRect.top_left_y = MBRTuple.top_left_y > MBRQuery.top_left_y ? MBRTuple.top_left_y : MBRQuery.top_left_y;
//    newRect.bottom_right_x = MBRTuple.bottom_right_x < MBRQuery.bottom_right_x ? MBRTuple.bottom_right_x : MBRQuery.bottom_right_x;
//    newRect.bottom_right_y = MBRTuple.bottom_right_y < MBRQuery.bottom_right_y ? MBRTuple.bottom_right_y : MBRQuery.bottom_right_y;
//    //new area
//    int newArea = (newRect.bottom_right_x - newRect.top_left_x)*(newRect.bottom_right_y - newRect.top_left_y);
//
//    return (newArea - area);
//  }

}

static bool print_string(void *value, int attrLength){
  char * str = (char *)malloc(attrLength + 1);
  memcpy(str, value, attrLength+1);
  str[attrLength] = '\0';
  printf("%s ", str);
  free(str);
  return true;
}

static bool print_int(void *value, int attrLength){
  int num = *(int*)value;
  printf("%d ", num);
  return true;
}

static bool print_float(void *value, int attrLength){
  float num = *(float *)value;
  printf("%f ", num);
  return true;
}

static bool print_mbr(void *value, int attrLength){
  mbr mbr = *(struct mbr *)value;
  printf("(%d, %d, %d, %d) ", mbr.top_left_x, mbr.top_left_y, mbr.bottom_right_x, mbr.bottom_right_y);
  return true;
}
