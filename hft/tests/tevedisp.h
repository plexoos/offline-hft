
#include <string>

#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TChain.h"

#include "StHftPool/HftMatchedTree/PrgOptionProcessor.h"


void decodeId(int id, int *sector, int *ladder, int *sensor);
void hft_display(PrgOptionProcessor &poProc, const double B = 1);
void hft_display(const double B = 1, const Int_t runnumber = 15037068);
void loadStatus(const int runnumber);
void init(const double B);
void make_visible_select_volumes(TGeoNavigator &geoNav, PrgOptionProcessor &poProc);
void make_visible_daughters(TGeoNavigator &geoNav, PrgOptionProcessor &poProc, bool usePattern=false);
void make_visible_mothers(TGeoNavigator &geoNav, int levelsUp);
void make_geometry(PrgOptionProcessor &poProc);
void build_gui(TChain &hftree);

