
Int_t gHftGuiEventCounter = 0;

void process_event(Int_t iEvt);

class GuiEventHandler : public TObject
{
public:

   void Fwd() {
      process_event(++gHftGuiEventCounter);
   }

   void Bck() {
      gHftGuiEventCounter--;
      process_event(gHftGuiEventCounter);
   }

   ClassDef(GuiEventHandler, 0)
};
