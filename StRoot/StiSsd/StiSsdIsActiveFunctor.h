// $Id: StiSsdIsActiveFunctor.h,v 1.1 2014/07/25 15:21:10 bouchet Exp $
// 
// $Log: StiSsdIsActiveFunctor.h,v $
// Revision 1.1  2014/07/25 15:21:10  bouchet
// update for y2014
//
// Revision 1.2  2005/06/21 15:31:48  lmartin
// CVS tags added
//
/**
 * file StiSsdIsActiveFunctor.h
 * class StiSsdIsActiveFunctor
 * brief function object for determine a SSD ladder's active regions
 */

#ifndef STI_SSD_IS_ACTIVE_FUNCTOR
#define STI_SSD_IS_ACTIVE_FUNCTOR

#include "Sti/StiIsActiveFunctor.h"

struct StiSsdIsActiveFunctor : public StiIsActiveFunctor
{
    StiSsdIsActiveFunctor();
    virtual ~StiSsdIsActiveFunctor();
    virtual bool operator()(double dYlocal, double dZlocal) const;
};

#endif // defined STI_SSD_IS_ACTIVE_FUNCTOR
