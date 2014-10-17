#include "StPxlSensor.h"
ClassImp(StPxlSensor);
StPxlSensor::StPxlSensor(){
  maskClear();
  FlagClear();
}
void StPxlSensor::SetId(Int_t ID){
  sensor=ID%10;
  ladder=((ID-sensor)%40)/10;
  sector=(ID-10*ladder-sensor)/40;
  return;
}
void StPxlSensor::FlagClear(){
  missing=0;
  masked_out=0;
  ind_badPix=0;
  status=-1;
  return;
}
void StPxlSensor::maskClear(){
  listBadPix.clear();
  listBadCol.clear();
  listBadRow.clear();
  return;
}
void StPxlSensor::setMaskInfo(Int_t col, Int_t row, Int_t pix, Int_t sentries, Int_t dead){
  hotCol=col;
  hotRow=row;
  hotPix=pix;
  missing=dead;
  entries=sentries;
  return;
}
