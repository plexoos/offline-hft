/*!
 * \class StSstConstants
 * \author Long Zhou, Nov 2013
 * \Initial Revision.
 */
/***************************************************************************
 *
 * $Id$
 *
 * Author: Long Zhou, Nov 2013
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log$
 * Revision 1.1  2014/03/25 14:57:50  zhoulong
 * First version of SST DAQ maker .
 *
 *
 **************************************************************************/

#ifndef StSstConstants_hh
#define StSstConstants_hh

const int nSstRdo = 5;
const int nSstSide = 2;
const int nSstLadder = 20;
const int nSstFiberPerRdo = 8;
const int nSstWaferPerLadder = 16;
const int nSstStripsPerWafer = 768;
const int RDO2LADDER[5][8] = { {1,2,3,4,5,6,7,8},
                           {9,10,11,12,13,14,15,16},
                           {17,18,19,20,1,2,3,4},
                           {5,6,7,8,9,10,11,12},
                           {13,14,15,16,17,18,19,20}
	                   };//Ladder cards number in each RDO channel .
const int ReadOutMap[128] ={
  97,96,98,95,99,94,100,93,
  101,92,102,91,103,90,104,89,
  105,88,106,87,107,86,108,85,
  109,84,110,83,111,82,112,81,
  113,80,114,79,115,78,116,77,
  117,76,118,75,119,74,120,73,
  121,72,122,71,123,70,124,69,
  125,68,126,67,127,66,128,65,
  1,64,2,63,3,62,4,61,
  5,60,6,59,7,58,8,57,
  9,56,10,55,11,54,12,53,
  13,52,14,51,15,50,16,49,
  17,48,18,47,19,46,20,45,
  21,44,22,43,23,42,24,41,
  25,40,26,39,27,38,28,37,
  29,36,30,35,31,34,32,33
};//silicon strip number ordered by ALICE128 readout order      
#endif
