  REAL FUNCTION FITT(X,Y) 
C
         COMMON/PAWPAR/PAR(3)
         FITT=(X-PAR(1))**2. +(Y-PAR(2))**2. + (2.168)**2. - PAR(3)**2.
         END
