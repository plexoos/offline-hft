#include "TGeoNode.h"

class MyHftDisplayDummy
{
   ClassDef(MyHftDisplayDummy, 0)
};

void make_geometry();
void decodeId(int id, int *sector, int *ladder, int *sensor);
void hft_display(const double B = 1, const Int_t runnumber = 15037068);
void loadStatus(const int runnumber);
void decodeId(int id, int *sector, int *ladder, int *sensor);
void init(const double B);
void process_event(Int_t iEvt);
void selectDaughterVisible(TGeoNode *node, const char *name);
void make_geometry();
void hft_make_gui();

static Int_t myEventCounter;

class EvNavHandler
{
public:
   void Fwd() {
      process_event(++myEventCounter);
   }
   void Bck() {
      myEventCounter--;
      process_event(myEventCounter);
   }

   ClassDef(EvNavHandler, 0)
};
