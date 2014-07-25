// $Id$
// 
// $Log$
// Revision 1.1  2014/07/25 15:21:10  bouchet
// update for y2014
//
// Revision 1.2  2005/06/21 15:31:48  lmartin
// CVS tags added
//
#include "StiSsd/StiSsdIsActiveFunctor.h"

StiSsdIsActiveFunctor::StiSsdIsActiveFunctor()
{} // StiSsdIsActiveFunctor

StiSsdIsActiveFunctor::~StiSsdIsActiveFunctor(){
} // ~StiSsdIsActiveFunctor

bool StiSsdIsActiveFunctor::operator()(double dYlocal, double dZlocal) const
{
  return true;
} // operator()
